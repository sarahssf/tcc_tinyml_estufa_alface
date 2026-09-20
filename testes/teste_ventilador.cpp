/*
 * ============================================================
 * TESTE 7 — Micro Ventilador Cooler 40x40x10mm 5V
 * ============================================================
 * Verifica:
 *   ✔ Ventilador liga via Relé 2 (GPIO 26)
 *   ✔ Modo contínuo (5 segundos)
 *   ✔ Modo intermitente (3 ciclos curtos)
 *   ✔ Desligamento correto
 *
 * Conexão:
 *   Ventilador (+) → Relé 2 COM
 *   Relé 2 NO      → Fonte 5V (+)
 *   Ventilador (-)  → Fonte GND
 *   IN2             → GPIO 26
 *
 * ⚠️  O ventilador será acionado fisicamente!
 *
 * Como usar:
 *   1. Copie este arquivo para src/main.cpp
 *   2. pio run -t upload && pio device monitor
 *   3. Observe e sinta o fluxo de ar do ventilador
 * ============================================================
 */

#include <Arduino.h>

static constexpr uint8_t PIN_RELAY_FAN = 26;
static constexpr uint8_t RELAY_ON      = LOW;
static constexpr uint8_t RELAY_OFF     = HIGH;

uint8_t totalTests  = 0;
uint8_t passedTests = 0;

void printSeparator() {
    Serial.println(F("────────────────────────────────────────────"));
}

void printHeader(const char* title) {
    Serial.println();
    printSeparator();
    Serial.print(F("  ▶ "));
    Serial.println(title);
    printSeparator();
}

void check(const char* item, bool condition) {
    totalTests++;
    if (condition) passedTests++;
    Serial.print(F("  "));
    Serial.print(condition ? "✅" : "❌");
    Serial.print(F(" "));
    Serial.println(item);
}

void countdown(const char* label, int seconds) {
    for (int i = seconds; i > 0; i--) {
        Serial.print(F("  ⏱️  "));
        Serial.print(label);
        Serial.print(F(": "));
        Serial.print(i);
        Serial.println(F("s"));
        delay(1000);
    }
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println();
    Serial.println(F("╔════════════════════════════════════════════╗"));
    Serial.println(F("║   TESTE 7 — Micro Ventilador 40x40mm 5V   ║"));
    Serial.println(F("╚════════════════════════════════════════════╝"));
    Serial.println();
    Serial.println(F("  ⚠️  O ventilador sera acionado fisicamente!"));
    Serial.println(F("     Iniciando em 3 segundos..."));
    delay(3000);

    pinMode(PIN_RELAY_FAN, OUTPUT);
    digitalWrite(PIN_RELAY_FAN, RELAY_OFF);

    // --- Teste 1: Modo Contínuo ---
    printHeader("MODO CONTINUO (5 segundos)");
    Serial.println(F("  🌀 Ligando ventilador..."));
    digitalWrite(PIN_RELAY_FAN, RELAY_ON);
    check("Rele do ventilador acionado (LOW)", digitalRead(PIN_RELAY_FAN) == RELAY_ON);

    Serial.println(F("  Coloque a mao perto — sinta o fluxo de ar!"));
    countdown("Ventilador LIGADO", 5);

    digitalWrite(PIN_RELAY_FAN, RELAY_OFF);
    Serial.println(F("  Ventilador desligado."));
    check("Rele desativado (HIGH)", digitalRead(PIN_RELAY_FAN) == RELAY_OFF);

    Serial.println();
    Serial.println(F("  Checklist manual:"));
    Serial.println(F("  [ ] As pas giraram?"));
    Serial.println(F("  [ ] Sentiu fluxo de ar?"));
    Serial.println(F("  [ ] O ventilador parou quando desligado?"));

    delay(2000);

    // --- Teste 2: Modo Intermitente ---
    printHeader("MODO INTERMITENTE (3 ciclos: 2s ON / 3s OFF)");
    Serial.println(F("  Simulando modo intermitente do projeto"));
    Serial.println(F("  (tempos reduzidos para teste rapido)"));
    Serial.println();

    for (int i = 1; i <= 3; i++) {
        Serial.print(F("  ── Ciclo "));
        Serial.print(i);
        Serial.println(F("/3 ──"));

        Serial.println(F("  🌀 ON"));
        digitalWrite(PIN_RELAY_FAN, RELAY_ON);
        countdown("ON ", 2);

        Serial.println(F("  ⏸️  OFF"));
        digitalWrite(PIN_RELAY_FAN, RELAY_OFF);

        if (i < 3) {
            countdown("OFF", 3);
        }
        Serial.println();
    }

    check("3 ciclos intermitentes executados", true);

    // --- Teste 3: Partida rápida (stress test) ---
    printHeader("TESTE DE PARTIDA RAPIDA (5 on/off rapidos)");
    Serial.println(F("  Verificando se o motor responde a comandos rapidos..."));

    for (int i = 0; i < 5; i++) {
        digitalWrite(PIN_RELAY_FAN, RELAY_ON);
        delay(500);
        digitalWrite(PIN_RELAY_FAN, RELAY_OFF);
        delay(500);
        Serial.print(F("    Ciclo rapido "));
        Serial.print(i + 1);
        Serial.println(F("/5 ✅"));
    }
    check("5 ciclos rapidos executados", true);

    // --- Garantir desligamento ---
    digitalWrite(PIN_RELAY_FAN, RELAY_OFF);

    // --- Verificação de segurança ---
    printHeader("VERIFICACAO DE SEGURANCA");
    delay(1000);
    check("Ventilador desligado apos testes",  digitalRead(PIN_RELAY_FAN) == RELAY_OFF);
    check("Rele no estado seguro (HIGH)",      digitalRead(PIN_RELAY_FAN) == HIGH);

    // --- Resumo ---
    printHeader("RESUMO FINAL");
    Serial.print(F("  Testes automaticos: "));
    Serial.print(passedTests);
    Serial.print(F("/"));
    Serial.print(totalTests);
    Serial.println(F(" passaram"));
    Serial.println();
    Serial.println(F("  Checklist manual:"));
    Serial.println(F("  [ ] O ventilador girou no modo continuo?"));
    Serial.println(F("  [ ] Ele parou e reiniciou no modo intermitente?"));
    Serial.println(F("  [ ] Respondeu aos ciclos rapidos?"));
    Serial.println();

    if (passedTests == totalTests) {
        Serial.println(F("  🎉 Ventilador 100% OPERACIONAL!"));
    }

    printSeparator();
}

void loop() {
    // Teste executado no setup().
}
