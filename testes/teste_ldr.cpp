/*
 * ============================================================
 * TESTE 4 — Sensor de Luz LDR (Digital/Analógico)
 * ============================================================
 * Verifica:
 *   ✔ Leitura analógica no pino ADC (GPIO 35)
 *   ✔ Valor dentro do range ADC (0-4095)
 *   ✔ Resposta à mudança de luminosidade
 *   ✔ Estabilidade de leitura
 *
 * Conexão:
 *   A0   → GPIO 35 (ADC1_CH7)
 *   VCC  → 3.3V a 5V
 *   GND  → GND
 *
 * Calibração:
 *   Valor ALTO = muita luz (divisor de tensão)
 *   Valor BAIXO = pouca luz / escuro
 *
 * Como usar:
 *   1. Copie este arquivo para src/main.cpp
 *   2. pio run -t upload && pio device monitor
 *   3. Cubra e descubra o sensor para testar a resposta
 * ============================================================
 */

#include <Arduino.h>

static constexpr uint8_t PIN_LDR = 35;
static constexpr uint16_t LDR_EXTREME = 3800;  // Threshold do projeto

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

void printBar(uint16_t val) {
    int barLen = map(val, 0, 4095, 0, 30);
    Serial.print(F("  ☀️ ["));
    for (int i = 0; i < 30; i++) {
        Serial.print(i < barLen ? "█" : "░");
    }
    Serial.print(F("] "));
    Serial.print(val);

    // Emoji indicativo
    if (val > LDR_EXTREME) {
        Serial.println(F("  🔆 EXTREMO"));
    } else if (val > 2500) {
        Serial.println(F("  ☀️  ALTA"));
    } else if (val > 1000) {
        Serial.println(F("  🌤️ MEDIA"));
    } else if (val > 300) {
        Serial.println(F("  🌥️ BAIXA"));
    } else {
        Serial.println(F("  🌑 ESCURO"));
    }
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println();
    Serial.println(F("╔════════════════════════════════════════════╗"));
    Serial.println(F("║   TESTE 4 — Sensor de Luz LDR             ║"));
    Serial.println(F("╚════════════════════════════════════════════╝"));

    analogReadResolution(12);

    // --- Leitura Inicial ---
    printHeader("LEITURA INICIAL");
    uint16_t rawValue = analogRead(PIN_LDR);
    Serial.print(F("  Valor ADC bruto: "));
    Serial.println(rawValue);
    printBar(rawValue);

    check("Pino ADC respondeu (valor > 0)",        rawValue > 0);
    check("Valor dentro do range ADC (0-4095)",     rawValue >= 0 && rawValue <= 4095);

    // --- Estabilidade ---
    printHeader("ESTABILIDADE (20 leituras)");

    uint32_t sum = 0;
    uint16_t minVal = 4095, maxVal = 0;

    for (int i = 0; i < 20; i++) {
        uint16_t v = analogRead(PIN_LDR);
        sum += v;
        if (v < minVal) minVal = v;
        if (v > maxVal) maxVal = v;
        delay(100);
    }

    float avg = (float)sum / 20.0f;
    uint16_t variation = maxVal - minVal;

    Serial.print(F("  Media:     "));
    Serial.println(avg, 1);
    Serial.print(F("  Min:       "));
    Serial.println(minVal);
    Serial.print(F("  Max:       "));
    Serial.println(maxVal);
    Serial.print(F("  Variacao:  "));
    Serial.print(variation);
    Serial.println(F(" unidades ADC"));

    check("Estabilidade OK (variacao < 150 ADC)",  variation < 150);

    // --- Teste de Resposta ---
    printHeader("TESTE DE RESPOSTA A LUZ");
    Serial.println(F("  Instrucoes:"));
    Serial.println(F("    1. CUBRA o sensor com a mao por 5 segundos"));
    Serial.println(F("    2. DESCUBRA o sensor"));
    Serial.println(F("    3. O valor deve mudar significativamente"));
    Serial.println();
    Serial.println(F("  Medindo por 10 segundos..."));

    uint16_t globalMin = 4095, globalMax = 0;
    for (int i = 0; i < 20; i++) {
        uint16_t v = analogRead(PIN_LDR);
        if (v < globalMin) globalMin = v;
        if (v > globalMax) globalMax = v;

        Serial.print(F("  ["));
        Serial.print(i + 1);
        Serial.print(F("/20] "));
        printBar(v);
        delay(500);
    }

    uint16_t responseRange = globalMax - globalMin;
    Serial.print(F("  Range de resposta: "));
    Serial.print(responseRange);
    Serial.println(F(" unidades ADC"));

    check("Sensor responde a mudanca de luz (range > 100)",  responseRange > 100);

    // --- Resumo ---
    printHeader("RESUMO FINAL");
    Serial.print(F("  Testes: "));
    Serial.print(passedTests);
    Serial.print(F("/"));
    Serial.print(totalTests);
    Serial.println(F(" passaram"));

    if (passedTests == totalTests) {
        Serial.println(F("  🎉 LDR 100% OPERACIONAL!"));
    } else {
        Serial.println(F("  ⚠️  Verifique a conexao A0→GPIO35 e o divisor de tensao."));
    }
    printSeparator();
    Serial.println();
    Serial.println(F("  Monitoramento continuo ativo (reset para parar)..."));
    printSeparator();
}

void loop() {
    uint16_t val = analogRead(PIN_LDR);
    printBar(val);
    delay(1000);
}
