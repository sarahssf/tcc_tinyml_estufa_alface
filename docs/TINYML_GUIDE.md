# Guia de Integração TinyML — Edge Impulse + ESP32

## Visão Geral

Este guia explica como usar o firmware da estufa para coletar dados, treinar um modelo de **Árvore de Decisão** no Edge Impulse, e integrar a inferência de volta no ESP32 — tudo 100% offline.

```
┌──────────────┐     ┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│ 1. COLETA    │────▶│ 2. UPLOAD    │────▶│ 3. TREINO    │────▶│ 4. DEPLOY    │
│ (ESP32+CSV)  │     │ (Edge Impulse)│     │ (Árvore Dec.)│     │ (ESP32)      │
└──────────────┘     └──────────────┘     └──────────────┘     └──────────────┘
```

---

## 1. Coleta de Dados (Dataset)

### 1.1 Ativar Modo Dataset

No arquivo `include/config.h`, descomente a linha:

```cpp
#define DATASET_MODE
```

Recompile e faça upload para o ESP32.

### 1.2 Formato da Saída

O sistema imprimirá no Serial Monitor em formato CSV:

```csv
timestamp_ms,temperature_c,humidity_air_pct,soil_moisture_adc,light_level_adc,state_label
2000,23.5,65.2,1800,2500,ESTADO_1_IDEAL
4000,27.3,72.1,1750,2600,ESTADO_2_CALOR
6000,22.1,55.8,3300,2400,ESTADO_3_ESTRESSE_HIDRICO
8000,24.0,85.5,2100,2200,ESTADO_4_RISCO_FUNGOS
```

### 1.3 Capturar os Dados

1. Abra o Serial Monitor (115200 baud)
2. Copie toda a saída CSV para um arquivo `.csv`
3. **Importante**: Colete dados em diferentes condições reais:
   - Manhã, tarde e noite (variação de luz)
   - Dias quentes e amenos
   - Solo seco e irrigado recentemente
   - Após chuva (umidade alta)

### 1.4 Tamanho Recomendado do Dataset

| Estado | Amostras Mínimas | Condição |
|--------|-----------------|----------|
| ESTADO_1_IDEAL | 500+ | Temperatura 15-25°C, umidade <80%, solo OK |
| ESTADO_2_CALOR | 300+ | Temperatura >25°C |
| ESTADO_3_ESTRESSE_HIDRICO | 200+ | Solo seco (ADC >3200) |
| ESTADO_4_RISCO_FUNGOS | 200+ | Umidade ar >80% |

**Total recomendado**: 1200+ amostras distribuídas ao longo de vários dias.

### 1.5 Enriquecer o Dataset

Para cobrir cenários raros, você pode **forçar condições**:
- Aquecer o sensor DHT22 com ar quente (secador) → gera amostras do ESTADO 2
- Borrifar água perto do DHT22 → gera amostras do ESTADO 4
- Deixar o solo secar completamente → gera amostras do ESTADO 3
- Cobrir o LDR com papel → variação de luz

---

## 2. Upload no Edge Impulse

### 2.1 Criar Projeto

1. Acesse [studio.edgeimpulse.com](https://studio.edgeimpulse.com)
2. Crie um novo projeto: "Estufa Alface TinyML"

### 2.2 Preparar CSV

Remova a coluna `timestamp_ms` do CSV. O arquivo final deve ter:

```csv
temperature_c,humidity_air_pct,soil_moisture_adc,light_level_adc,state_label
23.5,65.2,1800,2500,ESTADO_1_IDEAL
27.3,72.1,1750,2600,ESTADO_2_CALOR
...
```

### 2.3 Upload dos Dados

1. Vá em **Data acquisition** → **Upload data**
2. Selecione o CSV
3. Configure:
   - **Label column**: `state_label`
   - **Data columns**: todas as demais (4 features)
4. Divida automaticamente: 80% treino / 20% teste

---

## 3. Treinamento do Modelo

### 3.1 Criar Impulse

Em **Create impulse**:

1. **Input block**: Tabular data
   - Features: `temperature_c`, `humidity_air_pct`, `soil_moisture_adc`, `light_level_adc`
2. **Processing block**: Flatten (ou Raw data)
3. **Learning block**: **Classification** (selecione "Classifier")

### 3.2 Configurar Classifier

- **Tipo**: Árvore de Decisão (ou Random Forest para melhor generalização)
- **Número de árvores**: 1 (para minimizar memória no ESP32)
- **Profundidade máxima**: 5-8 (suficiente para as 4 regras do fluxograma)
- **Epochs**: não aplicável (árvore de decisão é treinamento único)

### 3.3 Treinar e Validar

1. Clique em **Start training**
2. Verifique a **accuracy** (esperado: >95% pois as regras são determinísticas)
3. Analise a **confusion matrix** para garantir que todos os 4 estados são classificados corretamente

### 3.4 Métricas Esperadas

Como o dataset é gerado por regras determinísticas (if/else), o modelo deve atingir **~99% de acurácia**. Se não atingir:
- Verifique se o dataset está balanceado
- Aumente a profundidade da árvore
- Adicione mais amostras nos estados com menor acurácia

---

## 4. Deploy no ESP32

### 4.1 Exportar Biblioteca

1. Vá em **Deployment**
2. Selecione: **Arduino library**
3. Otimizações:
   - Target: **ESP32**
   - Quantização: **Float32** (árvore de decisão já é leve)
4. Clique em **Build** → Baixe o `.zip`

### 4.2 Instalar Biblioteca

**PlatformIO**:
1. Extraia o `.zip` na pasta `lib/` do projeto:
   ```
   estufa_alface/
   └── lib/
       └── estufa_alface_inferencing/
           ├── src/
           └── ...
   ```

**Arduino IDE**:
1. Sketch → Incluir Biblioteca → Adicionar biblioteca .ZIP

### 4.3 Ativar Inferência no Código

No arquivo `src/state_machine.cpp`:

1. **Descomente** a linha do include:
   ```cpp
   #include <estufa_alface_inferencing.h>
   ```

2. **Descomente** a linha define:
   ```cpp
   #define TINYML_INFERENCE
   ```

3. **Descomente** todo o bloco dentro de `#ifdef TINYML_INFERENCE` na função `evaluate()`.

4. Recompile e faça upload.

### 4.4 Uso de Memória Esperado

| Recurso | Sem TinyML | Com TinyML (Árvore) |
|---------|-----------|---------------------|
| Flash | ~250 KB | ~260 KB (+10 KB) |
| RAM | ~30 KB | ~31 KB (+1 KB) |

A árvore de decisão é o modelo mais leve possível — ideal para ESP32.

---

## 5. Vantagens do TinyML vs. Regras Fixas

| Aspecto | Regras (if/else) | TinyML (Árvore) |
|---------|------------------|-----------------|
| Precisão em thresholds | Binária (acima/abaixo) | Gradual (aprendida) |
| Adaptação ao ambiente | Requer recompilação | Retreino com novos dados |
| Tratamento de ruído | Média móvel | Implícito no modelo |
| Manutenção | Editar código C++ | Coletar dados + retreinar |
| Zonas de transição | Abrupta | Suave |

---

## 6. Ciclo de Melhoria Contínua

```
    ┌─────────────────────────────────────────────┐
    │                                             │
    ▼                                             │
 [Rodar firmware]──▶[Coletar dados]──▶[Retreinar]─┘
    │                     │
    │                     ▼
    │              [Validar in-loco]
    │                     │
    │                     ▼
    └───────────[Deploy novo modelo]
```

1. Rode o firmware com `DATASET_MODE` por 1-2 semanas
2. Colete CSV em diferentes estações/clima
3. Retreine o modelo no Edge Impulse
4. Exporte e substitua a biblioteca
5. Repita sazonalmente (verão vs. inverno)
