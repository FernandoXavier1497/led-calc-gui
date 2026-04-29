# LED CALC v6.0
**Autor: Fernando Xavier**

---

## Estrutura do projeto

```
led_calc/
├── main.c               ← ponto de entrada, inicializa tudo
├── core/
│   ├── calc.h           ← declaracoes: structs e funcoes de calculo
│   └── calc.c           ← logica: E12, combinacoes, potencia
├── interface/
│   ├── ui.h             ← declaracoes: componentes e estado da UI
│   └── ui.c             ← interface grafica com Raylib
└── README.md
```

---

## Instalar Raylib

### Linux (Ubuntu/Debian)
```bash
sudo apt install libraylib-dev
```

### Windows
Baixe em: https://github.com/raysan5/raylib/releases
Extraia em `C:\raylib`

### macOS
```bash
brew install raylib
```

---

## Compilar

### Linux
```bash
gcc main.c core/calc.c interface/ui.c -o led_calc -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

### Windows (MinGW)
```bash
gcc main.c core/calc.c interface/ui.c -o led_calc.exe -I C:\raylib\include -L C:\raylib\lib -lraylib -lopengl32 -lgdi32 -lwinmm -lm
```

### macOS
```bash
gcc main.c core/calc.c interface/ui.c -o led_calc -lraylib -lm -framework OpenGL -framework Cocoa -framework IOKit
```

---

## Executar
```bash
# Linux / macOS
./led_calc

# Windows
led_calc.exe
```

---

## Funcionalidades
- Entrada de Vin, Vled, Iled (mA) e quantidade de LEDs
- Configuracao Serie / Paralelo
- Resistor E12 mais proximo com erro percentual
- Melhor combinacao com 2 ou 3 resistores (serie, paralelo, mista)
- Analise de potencia com margem de seguranca 2x
- Salvar resultado em `resultado_led.txt`
