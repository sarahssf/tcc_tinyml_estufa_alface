/*
 * ============================================================
 * sensors.cpp — Implementação do Módulo de Sensores
 * Estufa Inteligente de Alface Crespo | ESP32 + TinyML
 * ============================================================
 *
 * Lê DHT22 (temperatura + umidade ar), sensor capacitivo
 * (umidade solo) e LDR (luminosidade). Aplica média móvel
 * nos canais ADC para estabilidade. Non-blocking via millis().
 */

#include "sensors.h"
#include <DHT.h>

// ============================================================
// Instância global do DHT (interna ao módulo)
// ============================================================
static DHT dht(PIN_DHT22, DHT_TYPE);

// ============================================================
// SensorManager::begin()
// ============================================================
void SensorManager::begin() {
    // Inicializa DHT22
    dht.begin();

    // Configura ADC do ESP32 (12 bits = 0-4095)
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);  // Range completo 0-3.3V

    // Inicializa buffers de média móvel
    memset(_soilBuffer, 0, sizeof(_soilBuffer));
    memset(_ldrBuffer, 0, sizeof(_ldrBuffer));
    _bufferIndex = 0;
    _bufferFull = false;

    // Inicializa dados como inválidos
    _data = {};
    _data.isValid = false;

    // Força primeira leitura imediata
    _lastReadTime = 0;

    Serial.println(F("[SENSORS] Módulo de sensores inicializado."));
    Serial.print(F("[SENSORS] DHT22 no GPIO "));
    Serial.println(PIN_DHT22);
    Serial.print(F("[SENSORS] Capacitivo no GPIO "));
    Serial.println(PIN_SOIL_SENSOR);
    Serial.print(F("[SENSORS] LDR no GPIO "));
    Serial.println(PIN_LDR);
}

// ============================================================
// SensorManager::update()
// ============================================================
bool SensorManager::update() {
    unsigned long now = millis();

    // Verifica se é hora de ler (non-blocking)
    if (now - _lastReadTime < SENSOR_READ_INTERVAL_MS) {
        return false;  // Ainda não é hora
    }
    _lastReadTime = now;

    // --- Leitura do DHT22 ---
    float temp = dht.readTemperature();
    float hum  = dht.readHumidity();

    if (isnan(temp) || isnan(hum)) {
        // Leitura falhou — manter dados anteriores, marcar inválido
        _data.isValid = false;
        Serial.println(F("[SENSORS] ERRO: Falha na leitura do DHT22!"));
        return false;
    }

    _data.temperature = temp;
    _data.humidityAir = hum;
    _data.isValid = true;

    // --- Leitura do Sensor Capacitivo (ADC com média móvel) ---
    _data.soilMoisture = readADCFiltered(PIN_SOIL_SENSOR, _soilBuffer);

    // --- Leitura do LDR (ADC com média móvel) ---
    _data.lightLevel = readADCFiltered(PIN_LDR, _ldrBuffer);

    // --- Avançar índice do buffer circular ---
    _bufferIndex++;
    if (_bufferIndex >= MOVING_AVG_SAMPLES) {
        _bufferIndex = 0;
        _bufferFull = true;
    }

    // --- Atualizar flags booleanas ---
    updateFlags();

    return true;
}

// ============================================================
// SensorManager::getData()
// ============================================================
const SensorData& SensorManager::getData() const {
    return _data;
}

// ============================================================
// SensorManager::readADCFiltered()
// ============================================================
uint16_t SensorManager::readADCFiltered(uint8_t pin, uint16_t* buffer) {
    // Lê valor atual do ADC
    uint16_t rawValue = (uint16_t)analogRead(pin);

    // Armazena no buffer circular
    buffer[_bufferIndex] = rawValue;

    // Calcula média das amostras disponíveis
    uint8_t count = _bufferFull ? MOVING_AVG_SAMPLES : (_bufferIndex + 1);
    uint32_t sum = 0;
    for (uint8_t i = 0; i < count; i++) {
        sum += buffer[i];
    }

    return (uint16_t)(sum / count);
}

// ============================================================
// SensorManager::updateFlags()
// ============================================================
void SensorManager::updateFlags() {
    // PASSO 3: Solo criticamente seco (valor ADC alto = seco)
    _data.isSoilCritical = (_data.soilMoisture >= SOIL_DRY_THRESHOLD);

    // PASSO 4: Umidade do ar acima de 80%
    _data.isHighHumidity = (_data.humidityAir > HUMIDITY_AIR_MAX);

    // PASSO 5: Temperatura acima de 25°C
    _data.isHighTemperature = (_data.temperature > TEMP_MAX);

    // PASSO 2: Pico extremo de luminosidade
    _data.isExtremeLight = (_data.lightLevel >= LDR_EXTREME_THRESHOLD);
}
