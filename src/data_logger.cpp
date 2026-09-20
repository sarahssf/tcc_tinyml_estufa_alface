/*
 * ============================================================
 * data_logger.cpp — Implementação do Módulo de Log e Dataset
 * Estufa Inteligente de Alface Crespo | ESP32 + TinyML
 * ============================================================
 *
 * Dois modos de operação (compilação condicional):
 *
 * MODO NORMAL (padrão):
 *   Log formatado e legível no Serial Monitor com timestamp,
 *   valores dos sensores e estado atual do sistema.
 *
 * MODO DATASET (DATASET_MODE definido em config.h):
 *   Saída CSV pura no Serial para captura via terminal.
 *   Formato: timestamp,temperature,humidity_air,soil_moisture,light_level,state_label
 *   Usado para criar o dataset de treinamento do modelo TinyML.
 */

#include "data_logger.h"
#include "state_machine.h"  // Para SystemState

// ============================================================
// DataLogger::begin()
// ============================================================
void DataLogger::begin() {
    _lastLogTime = 0;

    #ifdef DATASET_MODE
        printCSVHeader();
        Serial.println(F("[LOGGER] Modo DATASET ativado. Saída CSV no Serial."));
    #else
        Serial.println(F(""));
        Serial.println(F("╔══════════════════════════════════════════════════════╗"));
        Serial.println(F("║   ESTUFA INTELIGENTE DE ALFACE CRESPO               ║"));
        Serial.println(F("║   ESP32 + TinyML | Modo Offline                     ║"));
        Serial.println(F("║   v1.0.0                                            ║"));
        Serial.println(F("╚══════════════════════════════════════════════════════╝"));
        Serial.println(F(""));
        Serial.println(F("[LOGGER] Módulo de log inicializado. Intervalo: 5s."));
    #endif
}

// ============================================================
// DataLogger::logStatus()
// ============================================================
void DataLogger::logStatus(const SensorData& data, SystemState state) {
    #ifdef DATASET_MODE
        printCSVLine(data, state);
    #else
        printFormattedLog(data, state);
    #endif
}

// ============================================================
// DataLogger::logLightWarning() — Frase EXATA do fluxograma
// ============================================================
void DataLogger::logLightWarning() {
    Serial.println(F("Aviso do Sistema: Sombrite mal posicionado ou insuficiente. Planta em risco de queimadura solar."));
}

// ============================================================
// DataLogger::logStateTransition()
// ============================================================
void DataLogger::logStateTransition(SystemState from, SystemState to) {
    #ifndef DATASET_MODE
        Serial.print(F("[FSM] Transição: "));
        Serial.print(stateToString(from));
        Serial.print(F(" → "));
        Serial.println(stateToString(to));
    #endif
}

// ============================================================
// DataLogger::logActuatorAction()
// ============================================================
void DataLogger::logActuatorAction(const char* message) {
    #ifndef DATASET_MODE
        Serial.print(F("[AÇÃO] "));
        Serial.println(message);
    #endif
}

// ============================================================
// DataLogger::shouldLog()
// ============================================================
bool DataLogger::shouldLog() {
    unsigned long now = millis();
    unsigned long interval;

    #ifdef DATASET_MODE
        interval = DATASET_LOG_INTERVAL_MS;
    #else
        interval = LOG_INTERVAL_MS;
    #endif

    if (now - _lastLogTime >= interval) {
        _lastLogTime = now;
        return true;
    }
    return false;
}

// ============================================================
// Métodos privados
// ============================================================

const char* DataLogger::stateToString(SystemState state) {
    switch (state) {
        case SystemState::STATE_1_IDEAL:        return "ESTADO_1_IDEAL";
        case SystemState::STATE_2_HEAT:         return "ESTADO_2_CALOR";
        case SystemState::STATE_3_WATER_STRESS: return "ESTADO_3_ESTRESSE_HIDRICO";
        case SystemState::STATE_4_FUNGUS_RISK:  return "ESTADO_4_RISCO_FUNGOS";
        default:                                return "DESCONHECIDO";
    }
}

void DataLogger::printCSVHeader() {
    Serial.println(F("timestamp_ms,temperature_c,humidity_air_pct,soil_moisture_adc,light_level_adc,state_label"));
}

void DataLogger::printCSVLine(const SensorData& data, SystemState state) {
    // Formato CSV para importação direta no Edge Impulse
    Serial.print(millis());
    Serial.print(',');
    Serial.print(data.temperature, 1);
    Serial.print(',');
    Serial.print(data.humidityAir, 1);
    Serial.print(',');
    Serial.print(data.soilMoisture);
    Serial.print(',');
    Serial.print(data.lightLevel);
    Serial.print(',');
    Serial.println(stateToString(state));
}

void DataLogger::printFormattedLog(const SensorData& data, SystemState state) {
    Serial.println(F("────────────────────────────────────────"));
    Serial.print(F("⏱ Tempo: "));
    // Formatar tempo em HH:MM:SS
    unsigned long totalSec = millis() / 1000;
    unsigned long hours = totalSec / 3600;
    unsigned long minutes = (totalSec % 3600) / 60;
    unsigned long seconds = totalSec % 60;
    if (hours < 10) Serial.print('0');
    Serial.print(hours);
    Serial.print(':');
    if (minutes < 10) Serial.print('0');
    Serial.print(minutes);
    Serial.print(':');
    if (seconds < 10) Serial.print('0');
    Serial.println(seconds);

    Serial.print(F("🌡 Temperatura:   "));
    Serial.print(data.temperature, 1);
    Serial.println(F(" °C"));

    Serial.print(F("💧 Umid. Ar:      "));
    Serial.print(data.humidityAir, 1);
    Serial.println(F(" %"));

    Serial.print(F("🌱 Umid. Solo:    "));
    Serial.print(data.soilMoisture);
    Serial.print(F(" (ADC) "));
    if (data.isSoilCritical) {
        Serial.println(F("[SECO CRÍTICO]"));
    } else {
        Serial.println(F("[OK]"));
    }

    Serial.print(F("☀ Luminosidade:  "));
    Serial.print(data.lightLevel);
    Serial.print(F(" (ADC) "));
    if (data.isExtremeLight) {
        Serial.println(F("[EXTREMA]"));
    } else {
        Serial.println(F("[OK]"));
    }

    Serial.print(F("🔄 Estado:        "));
    Serial.println(stateToString(state));
    Serial.println(F("────────────────────────────────────────"));
}
