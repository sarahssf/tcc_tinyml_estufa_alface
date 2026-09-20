/*
 * ============================================================
 * TESTE 1 — ESP32 DevKit WROOM32
 * ============================================================
 * Verifica:
 *   ✔ CPU: frequência e ID do chip
 *   ✔ Flash: tamanho e velocidade
 *   ✔ RAM: heap livre e maior bloco contíguo
 *   ✔ WiFi: scan de redes (confirma que o rádio funciona)
 *   ✔ GPIOs: teste rápido de escrita/leitura nos pinos usados
 *
 * Como usar:
 *   1. Copie este arquivo para src/main.cpp
 *   2. pio run -t upload && pio device monitor
 * ============================================================
 */

#include <Arduino.h>
#include <WiFi.h>

// ============================================================
// Helpers de formatação
// ============================================================
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

void printResult(const char* item, bool passed) {
    Serial.print(F("  "));
    Serial.print(passed ? "✅" : "❌");
    Serial.print(F(" "));
    Serial.println(item);
}

// ============================================================
// Contadores
// ============================================================
uint8_t totalTests  = 0;
uint8_t passedTests = 0;

void check(const char* item, bool condition) {
    totalTests++;
    if (condition) passedTests++;
    printResult(item, condition);
}

// ============================================================
// setup()
// ============================================================
void setup() {
    Serial.begin(115200);
    delay(2000);  // Tempo para o Serial Monitor conectar

    Serial.println();
    Serial.println(F("╔════════════════════════════════════════════╗"));
    Serial.println(F("║   TESTE 1 — ESP32 DevKit WROOM32          ║"));
    Serial.println(F("╚════════════════════════════════════════════╝"));

    // --- Informações do Chip ---
    printHeader("INFORMACOES DO CHIP");
    Serial.print(F("  Modelo:       "));
    Serial.println(ESP.getChipModel());
    Serial.print(F("  Revisao:      "));
    Serial.println(ESP.getChipRevision());
    Serial.print(F("  Cores CPU:    "));
    Serial.println(ESP.getChipCores());
    Serial.print(F("  Freq. CPU:    "));
    Serial.print(ESP.getCpuFreqMHz());
    Serial.println(F(" MHz"));

    check("CPU detectada (freq > 0)",     ESP.getCpuFreqMHz() > 0);
    check("Dual-core (2 nucleos)",        ESP.getChipCores() == 2);
    check("Frequencia padrao (240 MHz)",  ESP.getCpuFreqMHz() == 240);

    // --- Flash ---
    printHeader("MEMORIA FLASH");
    uint32_t flashSize  = ESP.getFlashChipSize();
    uint32_t flashSpeed = ESP.getFlashChipSpeed();
    Serial.print(F("  Tamanho:      "));
    Serial.print(flashSize / (1024 * 1024));
    Serial.println(F(" MB"));
    Serial.print(F("  Velocidade:   "));
    Serial.print(flashSpeed / 1000000);
    Serial.println(F(" MHz"));

    check("Flash >= 4 MB",               flashSize >= 4 * 1024 * 1024);
    check("Flash speed >= 40 MHz",       flashSpeed >= 40000000);

    // --- RAM / Heap ---
    printHeader("MEMORIA RAM (Heap)");
    uint32_t freeHeap     = ESP.getFreeHeap();
    uint32_t maxAllocHeap = ESP.getMaxAllocHeap();
    Serial.print(F("  Heap livre:          "));
    Serial.print(freeHeap / 1024);
    Serial.println(F(" KB"));
    Serial.print(F("  Maior bloco livre:   "));
    Serial.print(maxAllocHeap / 1024);
    Serial.println(F(" KB"));

    check("Heap livre > 100 KB",         freeHeap > 100 * 1024);
    check("Bloco contiguo > 50 KB",      maxAllocHeap > 50 * 1024);

    // --- WiFi Scan ---
    printHeader("WIFI (scan de redes)");
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println(F("  Escaneando redes..."));
    int networks = WiFi.scanNetworks();
    Serial.print(F("  Redes encontradas: "));
    Serial.println(networks);

    if (networks > 0) {
        for (int i = 0; i < min(networks, 5); i++) {
            Serial.print(F("    • "));
            Serial.print(WiFi.SSID(i));
            Serial.print(F(" ("));
            Serial.print(WiFi.RSSI(i));
            Serial.println(F(" dBm)"));
        }
        if (networks > 5) {
            Serial.print(F("    ... e mais "));
            Serial.print(networks - 5);
            Serial.println(F(" redes."));
        }
    }
    check("Radio WiFi funcional (>= 1 rede)", networks > 0);
    WiFi.mode(WIFI_OFF);

    // --- Teste de GPIOs (pinos usados no projeto) ---
    printHeader("GPIOs (pinos de saida)");

    // Testar GPIO 25 e 26 (relés): setar HIGH, ler, setar LOW, ler
    uint8_t outputPins[] = {25, 26};
    const char* outputNames[] = {"GPIO 25 (Rele Bomba)", "GPIO 26 (Rele Ventilador)"};

    for (int i = 0; i < 2; i++) {
        pinMode(outputPins[i], OUTPUT);
        digitalWrite(outputPins[i], HIGH);
        delay(10);
        int valHigh = digitalRead(outputPins[i]);
        digitalWrite(outputPins[i], LOW);
        delay(10);
        int valLow = digitalRead(outputPins[i]);

        char msg[64];
        snprintf(msg, sizeof(msg), "%s: escrita OK", outputNames[i]);
        check(msg, valHigh == HIGH && valLow == LOW);
    }

    // Testar ADC nos pinos 34 e 35 (leitura analógica)
    printHeader("ADC (pinos analogicos)");
    uint8_t adcPins[] = {34, 35};
    const char* adcNames[] = {"GPIO 34 (Solo ADC)", "GPIO 35 (LDR ADC)"};

    for (int i = 0; i < 2; i++) {
        int val = analogRead(adcPins[i]);
        Serial.print(F("  "));
        Serial.print(adcNames[i]);
        Serial.print(F(": "));
        Serial.println(val);

        char msg[64];
        snprintf(msg, sizeof(msg), "%s: leitura valida (0-4095)", adcNames[i]);
        check(msg, val >= 0 && val <= 4095);
    }

    // --- Resumo Final ---
    printHeader("RESUMO FINAL");
    Serial.print(F("  Testes: "));
    Serial.print(passedTests);
    Serial.print(F("/"));
    Serial.print(totalTests);
    Serial.println(F(" passaram"));
    Serial.println();

    if (passedTests == totalTests) {
        Serial.println(F("  🎉 ESP32 100% OPERACIONAL!"));
    } else {
        Serial.println(F("  ⚠️  Alguns testes falharam. Verifique as conexoes."));
    }

    printSeparator();
    Serial.println(F("  Teste concluido. Pode prosseguir para o proximo."));
    printSeparator();
}

// ============================================================
// loop() — vazio, teste roda apenas uma vez
// ============================================================
void loop() {
    // Nada a fazer. Teste executado no setup().
}
