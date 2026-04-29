#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ============================================================
//  LED CALC v6.0 - Implementacao da Interface
//  Autor: Fernando Xavier
// ============================================================

// -------------------------------------------------------
//  Helpers internos
// -------------------------------------------------------

static bool botaoClicado(Botao *b, Font font, Color cor, Color corHover) {
    Vector2 mouse = GetMousePosition();
    b->hover = CheckCollisionPointRec(mouse, b->rect);
    Color c = b->hover ? corHover : cor;
    DrawRectangleRounded(b->rect, 0.3f, 8, c);
    Vector2 ts = MeasureTextEx(font, b->label, 16, 0);
    DrawTextEx(font, b->label,
        (Vector2){b->rect.x + (b->rect.width  - ts.x) / 2,
                  b->rect.y + (b->rect.height - ts.y) / 2},
        16, 0, COR_FUNDO);
    return b->hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

static void desenharInput(InputBox *b, Font font) {
    DrawTextEx(font, b->label, (Vector2){b->rect.x, b->rect.y - 22}, 14, 0, COR_TEXTO_DIM);
    Color borda = b->ativo ? COR_ACENTO : COR_BORDA;
    DrawRectangleRec(b->rect, b->ativo ? COR_INPUT_ATIVO : COR_INPUT_INATI);
    DrawRectangleLinesEx(b->rect, 1.5f, borda);

    const char *txt = (b->len == 0 && !b->ativo) ? b->hint : b->texto;
    Color cor_txt   = (b->len == 0 && !b->ativo) ? COR_TEXTO_DIM : COR_TEXTO;
    DrawTextEx(font, txt, (Vector2){b->rect.x + 12, b->rect.y + b->rect.height/2 - 9}, 18, 0, cor_txt);

    // cursor piscante
    if (b->ativo && ((int)(GetTime() * 2) % 2 == 0)) {
        float cx = b->rect.x + 12 + MeasureTextEx(font, b->texto, 18, 0).x + 2;
        DrawRectangle((int)cx, (int)(b->rect.y + 10), 2, (int)(b->rect.height - 20), COR_ACENTO);
    }
}

static void processarTeclado(InputBox *b) {
    if (!b->ativo) return;
    int key = GetCharPressed();
    while (key > 0) {
        bool isNum = (key >= '0' && key <= '9');
        bool isDot = (key == '.' && strchr(b->texto, '.') == NULL);
        if ((isNum || isDot) && b->len < MAX_INPUT - 1) {
            b->texto[b->len++] = (char)key;
            b->texto[b->len]   = '\0';
        }
        key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE) && b->len > 0)
        b->texto[--b->len] = '\0';
}

static void desenharToggle(int configLed, Font font, int x, int y) {
    DrawTextEx(font, "Configuracao dos LEDs:", (Vector2){x, y - 22}, 14, 0, COR_TEXTO_DIM);

    Rectangle rS = {x,       y, 120, 38};
    Rectangle rP = {x + 130, y, 130, 38};

    Color cS = configLed == 1 ? COR_ACENTO : COR_BORDA;
    Color cP = configLed == 2 ? COR_ACENTO : COR_BORDA;

    DrawRectangleRounded(rS, 0.3f, 8, cS);
    DrawRectangleRounded(rP, 0.3f, 8, cP);

    Vector2 tS = MeasureTextEx(font, "Serie",    16, 0);
    Vector2 tP = MeasureTextEx(font, "Paralelo", 16, 0);

    DrawTextEx(font, "Serie",    (Vector2){rS.x+(rS.width-tS.x)/2, rS.y+(rS.height-tS.y)/2}, 16, 0, configLed==1 ? COR_FUNDO : COR_TEXTO);
    DrawTextEx(font, "Paralelo", (Vector2){rP.x+(rP.width-tP.x)/2, rP.y+(rP.height-tP.y)/2}, 16, 0, configLed==2 ? COR_FUNDO : COR_TEXTO);
}

static void desenharLedAnimado(int x, int y) {
    float brilho = 0.6f + 0.4f * sinf((float)GetTime() * 3.0f);
    Color ledOn = {
        (unsigned char)(0   * brilho),
        (unsigned char)(200 * brilho),
        (unsigned char)(120 * brilho),
        255
    };
    DrawCircle(x, y, 12, ledOn);
    DrawCircleLines(x, y, 16, (Color){ledOn.r, ledOn.g, ledOn.b, 80});
    DrawCircleLines(x, y, 22, (Color){ledOn.r, ledOn.g, ledOn.b, 30});
}

// -------------------------------------------------------
//  Tela de Input
// -------------------------------------------------------
static void desenharTelaInput(AppState *app, Font font) {
    int W = LARGURA_JANELA;
    int cx = W / 2;

    // Titulo
    const char *titulo = "LED CALC";
    Vector2 ts = MeasureTextEx(font, titulo, 36, 3);
    DrawTextEx(font, titulo, (Vector2){cx - ts.x/2, 28}, 36, 3, COR_ACENTO);

    const char *sub = "v6.0  |  Autor: Fernando Xavier";
    Vector2 ss = MeasureTextEx(font, sub, 13, 1);
    DrawTextEx(font, sub, (Vector2){cx - ss.x/2, 70}, 13, 1, COR_TEXTO_DIM);

    DrawRectangle(cx - 200, 90, 400, 1, COR_BORDA);

    // Inputs
    for (int i = 0; i < 4; i++)
        desenharInput(&app->inputs[i], font);

    // Toggle
    desenharToggle(app->configLed, font, 80, 455);

    // Botao calcular
    app->btnCalc.rect = (Rectangle){80, 525, 200, 46};
    strcpy(app->btnCalc.label, "CALCULAR");
    if (botaoClicado(&app->btnCalc, font, COR_BTN, COR_BTN_HOVER)) {
        float Vin    = atof(app->inputs[0].texto);
        float Vled   = atof(app->inputs[1].texto);
        float IledmA = atof(app->inputs[2].texto);
        int   nLeds  = atoi(app->inputs[3].texto);
        app->resultado = calcularLED(Vin, Vled, IledmA, nLeds, app->configLed);
        if (app->resultado.valido) {
            app->tela   = TELA_RESULTADO;
            app->fadeIn = 0.0f;
            app->salvou = false;
        }
    }

    // Mensagem de erro
    if (!app->resultado.valido && app->resultado.erro[0] != '\0')
        DrawTextEx(font, app->resultado.erro, (Vector2){80, 580}, 13, 0, COR_ERRO);

    // LED decorativo
    desenharLedAnimado(W - 50, 50);
}

// -------------------------------------------------------
//  Tela de Resultado
// -------------------------------------------------------
static void desenharTelaResultado(AppState *app, Font font) {
    app->fadeIn += GetFrameTime() * 3.0f;
    if (app->fadeIn > 1.0f) app->fadeIn = 1.0f;
    unsigned char alpha = (unsigned char)(app->fadeIn * 255);

    int W  = LARGURA_JANELA;
    int cx = W / 2;
    ResultadoLED *r = &app->resultado;
    Combinacao   *c = &r->comb;
    char buf[128];

    // Titulo
    const char *titulo = "RESULTADO";
    Vector2 ts = MeasureTextEx(font, titulo, 28, 2);
    DrawTextEx(font, titulo, (Vector2){cx - ts.x/2, 30}, 28, 2,
               (Color){COR_ACENTO.r, COR_ACENTO.g, COR_ACENTO.b, alpha});
    DrawRectangle(cx - 200, 68, 400, 2, (Color){COR_ACENTO.r, COR_ACENTO.g, COR_ACENTO.b, alpha/2});

    int py = 100;

    // Resistor ideal
    DrawTextEx(font, "Resistor Ideal", (Vector2){80, py}, 13, 0, (Color){COR_TEXTO_DIM.r, COR_TEXTO_DIM.g, COR_TEXTO_DIM.b, alpha});
    snprintf(buf, sizeof(buf), "%.2f Ohms", r->R_ideal);
    DrawTextEx(font, buf, (Vector2){80, py+18}, 26, 0, (Color){COR_TEXTO.r, COR_TEXTO.g, COR_TEXTO.b, alpha});

    // E12 simples
    DrawTextEx(font, "E12 Simples", (Vector2){320, py}, 13, 0, (Color){COR_TEXTO_DIM.r, COR_TEXTO_DIM.g, COR_TEXTO_DIM.b, alpha});
    snprintf(buf, sizeof(buf), "%d Ohms", r->R_simples);
    DrawTextEx(font, buf, (Vector2){320, py+18}, 26, 0, (Color){COR_ACENTO2.r, COR_ACENTO2.g, COR_ACENTO2.b, alpha});
    snprintf(buf, sizeof(buf), "Erro: %.2f%%", r->erroSimples);
    DrawTextEx(font, buf, (Vector2){320, py+48}, 13, 0, (Color){COR_TEXTO_DIM.r, COR_TEXTO_DIM.g, COR_TEXTO_DIM.b, alpha});

    py += 90;
    DrawRectangle(40, py, W-80, 1, (Color){COR_BORDA.r, COR_BORDA.g, COR_BORDA.b, alpha});
    py += 16;

    // Melhor combinacao
    DrawTextEx(font, "Melhor Combinacao", (Vector2){80, py}, 13, 0, (Color){COR_TEXTO_DIM.r, COR_TEXTO_DIM.g, COR_TEXTO_DIM.b, alpha});
    py += 18;
    DrawTextEx(font, c->tipo, (Vector2){80, py}, 20, 0, (Color){COR_ACENTO.r, COR_ACENTO.g, COR_ACENTO.b, alpha});
    py += 28;

    if (c->R3 == 0 && c->R2 == 0) snprintf(buf, sizeof(buf), "%d Ohms", c->R1);
    else if (c->R3 == 0)          snprintf(buf, sizeof(buf), "%d  +  %d Ohms", c->R1, c->R2);
    else                           snprintf(buf, sizeof(buf), "%d  +  %d  +  %d Ohms", c->R1, c->R2, c->R3);
    DrawTextEx(font, buf, (Vector2){80, py}, 22, 0, (Color){COR_TEXTO.r, COR_TEXTO.g, COR_TEXTO.b, alpha});
    py += 28;

    snprintf(buf, sizeof(buf), "Req = %.2f Ohms   |   Erro: %.2f%%", c->Req, c->erroPct);
    DrawTextEx(font, buf, (Vector2){80, py}, 14, 0, (Color){COR_TEXTO_DIM.r, COR_TEXTO_DIM.g, COR_TEXTO_DIM.b, alpha});

    py += 50;
    DrawRectangle(40, py, W-80, 1, (Color){COR_BORDA.r, COR_BORDA.g, COR_BORDA.b, alpha});
    py += 16;

    // Potencia
    DrawTextEx(font, "Analise de Potencia", (Vector2){80, py}, 13, 0, (Color){COR_TEXTO_DIM.r, COR_TEXTO_DIM.g, COR_TEXTO_DIM.b, alpha});
    py += 18;
    snprintf(buf, sizeof(buf), "%.4f W  dissipados", r->potencia);
    DrawTextEx(font, buf, (Vector2){80, py}, 22, 0, (Color){COR_TEXTO.r, COR_TEXTO.g, COR_TEXTO.b, alpha});
    py += 28;
    snprintf(buf, sizeof(buf), "Use resistor de pelo menos  %s  (margem 2x)", r->potSugerida);
    DrawTextEx(font, buf, (Vector2){80, py}, 15, 0, (Color){COR_ACENTO2.r, COR_ACENTO2.g, COR_ACENTO2.b, alpha});

    py += 60;

    // Botoes
    app->btnVoltar.rect = (Rectangle){80,  py, 150, 42};
    app->btnSalvar.rect = (Rectangle){250, py, 180, 42};
    strcpy(app->btnVoltar.label, "< Voltar");
    strcpy(app->btnSalvar.label, app->salvou ? "Salvo!" : "Salvar .txt");

    if (botaoClicado(&app->btnVoltar, font, COR_BORDA, COR_BORDA))
        app->tela = TELA_INPUT;

    Color corSalvar = app->salvou ? (Color){40, 100, 60, 255} : COR_BTN;
    if (botaoClicado(&app->btnSalvar, font, corSalvar, COR_BTN_HOVER)) {
        salvarResultado(&app->resultado);
        app->salvou = true;
    }
}

// -------------------------------------------------------
//  Funcoes publicas
// -------------------------------------------------------
void ui_init(AppState *app) {
    memset(app, 0, sizeof(AppState));
    app->configLed = 1;
    app->tela      = TELA_INPUT;

    float startY = 115;
    const char *labels[] = {
        "Tensao de alimentacao  Vin (V)",
        "Tensao do LED  Vled (V)",
        "Corrente do LED  Iled (mA)",
        "Quantidade de LEDs"
    };
    const char *hints[] = {"ex: 12", "ex: 2.1", "ex: 20", "ex: 1"};

    for (int i = 0; i < 4; i++) {
        app->inputs[i].rect = (Rectangle){80, startY + i * 80, 360, 44};
        strcpy(app->inputs[i].label, labels[i]);
        strcpy(app->inputs[i].hint,  hints[i]);
    }
}

void ui_atualizar(AppState *app, Font font) {
    (void)font;
    if (app->tela == TELA_INPUT) {
        // Clique nos inputs
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            for (int i = 0; i < 4; i++)
                app->inputs[i].ativo = CheckCollisionPointRec(m, app->inputs[i].rect);

            // Toggle serie/paralelo
            if (CheckCollisionPointRec(m, (Rectangle){80, 455, 120, 38}))  app->configLed = 1;
            if (CheckCollisionPointRec(m, (Rectangle){210, 455, 130, 38})) app->configLed = 2;
        }
        for (int i = 0; i < 4; i++)
            processarTeclado(&app->inputs[i]);
    }
}

void ui_desenhar(AppState *app, Font font) {
    // Painel de fundo
    DrawRectangleRounded((Rectangle){20, 10, LARGURA_JANELA-40, ALTURA_JANELA-20}, 0.05f, 8, COR_PAINEL);
    DrawRectangleRoundedLines((Rectangle){20, 10, LARGURA_JANELA-40, ALTURA_JANELA-20}, 0.05f, 8, COR_BORDA);

    if (app->tela == TELA_INPUT)
        desenharTelaInput(app, font);
    else
        desenharTelaResultado(app, font);
}
