# 🧪 Testes de Hardware — Estufa Inteligente de Alface

Cada arquivo nesta pasta é um teste **independente** para verificar se um
dispositivo/componente está funcionando corretamente.

## Como Usar

1. **Copie** o conteúdo do teste desejado para `src/main.cpp`
   (ou renomeie o arquivo para `src/main.cpp`).
2. **Compile e envie** para o ESP32:
   ```
   pio run -t upload
   ```
3. **Abra o Serial Monitor** a 115200 baud:
   ```
   pio device monitor
   ```
4. **Siga as instruções** exibidas no Serial Monitor.
5. Ao terminar, **restaure** o `src/main.cpp` original.

## Lista de Testes

| # | Arquivo                          | Dispositivo Testado                          |
|---|----------------------------------|----------------------------------------------|
| 1 | `teste_esp32.cpp`                | ESP32 DevKit WROOM32 (CPU, WiFi, RAM, Flash) |
| 2 | `teste_dht22.cpp`                | Sensor DHT22 (AM2302) — Temp & Umidade do Ar |
| 3 | `teste_solo.cpp`                 | Sensor Capacitivo de Umidade do Solo v1.2     |
| 4 | `teste_ldr.cpp`                  | Sensor de Luz LDR (Digital/Analógico)         |
| 5 | `teste_rele.cpp`                 | Módulo Relé 5V 2 Canais                       |
| 6 | `teste_bomba.cpp`                | Mini Bomba D'Água Submersa 3-6V               |
| 7 | `teste_ventilador.cpp`           | Micro Ventilador Cooler 40x40mm 5V            |
| 8 | `teste_completo.cpp`             | ✅ Teste integrado de TODOS os componentes     |

## Pinagem (conforme `config.h`)

```
DHT22 DATA       → GPIO 4
Solo Capacitivo  → GPIO 34 (ADC)
LDR Analógico    → GPIO 35 (ADC)
Relé 1 (Bomba)   → GPIO 25
Relé 2 (Ventil.) → GPIO 26
```

> ⚠️ **Atenção:** Os testes do relé, bomba e ventilador acionam os componentes
> fisicamente. Certifique-se de que as conexões estão seguras antes de executar.
