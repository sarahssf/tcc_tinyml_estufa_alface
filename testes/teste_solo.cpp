/*
 * ============================================================
 * TESTE 3 — Sensor Capacitivo de Umidade do Solo v1.2
 * ============================================================
 * Verifica:
 *   ✔ Leitura analógica no pino ADC (GPIO 34)
 *   ✔ Valor dentro do range ADC (0-4095, 12 bits)
 *   ✔ Resposta à mudança (secar/molhar o sensor)
 *   ✔ Estabilidade de leitura
 *
 * Conexão:
 *   AOUT → GPIO 34 (ADC1_CH6)
 *   VCC  → 3.3V
 *   GND  → GND
 *
 * Calibração:
 *   Sensor capacitivo: valor ALTO = solo seco
 *                      valor BAIXO = solo úmido
 *
 * Como usar:
 *   1. Copie este arquivo para src/main.cpp
 *   2. pio run -t upload && pio device monitor
 *   3. Observe os valores com o sensor no AR, na AGUA e no SOLO
 * ============================================================
 */

#include <Arduino.h>

static constexpr uint8_t PIN_SOIL = 34;

// Thresholds do projeto (de config.h)
static constexpr uint16_t SOIL_DRY   = 3200;
static constexpr uint16_t SOIL_WET   = 1500;

// --- Contadores ---
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

// Barra visual para o valor ADC
void printBar(uint16_t val) {
    int barLen = map(val, 0, 4095, 0, 30);
    Serial.print(F("  ["));
    for (int i = 0; i < 30; i++) {
        Serial.print(i < barLen ? "█" : "░");
    }
    Serial.print(F("] "));
    Serial.println(val);
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println();
    Serial.println(F("╔════════════════════════════════════════════╗"));
    Serial.println(F("║   TESTE 3 — Sensor Capacitivo de Solo v1.2║"));
    Serial.println(F("╚════════════════════════════════════════════╝"));

    analogReadResolution(12);  // 12 bits: 0-4095

    // --- Leitura Única ---
    printHeader("LEITURA INICIAL");
    uint16_t rawValue = analogRead(PIN_SOIL);
    Serial.print(F("  Valor ADC bruto: "));
    Serial.println(rawValue);
    printBar(rawValue);

    // Classificar
    Serial.print(F("  Classificacao:   "));
    if (rawValue >= SOIL_DRY) {
        Serial.println(F("🏜️  SECO (ou no ar)"));
    } else if (rawValue <= SOIL_WET) {
        Serial.println(F("💧 ENCHARCADO (ou na agua)"));
    } else {
        Serial.println(F("🌱 UMIDADE ADEQUADA"));
    }

    check("Pino ADC respondeu (valor > 0)",        rawValue > 0);
    check("Valor dentro do range ADC (0-4095)",     rawValue >= 0 && rawValue <= 4095);
    check("Valor nao esta saturado em 0",           rawValue > 10);
    check("Valor nao esta saturado em 4095",        rawValue < 4090);

    // --- Teste de Estabilidade: 20 leituras ---
    printHeader("ESTABILIDADE (20 leituras rapidas)");

    uint32_t sum = 0;
    uint16_t minVal = 4095, maxVal = 0;
    uint16_t readings[20];

    for (int i = 0; i < 20; i++) {
        readings[i] = analogRead(PIN_SOIL);
        sum += readings[i];
        if (readings[i] < minVal) minVal = readings[i];
        if (readings[i] > maxVal) maxVal = readings[i];
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

    check("Estabilidade OK (variacao < 100 ADC)",  variation < 100);

    // --- Monitoramento Contínuo ---
    printHeader("MONITORAMENTO CONTINUO");
    Serial.println(F("  Agora voce pode testar o sensor em diferentes condicoes:"));
    Serial.println(F("    1. Deixe no AR     → valor alto (~3500+)"));
    Serial.println(F("    2. Coloque na AGUA → valor baixo (~1000-)"));
    Serial.println(F("    3. Coloque no SOLO → valor intermediario"));
    Serial.println();
    Serial.println(F("  Atualizando a cada 1 segundo... (reset para parar)"));
    printSeparator();

    // --- Resumo ---
    printHeader("RESUMO PARCIAL");
    Serial.print(F("  Testes automaticos: "));
    Serial.print(passedTests);
    Serial.print(F("/"));
    Serial.print(totalTests);
    Serial.println(F(" passaram"));

    if (passedTests == totalTests) {
        Serial.println(F("  🎉 Sensor de solo respondendo corretamente!"));
    } else {
        Serial.println(F("  ⚠️  Verifique a conexao AOUT→GPIO34."));
    }
    printSeparator();
    Serial.println();
}

void loop() {
    // Leitura contínua para teste manual
    uint16_t val = analogRead(PIN_SOIL);
    Serial.print(F("  Solo ADC: "));
    Serial.print(val);
    Serial.print(F("  "));
    printBar(val);
    delay(1000);
}
