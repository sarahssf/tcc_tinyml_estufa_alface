/*
 * ============================================================
 * state_machine.cpp — Implementação da Máquina de Estados
 * Estufa Inteligente de Alface Crespo | ESP32 + TinyML
 * ============================================================
 *
 * LÓGICA DE DECISÃO — Segue ESTRITAMENTE o fluxograma:
 *
 *   PASSO 1: Coleta de dados (feita em sensors.cpp)
 *   PASSO 2: Fiscalização de luz (independente — apenas log)
 *   PASSO 3: Solo seco? → ESTADO 3 (Prioridade 1)
 *   PASSO 4: Umid. ar > 80%? → ESTADO 4 (Prioridade 2)
 *   PASSO 5: Temp > 25°C? → ESTADO 2 | Senão → ESTADO 1
 *
 * NENHUMA outra regra climática é implementada.
 *
 * --- INTEGRAÇÃO TinyML (Futuro) ---
 * Para substituir as regras if/else por inferência do modelo:
 * 1. Descomentar a seção marcada com "TINYML_INFERENCE"
 * 2. Incluir o header gerado pelo Edge Impulse
 * 3. A função evaluate() usará o resultado do modelo
 */

#include "state_machine.h"

// ============================================================
// FUTURO: Incluir biblioteca do Edge Impulse aqui
// ============================================================
// #define TINYML_INFERENCE
// #ifdef TINYML_INFERENCE
// #include <estufa_alface_inferencing.h>
// #endif

// ============================================================
// StateMachine::StateMachine()
// ============================================================
StateMachine::StateMachine(SensorManager& sensors, ActuatorManager& actuators, DataLogger& logger)
    : _sensors(sensors)
    , _actuators(actuators)
    , _logger(logger)
    , _currentState(SystemState::STATE_1_IDEAL)
    , _previousState(SystemState::STATE_1_IDEAL)
{
}

// ============================================================
// StateMachine::begin()
// ============================================================
void StateMachine::begin() {
    _currentState = SystemState::STATE_1_IDEAL;
    _previousState = SystemState::STATE_1_IDEAL;
    Serial.println(F("[FSM] Máquina de estados inicializada. Estado inicial: ESTADO_1_IDEAL"));
}

// ============================================================
// StateMachine::update() — Ciclo principal
// ============================================================
void StateMachine::update() {
    // PASSO 1: Leitura dos sensores (controlado por millis em SensorManager)
    bool newReading = _sensors.update();

    if (!newReading) {
        // Sem nova leitura — apenas atualizar timers dos atuadores
        _actuators.update();
        return;
    }

    const SensorData& data = _sensors.getData();

    // Verificar validade dos dados
    if (!data.isValid) {
        _actuators.update();
        return;
    }

    // PASSO 2: Fiscalização de luz (INDEPENDENTE — não altera estado)
    checkLightWarning(data);

    // PASSOS 3-5: Avaliar estado baseado nas prioridades do fluxograma
    SystemState newState = evaluate(data);

    // Executar ações do novo estado
    executeState(newState, data);

    // Atualizar timers dos atuadores (pulso bomba, intermitência ventilador)
    _actuators.update();

    // Log periódico
    if (_logger.shouldLog()) {
        _logger.logStatus(data, _currentState);
    }
}

// ============================================================
// StateMachine::evaluate() — Determina o estado
// ============================================================
// Segue ESTRITAMENTE o fluxograma na ordem de prioridade.
// ============================================================
SystemState StateMachine::evaluate(const SensorData& data) {

    // ==========================================================
    // MODO REGRAS (padrão) — Lógica if/else do fluxograma
    // ==========================================================
    #ifndef TINYML_INFERENCE

    // PASSO 3 — PRIORIDADE 1: Estresse Hídrico
    // Condição: Solo em nível crítico (seco)?
    if (data.isSoilCritical) {
        return SystemState::STATE_3_WATER_STRESS;
    }

    // PASSO 4 — PRIORIDADE 2: Risco de Fungos
    // Condição: Umidade do ar > 80%?
    if (data.isHighHumidity) {
        return SystemState::STATE_4_FUNGUS_RISK;
    }

    // PASSO 5 — Controle de Temperatura
    // Condição: Temperatura > 25°C?
    if (data.isHighTemperature) {
        return SystemState::STATE_2_HEAT;
    }

    // ESTADO 1: Clima ideal (15°C a 25°C, umidade OK, solo OK)
    return SystemState::STATE_1_IDEAL;

    #else
    // ==========================================================
    // MODO TinyML — Inferência via Edge Impulse
    // ==========================================================
    // Descomentar e adaptar quando o modelo estiver treinado:
    //
    // float features[4] = {
    //     data.temperature,
    //     data.humidityAir,
    //     (float)data.soilMoisture,
    //     (float)data.lightLevel
    // };
    //
    // signal_t signal;
    // numpy::signal_from_buffer(features, 4, &signal);
    //
    // ei_impulse_result_t result;
    // EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);
    //
    // if (res != EI_IMPULSE_OK) {
    //     Serial.print(F("[TINYML] Erro na inferência: "));
    //     Serial.println(res);
    //     return _currentState;  // Manter estado anterior em caso de erro
    // }
    //
    // // Encontrar classe com maior confiança
    // uint8_t maxIdx = 0;
    // float maxVal = 0.0f;
    // for (uint8_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
    //     if (result.classification[i].value > maxVal) {
    //         maxVal = result.classification[i].value;
    //         maxIdx = i;
    //     }
    // }
    //
    // // Mapear índice para SystemState
    // // Labels no Edge Impulse: "STATE_1", "STATE_2", "STATE_3", "STATE_4"
    // switch (maxIdx) {
    //     case 0: return SystemState::STATE_1_IDEAL;
    //     case 1: return SystemState::STATE_2_HEAT;
    //     case 2: return SystemState::STATE_3_WATER_STRESS;
    //     case 3: return SystemState::STATE_4_FUNGUS_RISK;
    //     default: return SystemState::STATE_1_IDEAL;
    // }

    return SystemState::STATE_1_IDEAL;  // Fallback
    #endif
}

// ============================================================
// StateMachine::executeState() — Aplica ações do estado
// ============================================================
void StateMachine::executeState(SystemState newState, const SensorData& data) {
    // Detectar transição de estado
    if (newState != _currentState) {
        _previousState = _currentState;
        _currentState = newState;
        _logger.logStateTransition(_previousState, _currentState);
    }

    // Executar ações correspondentes ao estado
    switch (_currentState) {

        // ====================================================
        // ESTADO 3 — Estresse Hídrico (Prioridade 1)
        // Ação: Bomba em pulsos curtos de 3 segundos
        // ====================================================
        case SystemState::STATE_3_WATER_STRESS:
            _actuators.unlockPump();
            if (!_actuators.isPumpRunning()) {
                _actuators.startPumpPulse();
                _logger.logActuatorAction("ESTADO 3: Bomba acionada em pulso de 3s (estresse hídrico).");
            }
            // Ventilador mantém modo anterior ou desliga
            break;

        // ====================================================
        // ESTADO 4 — Risco de Fungos (Prioridade 2)
        // Ação: BLOQUEAR bomba + ventilador 100%
        // ====================================================
        case SystemState::STATE_4_FUNGUS_RISK:
            _actuators.lockPump();
            _actuators.setFanMode(FanMode::ON_CONTINUOUS);
            _logger.logActuatorAction("ESTADO 4: Bomba BLOQUEADA. Ventilador 100% (risco de fungos).");
            break;

        // ====================================================
        // ESTADO 2 — Calor (Temperatura > 25°C)
        // Ação: Ventilador ininterrupto
        // ====================================================
        case SystemState::STATE_2_HEAT:
            _actuators.unlockPump();
            _actuators.stopPump();
            _actuators.setFanMode(FanMode::ON_CONTINUOUS);
            _logger.logActuatorAction("ESTADO 2: Ventilador ininterrupto (temperatura elevada).");
            break;

        // ====================================================
        // ESTADO 1 — Clima Ideal (15°C a 25°C)
        // Ação: Bomba OFF + ventilador intermitente
        // ====================================================
        case SystemState::STATE_1_IDEAL:
            _actuators.unlockPump();
            _actuators.stopPump();
            _actuators.setFanMode(FanMode::INTERMITTENT);
            _logger.logActuatorAction("ESTADO 1: Clima ideal. Ventilador intermitente (5min/30min).");
            break;
    }
}

// ============================================================
// StateMachine::checkLightWarning() — PASSO 2 (Independente)
// ============================================================
void StateMachine::checkLightWarning(const SensorData& data) {
    if (data.isExtremeLight) {
        _logger.logLightWarning();
    }
}

// ============================================================
// StateMachine::getCurrentState()
// ============================================================
SystemState StateMachine::getCurrentState() const {
    return _currentState;
}

// ============================================================
// StateMachine::stateToString()
// ============================================================
const char* StateMachine::stateToString(SystemState state) {
    switch (state) {
        case SystemState::STATE_1_IDEAL:        return "ESTADO_1_IDEAL";
        case SystemState::STATE_2_HEAT:         return "ESTADO_2_CALOR";
        case SystemState::STATE_3_WATER_STRESS: return "ESTADO_3_ESTRESSE_HIDRICO";
        case SystemState::STATE_4_FUNGUS_RISK:  return "ESTADO_4_RISCO_FUNGOS";
        default:                                return "DESCONHECIDO";
    }
}
