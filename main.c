#include "raylib.h"
#include "calc.h"
#include "ui.h"

// ============================================================
//  LED CALC v6.0 - Ponto de entrada
//  Autor: Fernando Xavier
// ============================================================

int main(void) {
    montarTabela();

    InitWindow(LARGURA_JANELA, ALTURA_JANELA, "LED CALC v6.0 - Fernando Xavier");
    SetTargetFPS(60);

    Font font = LoadFontEx("Inter_18pt-Regular.ttf", 32, 0, 250);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

    AppState app;
    ui_init(&app);

    while (!WindowShouldClose()) {
        ui_atualizar(&app, font);

        BeginDrawing();
        ClearBackground(COR_FUNDO);
        ui_desenhar(&app, font);
        EndDrawing();
    }

    UnloadFont(font);
    CloseWindow();
    return 0;
}