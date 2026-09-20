/*
 * ============================================================
 * actuators.cpp — Implementação do Módulo de Atuadores
 * Estufa Inteligente de Alface Crespo | ESP32 + TinyML
 * ============================================================
 *
 * Controle non-blocking dos relés da bomba d'água e do
 * micro-ventilador usando millis(). ZERO uso de delay().
 *
 * Bomba: opera em pulsos de 3 segundos (ESTADO 3).
 * Ventilador: 3 modos — OFF, ON contínuo, Intermitente.
 * Relés: ativo em LOW (nível lógico invertido).
 */

#include "actuators.h"

// ============================================================
// ActuatorManager::begin()
// ============================================================
void ActuatorManager::begin() {
    // Configura pinos como saída
    pinMode(PIN_RELAY_PUMP, OUTPUT);
    pinMode(PIN_RELAY_FAN, OUTPUT);

    // Desliga tudo no início (relé ativo LOW → HIGH = desligado)
    setPumpRelay(false);
    setFanRelay(false);

    // Inicializa estado interno
    _pumpActive    = false;
    _pumpLocked    = false;
    _pumpStartTime = 0;

    _fanMode       = FanMode::OFF;
    _fanOn         = false;
    _fanToggleTime = millis();

    Serial.println(F("[ACTUATORS] Módulo de atuadores inicializado."));
    Serial.print(F("[ACTUATORS] Relé Bomba no GPIO "));
    Serial.println(PIN_RELAY_PUMP);
    Serial.print(F("[ACTUATORS] Relé Ventilador no GPIO "));
    Serial.println(PIN_RELAY_FAN);
}

// ============================================================
// ActuatorManager::update()
// ============================================================
void ActuatorManager::update() {
    updatePump();
    updateFan();
}

// ============================================================
// Controle da Bomba d'Água
// ============================================================

void ActuatorManager::startPumpPulse() {
    // Verifica trava de segurança
    if (_pumpLocked) {
        Serial.println(F("[ACTUATORS] Bomba BLOQUEADA (trava de segurança ativa)."));
        return;
    }

    // Inicia pulso apenas se não está já em curso
    if (!_pumpActive) {
        _pumpActive = true;
        _pumpStartTime = millis();
        setPumpRelay(true);
        Serial.println(F("[ACTUATORS] Bomba LIGADA (pulso de 3s iniciado)."));
    }
}

void ActuatorManager::stopPump() {
    if (_pumpActive) {
        _pumpActive = false;
        setPumpRelay(false);
        Serial.println(F("[ACTUATORS] Bomba DESLIGADA."));
    }
}

void ActuatorManager::lockPump() {
    _pumpLocked = true;
    stopPump();  // Desliga imediatamente se estiver ligada
    Serial.println(F("[ACTUATORS] Bomba TRAVADA (segurança anti-fungos)."));
}

void ActuatorManager::unlockPump() {
    if (_pumpLocked) {
        _pumpLocked = false;
        Serial.println(F("[ACTUATORS] Bomba DESTRAVADA."));
    }
}

bool ActuatorManager::isPumpRunning() const {
    return _pumpActive;
}

bool ActuatorManager::isPumpLocked() const {
    return _pumpLocked;
}

// ============================================================
// Controle do Micro-Ventilador
// ============================================================

void ActuatorManager::setFanMode(FanMode mode) {
    if (_fanMode == mode) return;  // Evita reset desnecessário do timer

    _fanMode = mode;
    _fanToggleTime = millis();

    switch (mode) {
        case FanMode::OFF:
            _fanOn = false;
            setFanRelay(false);
            Serial.println(F("[ACTUATORS] Ventilador DESLIGADO."));
            break;

        case FanMode::ON_CONTINUOUS:
            _fanOn = true;
            setFanRelay(true);
            Serial.println(F("[ACTUATORS] Ventilador LIGADO (modo contínuo)."));
            break;

        case FanMode::INTERMITTENT:
            // Começa ligado no modo intermitente
            _fanOn = true;
            setFanRelay(true);
            Serial.println(F("[ACTUATORS] Ventilador em modo INTERMITENTE (5min ON / 30min OFF)."));
            break;
    }
}

FanMode ActuatorManager::getFanMode() const {
    return _fanMode;
}

bool ActuatorManager::isFanOn() const {
    return _fanOn;
}

// ============================================================
// Desligar tudo
// ============================================================

void ActuatorManager::allOff() {
    stopPump();
    _pumpLocked = false;
    setFanMode(FanMode::OFF);
    Serial.println(F("[ACTUATORS] TODOS os atuadores DESLIGADOS."));
}

// ============================================================
// Métodos privados — Timers non-blocking
// ============================================================

void ActuatorManager::updatePump() {
    if (!_pumpActive) return;

    unsigned long elapsed = millis() - _pumpStartTime;

    // Pulso de 3 segundos completo → desliga
    if (elapsed >= PUMP_PULSE_ON_MS) {
        _pumpActive = false;
        setPumpRelay(false);
        Serial.println(F("[ACTUATORS] Bomba: pulso de 3s concluído. Desligada."));
    }
}

void ActuatorManager::updateFan() {
    // Só gerencia timer no modo intermitente
    if (_fanMode != FanMode::INTERMITTENT) return;

    unsigned long elapsed = millis() - _fanToggleTime;

    if (_fanOn) {
        // Ventilador está ligado — verificar se já passou 5 minutos
        if (elapsed >= FAN_INTERMITTENT_ON_MS) {
            _fanOn = false;
            setFanRelay(false);
            _fanToggleTime = millis();
            Serial.println(F("[ACTUATORS] Ventilador intermitente: DESLIGADO (pausa de 30min)."));
        }
    } else {
        // Ventilador está desligado — verificar se já passou 30 minutos
        if (elapsed >= FAN_INTERMITTENT_OFF_MS) {
            _fanOn = true;
            setFanRelay(true);
            _fanToggleTime = millis();
            Serial.println(F("[ACTUATORS] Ventilador intermitente: LIGADO (ciclo de 5min)."));
        }
    }
}

// ============================================================
// Controle físico dos relés (inversão de nível)
// ============================================================

void ActuatorManager::setPumpRelay(bool active) {
    digitalWrite(PIN_RELAY_PUMP, active ? RELAY_ACTIVE : RELAY_INACTIVE);
}

void ActuatorManager::setFanRelay(bool active) {
    digitalWrite(PIN_RELAY_FAN, active ? RELAY_ACTIVE : RELAY_INACTIVE);
}
