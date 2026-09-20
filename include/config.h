/*
 * ============================================================
 * config.h — Configuração Central do Sistema
 * Estufa Inteligente de Alface Crespo | ESP32 + TinyML
 * ============================================================
 *
 * Todas as constantes do projeto ficam aqui: pinos, thresholds
 * climáticos e constantes de tempo. Alterar apenas este arquivo
 * para ajustar o comportamento do sistema.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================
// MODO DE OPERAÇÃO
// ============================================================
// Descomente a linha abaixo para ativar o modo de coleta de
// dataset (CSV via Serial) para treinamento TinyML.
// Em modo normal, comente esta linha.
// ------------------------------------------------------------
// #define DATASET_MODE

// ============================================================
// MAPEAMENTO DE PINOS (conforme diagrama de conexões)
// ============================================================

// --- Sensores (Entradas) ---
static constexpr uint8_t PIN_DHT22        = 4;   // GPIO 4  — DHT22 DATA (digital)
static constexpr uint8_t PIN_SOIL_SENSOR  = 34;  // GPIO 34 — Sensor Capacitivo AOUT (ADC)
static constexpr uint8_t PIN_LDR          = 35;  // GPIO 35 — LDR A0 (ADC)

// --- Atuadores (Saídas para Módulo Relé) ---
static constexpr uint8_t PIN_RELAY_PUMP   = 25;  // GPIO 25 — Relé 1 (Bomba d'Água)
static constexpr uint8_t PIN_RELAY_FAN    = 26;  // GPIO 26 — Relé 2 (Micro-ventilador)

// --- Tipo do sensor DHT ---
static constexpr uint8_t DHT_TYPE         = 22;  // DHT22 (AM2302)

// ============================================================
// THRESHOLDS CLIMÁTICOS
// ============================================================
// Ajustados para cultivo de alface crespo (Lactuca sativa)

// --- Temperatura (°C) ---
static constexpr float TEMP_MAX           = 25.0f;  // Acima: ventilador contínuo
static constexpr float TEMP_MIN           = 15.0f;  // Abaixo: considerar frio extremo

// --- Umidade do Ar (%) ---
static constexpr float HUMIDITY_AIR_MAX   = 80.0f;  // Acima: risco de fungos

// --- Umidade do Solo (valor ADC 12 bits: 0-4095) ---
// Sensor capacitivo: valor ALTO = solo seco, valor BAIXO = solo úmido
// Calibrar empiricamente para o substrato utilizado.
static constexpr uint16_t SOIL_DRY_THRESHOLD   = 3200;  // Acima = solo criticamente seco
static constexpr uint16_t SOIL_WET_THRESHOLD   = 1500;  // Abaixo = solo encharcado

// --- Luminosidade (valor ADC 12 bits: 0-4095) ---
// LDR com divisor de tensão: valor ALTO = muita luz
static constexpr uint16_t LDR_EXTREME_THRESHOLD = 3800;  // Pico extremo de luminosidade

// ============================================================
// CONSTANTES DE TEMPO (em milissegundos)
// ============================================================
// Todas as temporizações usam millis() — ZERO delay()

// --- Intervalo de leitura dos sensores ---
static constexpr unsigned long SENSOR_READ_INTERVAL_MS  = 2000UL;    // 2 segundos

// --- Bomba d'água (ESTADO 3) ---
static constexpr unsigned long PUMP_PULSE_ON_MS         = 3000UL;    // Pulso de 3 segundos
static constexpr unsigned long PUMP_PULSE_PAUSE_MS      = 5000UL;    // Pausa entre pulsos

// --- Ventilador modo intermitente (ESTADO 1) ---
static constexpr unsigned long FAN_INTERMITTENT_ON_MS   = 300000UL;  // 5 minutos ligado
static constexpr unsigned long FAN_INTERMITTENT_OFF_MS  = 1800000UL; // 30 minutos desligado

// --- Log serial ---
static constexpr unsigned long LOG_INTERVAL_MS          = 5000UL;    // Log a cada 5 segundos
static constexpr unsigned long DATASET_LOG_INTERVAL_MS  = 2000UL;    // Dataset a cada 2 segundos

// ============================================================
// MÉDIA MÓVEL DOS SENSORES
// ============================================================
static constexpr uint8_t MOVING_AVG_SAMPLES = 5;  // Janela de 5 amostras

// ============================================================
// CONFIGURAÇÃO DO RELÉ
// ============================================================
// Módulo relé ativo em LOW (nível lógico invertido)
static constexpr uint8_t RELAY_ACTIVE   = LOW;
static constexpr uint8_t RELAY_INACTIVE = HIGH;

#endif // CONFIG_H
