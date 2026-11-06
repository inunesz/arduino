// CCP020 - Reprodutor musical para Arduino UNO
// LCD I2C do Tinkercad com expansor MCP23008 no endereco padrao.
// Biblioteca: Adafruit_LiquidCrystal
// Botões INPUT_PULLUP: pressionado = LOW; ligar cada botao ao GND.
// Revise, teste e compreenda o codigo antes da apresentacao.

#include <Adafruit_LiquidCrystal.h>
#include <avr/pgmspace.h>
#include <string.h>

// 0 = endereco padrao do expansor MCP23008 (0x20).
// Nao e o endereco 0x27 de modulos PCF8574.
Adafruit_LiquidCrystal lcd(0);

// Pinagem conferida com o circuito do Tinkercad.
const byte PIN_UP = 5;
const byte PIN_DOWN = 4;
const byte PIN_PLAY = 3;
const byte PIN_STOP = 2;
const byte PIN_LED_VERDE = 9;      // CORRIGIDO (antes: 8)
const byte PIN_LED_VERMELHO = 8;   // CORRIGIDO (antes: 7)
const byte PIN_BUZZER = 10;        // CORRIGIDO (antes: 9)

// Frequencias sem nomes A4/A5, pois esses nomes ja identificam pinos.
const uint16_t DO4 = 262;
const uint16_t RE4 = 294;
const uint16_t MI4 = 330;
const uint16_t FA4 = 349;
const uint16_t SOL4 = 392;
const uint16_t LA4 = 440;
const uint16_t SI4 = 494;
const uint16_t DO5 = 523;
const uint16_t RE5 = 587;
const uint16_t MI5 = 659;
const uint16_t SOL3 = 196;

struct Nota {
  uint16_t frequencia;
  uint16_t duracao;  // Tempo total da nota, em milissegundos.
};

// Arranjos monofonicos simplificados. Todos os vetores ficam na FLASH.
const Nota Musica1[] PROGMEM = {
  {DO4,300},{DO4,150},{RE4,450},{DO4,450},{FA4,450},{MI4,900},
  {DO4,300},{DO4,150},{RE4,450},{DO4,450},{SOL4,450},{FA4,900},
  {DO4,300},{DO4,150},{DO5,450},{LA4,450},{FA4,450},{MI4,450},{RE4,900},
  {SI4,300},{SI4,150},{LA4,450},{FA4,450},{SOL4,450},{FA4,900}
};

const Nota Musica2[] PROGMEM = {
  {DO4,400},{DO4,400},{SOL4,400},{SOL4,400},{LA4,400},{LA4,400},{SOL4,800},
  {FA4,400},{FA4,400},{MI4,400},{MI4,400},{RE4,400},{RE4,400},{DO4,800},
  {SOL4,400},{SOL4,400},{FA4,400},{FA4,400},{MI4,400},{MI4,400},{RE4,800},
  {SOL4,400},{SOL4,400},{FA4,400},{FA4,400},{MI4,400},{MI4,400},{RE4,800},
  {DO4,400},{DO4,400},{SOL4,400},{SOL4,400},{LA4,400},{LA4,400},{SOL4,800},
  {FA4,400},{FA4,400},{MI4,400},{MI4,400},{RE4,400},{RE4,400},{DO4,800}
};

const Nota Musica3[] PROGMEM = {
  {MI4,350},{MI4,350},{FA4,350},{SOL4,350},{SOL4,350},{FA4,350},{MI4,350},{RE4,350},
  {DO4,350},{DO4,350},{RE4,350},{MI4,350},{MI4,525},{RE4,175},{RE4,700},
  {MI4,350},{MI4,350},{FA4,350},{SOL4,350},{SOL4,350},{FA4,350},{MI4,350},{RE4,350},
  {DO4,350},{DO4,350},{RE4,350},{MI4,350},{RE4,525},{DO4,175},{DO4,700},
  {RE4,350},{RE4,350},{MI4,350},{DO4,350},{RE4,350},{MI4,175},{FA4,175},{MI4,350},{DO4,350},
  {RE4,350},{MI4,175},{FA4,175},{MI4,350},{RE4,350},{DO4,350},{RE4,350},{SOL3,700},
  {MI4,350},{MI4,350},{FA4,350},{SOL4,350},{SOL4,350},{FA4,350},{MI4,350},{RE4,350},
  {DO4,350},{DO4,350},{RE4,350},{MI4,350},{RE4,525},{DO4,175},{DO4,700}
};

const Nota Musica4[] PROGMEM = {
  {DO4,400},{RE4,400},{MI4,400},{DO4,400},
  {DO4,400},{RE4,400},{MI4,400},{DO4,400},
  {MI4,400},{FA4,400},{SOL4,800},
  {MI4,400},{FA4,400},{SOL4,800},
  {SOL4,200},{LA4,200},{SOL4,200},{FA4,200},{MI4,400},{DO4,400},
  {SOL4,200},{LA4,200},{SOL4,200},{FA4,200},{MI4,400},{DO4,400},
  {DO4,400},{SOL3,400},{DO4,800},
  {DO4,400},{SOL3,400},{DO4,800}
};

const Nota Musica5[] PROGMEM = {
  {SOL4,450},{LA4,225},{SOL4,225},{FA4,450},{MI4,450},{FA4,450},{SOL4,900},
  {RE4,450},{MI4,450},{FA4,900},{MI4,450},{FA4,450},{SOL4,900},
  {SOL4,450},{LA4,225},{SOL4,225},{FA4,450},{MI4,450},{FA4,450},{SOL4,900},
  {RE4,900},{SOL4,900},{MI4,450},{DO4,1350}
};

// Nomes tambem na memoria de programa.
const char nomes[5][17] PROGMEM = {
  "Musica1",
  "Musica2",
  "Musica3",
  "Musica4",
  "Musica5"
};

struct Musica {
  const Nota* notas;
  uint16_t quantidade;
};

const Musica biblioteca[] PROGMEM = {
  {Musica1, sizeof(Musica1) / sizeof(Musica1[0])},
  {Musica2, sizeof(Musica2) / sizeof(Musica2[0])},
  {Musica3, sizeof(Musica3) / sizeof(Musica3[0])},
  {Musica4, sizeof(Musica4) / sizeof(Musica4[0])},
  {Musica5, sizeof(Musica5) / sizeof(Musica5[0])}
};
const byte TOTAL_MUSICAS = sizeof(biblioteca) / sizeof(biblioteca[0]);

enum Estado { MENU, TOCANDO, PAUSADO };
Estado estado = MENU;
byte selecionada = 0;
Musica musicaAtual;
Nota notaAtual;
uint16_t indiceNota = 0;
unsigned long inicioNota = 0;
unsigned long instantePausa = 0;
bool somLigado = false;

// Debounce real: a leitura precisa permanecer estavel por 30 ms.
struct Botao {
  byte pino;
  byte leituraAnterior;
  byte estadoEstavel;
  unsigned long ultimaMudanca;
};

// Prototipo explicito depois da definicao de Botao.
// Evita que o preprocessador Arduino gere este prototipo antes do tipo.
bool foiPressionado(Botao &botao);

Botao botaoUp = {PIN_UP, HIGH, HIGH, 0};
Botao botaoDown = {PIN_DOWN, HIGH, HIGH, 0};
Botao botaoPlay = {PIN_PLAY, HIGH, HIGH, 0};
Botao botaoStop = {PIN_STOP, HIGH, HIGH, 0};

bool foiPressionado(Botao &botao) {
  const byte leitura = digitalRead(botao.pino);
  const unsigned long agora = millis();
  if (leitura != botao.leituraAnterior) {
    botao.leituraAnterior = leitura;
    botao.ultimaMudanca = agora;
  }
  if (agora - botao.ultimaMudanca >= 30 && leitura != botao.estadoEstavel) {
    botao.estadoEstavel = leitura;
    return leitura == LOW;
  }
  return false;
}

void atualizarLeds() {
  digitalWrite(PIN_LED_VERDE, estado == TOCANDO ? HIGH : LOW);
  digitalWrite(PIN_LED_VERMELHO, estado == PAUSADO ? HIGH : LOW);
}

void escreverNome(byte numero) {
  char buffer[17];
  memcpy_P(buffer, nomes[numero], sizeof(buffer));
  buffer[16] = '\0';
  lcd.print(buffer);
}

void mostrarMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print('>');
  escreverNome(selecionada);
  lcd.setCursor(0, 1);
  lcd.print(' ');
  escreverNome((selecionada + 1) % TOTAL_MUSICAS);
}

void mostrarReproducao() {
  lcd.clear();
  lcd.setCursor(0, 0);
  if (estado == PAUSADO) lcd.print(F("Pausado"));
  else lcd.print(F("Tocando"));
  lcd.setCursor(0, 1);
  escreverNome(selecionada);
}

// Ultimos 10% do tempo de cada nota ficam em silencio.
// Isso permite distinguir notas consecutivas da mesma frequencia.
unsigned long duracaoSom() {
  return ((unsigned long)notaAtual.duracao * 9UL) / 10UL;
}

void silenciar() {
  noTone(PIN_BUZZER);
  somLigado = false;
}

void carregarNota() {
  memcpy_P(&notaAtual, musicaAtual.notas + indiceNota, sizeof(Nota));
  inicioNota = millis();
  silenciar();
  if (notaAtual.frequencia > 0) {
    tone(PIN_BUZZER, notaAtual.frequencia);
    somLigado = true;
  }
}

void voltarAoMenu() {
  silenciar();
  estado = MENU;
  indiceNota = 0;
  atualizarLeds();
  mostrarMenu();
}

void iniciarMusica() {
  memcpy_P(&musicaAtual, &biblioteca[selecionada], sizeof(Musica));
  indiceNota = 0;
  estado = TOCANDO;
  atualizarLeds();
  mostrarReproducao();
  carregarNota();
}

void pausarOuRetomar() {
  if (estado == TOCANDO) {
    instantePausa = millis();
    estado = PAUSADO;
    silenciar();
  } else if (estado == PAUSADO) {
    // Desconta a pausa do tempo transcorrido da nota.
    inicioNota += millis() - instantePausa;
    estado = TOCANDO;
    if (notaAtual.frequencia > 0 && millis() - inicioNota < duracaoSom()) {
      tone(PIN_BUZZER, notaAtual.frequencia);
      somLigado = true;
    }
  }
  atualizarLeds();
  mostrarReproducao();
}

void atualizarReproducao() {
  if (estado != TOCANDO) return;
  const unsigned long decorrido = millis() - inicioNota;
  if (somLigado && decorrido >= duracaoSom()) silenciar();
  if (decorrido >= notaAtual.duracao) {
    indiceNota++;
    if (indiceNota >= musicaAtual.quantidade) {
      voltarAoMenu();  // Fim da musica: sem repeticao automatica.
    } else {
      carregarNota();
    }
  }
}

void setup() {
  pinMode(PIN_UP, INPUT_PULLUP);
  pinMode(PIN_DOWN, INPUT_PULLUP);
  pinMode(PIN_PLAY, INPUT_PULLUP);
  pinMode(PIN_STOP, INPUT_PULLUP);
  pinMode(PIN_LED_VERDE, OUTPUT);
  pinMode(PIN_LED_VERMELHO, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  lcd.begin(16, 2);
  lcd.setBacklight(HIGH);
  silenciar();
  atualizarLeds();
  mostrarMenu();
}

void loop() {
  // Todos os botoes sao atualizados em todos os estados.
  const bool up = foiPressionado(botaoUp);
  const bool down = foiPressionado(botaoDown);
  const bool play = foiPressionado(botaoPlay);
  const bool stop = foiPressionado(botaoStop);

  // Stop tem prioridade, inclusive se houver cliques simultaneos.
  if (stop) {
    voltarAoMenu();
    return;
  }

  if (estado == MENU) {
    if (up) {
      selecionada = (selecionada + TOTAL_MUSICAS - 1) % TOTAL_MUSICAS;
      mostrarMenu();
    } else if (down) {
      selecionada = (selecionada + 1) % TOTAL_MUSICAS;
      mostrarMenu();
    }
    if (play) iniciarMusica();
  } else if (play) {
    pausarOuRetomar();
  }

  atualizarReproducao();
}
