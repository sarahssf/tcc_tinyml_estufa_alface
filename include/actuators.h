/*
 * ============================================================
 * actuators.h — Interface do Módulo de Atuadores
 * Estufa Inteligente de Alface Crespo | ESP32 + TinyML
 * ============================================================
 */

#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <Arduino.h>
#include "config.h"

// ============================================================
// Modos de operação do ventilador
// ============================================================
enum class FanMode : uint8_t {
    OFF,            // Desligado
    ON_CONTINUOUS,  // Ligado ininterruptamente (ESTADO 2 e 4)
    INTERMITTENT    // 5 min ligado / 30 min desligado (ESTADO 1)
};

// ============================================================
// Classe ActuatorManager
// ============================================================
class ActuatorManager {
public:
    /// @brief Inicializa os pinos dos relés como OUTPUT.
    void begin();

    /// @brief Atualiza os timers internos (pulso da bomba, intermitência).
    ///        Deve ser chamado a cada iteração do loop().
    void update();

    // --- Controle da Bomba d'Água ---

    /// @brief Inicia um pulso de irrigação (3 segundos ON via millis).
    void startPumpPulse();

    /// @brief Desliga a bomba imediatamente.
    void stopPump();

    /// @brief Ativa a trava de segurança (bloqueia a bomba).
    void lockPump();

    /// @brief Desativa a trava de segurança.
    void unlockPump();

    /// @brief Verifica se a bomba está atualmente em pulso.
    bool isPumpRunning() const;

    /// @brief Verifica se a bomba está bloqueada.
    bool isPumpLocked() const;

    // --- Controle do Micro-Ventilador ---

    /// @brief Define o modo de operação do ventilador.
    void setFanMode(FanMode mode);

    /// @brief Retorna o modo atual do ventilador.
    FanMode getFanMode() const;

    /// @brief Verifica se o ventilador está ligado agora.
    bool isFanOn() const;

    // --- Desligar tudo (emergência) ---

    /// @brief Desliga bomba e ventilador imediatamente.
    void allOff();

private:
    // --- Estado da Bomba ---
    bool          _pumpActive;
    bool          _pumpLocked;
    unsigned long _pumpStartTime;

    // --- Estado do Ventilador ---
    FanMode       _fanMode;
    bool          _fanOn;
    unsigned long _fanToggleTime;

    /// @brief Atualiza o timer de pulso da bomba.
    void updatePump();

    /// @brief Atualiza o timer do modo intermitente do ventilador.
    void updateFan();

    /// @brief Aciona o relé da bomba (nível físico).
    void setPumpRelay(bool active);

    /// @brief Aciona o relé do ventilador (nível físico).
    void setFanRelay(bool active);
};

#endif // ACTUATORS_H
