#ifndef UI_H
#define UI_H

// ============================================================
//  LED CALC v6.0 - Camada de Interface (Raylib)
//  Autor: Fernando Xavier
// ============================================================

#include "raylib.h"
#include "calc.h"

// ---------- Constantes de layout ----------
#define LARGURA_JANELA  620
#define ALTURA_JANELA   700
#define MAX_INPUT       16

#define COR_FUNDO       (Color){8,   12,  22,  255}
#define COR_PAINEL      (Color){14,  20,  38,  255}
#define COR_BORDA       (Color){30,  45,  80,  255}
#define COR_ACENTO      (Color){0,   140, 255, 255}
#define COR_ACENTO2     (Color){255, 180, 0,   255}
#define COR_TEXTO       (Color){210, 225, 245, 255}
#define COR_TEXTO_DIM   (Color){80,  100, 140, 255}
#define COR_ERRO        (Color){255, 70,  70,  255}
#define COR_INPUT_ATIVO (Color){18,  28,  55,  255}
#define COR_INPUT_INATI (Color){12,  18,  32,  255}
#define COR_BTN         (Color){0,   100, 200, 255}
#define COR_BTN_HOVER   (Color){0,   140, 255, 255}

// ---------- Componentes ----------
typedef struct {
    Rectangle rect;
    char      texto[MAX_INPUT];
    int       len;
    bool      ativo;
    char      label[48];
    char      hint[32];
} InputBox;

typedef struct {
    Rectangle rect;
    char      label[32];
    bool      hover;
} Botao;

// ---------- Telas ----------
typedef enum { TELA_INPUT, TELA_RESULTADO } Tela;

// ---------- Estado global da aplicacao ----------
typedef struct {
    InputBox    inputs[4];   // Vin, Vled, Iled(mA), numLeds
    int         configLed;   // 1=serie, 2=paralelo
    Botao       btnCalc;
    Botao       btnVoltar;
    Botao       btnSalvar;
    Tela        tela;
    ResultadoLED resultado;
    float       fadeIn;
    bool        salvou;
} AppState;

// ---------- Funcoes publicas ----------
void ui_init(AppState *app);
void ui_atualizar(AppState *app, Font font);
void ui_desenhar(AppState *app, Font font);

#endif // UI_H
