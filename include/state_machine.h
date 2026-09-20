/*
 * ============================================================
 * state_machine.h — Interface da Máquina de Estados
 * Estufa Inteligente de Alface Crespo | ESP32 + TinyML
 * ============================================================
 *
 * Implementa o fluxograma de decisão EXATAMENTE como
 * especificado, com prioridades:
 *   1. Estresse Hídrico (ESTADO 3)
 *   2. Risco de Fungos  (ESTADO 4)
 *   3. Calor            (ESTADO 2)
 *   4. Clima Ideal      (ESTADO 1)
 */

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>
#include "config.h"
#include "sensors.h"
#include "actuators.h"
#include "data_logger.h"

// ============================================================
// Estados do Sistema
// ============================================================
enum class SystemState : uint8_t {
    STATE_1_IDEAL        = 1,  // Temp 15-25°C, clima ideal
    STATE_2_HEAT         = 2,  // Temp > 25°C
    STATE_3_WATER_STRESS = 3,  // Solo criticamente seco
    STATE_4_FUNGUS_RISK  = 4   // Umidade do ar > 80%
};

// ============================================================
// Classe StateMachine
// ============================================================
class StateMachine {
public:
    /// @brief Construtor — recebe referências para os módulos.
    StateMachine(SensorManager& sensors, ActuatorManager& actuators, DataLogger& logger);

    /// @brief Inicializa a máquina de estados.
    void begin();

    /// @brief Executa um ciclo completo: avalia sensores → determina estado → aciona atuadores.
    ///        Deve ser chamado a cada iteração do loop().
    void update();

    /// @brief Retorna o estado atual do sistema.
    SystemState getCurrentState() const;

    /// @brief Retorna o nome do estado como string (para log).
    static const char* stateToString(SystemState state);

private:
    SensorManager&  _sensors;
    ActuatorManager& _actuators;
    DataLogger&     _logger;

    SystemState     _currentState;
    SystemState     _previousState;

    /// @brief Avalia os sensores e determina o próximo estado.
    ///        Segue ESTRITAMENTE o fluxograma na ordem de prioridade.
    SystemState evaluate(const SensorData& data);

    /// @brief Executa as ações correspondentes ao estado determinado.
    void executeState(SystemState newState, const SensorData& data);

    /// @brief PASSO 2: Fiscalização de luz (independente).
    void checkLightWarning(const SensorData& data);
};

#endif // STATE_MACHINE_H
