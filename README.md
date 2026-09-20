# 🌱 Estufa Inteligente de Alface Crespo

**Sistema embarcado autônomo para monitoramento e controle climático de estufa de alface crespo (*Lactuca sativa*), utilizando ESP32 e TinyML — 100% offline.**

---

## 📋 Sobre o Projeto

Este projeto implementa uma estufa inteligente que monitora continuamente as condições climáticas e do solo, tomando decisões automatizadas para garantir o crescimento ideal da alface crespo. O sistema opera de forma completamente offline, sem dependência de Wi-Fi, Bluetooth, MQTT ou qualquer serviço em nuvem.

### Objetivo

Automatizar o controle de irrigação e ventilação de uma estufa de pequeno porte, utilizando sensores de baixo custo e um microcontrolador ESP32, com suporte futuro a inferência de Machine Learning embarcado (TinyML) para tomada de decisão mais inteligente.

### Características Principais

- ⚡ **100% Offline** — sem Wi-Fi, Bluetooth, MQTT ou HTTP
- 🔄 **Non-blocking** — zero uso de `delay()`, controlado por `millis()`
- 🧠 **TinyML Ready** — infraestrutura para coleta de dataset e inferência com Árvore de Decisão
- 📊 **Máquina de Estados** — 4 estados com prioridades definidas por fluxograma
- 🌡️ **Média Móvel** — filtragem de ruído nos sensores analógicos (ADC)
- 📝 **Dual-mode Logger** — log formatado (operação) ou CSV (coleta de dataset)

---

## 🏗️ Arquitetura do Sistema

```
┌─────────────────────────────────────────────────────────────────┐
│                         ESP32 DevKit V1                         │
│                                                                 │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────────┐  │
│  │ SensorManager│───▶│ StateMachine │───▶│ ActuatorManager  │  │
│  │              │    │   (FSM)      │    │                  │  │
│  │ • DHT22      │    │ 4 Estados    │    │ • Bomba (Relé 1) │  │
│  │ • Solo Cap.  │    │ Prioridades  │    │ • Ventil.(Relé 2)│  │
│  │ • LDR        │    │              │    │                  │  │
│  └──────────────┘    └──────┬───────┘    └──────────────────┘  │
│                             │                                   │
│                      ┌──────▼───────┐                          │
│                      │  DataLogger  │                          │
│                      │ Serial/CSV   │                          │
│                      └──────────────┘                          │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🔌 Hardware Utilizado

| Componente | Modelo | Função | GPIO |
|------------|--------|--------|:----:|
| Microcontrolador | ESP32 DevKit WROOM32 | Processamento central | — |
| Sensor de Temperatura e Umidade | DHT22 (AM2302) | Temp. e umidade do ar | 4 |
| Sensor de Umidade do Solo | Capacitivo v1.2 | Umidade do substrato | 34 (ADC) |
| Sensor de Luminosidade | LDR com divisor de tensão | Nível de luz | 35 (ADC) |
| Módulo Relé | 2 Canais 5V | Acionamento dos atuadores | 25, 26 |
| Bomba d'Água | Mini submersa 3-6V | Irrigação por pulso | via Relé 1 |
| Ventilador | Cooler 40x40mm 5V | Circulação de ar | via Relé 2 |

---

## 🔄 Máquina de Estados

O sistema opera em 4 estados mutuamente exclusivos, avaliados em ordem de prioridade:

| Prioridade | Estado | Condição | Ação |
|:----------:|--------|----------|------|
| 1ª | **ESTADO 3** — Estresse Hídrico | Solo seco (ADC > 3200) | Bomba em pulsos (3s ON / 5s OFF) |
| 2ª | **ESTADO 4** — Risco de Fungos | Umidade ar > 80% | Ventilador contínuo + bomba bloqueada |
| 3ª | **ESTADO 2** — Calor | Temperatura > 25°C | Ventilador contínuo |
| 4ª | **ESTADO 1** — Clima Ideal | Condições normais | Ventilador intermitente (5min ON / 30min OFF) |

> **Fiscalização de Luz** (independente dos estados): se a luminosidade ultrapassar o limiar extremo (ADC > 3800), o sistema emite alerta de queimadura solar.

---

## 📁 Estrutura do Projeto

```
estufa_alface/
├── include/
│   ├── config.h            # Configuração central (pinos, thresholds, tempos)
│   ├── sensors.h           # Interface do módulo de sensores
│   ├── actuators.h         # Interface do módulo de atuadores
│   ├── data_logger.h       # Interface do módulo de log
│   └── state_machine.h     # Interface da máquina de estados
├── src/
│   ├── main.cpp            # Entry point — orquestra todos os módulos
│   ├── sensors.cpp         # Leitura de DHT22, solo capacitivo e LDR
│   ├── actuators.cpp       # Controle de relés (bomba e ventilador)
│   ├── data_logger.cpp     # Log serial formatado e saída CSV
│   └── state_machine.cpp   # Lógica de decisão (FSM + TinyML futuro)
├── testes/
│   ├── teste_esp32.cpp     # Teste 1 — ESP32 (CPU, RAM, Flash, WiFi, GPIOs)
│   ├── teste_dht22.cpp     # Teste 2 — Sensor DHT22
│   ├── teste_solo.cpp      # Teste 3 — Sensor capacitivo de solo
│   ├── teste_ldr.cpp       # Teste 4 — Sensor de luz LDR
│   ├── teste_rele.cpp      # Teste 5 — Módulo relé 2 canais
│   ├── teste_bomba.cpp     # Teste 6 — Mini bomba d'água
│   ├── teste_ventilador.cpp# Teste 7 — Micro ventilador
│   └── teste_completo.cpp  # Teste 8 — Integração completa
├── docs/
│   └── TINYML_GUIDE.md     # Guia de integração TinyML com Edge Impulse
├── platformio.ini          # Configuração do PlatformIO
└── README.md
```

---

## 🚀 Como Usar

### Pré-requisitos

- [PlatformIO](https://platformio.org/) (CLI ou extensão VS Code)
- ESP32 DevKit V1 conectado via USB
- Sensores e atuadores conectados conforme a tabela de hardware

### Compilar e Enviar

```bash
# Compilar e fazer upload para o ESP32
pio run -t upload

# Abrir o Serial Monitor (115200 baud)
pio device monitor
```

### Testar Componentes Individualmente

Cada sensor/atuador possui um teste independente na pasta `testes/`:

```bash
# Exemplo: testar o DHT22
cp testes/teste_dht22.cpp src/main.cpp
pio run -t upload && pio device monitor

# Ao terminar, restaurar o firmware principal
cp src/main.cpp.bak src/main.cpp
```

---

## 🧠 TinyML — Coleta de Dados e Treinamento

O sistema possui um modo de coleta de dataset integrado para treinamento de um modelo de **Árvore de Decisão** no [Edge Impulse](https://studio.edgeimpulse.com).

### Ativar Coleta

Em `include/config.h`, descomente:

```cpp
#define DATASET_MODE
```

O ESP32 passará a imprimir no Serial Monitor em formato CSV:

```csv
timestamp_ms,temperature_c,humidity_air_pct,soil_moisture_adc,light_level_adc,state_label
2000,23.5,65.2,1800,2500,ESTADO_1_IDEAL
4000,27.3,72.1,1750,2600,ESTADO_2_CALOR
```

### Salvar Dados

```bash
pio device monitor > dataset.csv
```

Consulte o guia completo em [`docs/TINYML_GUIDE.md`](docs/TINYML_GUIDE.md).

---

## ⚙️ Configuração

Todos os parâmetros ajustáveis estão centralizados em [`include/config.h`](include/config.h):

| Parâmetro | Valor Padrão | Descrição |
|-----------|:------------:|-----------|
| `TEMP_MAX` | 25.0°C | Limite para ativar ventilação contínua |
| `TEMP_MIN` | 15.0°C | Limite inferior de temperatura |
| `HUMIDITY_AIR_MAX` | 80% | Limite para risco de fungos |
| `SOIL_DRY_THRESHOLD` | 3200 (ADC) | Solo criticamente seco |
| `SOIL_WET_THRESHOLD` | 1500 (ADC) | Solo encharcado |
| `LDR_EXTREME_THRESHOLD` | 3800 (ADC) | Alerta de luminosidade extrema |
| `PUMP_PULSE_ON_MS` | 3000ms | Duração do pulso de irrigação |
| `FAN_INTERMITTENT_ON_MS` | 5 min | Tempo ligado (modo intermitente) |
| `FAN_INTERMITTENT_OFF_MS` | 30 min | Tempo desligado (modo intermitente) |

---

## 📦 Dependências

| Biblioteca | Versão | Uso |
|------------|:------:|-----|
| [DHT sensor library](https://github.com/adafruit/DHT-sensor-library) | ≥ 1.4.6 | Leitura do sensor DHT22 |
| [Adafruit Unified Sensor](https://github.com/adafruit/Adafruit_Sensor) | ≥ 1.1.14 | Dependência do DHT library |

Gerenciadas automaticamente pelo PlatformIO via `platformio.ini`.

---

## 📄 Licença

Este projeto é de uso acadêmico/educacional.

---

<p align="center">
  Feito com 💚 para o cultivo inteligente de alface crespo 🥬
</p>
