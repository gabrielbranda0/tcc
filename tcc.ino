#include <SPI.h>
#include <TFT_eSPI.h>
#include <Arduino.h>

#define TFT_CS   5
#define TFT_DC   15
#define TFT_RST  4

const int pinoBotao16 = 16; // Pino do botão push
const int pinoBotao17 = 17; // Pino do botão push (tecla "D")
const int pinoBotao19 = 19; // Pino do botão push (tecla "U")

bool botaoPressionado16 = false; // Flag para indicar que o botão foi pressionado
bool botaoPressionado17 = false; // Flag para indicar que o botão foi pressionado
bool botaoPressionado19 = false; // Flag para indicar que o botão "U" foi pressionado


TFT_eSPI tft;

const int numLinhas = 7;
const int numColunas = 2;
const int larguraTela = 478;
const int alturaTela = 250;
const int alturaCabecalho = 40;
const int alturaLinha = 30;
const int larguraColuna = larguraTela / numColunas;

int linhaSelecionada = 0; // Linha atualmente selecionada
bool linhaMudou = true; // Flag para indicar se a linha selecionada mudou
bool linhaMudouColuna2 = false; // Flag para indicar se a linha selecionada mudou para a Coluna 2

String dadosColuna1[numLinhas] = { "Dado 1A", "Dado 2A", "Dado 3A", "Dado 4A", "Dado 5A", "Dado 6A", "Dado 7A" };
String dadosColuna2[numLinhas] = { "", "", "", "", "", "", "" }; // Coluna 2 inicialmente em branco

void setup() {
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextFont(1);

  // Cabeçalho da tabela
  tft.fillRect(0, 0, larguraTela, alturaCabecalho, TFT_DARKGREY);
  tft.drawRect(0, 0, larguraTela, alturaCabecalho, TFT_WHITE);

  // Centralizar texto "Coluna 1" no cabeçalho
  int texto1Width = tft.textWidth("Coluna 1");
  int posTexto1X = (larguraColuna / 2) - (texto1Width / 2);
  tft.drawString("Coluna 1", posTexto1X, 10);

  // Centralizar texto "Coluna 2" no cabeçalho
  int texto2Width = tft.textWidth("Coluna 2");
  int posTexto2X = larguraColuna + (larguraColuna / 2) - (texto2Width / 2);
  tft.drawString("Coluna 2", posTexto2X, 10);

  // Linha vertical para dividir as colunas
  tft.drawFastVLine(larguraColuna, 0, alturaCabecalho, TFT_BLACK);

  // Linhas da tabela
  for (int linha = 0; linha < numLinhas; linha++) {
    int posY = alturaCabecalho + linha * alturaLinha;
    tft.fillRect(0, posY, larguraTela, alturaLinha, TFT_LIGHTGREY);
    tft.drawRect(0, posY, larguraTela, alturaLinha, TFT_BLACK);
    tft.drawFastVLine(larguraColuna, posY, alturaLinha, TFT_BLACK);
    tft.drawString(dadosColuna1[linha], 5, posY + 5);
    tft.drawString(dadosColuna2[linha], larguraColuna + 5, posY + 5);
  }

  // Moldura externa da tabela
  tft.drawRect(0, 0, larguraTela, alturaTela, TFT_BLACK);

  Serial.begin(115200); // Inicializar comunicação serial

  pinMode(pinoBotao16, INPUT_PULLUP); // Define o pino do botão como entrada
  pinMode(pinoBotao17, INPUT_PULLUP); // Define o pino do botão como entrada
  pinMode(pinoBotao19, INPUT_PULLUP); // Define o pino do botão como entrada
}

void loop() {

  // Verificar se a linha selecionada mudou
  if (linhaMudou) {
    // Desenhar a tabela com a linha selecionada destacada
    desenharTabela();
    linhaMudou = false;
  }

  // Verificar se o botão "D" foi pressionado
  bool botaoEstado17 = digitalRead(pinoBotao17);

  if (botaoEstado17 == LOW && !botaoPressionado17) {
   delay(50); // Debounce
   if (linhaSelecionada < numLinhas - 1) { // Verifica se é possível trocar a linha
     linhaMudou = true;
     linhaSelecionada++;
     linhaMudouColuna2 = false;
    }
    botaoPressionado17 = true; // Define a flag como true para evitar ação repetida
  }

    // Verifica se o botão foi solto
  if (botaoEstado17 == HIGH && botaoPressionado17) {
    botaoPressionado17 = false; // Reinicia a flag quando o botão é solto
  }

    // Verificar o estado do botão "U"
  bool botaoEstado19 = digitalRead(pinoBotao19);

    // Verificar se o botão "U" foi pressionado
  if (botaoEstado19 == LOW && !botaoPressionado19) {
    // Ação para o botão "U"
    delay(50); // Debounce
    if (linhaSelecionada > 0) { // Verifica se é possível trocar a linha
      linhaMudou = true;
      linhaSelecionada--;
      linhaMudouColuna2 = false;
    }
    botaoPressionado19 = true; // Define a flag como true para evitar ação repetida
  }

    // Verifica se o botão "U" foi solto
  if (botaoEstado19 == HIGH && botaoPressionado19) {
    botaoPressionado19 = false; // Reinicia a flag quando o botão é solto
  }

  if (Serial.available() > 0) {
    char tecla = Serial.read();
    if (tecla == 'U' && linhaSelecionada > 0) {
      linhaMudou = true;
      linhaSelecionada--;
      linhaMudouColuna2 = false;
    } else if (tecla == 'D' && linhaSelecionada < numLinhas - 1) {
      linhaMudou = true;
      linhaSelecionada++;
      linhaMudouColuna2 = false;
    }
  }

  // Verifica o estado do botão
  bool botaoEstado16 = digitalRead(pinoBotao16);

  // Verifica se o botão foi pressionado
  if (botaoEstado16 == LOW && !botaoPressionado16) {
    delay(50); // Debounce
    if (linhaSelecionada < numLinhas - 1) { // Verifica se é possível trocar a linha
      linhaMudou = true;
      linhaMudouColuna2 = true;
    }
    botaoPressionado16 = true; // Define a flag como true para evitar ação repetida
  }

  // Verifica se o botão foi solto
  if (botaoEstado16 == HIGH && botaoPressionado16) {
    botaoPressionado16 = false; // Reinicia a flag quando o botão é solto
  }

  // Trocar a linha para a Coluna 2, se necessário
  if (linhaMudouColuna2) {
    // Faz um loop para encontrar a primeira linha vazia na coluna 2
    for (int i = 0; i < numLinhas; i++) {
      if (dadosColuna2[i] == "") {
        dadosColuna2[i] = dadosColuna1[linhaSelecionada];
        dadosColuna1[linhaSelecionada] = "";
        organizarColuna1(); // Chama a função para organizar a Coluna 1
        linhaSelecionada = i;
        break;
      }
    }
    linhaMudouColuna2 = false;
  }
}

void desenharTabela() {
  // Desenhar cabeçalho da tabela
  tft.fillRect(0, 0, larguraTela, alturaCabecalho, TFT_DARKGREY);
  tft.drawRect(0, 0, larguraTela, alturaCabecalho, TFT_WHITE);
  int texto1Width = tft.textWidth("Coluna 1");
  int posTexto1X = (larguraColuna / 2) - (texto1Width / 2);
  tft.drawString("Coluna 1", posTexto1X, 10);
  int texto2Width = tft.textWidth("Coluna 2");
  int posTexto2X = larguraColuna + (larguraColuna / 2) - (texto2Width / 2);
  tft.drawString("Coluna 2", posTexto2X, 10);
  tft.drawFastVLine(larguraColuna, 0, alturaCabecalho, TFT_BLACK);

  // Desenhar linhas da tabela (apenas coluna 1 e coluna 2)
  for (int linha = 0; linha < numLinhas; linha++) {
    int posY = alturaCabecalho + linha * alturaLinha;
    if (linha == linhaSelecionada) {
      tft.fillRect(0, posY, larguraColuna, alturaLinha, TFT_BLUE);
      tft.setTextColor(TFT_BLACK);
    } else {
      tft.fillRect(0, posY, larguraColuna, alturaLinha, TFT_LIGHTGREY);
      tft.setTextColor(TFT_BLACK);
    }
    tft.drawRect(0, posY, larguraTela, alturaLinha, TFT_BLACK);
    tft.drawFastVLine(larguraColuna, posY, alturaLinha, TFT_BLACK);
    tft.drawString(dadosColuna1[linha], 5, posY + 5);
    tft.drawString(dadosColuna2[linha], larguraColuna + 5, posY + 5);
  }

  // Moldura externa da tabela
  tft.drawRect(0, 0, larguraTela, alturaTela, TFT_BLACK);
}
void organizarColuna1() {
  // Faz um loop para organizar a Coluna 1, eliminando linhas em branco entre as informações
  for (int i = 0; i < numLinhas - 1; i++) {
    if (dadosColuna1[i] == "" && dadosColuna1[i + 1] != "") {
      dadosColuna1[i] = dadosColuna1[i + 1];
      dadosColuna1[i + 1] = "";
    }
  }
}

