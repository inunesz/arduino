# Reprodutor musical com Arduino UNO — CCP020

Projeto de um reprodutor de melodias monofônicas para a disciplina **CCP020 — Digital Experience Ultimate**, do professor Isaac. O sistema utiliza Arduino UNO, LCD 16×2 I2C, quatro botões, dois LEDs e um buzzer passivo.

> **Antes da entrega:** substitua os campos entre colchetes, acrescente fotos e vídeos reais e registre somente testes efetivamente realizados. Este README é um rascunho para revisão dos integrantes, não um registro de experimentos já comprovados.

**Integrantes:**
- [Nome do integrante 1 — matrícula]
- [Nome do integrante 2 — matrícula, se houver]

## 1. Introdução

O projeto aborda a construção de uma interface simples para selecionar e reproduzir músicas em um sistema embarcado com recursos limitados. O usuário navega por uma biblioteca de cinco melodias, acompanha a seleção pelo LCD e controla a execução por botões físicos.

O objetivo é integrar interface, geração de áudio e controle de estados sem bloquear a leitura dos botões. Outro objetivo é preservar a memória RAM do Arduino UNO, armazenando as sequências musicais e seus nomes na memória FLASH com `PROGMEM`.

### Funcionalidades implementadas no código

- Biblioteca com cinco melodias, identificadas como **Musica1** a **Musica5**.
- Menu de seleção com navegação circular por UP e DOWN.
- Reprodução de uma nota por vez no buzzer passivo.
- Play/Pause para iniciar, pausar e retomar a reprodução.
- Stop para interromper e retornar ao menu.
- LED verde durante a reprodução e vermelho durante a pausa.
- Retorno automático ao menu quando a melodia termina.
- Tratamento de oscilações dos botões por debounce de 30 ms.

## 2. Metodologia

### 2.1. Materiais

| Quantidade | Componente | Observação |
|---|---|---|
| 1 | Arduino UNO | Plataforma de controle |
| 1 | LCD 16×2 I2C | Configuração do código: expansor MCP23008 em 0x20 |
| 4 | Push buttons | UP, DOWN, Play/Pause e Stop |
| 1 | Buzzer passivo ou piezo | Reprodução por `tone()` |
| 1 | LED verde | Indica reprodução |
| 1 | LED vermelho | Indica pausa |
| 2 | Resistores de 220 a 330 Ω | Um em série com cada LED |
| 1 | Protoboard | Montagem das conexões |
| Conforme necessário | Jumpers | Interligação dos componentes |
| 1 | Cabo USB | Necessário para programação da placa física |

### 2.2. Pinagem

| Componente | Pino do Arduino UNO |
|---|---|
| Botão UP | D5 |
| Botão DOWN | D4 |
| Botão Play/Pause | D3 |
| Botão Stop | D2 |
| LED verde | D9 |
| LED vermelho | D8 |
| Buzzer passivo — positivo | D10 |
| LCD — SDA | A4 |
| LCD — SCL | A5 |
| LCD — VCC | 5 V |
| LCD — GND | GND |

Os botões utilizam `INPUT_PULLUP`: cada um deve conectar seu pino ao **GND quando pressionado**. Assim, a leitura é `HIGH` em repouso e `LOW` durante o acionamento, sem necessidade de resistores externos para os botões.

Cada LED deve ter um resistor em série e o cátodo ligado ao GND. O terminal negativo do buzzer deve ir ao GND. Todos os componentes precisam compartilhar o mesmo GND do Arduino.

> Não conecte um alto-falante de baixa impedância diretamente ao pino do Arduino. O circuito descrito utiliza piezo ou buzzer passivo.

### 2.3. Montagem

1. Conectar a alimentação e o GND do circuito.
2. Conectar o LCD ao barramento I2C, usando A4 para SDA e A5 para SCL.
3. Conectar os quatro botões entre seus respectivos pinos e GND.
4. Montar os LEDs com resistores em série.
5. Conectar o buzzer passivo ao D10 e ao GND.
6. Conferir as conexões antes de iniciar a simulação ou alimentar a placa física.

**Foto da montagem:** [Inserir foto ou captura real e sua legenda.]

**Circuito no Tinkercad:** [Inserir link compartilhável, se disponível.]

### 2.4. Organização do software

O programa é organizado em três estados:

| Estado | Comportamento | LEDs |
|---|---|---|
| `MENU` | Exibe a seleção e permite navegar pelas melodias | Ambos apagados |
| `TOCANDO` | Reproduz a sequência de notas selecionada | Verde aceso |
| `PAUSADO` | Interrompe o som e preserva a posição da reprodução | Vermelho aceso |

No menu, Play inicia a melodia selecionada. Durante a execução, o mesmo botão alterna entre reprodução e pausa. Stop tem prioridade sobre os demais comandos e retorna ao menu, mantendo a seleção atual.

A temporização utiliza `millis()`, sem `delay()` na lógica de reprodução. Isso permite consultar os botões enquanto a música toca. Ao retomar uma pausa, o código compensa o tempo pausado para continuar a nota em andamento.

Cada nota contém uma frequência em hertz e uma duração em milissegundos. Frequência zero representa silêncio. O programa prevê um intervalo de articulação de 8 ms ao final das notas, para distinguir notas consecutivas.

### 2.5. Uso da memória

O Arduino UNO possui 2 KB de SRAM. Para reduzir o consumo dessa memória, o código armazena as sequências de notas, os nomes e a tabela da biblioteca musical na FLASH com `PROGMEM`.

A leitura utiliza `memcpy_P()`. Durante a execução, o programa copia para a RAM os dados necessários à reprodução e à exibição, em vez de carregar todas as melodias de uma vez.

## 3. Execução do projeto

### 3.1. No Tinkercad

1. Abrir o circuito com Arduino UNO e LCD I2C compatível com MCP23008.
2. Acessar **Código** e selecionar a edição em texto.
3. Colar o sketch completo.
4. Iniciar a simulação.
5. Conferir o menu e executar os testes descritos na próxima seção.

O sketch utiliza a biblioteca **Adafruit LiquidCrystal**, incluída pelo cabeçalho `Adafruit_LiquidCrystal.h`.

### 3.2. Na Arduino IDE

1. Instalar **Adafruit LiquidCrystal** pelo Gerenciador de Bibliotecas e as dependências solicitadas.
2. Abrir o sketch e salvá-lo como `reprodutor_ccp020.ino`.
3. Selecionar a placa **Arduino Uno** e a porta correspondente.
4. Compilar e carregar o programa.
5. Testar o circuito físico com a pinagem indicada.

**Compatibilidade do LCD:** esta versão utiliza MCP23008 no endereço 0x20, configurado no construtor como `lcd(0)`. Módulos físicos com **PCF8574**, comuns em 0x27 e 0x3F, exigem adaptação da biblioteca e do driver. Apenas alterar o endereço não resolve essa diferença.

## 4. Experimentos e validação

A tabela abaixo é um **roteiro de testes**. Preencha a coluna de resultado somente após executar cada procedimento.

| Teste | Procedimento | Resultado esperado | Resultado observado |
|---|---|---|---|
| Inicialização | Iniciar a simulação ou energizar a placa | Menu visível, buzzer silencioso e LEDs apagados | [Preencher] |
| Navegação | Pressionar UP e DOWN | Seleção muda e circula entre as cinco músicas | [Preencher] |
| Reprodução | Selecionar uma música e pressionar Play | Nome exibido, som no buzzer e LED verde aceso | [Preencher] |
| Pausa | Pressionar Play/Pause enquanto toca | Som interrompido e LED vermelho aceso | [Preencher] |
| Retomada | Pressionar Play/Pause novamente | Reprodução continua sem reiniciar a melodia | [Preencher] |
| Stop | Pressionar Stop durante reprodução e pausa | Som interrompido, LEDs apagados e retorno ao menu | [Preencher] |
| Fim da melodia | Aguardar a execução completa | Retorno automático ao menu | [Preencher] |
| Biblioteca completa | Reproduzir Musica1 a Musica5 | Cada seleção executa sua própria sequência | [Preencher] |
| Debounce | Realizar acionamentos individuais dos botões | Um evento por acionamento estável | [Preencher] |

### Evidências

Organize fotos e vídeos em uma sequência que permita acompanhar o funcionamento:

1. **Montagem geral:** [Inserir foto real e explicar as conexões principais.]
2. **Menu e navegação:** [Inserir foto ou vídeo real da mudança de seleção.]
3. **Reprodução:** [Inserir vídeo com áudio e LED verde visível.]
4. **Pausa e retomada:** [Inserir vídeo mostrando o LED vermelho e a continuidade da melodia.]
5. **Stop e finalização:** [Inserir vídeo mostrando o retorno ao menu.]

**Ambiente testado:** [Tinkercad, placa física ou ambos.]

**Observações sobre áudio:** [Registrar ruídos, interrupções, alterações de notas e diferenças entre a simulação e a montagem física, se observados.]

A saída de `tone()` é uma onda quadrada; portanto, o timbre do buzzer não equivale ao de um instrumento musical. Qualidade sonora, ruídos e estabilidade devem ser avaliados no ambiente efetivamente utilizado.

## 5. Tecnologias utilizadas

- **Arduino UNO / ATmega328P:** execução do programa e controle dos periféricos.
- **C++ com framework Arduino:** implementação do sketch.
- **Adafruit LiquidCrystal:** controle do LCD com expansor compatível.
- **I2C:** comunicação com o display.
- **`PROGMEM` e `memcpy_P()`:** armazenamento e leitura dos dados na FLASH.
- **`tone()` e `noTone()`:** geração e interrupção do áudio.
- **`millis()`:** temporização da reprodução e do debounce.
- **Tinkercad Circuits:** montagem e simulação do circuito.
- **Arduino IDE:** compilação e programação da placa física, quando utilizada.
- **Git e GitHub:** versionamento e registro das contribuições.

## 6. Conclusão

A solução implementada integra seleção de melodias, reprodução monofônica, pausa, retomada e parada em uma interface com quatro botões e LCD. A máquina de estados separa o menu da reprodução e da pausa, enquanto os LEDs oferecem uma indicação visual do estado atual.

O uso de `millis()` permite controlar a duração das notas sem bloquear a consulta aos botões. O armazenamento em FLASH preserva a SRAM limitada do Arduino UNO, e o debounce reduz eventos indesejados causados pela oscilação dos contatos.

**Validação final:** [Descrever quais funcionalidades foram confirmadas nos testes, quais dificuldades ocorreram, como foram resolvidas e quais limitações permaneceram. Não declarar sucesso em testes ainda não realizados.]

Como melhorias futuras, podem ser considerados controle de volume com circuito apropriado, ajuste de andamento e ampliação da biblioteca musical.

## 7. Versionamento e participação

Manter o repositório privado e adicionar os integrantes como colaboradores, conforme as orientações do projeto. Cada participante deve registrar contribuições próprias por meio de commits que representem a evolução real do trabalho.

**Contribuições:**
- [Integrante 1: atividades efetivamente realizadas.]
- [Integrante 2: atividades efetivamente realizadas, se houver.]

## 8. Uso de inteligência artificial

Foi utilizada assistência de IA na elaboração e revisão de trechos de código e deste rascunho de documentação. Os integrantes devem revisar, compreender, adaptar e validar o material antes da entrega, observando as regras da disciplina. Fotos, vídeos e resultados experimentais devem ser produzidos a partir do circuito real ou da simulação efetivamente executada.

[Complementar com uma descrição fiel do uso de IA e da revisão realizada pela equipe.]
