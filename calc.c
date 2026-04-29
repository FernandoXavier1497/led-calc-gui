#include "calc.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

// ============================================================
//  LED CALC v6.0 - Implementacao do core
//  Autor: Fernando Xavier
// ============================================================

// ---------- Tabela E12 ----------
static const int valoresE12[] = {10,12,15,18,22,27,33,39,47,56,68,82};
static const int numValores   = 12;
static const int numDecadas   = 5;

static int tabelaR[60];
static int totalR = 0;

void montarTabela() {
    totalR = 0;
    for (int d = 0; d < numDecadas; d++) {
        int pot = (int)round(pow(10, d));
        for (int i = 0; i < numValores; i++)
            tabelaR[totalR++] = valoresE12[i] * pot;
    }
}

// ---------- Resistor comercial mais proximo ----------
int resistorComercial(float R) {
    if (R <= 0) return 0;
    int melhor = tabelaR[0];
    float menor = fabs(R - tabelaR[0]);
    for (int i = 1; i < totalR; i++) {
        float d = fabs(R - tabelaR[i]);
        if (d < menor) { menor = d; melhor = tabelaR[i]; }
    }
    return melhor;
}

// ---------- Busca melhor combinacao ----------
Combinacao buscarCombinacao(float R) {
    Combinacao best;
    memset(&best, 0, sizeof(best));
    best.erroPct = 1e9f;

    for (int i = 0; i < totalR; i++) {
        float A = tabelaR[i];

        // Simples
        float d = fabs(R - A);
        if (d < best.erroPct) {
            best.erroPct = d; best.R1 = A; best.R2 = 0; best.R3 = 0;
            best.Req = A; strcpy(best.tipo, "Simples");
        }

        for (int j = 0; j < totalR; j++) {
            float B = tabelaR[j];

            // Serie (2)
            float Rs = A + B;
            d = fabs(R - Rs);
            if (d < best.erroPct) { best.erroPct=d; best.R1=A; best.R2=B; best.R3=0; best.Req=Rs; strcpy(best.tipo,"Serie (2)"); }

            // Paralelo (2)
            float Rp = (A * B) / (A + B);
            d = fabs(R - Rp);
            if (d < best.erroPct) { best.erroPct=d; best.R1=A; best.R2=B; best.R3=0; best.Req=Rp; strcpy(best.tipo,"Paralelo (2)"); }

            for (int k = 0; k < totalR; k++) {
                float C = tabelaR[k];

                // Serie (3)
                float Rs3 = A + B + C;
                d = fabs(R - Rs3);
                if (d < best.erroPct) { best.erroPct=d; best.R1=A; best.R2=B; best.R3=C; best.Req=Rs3; strcpy(best.tipo,"Serie (3)"); }

                // Paralelo (3)
                float Rp3 = 1.0f / (1.0f/A + 1.0f/B + 1.0f/C);
                d = fabs(R - Rp3);
                if (d < best.erroPct) { best.erroPct=d; best.R1=A; best.R2=B; best.R3=C; best.Req=Rp3; strcpy(best.tipo,"Paralelo (3)"); }

                // Mista: (A+B) || C
                float m1 = ((A + B) * C) / (A + B + C);
                d = fabs(R - m1);
                if (d < best.erroPct) { best.erroPct=d; best.R1=A; best.R2=B; best.R3=C; best.Req=m1; strcpy(best.tipo,"Mista (R1+R2)||R3"); }

                // Mista: (A||B) + C
                float m2 = (A * B) / (A + B) + C;
                d = fabs(R - m2);
                if (d < best.erroPct) { best.erroPct=d; best.R1=A; best.R2=B; best.R3=C; best.Req=m2; strcpy(best.tipo,"Mista (R1||R2)+R3"); }
            }
        }
    }

    best.erroPct = fabs(best.Req - R) / R * 100.0f;
    return best;
}

// ---------- Calculo completo ----------
ResultadoLED calcularLED(float Vin, float Vled, float IledmA, int numLeds, int configLed) {
    ResultadoLED res;
    memset(&res, 0, sizeof(res));

    res.Vin      = Vin;
    res.Vled     = Vled;
    res.IledmA   = IledmA;
    res.numLeds  = numLeds <= 0 ? 1 : numLeds;
    res.configLed = configLed;

    float Iled = IledmA / 1000.0f;

    res.VledTotal = (configLed == 1) ? Vled * res.numLeds : Vled;
    res.IledTotal = (configLed == 1) ? Iled : Iled * res.numLeds;

    if (Vin <= res.VledTotal) {
        snprintf(res.erro, sizeof(res.erro),
                 "Erro: Vin (%.2fV) deve ser maior que Vled total (%.2fV)!",
                 Vin, res.VledTotal);
        res.valido = 0;
        return res;
    }

    res.valido     = 1;
    res.R_ideal    = (Vin - res.VledTotal) / res.IledTotal;
    res.R_simples  = resistorComercial(res.R_ideal);
    res.erroSimples = fabs((float)res.R_simples - res.R_ideal) / res.R_ideal * 100.0f;
    res.comb       = buscarCombinacao(res.R_ideal);
    res.potencia   = res.IledTotal * res.IledTotal * res.R_ideal;

    // Potencia sugerida (margem 2x)
    float Pseg = res.potencia * 2.0f;
    const float pots[] = {0.125f, 0.25f, 0.5f, 1.0f, 2.0f, 5.0f};
    const char *labs[] = {"1/8 W", "1/4 W", "1/2 W", "1 W", "2 W", "5 W"};
    strcpy(res.potSugerida, "> 5 W");
    for (int i = 0; i < 6; i++) {
        if (Pseg <= pots[i]) { strcpy(res.potSugerida, labs[i]); break; }
    }

    return res;
}

// ---------- Salvar resultado ----------
void salvarResultado(ResultadoLED *r) {
    FILE *fp = fopen("resultado_led.txt", "w");
    if (!fp) return;

    fprintf(fp, "LED CALC v6.0 - Fernando Xavier\n");
    fprintf(fp, "================================\n\n");
    fprintf(fp, "Vin             : %.2f V\n",  r->Vin);
    fprintf(fp, "Vled            : %.2f V\n",  r->Vled);
    fprintf(fp, "Iled            : %.1f mA\n", r->IledmA);
    fprintf(fp, "LEDs            : %d em %s\n\n",
            r->numLeds, r->configLed == 1 ? "serie" : "paralelo");
    fprintf(fp, "Resistor ideal  : %.2f Ohms\n", r->R_ideal);
    fprintf(fp, "E12 simples     : %d Ohms (erro: %.2f%%)\n", r->R_simples, r->erroSimples);

    Combinacao *c = &r->comb;
    if (c->R3 == 0 && c->R2 == 0)
        fprintf(fp, "Melhor comb.    : %d Ohms (%s) = %.2f Ohms | Erro: %.2f%%\n",
                c->R1, c->tipo, c->Req, c->erroPct);
    else if (c->R3 == 0)
        fprintf(fp, "Melhor comb.    : %d + %d Ohms (%s) = %.2f Ohms | Erro: %.2f%%\n",
                c->R1, c->R2, c->tipo, c->Req, c->erroPct);
    else
        fprintf(fp, "Melhor comb.    : %d + %d + %d Ohms (%s) = %.2f Ohms | Erro: %.2f%%\n",
                c->R1, c->R2, c->R3, c->tipo, c->Req, c->erroPct);

    fprintf(fp, "Potencia        : %.4f W\n",   r->potencia);
    fprintf(fp, "Sugestao        : %s\n",        r->potSugerida);
    fclose(fp);
}
