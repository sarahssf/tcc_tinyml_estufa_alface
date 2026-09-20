/*
 * ============================================================
 * sensors.h — Interface do Módulo de Sensores
 * Estufa Inteligente de Alface Crespo | ESP32 + TinyML
 * ============================================================
 */

#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include "config.h"

// ============================================================
// Estrutura de dados dos sensores
// ============================================================
struct SensorData {
    // --- Valores brutos processados ---
    float    temperature;     // °C (DHT22)
    float    humidityAir;     // % (DHT22)
    uint16_t soilMoisture;    // 0-4095 ADC (Capacitivo)
    uint16_t lightLevel;      // 0-4095 ADC (LDR)

    // --- Flags derivadas (decisão do fluxograma) ---
    bool isSoilCritical;      // Solo criticamente seco
    bool isHighHumidity;      // Umidade do ar > 80%
    bool isHighTemperature;   // Temperatura > 25°C
    bool isExtremeLight;      // Pico extremo de luminosidade

    // --- Validade ---
    bool isValid;             // true se leitura DHT22 foi bem-sucedida
};

// ============================================================
// Classe SensorManager
// ============================================================
class SensorManager {
public:
    /// @brief Inicializa os sensores (DHT22, ADC).
    void begin();

    /// @brief Atualiza as leituras dos sensores (non-blocking via millis).
    /// @return true se novas leituras foram processadas neste ciclo.
    bool update();

    /// @brief Retorna referência constante para os dados atuais.
    const SensorData& getData() const;

private:
    SensorData _data;
    unsigned long _lastReadTime;

    // --- Buffers de média móvel (ADC) ---
    uint16_t _soilBuffer[MOVING_AVG_SAMPLES];
    uint16_t _ldrBuffer[MOVING_AVG_SAMPLES];
    uint8_t  _bufferIndex;
    bool     _bufferFull;

    /// @brief Lê o ADC com média móvel.
    uint16_t readADCFiltered(uint8_t pin, uint16_t* buffer);

    /// @brief Atualiza as flags booleanas baseadas nos thresholds.
    void updateFlags();
};

#endif // SENSORS_H
