/*
 * ============================================================
 * main.cpp — Entry Point do Sistema
 * Estufa Inteligente de Alface Crespo | ESP32 + TinyML
 * ============================================================
 *
 * Orquestra todos os módulos em um loop non-blocking:
 *   1. SensorManager  → Lê DHT22, Capacitivo, LDR
 *   2. StateMachine   → Avalia estado conforme fluxograma
 *   3. ActuatorManager → Controla relés (bomba + ventilador)
 *   4. DataLogger     → Log serial / coleta de dataset
 *
 * ZERO uso de delay(). ZERO WiFi/BT/MQTT/HTTP.
 * 100% Offline. Otimizado para ESP32.
 */

#include <Arduino.h>
#include "config.h"
#include "sensors.h"
#include "actuators.h"
#include "data_logger.h"
#include "state_machine.h"

// ============================================================
// Instâncias globais dos módulos
// ============================================================
static SensorManager   sensorManager;
static ActuatorManager actuatorManager;
static DataLogger      dataLogger;
static StateMachine    stateMachine(sensorManager, actuatorManager, dataLogger);

// ============================================================
// setup() — Inicialização única
// ============================================================
void setup() {
    // --- Serial ---
    Serial.begin(115200);
    while (!Serial) { ; }  // Aguarda conexão serial (necessário em algumas placas)

    // --- Inicializar módulos na ordem correta ---
    dataLogger.begin();       // 1. Logger primeiro (para capturar mensagens de init)
    sensorManager.begin();    // 2. Sensores
    actuatorManager.begin();  // 3. Atuadores
    stateMachine.begin();     // 4. Máquina de estados

    Serial.println(F(""));
    Serial.println(F("[SISTEMA] Inicialização completa. Loop principal iniciado."));
    Serial.println(F("[SISTEMA] Modo: 100% OFFLINE | Sem WiFi/BT/MQTT"));
    Serial.println(F(""));

    #ifdef DATASET_MODE
        Serial.println(F("[SISTEMA] *** MODO DATASET ATIVO ***"));
        Serial.println(F("[SISTEMA] Copie a saída CSV para treinar o modelo TinyML."));
        Serial.println(F(""));
    #endif
}

// ============================================================
// loop() — Ciclo principal non-blocking
// ============================================================
void loop() {
    // A StateMachine orquestra todo o ciclo:
    //   1. Lê sensores (se chegou o intervalo)
    //   2. Fiscaliza luz (log independente)
    //   3. Avalia estado (prioridade: solo → umidade → temp)
    //   4. Executa ações nos atuadores
    //   5. Atualiza timers (pulso bomba, intermitência ventilador)
    //   6. Loga dados periodicamente
    stateMachine.update();

    // Nota: Não há delay() aqui. O loop roda o mais rápido possível.
    // Os intervalos são controlados por millis() dentro de cada módulo.
}
