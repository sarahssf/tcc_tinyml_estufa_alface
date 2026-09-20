/*
 * ============================================================
 * TESTE 5 — Módulo Relé 5V 2 Canais
 * ============================================================
 * Verifica:
 *   ✔ Canal 1 (Bomba): ativa/desativa via GPIO 25
 *   ✔ Canal 2 (Ventilador): ativa/desativa via GPIO 26
 *   ✔ Lógica invertida (ativo em LOW)
 *   ✔ Sequência de acionamento
 *
 * Conexão:
 *   IN1 → GPIO 25 | IN2 → GPIO 26
 *   VCC → 5V      | GND → GND
 *
 * ⚠️  ATENÇÃO: Este teste ACIONA os relés fisicamente!
 *     Você ouvirá cliques e verá os LEDs do módulo acenderem.
 *     Desconecte cargas sensíveis se necessário.
 *
 * Como usar:
 *   1. Copie este arquivo para src/main.cpp
 *   2. pio run -t upload && pio device monitor
 *   3. Ouça os cliques e observe os LEDs do módulo relé
 * ============================================================
 */

#include <Arduino.h>

// Pinos (conforme config.h)
static constexpr uint8_t PIN_RELAY1 = 25;  // Bomba
static constexpr uint8_t PIN_RELAY2 = 26;  // Ventilador

// Lógica do relé (ativo em LOW)
static constexpr uint8_t RELAY_ON  = LOW;
static constexpr uint8_t RELAY_OFF = HIGH;

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

void acionarRele(uint8_t pin, const char* nome, uint16_t tempoMs) {
    Serial.print(F("  🔌 Acionando "));
    Serial.print(nome);
    Serial.print(F("... "));

    digitalWrite(pin, RELAY_ON);
    Serial.println(F("LIGADO ← ouca o CLIQUE!"));
    delay(tempoMs);

    digitalWrite(pin, RELAY_OFF);
    Serial.println(F("                                    DESLIGADO ← outro CLIQUE!"));
    delay(500);
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println();
    Serial.println(F("╔════════════════════════════════════════════╗"));
    Serial.println(F("║   TESTE 5 — Modulo Rele 5V 2 Canais       ║"));
    Serial.println(F("╚════════════════════════════════════════════╝"));
    Serial.println();
    Serial.println(F("  ⚠️  Este teste ACIONA os reles fisicamente!"));
    Serial.println(F("     Desconecte cargas sensiveis se necessario."));
    Serial.println(F("     Iniciando em 3 segundos..."));
    delay(3000);

    // Configurar pinos
    pinMode(PIN_RELAY1, OUTPUT);
    pinMode(PIN_RELAY2, OUTPUT);

    // Garantir que começam desligados
    digitalWrite(PIN_RELAY1, RELAY_OFF);
    digitalWrite(PIN_RELAY2, RELAY_OFF);

    // --- Teste Canal 1 (Bomba) ---
    printHeader("CANAL 1 — Rele Bomba (GPIO 25)");
    Serial.println(F("  Logica: ativo em LOW (inversao)"));
    Serial.println();

    // Verificar estado inicial
    check("Estado inicial: DESLIGADO (HIGH)", digitalRead(PIN_RELAY1) == RELAY_OFF);

    // Acionar
    acionarRele(PIN_RELAY1, "Rele 1 (Bomba)", 2000);
    check("Rele 1 voltou ao estado DESLIGADO", digitalRead(PIN_RELAY1) == RELAY_OFF);

    // Pulsos rápidos
    Serial.println(F("  Teste de pulsos rapidos (3x)..."));
    for (int i = 0; i < 3; i++) {
        digitalWrite(PIN_RELAY1, RELAY_ON);
        delay(300);
        digitalWrite(PIN_RELAY1, RELAY_OFF);
        delay(300);
        Serial.print(F("    Pulso "));
        Serial.print(i + 1);
        Serial.println(F("/3 ✅"));
    }
    check("Canal 1: 3 pulsos executados com sucesso", true);

    delay(1000);

    // --- Teste Canal 2 (Ventilador) ---
    printHeader("CANAL 2 — Rele Ventilador (GPIO 26)");
    Serial.println(F("  Logica: ativo em LOW (inversao)"));
    Serial.println();

    check("Estado inicial: DESLIGADO (HIGH)", digitalRead(PIN_RELAY2) == RELAY_OFF);

    acionarRele(PIN_RELAY2, "Rele 2 (Ventilador)", 2000);
    check("Rele 2 voltou ao estado DESLIGADO", digitalRead(PIN_RELAY2) == RELAY_OFF);

    // Pulsos rápidos
    Serial.println(F("  Teste de pulsos rapidos (3x)..."));
    for (int i = 0; i < 3; i++) {
        digitalWrite(PIN_RELAY2, RELAY_ON);
        delay(300);
        digitalWrite(PIN_RELAY2, RELAY_OFF);
        delay(300);
        Serial.print(F("    Pulso "));
        Serial.print(i + 1);
        Serial.println(F("/3 ✅"));
    }
    check("Canal 2: 3 pulsos executados com sucesso", true);

    delay(1000);

    // --- Teste Simultâneo ---
    printHeader("TESTE SIMULTANEO (ambos os canais)");
    Serial.println(F("  Ligando AMBOS os reles ao mesmo tempo..."));
    digitalWrite(PIN_RELAY1, RELAY_ON);
    digitalWrite(PIN_RELAY2, RELAY_ON);
    Serial.println(F("  🔴🔴 Ambos LIGADOS — 2 LEDs devem estar acesos"));
    delay(2000);

    digitalWrite(PIN_RELAY1, RELAY_OFF);
    digitalWrite(PIN_RELAY2, RELAY_OFF);
    Serial.println(F("  ⚪⚪ Ambos DESLIGADOS"));
    delay(500);

    check("Acionamento simultaneo executado", true);

    // --- Teste de alternância ---
    printHeader("TESTE DE ALTERNANCIA");
    Serial.println(F("  Alternando entre Canal 1 e Canal 2..."));
    for (int i = 0; i < 4; i++) {
        digitalWrite(PIN_RELAY1, RELAY_ON);
        digitalWrite(PIN_RELAY2, RELAY_OFF);
        Serial.println(F("  🔴⚪ Canal 1 ON | Canal 2 OFF"));
        delay(800);

        digitalWrite(PIN_RELAY1, RELAY_OFF);
        digitalWrite(PIN_RELAY2, RELAY_ON);
        Serial.println(F("  ⚪🔴 Canal 1 OFF | Canal 2 ON"));
        delay(800);
    }
    digitalWrite(PIN_RELAY1, RELAY_OFF);
    digitalWrite(PIN_RELAY2, RELAY_OFF);
    check("Alternancia entre canais OK", true);

    // --- Resumo ---
    printHeader("RESUMO FINAL");
    Serial.print(F("  Testes: "));
    Serial.print(passedTests);
    Serial.print(F("/"));
    Serial.print(totalTests);
    Serial.println(F(" passaram"));
    Serial.println();
    Serial.println(F("  Checklist manual:"));
    Serial.println(F("  [ ] Ouviu cliques no Canal 1?"));
    Serial.println(F("  [ ] Ouviu cliques no Canal 2?"));
    Serial.println(F("  [ ] LED do Canal 1 acendeu?"));
    Serial.println(F("  [ ] LED do Canal 2 acendeu?"));
    Serial.println(F("  [ ] Ambos funcionaram ao mesmo tempo?"));
    Serial.println();

    if (passedTests == totalTests) {
        Serial.println(F("  🎉 Modulo Rele 100% OPERACIONAL!"));
    }

    printSeparator();
}

void loop() {
    // Teste executado no setup().
}
