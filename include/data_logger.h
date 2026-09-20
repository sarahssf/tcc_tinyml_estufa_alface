/*
 * ============================================================
 * data_logger.h — Interface do Módulo de Log e Dataset
 * Estufa Inteligente de Alface Crespo | ESP32 + TinyML
 * ============================================================
 */

#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include <Arduino.h>
#include "config.h"
#include "sensors.h"

// Forward declaration para evitar dependência circular
enum class SystemState : uint8_t;

// ============================================================
// Classe DataLogger
// ============================================================
class DataLogger {
public:
    /// @brief Inicializa o Serial e imprime cabeçalho.
    void begin();

    /// @brief Log formatado dos sensores e estado atual.
    /// @param data Dados dos sensores.
    /// @param state Estado atual do sistema.
    void logStatus(const SensorData& data, SystemState state);

    /// @brief Imprime o alerta de luminosidade extrema (frase exata).
    void logLightWarning();

    /// @brief Imprime a transição de estado.
    void logStateTransition(SystemState from, SystemState to);

    /// @brief Imprime mensagem de ação de atuador.
    void logActuatorAction(const char* message);

    /// @brief Verifica se é hora de logar (non-blocking).
    bool shouldLog();

private:
    unsigned long _lastLogTime;

    /// @brief Retorna nome do estado como string.
    static const char* stateToString(SystemState state);

    /// @brief Imprime cabeçalho CSV para modo dataset.
    void printCSVHeader();

    /// @brief Imprime linha CSV para modo dataset.
    void printCSVLine(const SensorData& data, SystemState state);

    /// @brief Imprime log formatado para modo normal.
    void printFormattedLog(const SensorData& data, SystemState state);
};

#endif // DATA_LOGGER_H
