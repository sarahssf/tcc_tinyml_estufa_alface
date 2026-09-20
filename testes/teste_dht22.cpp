/*
 * ============================================================
 * TESTE 2 — Sensor DHT22 (AM2302)
 * ============================================================
 * Verifica:
 *   ✔ Comunicação com o sensor (leitura válida)
 *   ✔ Temperatura no range esperado (-40°C a 80°C)
 *   ✔ Umidade do ar no range esperado (0% a 100%)
 *   ✔ Estabilidade: 10 leituras consecutivas
 *
 * Conexão:
 *   DHT22 DATA → GPIO 4
 *   VCC → 3.3V | GND → GND
 *   Resistor pull-up 10kΩ entre DATA e VCC
 *
 * Como usar:
 *   1. Copie este arquivo para src/main.cpp
 *   2. pio run -t upload && pio device monitor
 * ============================================================
 */

#include <Arduino.h>
#include <DHT.h>

// --- Pino e tipo (conforme config.h) ---
static constexpr uint8_t PIN_DHT = 4;
static constexpr uint8_t DHT_TYPE_SENSOR = DHT22;

DHT dht(PIN_DHT, DHT_TYPE_SENSOR);

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

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println();
    Serial.println(F("╔════════════════════════════════════════════╗"));
    Serial.println(F("║   TESTE 2 — Sensor DHT22 (AM2302)         ║"));
    Serial.println(F("╚════════════════════════════════════════════╝"));

    dht.begin();

    // --- Teste Inicial: Leitura Única ---
    printHeader("LEITURA INICIAL");
    Serial.println(F("  Aguardando estabilizacao do sensor (2s)..."));
    delay(2500);  // DHT22 precisa de pelo menos 2s entre leituras

    float temp = dht.readTemperature();
    float hum  = dht.readHumidity();

    bool validRead = !isnan(temp) && !isnan(hum);

    if (validRead) {
        Serial.print(F("  Temperatura: "));
        Serial.print(temp, 1);
        Serial.println(F(" °C"));
        Serial.print(F("  Umidade:     "));
        Serial.print(hum, 1);
        Serial.println(F(" %"));
    } else {
        Serial.println(F("  ❌ Falha na leitura! Verifique:"));
        Serial.println(F("     - Fiação (DATA→GPIO4, VCC→3.3V, GND→GND)"));
        Serial.println(F("     - Resistor pull-up 10kΩ entre DATA e VCC"));
    }

    check("Comunicacao com DHT22 OK",                       validRead);
    check("Temperatura no range (-40 a 80 C)",              validRead && temp >= -40.0 && temp <= 80.0);
    check("Temperatura plausivel (10 a 45 C, ambiente)",    validRead && temp >= 10.0 && temp <= 45.0);
    check("Umidade no range (0 a 100%)",                    validRead && hum >= 0.0 && hum <= 100.0);
    check("Umidade plausivel (20 a 95%, ambiente)",         validRead && hum >= 20.0 && hum <= 95.0);

    // --- Teste de Estabilidade: 10 leituras ---
    printHeader("TESTE DE ESTABILIDADE (10 leituras)");
    Serial.println(F("  Leitura  |  Temp (C)  |  Umid (%)  |  Status"));
    Serial.println(F("  ---------|------------|------------|--------"));

    uint8_t successCount = 0;
    float tempSum = 0, humSum = 0;
    float tempMin = 999, tempMax = -999;
    float humMin  = 999, humMax  = -999;

    for (int i = 1; i <= 10; i++) {
        delay(2500);  // Intervalo mínimo do DHT22

        temp = dht.readTemperature();
        hum  = dht.readHumidity();
        bool ok = !isnan(temp) && !isnan(hum);

        Serial.print(F("     "));
        if (i < 10) Serial.print(F(" "));
        Serial.print(i);
        Serial.print(F("    |   "));

        if (ok) {
            successCount++;
            tempSum += temp;
            humSum  += hum;
            if (temp < tempMin) tempMin = temp;
            if (temp > tempMax) tempMax = temp;
            if (hum < humMin) humMin = hum;
            if (hum > humMax) humMax = hum;

            if (temp < 10) Serial.print(F(" "));
            Serial.print(temp, 1);
            Serial.print(F("    |    "));
            if (hum < 10) Serial.print(F(" "));
            Serial.print(hum, 1);
            Serial.println(F("   |  ✅ OK"));
        } else {
            Serial.println(F("  ---     |     ---    |  ❌ FALHA"));
        }
    }

    // --- Estatísticas ---
    printHeader("ESTATISTICAS");
    Serial.print(F("  Leituras validas: "));
    Serial.print(successCount);
    Serial.println(F("/10"));

    if (successCount > 0) {
        float tempAvg = tempSum / successCount;
        float humAvg  = humSum / successCount;
        float tempVar = tempMax - tempMin;
        float humVar  = humMax - humMin;

        Serial.print(F("  Temp media:  "));
        Serial.print(tempAvg, 1);
        Serial.println(F(" °C"));
        Serial.print(F("  Temp range:  "));
        Serial.print(tempMin, 1);
        Serial.print(F(" ~ "));
        Serial.print(tempMax, 1);
        Serial.print(F(" °C (variacao: "));
        Serial.print(tempVar, 1);
        Serial.println(F(" °C)"));

        Serial.print(F("  Umid media:  "));
        Serial.print(humAvg, 1);
        Serial.println(F(" %"));
        Serial.print(F("  Umid range:  "));
        Serial.print(humMin, 1);
        Serial.print(F(" ~ "));
        Serial.print(humMax, 1);
        Serial.print(F(" % (variacao: "));
        Serial.print(humVar, 1);
        Serial.println(F(" %)"));

        check("Estabilidade temp (variacao < 3 C)", tempVar < 3.0);
        check("Estabilidade umid (variacao < 5%)",  humVar < 5.0);
    }

    check("Taxa de sucesso >= 80% (8/10 leituras)", successCount >= 8);

    // --- Resumo ---
    printHeader("RESUMO FINAL");
    Serial.print(F("  Testes: "));
    Serial.print(passedTests);
    Serial.print(F("/"));
    Serial.print(totalTests);
    Serial.println(F(" passaram"));
    Serial.println();

    if (passedTests == totalTests) {
        Serial.println(F("  🎉 DHT22 100% OPERACIONAL!"));
    } else {
        Serial.println(F("  ⚠️  Verifique as conexoes e o resistor pull-up."));
    }

    printSeparator();
}

void loop() {
    // Teste executado no setup().
}
