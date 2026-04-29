#ifndef CALC_H
#define CALC_H

// ============================================================
//  LED CALC v6.0 - Camada de Calculo (core)
//  Autor: Fernando Xavier
// ============================================================

// ---------- Tabela E12 ----------
void montarTabela();

// ---------- Resistor comercial mais proximo ----------
int resistorComercial(float R);

// ---------- Resultado de combinacao ----------
typedef struct {
    int   R1, R2, R3;
    float Req;
    float erroPct;
    char  tipo[40];
} Combinacao;

// ---------- Busca melhor combinacao ----------
Combinacao buscarCombinacao(float R);

// ---------- Resultado completo do calculo LED ----------
typedef struct {
    float Vin;
    float Vled;
    float IledmA;
    int   numLeds;
    int   configLed;     // 1 = serie, 2 = paralelo

    float VledTotal;
    float IledTotal;
    float R_ideal;
    int   R_simples;
    float erroSimples;
    Combinacao comb;
    float potencia;
    char  potSugerida[20];

    int   valido;        // 0 = erro de entrada
    char  erro[100];
} ResultadoLED;

// ---------- Executa o calculo completo ----------
ResultadoLED calcularLED(float Vin, float Vled, float IledmA, int numLeds, int configLed);

// ---------- Salva resultado em arquivo ----------
void salvarResultado(ResultadoLED *r);

#endif // CALC_H
