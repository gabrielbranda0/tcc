#include <SPI.h>
#include <TFT_eSPI.h>

#define TFT_CS   5
#define TFT_DC   15
#define TFT_RST  4

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

#include <FS.h>
#include "SPIFFS.h"

#define FILENAME "/cadeira.bmp"

void loadBitmap(const char *filename, int16_t x, int16_t y) {
  fs::File bmpFS;
  bmpFS = SPIFFS.open(filename, "r");
  if (!bmpFS) {
    Serial.println("File not found");
    return;
  }

  if (bmpFS.read() == 'B' && bmpFS.read() == 'M') {
    bmpFS.seek(18);
    int imgWidth = bmpFS.read() | (bmpFS.read() << 8);
    int imgHeight = bmpFS.read() | (bmpFS.read() << 8);

    bmpFS.seek(54);

    uint16_t imgColor;
    int y_pos = y + imgHeight - 1;

    for (int i = 0; i < imgHeight; i++, y_pos--) {
      for (int j = 0; j < imgWidth; j++) {
        uint8_t r = bmpFS.read();
        uint8_t g = bmpFS.read();
        uint8_t b = bmpFS.read();

        imgColor = tft.color565(r, g, b);
        tft.drawPixel(j + x, y_pos, imgColor);
      }
    }
    bmpFS.close();
  }
}

void setup() {
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextFont(1);

  if (!SPIFFS.begin(true)) {
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }

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
}

void loop() {
  // Verificar se a linha selecionada mudou
  if (linhaMudou) {
    // Desenhar a tabela com a linha selecionada destacada
    desenharTabela();
    linhaMudou = false;
  }

  // Verificar entrada do teclado para mudar a linha selecionada
  if (Serial.available() > 0) {
    char tecla = Serial.read();
    if (tecla == 'U' && linhaSelecionada > 0) { // Tecla de seta para cima
      linhaMudou = true;
      linhaSelecionada--;
      linhaMudouColuna2 = false; // Reseta a flag para evitar que a linha fique em branco na coluna 2
    } else if (tecla == 'D' && linhaSelecionada < numLinhas - 1) { // Tecla de seta para baixo
      linhaMudou = true;
      linhaSelecionada++;
      linhaMudouColuna2 = false; // Reseta a flag para evitar que a linha fique em branco na coluna 2
    } else if (tecla == 'Z') { // Tecla "Z" para trocar a linha para a coluna 2
      linhaMudou = true;
      linhaMudouColuna2 = true;
    }
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

  // Exibir a imagem abaixo da tabela
  loadBitmap(FILENAME, 0, alturaTela - 120);
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
