/*
 * ============================================================
 * TESTE 8 — Teste Integrado de TODOS os Componentes
 * ============================================================
 * Executa uma verificação sequencial de cada dispositivo:
 *   1. ESP32 (CPU, RAM, Flash)
 *   2. DHT22 (Temperatura e Umidade do Ar)
 *   3. Sensor Capacitivo de Solo
 *   4. Sensor LDR
 *   5. Módulo Relé (Canal 1 e 2)
 *   6. Bomba D'Água (via Relé 1) — pulso breve
 *   7. Ventilador (via Relé 2) — pulso breve
 *
 * ⚠️  ATENÇÃO:
 *   - A bomba será acionada por 2 segundos (submerja na água!)
 *   - O ventilador será acionado por 2 segundos
 *   - Os relés farão cliques audíveis
 *
 * Como usar:
 *   1. Conecte TODOS os dispositivos
 *   2. Submerja a bomba em água
 *   3. Copie este arquivo para src/main.cpp
 *   4. pio run -t upload && pio device monitor
 * ============================================================
 */

#include <Arduino.h>
#include <DHT.h>

// ============================================================
// Pinos (conforme config.h)
// ============================================================
static constexpr uint8_t PIN_DHT         = 4;
static constexpr uint8_t PIN_SOIL        = 34;
static constexpr uint8_t PIN_LDR         = 35;
static constexpr uint8_t PIN_RELAY_PUMP  = 25;
static constexpr uint8_t PIN_RELAY_FAN   = 26;

static constexpr uint8_t RELAY_ON  = LOW;
static constexpr uint8_t RELAY_OFF = HIGH;

DHT dht(PIN_DHT, DHT22);

// ============================================================
// Contadores globais
// ============================================================
uint8_t totalTests  = 0;
uint8_t passedTests = 0;

// Resultados por módulo
struct ModuleResult {
    const char* name;
    uint8_t total;
    uint8_t passed;
    bool tested;
};

ModuleResult modules[] = {
    {"ESP32 (CPU/RAM/Flash)", 0, 0, false},
    {"DHT22 (Temp/Umidade)",  0, 0, false},
    {"Sensor Solo (ADC)",     0, 0, false},
    {"Sensor LDR (ADC)",      0, 0, false},
    {"Modulo Rele (2ch)",     0, 0, false},
    {"Bomba D'Agua",          0, 0, false},
    {"Ventilador Cooler",     0, 0, false},
};
static constexpr uint8_t NUM_MODULES = 7;
uint8_t currentModule = 0;

void printSeparator() {
    Serial.println(F("════════════════════════════════════════════"));
}

void printHeader(const char* title) {
    Serial.println();
    printSeparator();
    Serial.print(F("  "));
    Serial.print(currentModule + 1);
    Serial.print(F("/7  "));
    Serial.println(title);
    printSeparator();
}

void check(const char* item, bool condition) {
    totalTests++;
    modules[currentModule].total++;
    if (condition) {
        passedTests++;
        modules[currentModule].passed++;
    }
    Serial.print(F("  "));
    Serial.print(condition ? "✅" : "❌");
    Serial.print(F(" "));
    Serial.println(item);
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println();
    Serial.println(F("╔════════════════════════════════════════════════════╗"));
    Serial.println(F("║                                                    ║"));
    Serial.println(F("║   🌱 TESTE INTEGRADO — Estufa Inteligente         ║"));
    Serial.println(F("║      Verificacao completa de todos os dispositivos ║"));
    Serial.println(F("║                                                    ║"));
    Serial.println(F("╚════════════════════════════════════════════════════╝"));
    Serial.println();
    Serial.println(F("  ⚠️  A bomba e o ventilador serao acionados!"));
    Serial.println(F("     Certifique-se de que a bomba esta na agua."));
    Serial.println(F("     Iniciando em 5 segundos..."));
    delay(5000);

    // ============================================================
    // 1/7 — ESP32
    // ============================================================
    currentModule = 0;
    printHeader("ESP32 WROOM32");

    check("CPU freq > 0 MHz",            ESP.getCpuFreqMHz() > 0);
    check("Dual-core (2 nucleos)",        ESP.getChipCores() == 2);
    check("Flash >= 4 MB",               ESP.getFlashChipSize() >= 4 * 1024 * 1024);
    check("Heap livre > 100 KB",         ESP.getFreeHeap() > 100 * 1024);

    Serial.print(F("  Info: "));
    Serial.print(ESP.getChipModel());
    Serial.print(F(" @ "));
    Serial.print(ESP.getCpuFreqMHz());
    Serial.print(F(" MHz | RAM livre: "));
    Serial.print(ESP.getFreeHeap() / 1024);
    Serial.println(F(" KB"));
    modules[currentModule].tested = true;

    // ============================================================
    // 2/7 — DHT22
    // ============================================================
    currentModule = 1;
    printHeader("DHT22 (AM2302)");

    dht.begin();
    delay(2500);

    float temp = dht.readTemperature();
    float hum  = dht.readHumidity();
    bool dhtOk = !isnan(temp) && !isnan(hum);

    if (dhtOk) {
        Serial.print(F("  Temp: "));
        Serial.print(temp, 1);
        Serial.print(F(" °C | Umid: "));
        Serial.print(hum, 1);
        Serial.println(F(" %"));
    }

    check("Comunicacao com DHT22 OK",             dhtOk);
    check("Temperatura plausivel (10-45 C)",      dhtOk && temp >= 10.0 && temp <= 45.0);
    check("Umidade plausivel (20-95%)",            dhtOk && hum >= 20.0 && hum <= 95.0);
    modules[currentModule].tested = true;

    // ============================================================
    // 3/7 — Sensor Solo
    // ============================================================
    currentModule = 2;
    printHeader("SENSOR CAPACITIVO DE SOLO");

    analogReadResolution(12);
    uint16_t soilVal = analogRead(PIN_SOIL);
    Serial.print(F("  Valor ADC: "));
    Serial.println(soilVal);

    check("Leitura ADC valida (> 0)",              soilVal > 0);
    check("Valor no range (0-4095)",                soilVal >= 0 && soilVal <= 4095);
    check("Nao saturado em 0 ou 4095",              soilVal > 10 && soilVal < 4090);
    modules[currentModule].tested = true;

    // ============================================================
    // 4/7 — Sensor LDR
    // ============================================================
    currentModule = 3;
    printHeader("SENSOR DE LUZ LDR");

    uint16_t ldrVal = analogRead(PIN_LDR);
    Serial.print(F("  Valor ADC: "));
    Serial.println(ldrVal);

    check("Leitura ADC valida (> 0)",              ldrVal > 0);
    check("Valor no range (0-4095)",                ldrVal >= 0 && ldrVal <= 4095);
    check("Nao saturado em 0 ou 4095",              ldrVal > 10 && ldrVal < 4090);
    modules[currentModule].tested = true;

    // ============================================================
    // 5/7 — Módulo Relé
    // ============================================================
    currentModule = 4;
    printHeader("MODULO RELE 2 CANAIS");

    pinMode(PIN_RELAY_PUMP, OUTPUT);
    pinMode(PIN_RELAY_FAN, OUTPUT);
    digitalWrite(PIN_RELAY_PUMP, RELAY_OFF);
    digitalWrite(PIN_RELAY_FAN, RELAY_OFF);

    // Canal 1
    check("Rele 1 inicia desligado (HIGH)",     digitalRead(PIN_RELAY_PUMP) == RELAY_OFF);
    digitalWrite(PIN_RELAY_PUMP, RELAY_ON);
    delay(200);
    check("Rele 1 aciona (LOW)",                digitalRead(PIN_RELAY_PUMP) == RELAY_ON);
    digitalWrite(PIN_RELAY_PUMP, RELAY_OFF);
    delay(200);
    check("Rele 1 desliga (HIGH)",              digitalRead(PIN_RELAY_PUMP) == RELAY_OFF);

    // Canal 2
    check("Rele 2 inicia desligado (HIGH)",     digitalRead(PIN_RELAY_FAN) == RELAY_OFF);
    digitalWrite(PIN_RELAY_FAN, RELAY_ON);
    delay(200);
    check("Rele 2 aciona (LOW)",                digitalRead(PIN_RELAY_FAN) == RELAY_ON);
    digitalWrite(PIN_RELAY_FAN, RELAY_OFF);
    delay(200);
    check("Rele 2 desliga (HIGH)",              digitalRead(PIN_RELAY_FAN) == RELAY_OFF);
    modules[currentModule].tested = true;

    // ============================================================
    // 6/7 — Bomba D'Água
    // ============================================================
    currentModule = 5;
    printHeader("BOMBA D'AGUA (pulso de 2s)");

    Serial.println(F("  💧 Ligando bomba por 2 segundos..."));
    digitalWrite(PIN_RELAY_PUMP, RELAY_ON);
    check("Bomba acionada via rele",    digitalRead(PIN_RELAY_PUMP) == RELAY_ON);
    delay(2000);
    digitalWrite(PIN_RELAY_PUMP, RELAY_OFF);
    check("Bomba desligada com sucesso", digitalRead(PIN_RELAY_PUMP) == RELAY_OFF);
    Serial.println(F("  [ ] Verificar manualmente: a agua fluiu?"));
    modules[currentModule].tested = true;

    delay(1000);

    // ============================================================
    // 7/7 — Ventilador
    // ============================================================
    currentModule = 6;
    printHeader("VENTILADOR COOLER (pulso de 2s)");

    Serial.println(F("  🌀 Ligando ventilador por 2 segundos..."));
    digitalWrite(PIN_RELAY_FAN, RELAY_ON);
    check("Ventilador acionado via rele", digitalRead(PIN_RELAY_FAN) == RELAY_ON);
    delay(2000);
    digitalWrite(PIN_RELAY_FAN, RELAY_OFF);
    check("Ventilador desligado com sucesso", digitalRead(PIN_RELAY_FAN) == RELAY_OFF);
    Serial.println(F("  [ ] Verificar manualmente: as pas giraram?"));
    modules[currentModule].tested = true;

    // ============================================================
    // RELATÓRIO FINAL
    // ============================================================
    Serial.println();
    Serial.println();
    printSeparator();
    Serial.println(F("            📋 RELATORIO FINAL"));
    printSeparator();
    Serial.println();
    Serial.println(F("  Modulo                   | Result | Status"));
    Serial.println(F("  ─────────────────────────|────────|────────"));

    uint8_t modulesOk = 0;
    for (int i = 0; i < NUM_MODULES; i++) {
        Serial.print(F("  "));

        // Nome alinhado (25 chars)
        int nameLen = strlen(modules[i].name);
        Serial.print(modules[i].name);
        for (int j = nameLen; j < 26; j++) Serial.print(' ');

        Serial.print(F("| "));
        Serial.print(modules[i].passed);
        Serial.print(F("/"));
        Serial.print(modules[i].total);
        Serial.print(F("    | "));

        bool allPassed = modules[i].passed == modules[i].total;
        if (allPassed) modulesOk++;
        Serial.println(allPassed ? "✅ OK" : "❌ FALHA");
    }

    Serial.println(F("  ─────────────────────────|────────|────────"));
    Serial.print(F("  TOTAL                    | "));
    Serial.print(passedTests);
    Serial.print(F("/"));
    Serial.print(totalTests);
    if (totalTests < 10) Serial.print(F(" "));
    Serial.print(F("  | "));
    Serial.print(modulesOk);
    Serial.print(F("/"));
    Serial.println(NUM_MODULES);

    Serial.println();
    if (passedTests == totalTests) {
        Serial.println(F("  ╔════════════════════════════════════════╗"));
        Serial.println(F("  ║  🎉 TODOS OS DISPOSITIVOS OPERACIONAIS ║"));
        Serial.println(F("  ║     Sistema pronto para uso!            ║"));
        Serial.println(F("  ╚════════════════════════════════════════╝"));
    } else {
        Serial.println(F("  ⚠️  Alguns testes falharam."));
        Serial.println(F("     Execute os testes individuais dos modulos"));
        Serial.println(F("     com falha para diagnostico detalhado."));
    }

    Serial.println();
    printSeparator();
    Serial.println(F("  Teste integrado concluido."));
    printSeparator();
}

void loop() {
    // Teste executado no setup().
}
