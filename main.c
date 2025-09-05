#include <stdio.h>
#include <stdlib.h>
#include <math.h>
const int valoresE12[] = {10, 12, 15, 18, 22, 27, 33, 39, 47, 56, 68, 82};
const int numValores = 12;
int resistorComercial(float R) {
    if (R <= 0) return 0;
    
    int exp10 = (int)floor(log10(R));
    float potencia10 = pow(10, exp10);
    
    float valorNormalizado = R / potencia10;
    
    if (valorNormalizado < valoresE12[0]) {
        exp10--;
        potencia10 = pow(10, exp10);
        valorNormalizado = R / potencia10;
    }
    
    int escolhido = valoresE12[numValores - 1]; 
    for (int i = 0; i < numValores; i++) {
        if (valorNormalizado <= valoresE12[i]) {
            escolhido = valoresE12[i];
            break;
        }
    }
    
    return escolhido * potencia10;
}
void sugestaoCombinacao(float R) {
    float melhorDiff = 1e9;
    int melhorR1 = 0, melhorR2 = 0, melhorR3 = 0;
    float melhorReq = 0.0;
    char tipo[20] = "";
    int numDecadas = 4;
    int numResistoresTotais = numValores * numDecadas;
    int resistoresComerciais[numResistoresTotais];
    
    for (int d = 0; d < numDecadas; d++) {
        int potencia10 = pow(10, d);
        for (int i = 0; i < numValores; i++) {
            resistoresComerciais[d * numValores + i] = valoresE12[i] * potencia10;
        }
    }
    
    for (int i = 0; i < numResistoresTotais; i++) {
        int R1 = resistoresComerciais[i];
        for (int j = 0; j < numResistoresTotais; j++) {
            int R2 = resistoresComerciais[j];
            float Rs = R1 + R2;
            if (fabs(R - Rs) < melhorDiff) {
                melhorDiff = fabs(R - Rs);
                melhorR1 = R1; melhorR2 = R2; melhorR3 = 0;
                melhorReq = Rs;
                sprintf(tipo, "Serie (2)");
            }
            float Rp = (float)(R1 * R2) / (R1 + R2);
            if (fabs(R - Rp) < melhorDiff) {
                melhorDiff = fabs(R - Rp);
                melhorR1 = R1; melhorR2 = R2; melhorR3 = 0;
                melhorReq = Rp;
                sprintf(tipo, "Paralelo (2)");
            }
        }
        
        for (int j = 0; j < numResistoresTotais; j++) {
            for (int k = 0; k < numResistoresTotais; k++) {
                int R2 = resistoresComerciais[j];
                int R3 = resistoresComerciais[k];
                float Rs3 = (float)R1 + R2 + R3;
                if (fabs(R - Rs3) < melhorDiff) {
                    melhorDiff = fabs(R - Rs3);
                    melhorR1 = R1; melhorR2 = R2; melhorR3 = R3;
                    melhorReq = Rs3;
                    sprintf(tipo, "Serie (3)");
                }
                float Rp3 = 1.0 / (1.0/R1 + 1.0/R2 + 1.0/R3);
                if (fabs(R - Rp3) < melhorDiff) {
                    melhorDiff = fabs(R - Rp3);
                    melhorR1 = R1; melhorR2 = R2; melhorR3 = R3;
                    melhorReq = Rp3;
                    sprintf(tipo, "Paralelo (3)");
                }
            }
        }
    }
    printf("Melhor combinacao encontrada: ");
    if (melhorR3 == 0) {
        printf("%d Ohms e %d Ohms (%s) = %.2f Ohms\n",
               melhorR1, melhorR2, tipo, melhorReq);
    } else {
        printf("%d Ohms, %d Ohms e %d Ohms (%s) = %.2f Ohms\n",
               melhorR1, melhorR2, melhorR3, tipo, melhorReq);
    }
}
void potenciaResistor(float I, float R) {
    float P = I * I * R;
    float Psegura = P * 2; 
    float comerciais[] = {0.125, 0.25, 0.5, 1, 2, 5};
    const char *labels[] = {"1/8 W", "1/4 W", "1/2 W", "1 W", "2 W", "5 W"};
    printf("Potencia dissipada no resistor: %.4f W\n", P);
    for (int i = 0; i < 6; i++) {
        if (Psegura <= comerciais[i]) {
            printf("Sugestao de potencia: usar resistor de pelo menos %s\n", labels[i]);
            return;
        }
    }
    printf("Sugestao de potencia: usar resistor > 5 W\n");
}
int main () {
    float Vin, Vled, Iled, R;
    char continuar;
    printf("LED CALC v5.0\n\n");
    printf("Autor: Fernando Xavier\n\n");
    do {
        printf("\n--- Novo Calculo ---\n\n");
        printf("Digite o valor de Vin (em Volts): ");
        scanf("%f", &Vin);
        
        printf("Digite o valor de Vled (em Volts): ");
        scanf("%f", &Vled);
        
        printf("Digite o valor de Iled (em Amperes): ");
        scanf("%f", &Iled);
        if (Vin <= Vled) {
            printf("\nErro: Vin deve ser maior que Vled!\n");
        } else {
            R = (Vin - Vled) / Iled;
            printf("\nVin = %.2f V\n", Vin);
            printf("Vled = %.2f V\n", Vled);
            printf("Iled = %.3f A\n\n", Iled);
            printf("Resistor calculado = %.2f Ohms\n", R);
            int Rcom = resistorComercial(R);
            if (Rcom > 0) {
                printf("Resistor comercial sugerido (E12) = %d Ohms\n", Rcom);
            }
            
            sugestaoCombinacao(R);
            
            printf("\n--- Analise de Potencia ---\n");
            potenciaResistor(Iled, R);
        }
        printf("\nDeseja realizar outro calculo? (s/n): ");
        
        while (getchar() != '\n');
        scanf(" %c", &continuar);
    } while (continuar == 's' || continuar == 'S');
    printf("\nPrograma finalizado.\n");
    
    return 0;
}
