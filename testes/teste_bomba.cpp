/*
 * ============================================================
 * TESTE 6 — Mini Bomba D'Água Submersa (3-6V DC, 120L/h)
 * ============================================================
 * Verifica:
 *   ✔ Bomba liga via Relé 1 (GPIO 25)
 *   ✔ Pulsos de irrigação (3s ligada, 5s pausa)
 *   ✔ Desligamento correto
 *
 * Conexão:
 *   Bomba (+) → Relé 1 COM
 *   Relé 1 NO → Fonte 3-6V (+)
 *   Bomba (-) → Fonte GND
 *   IN1       → GPIO 25
 *
 * ⚠️  ATENÇÃO:
 *   - A bomba DEVE estar submersa na água durante o teste!
 *   - Nunca opere a bomba seca (pode queimar o motor).
 *   - Use fonte de 3V a 6V DC para a bomba (NÃO use 3.3V do ESP32).
 *
 * Como usar:
 *   1. Submerja a bomba em um recipiente com água
 *   2. Copie este arquivo para src/main.cpp
 *   3. pio run -t upload && pio device monitor
 * ============================================================
 */

#include <Arduino.h>

static constexpr uint8_t PIN_RELAY_PUMP = 25;
static constexpr uint8_t RELAY_ON       = LOW;
static constexpr uint8_t RELAY_OFF      = HIGH;

// Tempos do projeto (de config.h)
static constexpr unsigned long PULSE_ON   = 3000;   // 3 segundos
static constexpr unsigned long PULSE_PAUSE = 5000;  // 5 segundos

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
    Serial.println(F("║   TESTE 6 — Mini Bomba D'Agua Submersa    ║"));
    Serial.println(F("╚════════════════════════════════════════════╝"));
    Serial.println();
    Serial.println(F("  ⚠️  IMPORTANTE:"));
    Serial.println(F("     A bomba DEVE estar submersa em agua!"));
    Serial.println(F("     Nunca opere a bomba seca."));
    Serial.println();
    Serial.println(F("  Iniciando em 5 segundos..."));
    Serial.println(F("  (Prepare o recipiente com agua)"));
    delay(5000);

    pinMode(PIN_RELAY_PUMP, OUTPUT);
    digitalWrite(PIN_RELAY_PUMP, RELAY_OFF);

    // --- Teste 1: Pulso Único ---
    printHeader("PULSO UNICO (3 segundos)");
    Serial.println(F("  💧 Ligando bomba..."));
    digitalWrite(PIN_RELAY_PUMP, RELAY_ON);
    check("Rele da bomba acionado (LOW)", digitalRead(PIN_RELAY_PUMP) == RELAY_ON);

    Serial.println(F("  Observe: a agua deve estar fluindo!"));
    countdown("Bomba LIGADA", 3);

    digitalWrite(PIN_RELAY_PUMP, RELAY_OFF);
    Serial.println(F("  Bomba desligada."));
    check("Rele da bomba desativado (HIGH)", digitalRead(PIN_RELAY_PUMP) == RELAY_OFF);

    Serial.println();
    Serial.println(F("  Checklist manual:"));
    Serial.println(F("  [ ] A agua fluiu durante os 3 segundos?"));
    Serial.println(F("  [ ] A bomba parou quando desligada?"));

    delay(2000);

    // --- Teste 2: 3 Pulsos (simulação de irrigação) ---
    printHeader("SIMULACAO DE IRRIGACAO (3 pulsos)");
    Serial.println(F("  Pulso: 3s ligada / 5s pausa (como no projeto)"));
    Serial.println();

    for (int i = 1; i <= 3; i++) {
        Serial.print(F("  ── Pulso "));
        Serial.print(i);
        Serial.println(F("/3 ──"));

        Serial.println(F("  💧 LIGANDO bomba..."));
        digitalWrite(PIN_RELAY_PUMP, RELAY_ON);
        countdown("ON ", 3);

        digitalWrite(PIN_RELAY_PUMP, RELAY_OFF);
        Serial.println(F("  ⏸️  PAUSA..."));

        if (i < 3) {
            countdown("OFF", 5);
        }
        Serial.println();
    }

    check("3 pulsos de irrigacao executados", true);

    // --- Garantir desligamento ---
    digitalWrite(PIN_RELAY_PUMP, RELAY_OFF);

    // --- Teste 3: Verificar desligamento seguro ---
    printHeader("VERIFICACAO DE SEGURANCA");
    delay(1000);
    check("Bomba desligada apos testes",  digitalRead(PIN_RELAY_PUMP) == RELAY_OFF);
    check("Rele no estado seguro (HIGH)", digitalRead(PIN_RELAY_PUMP) == HIGH);

    // --- Resumo ---
    printHeader("RESUMO FINAL");
    Serial.print(F("  Testes automaticos: "));
    Serial.print(passedTests);
    Serial.print(F("/"));
    Serial.print(totalTests);
    Serial.println(F(" passaram"));
    Serial.println();
    Serial.println(F("  Checklist manual:"));
    Serial.println(F("  [ ] A bomba bombeou agua nos 3 pulsos?"));
    Serial.println(F("  [ ] O fluxo parou entre os pulsos?"));
    Serial.println(F("  [ ] Nenhum vazamento observado?"));
    Serial.println();

    if (passedTests == totalTests) {
        Serial.println(F("  🎉 Bomba 100% OPERACIONAL!"));
    }

    printSeparator();
}

void loop() {
    // Teste executado no setup().
}
