# LED Calc v5.0

Um programa em **C** para auxiliar no cálculo de resistores para LEDs.  
O sistema sugere valores de resistores **comerciais (E12)**, combinações em série/paralelo e analisa a dissipação de potência.

> 🔧 O projeto está em desenvolvimento e futuramente contará com uma **interface gráfica (GUI)** utilizando a biblioteca **GTK**.

---

## ✨ Funcionalidades
- Cálculo do resistor ideal para LEDs a partir de **Vin**, **Vled** e **Iled**.
- Sugestão do resistor **comercial mais próximo** (E12).
- Sugestão de **combinações** de resistores (série e paralelo, com 2 ou 3 resistores).
- Análise da **potência dissipada** e recomendação da potência mínima do resistor.

---

## 📦 Como compilar

No Linux (ou WSL):

```bash
gcc led_calc.c -o led_calc -lm
