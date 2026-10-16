// ============================================================
// ROBÔ DE EXPLORAÇÃO - ESP32 - WOKWI
// ============================================================
//
// Sensores:
//   DHT22  -> temperatura e umidade
//   LDR    -> intensidade da luz
//   PIR    -> presença
//
// Controle:
//   Joystick -> comandos de direção (apenas monitor serial)
//   Botão    -> liga/desliga
//
// LEDs:
//   Verde    -> funcionamento normal
//   Vermelho -> desligado ou alerta
//
// Probabilidade de vida:
//   Temperatura adequada -> +25%
//   Umidade adequada     -> +25%
//   Luz adequada         -> +20%
//   Presença detectada   -> +30%
//
// Alerta:
//   Probabilidade > 75%
//   Envio pelo CallMeBot
//
// ============================================================


// ============================================================
// BIBLIOTECAS NATIVAS DO ESP32
// ============================================================

#include <WiFi.h>
#include <WiFiClientSecure.h>   // NOVO: necessário para HTTPS
#include <HTTPClient.h>


// ============================================================
// WI-FI WOKWI
// ============================================================

const char* ssid = "Wokwi-GUEST";
const char* password = "";


// ============================================================
// CALLMEBOT
// ============================================================
//
// phoneNumber = número com DDI, sem + e sem espaços
// apiKey      = chave fornecida pelo CallMeBot
//
// ============================================================

String phoneNumber = "5571999542005";
String apiKey = "5957643";


// ============================================================
// DHT22
// ============================================================

#define DHT_PIN 15

float temperatura = 24.0;
float umidade = 40.0;


// ============================================================
// LDR
// ============================================================

#define LDR_PIN 32
#define LIMITE_LUZ 1500

int luz = 2000;


// ============================================================
// PIR
// ============================================================

#define PIR_PIN 14

bool presenca = false;


// ============================================================
// JOYSTICK
// ============================================================

#define JOYSTICK_X 34
#define JOYSTICK_Y 35
#define JOYSTICK_SEL 26

#define CENTRO 2048
#define ZONA_MORTA 500


// ============================================================
// LEDs
// ============================================================

#define LED_VERDE 23
#define LED_VERMELHO 2


// ============================================================
// BOTÃO LIGA/DESLIGA
// ============================================================

#define BOTAO_DESLIGAR 4


// ============================================================
// ESTADO DO ROBÔ
// ============================================================

bool roboLigado = true;
bool ultimoEstadoBotao = HIGH;

String ultimoComando = "";

int probabilidadeVida = 0;

bool alertaEnviado = false;


// ============================================================
// TEMPO
// ============================================================

unsigned long ultimoMonitoramento = 0;
const unsigned long INTERVALO = 2000;


// ============================================================
// FUNÇÃO PARA CODIFICAR TEXTO PARA URL
// ============================================================

String urlEncode(String texto) {

  String resultado = "";

  for (int i = 0; i < texto.length(); i++) {

    char c = texto.charAt(i);

    if (
      (c >= 'a' && c <= 'z') ||
      (c >= 'A' && c <= 'Z') ||
      (c >= '0' && c <= '9') ||
      c == '-' || c == '_' || c == '.' || c == '~'
    ) {
      resultado += c;
    }
    else if (c == ' ') {
      resultado += "%20";
    }
    else {
      char buffer[4];
      sprintf(buffer, "%%%02X", (unsigned char)c);
      resultado += buffer;
    }
  }

  return resultado;
}


// ============================================================
// DHT22 - LEITURA DIRETA
// ============================================================

bool lerDHT22(float &temp, float &hum) {

  uint8_t dados[5] = {0, 0, 0, 0, 0};

  // Inicia comunicação
  pinMode(DHT_PIN, OUTPUT);
  digitalWrite(DHT_PIN, LOW);
  delay(2);
  digitalWrite(DHT_PIN, HIGH);
  delayMicroseconds(30);
  pinMode(DHT_PIN, INPUT_PULLUP);

  // Resposta do DHT
  unsigned long inicio = micros();

  while (digitalRead(DHT_PIN) == HIGH && micros() - inicio < 100) {
  }

  inicio = micros();

  while (digitalRead(DHT_PIN) == LOW && micros() - inicio < 100) {
  }

  inicio = micros();

  while (digitalRead(DHT_PIN) == HIGH && micros() - inicio < 100) {
  }

  // Recebe 40 bits
  for (int i = 0; i < 40; i++) {

    unsigned long inicioLow = micros();

    while (digitalRead(DHT_PIN) == LOW && micros() - inicioLow < 100) {
    }

    unsigned long inicioHigh = micros();

    while (digitalRead(DHT_PIN) == HIGH && micros() - inicioHigh < 100) {
    }

    unsigned long duracao = micros() - inicioHigh;

    // ~26-28 us = 0 | ~70 us = 1
    int bit = duracao > 40 ? 1 : 0;

    dados[i / 8] <<= 1;
    dados[i / 8] |= bit;
  }

  // Checksum
  uint8_t checksum = (dados[0] + dados[1] + dados[2] + dados[3]) & 0xFF;

  if (checksum != dados[4]) {
    return false;
  }

  // Umidade
  hum = (((uint16_t)dados[0] << 8) | dados[1]) / 10.0;

  // Temperatura
  int16_t valorTemp = (((uint16_t)(dados[2] & 0x7F) << 8) | dados[3]);
  temp = valorTemp / 10.0;

  if (dados[2] & 0x80) {
    temp = -temp;
  }

  return true;
}


// ============================================================
// WI-FI
// ============================================================

void conectarWiFi() {

  Serial.println();
  Serial.println("================================");
  Serial.println("         CONECTANDO WI-FI");
  Serial.println("================================");

  WiFi.persistent(false);
  WiFi.disconnect(true, true);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.setMinSecurity(WIFI_AUTH_OPEN);   // rede Wokwi-GUEST é aberta
  WiFi.setSleep(false);

  for (int rodada = 1; rodada <= 5 && WiFi.status() != WL_CONNECTED; rodada++) {

    Serial.print("Tentativa ");
    Serial.print(rodada);
    Serial.print(" ");

    WiFi.begin(ssid, password, 6);   // canal 6 é o do Wokwi-GUEST

    int espera = 0;

    while (WiFi.status() != WL_CONNECTED && espera < 20) {
      delay(500);
      Serial.print(".");
      espera++;
    }

    Serial.println();

    if (WiFi.status() != WL_CONNECTED) {
      WiFi.disconnect();
      delay(1000);
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wi-Fi conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("Não foi possível conectar ao Wi-Fi.");
  }

  Serial.println();
}


// ============================================================
// ENVIO PELO CALLMEBOT
// ============================================================

void sendMessage(String message) {

  // CORRIGIDO: tenta reconectar se o Wi-Fi caiu
  if (WiFi.status() != WL_CONNECTED) {

    Serial.println();
    Serial.println("Wi-Fi caiu. Tentando reconectar...");

    WiFi.reconnect();
    delay(2000);
  }

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println("Wi-Fi desconectado.");
    Serial.println("Mensagem não enviada.");
    return;
  }

  String url =
    "https://api.callmebot.com/whatsapp.php?phone=" +
    phoneNumber +
    "&apikey=" +
    apiKey +
    "&text=" +
    urlEncode(message);

  // CORRIGIDO: HTTPS com WiFiClientSecure (sem validar certificado)
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;

  Serial.println();
  Serial.println("Enviando alerta para WhatsApp...");

  http.begin(client, url);
  http.setTimeout(15000);

  int codigo = http.GET();

  Serial.print("Código HTTP: ");
  Serial.println(codigo);

  if (codigo == 200) {
    Serial.println("Mensagem enviada com sucesso!");
  }
  else {
    Serial.println("Erro ao enviar mensagem.");
  }

  // NOVO: mostra a resposta do CallMeBot (explica o motivo de erros)
  if (codigo > 0) {
    Serial.println("Resposta do CallMeBot:");
    Serial.println(http.getString());
  }
  else {
    Serial.print("Erro de conexão: ");
    Serial.println(http.errorToString(codigo));
  }

  http.end();
}


// ============================================================
// LEITURA DOS SENSORES
// ============================================================

void lerSensores() {

  float novaTemperatura;
  float novaUmidade;

  bool leituraOK = lerDHT22(novaTemperatura, novaUmidade);

  if (leituraOK) {
    temperatura = novaTemperatura;
    umidade = novaUmidade;
  }

  luz = analogRead(LDR_PIN);

  presenca = digitalRead(PIR_PIN);
}


// ============================================================
// PROBABILIDADE DE VIDA
// ============================================================

void calcularProbabilidadeVida() {

  probabilidadeVida = 0;

  if (temperatura >= 15.0 && temperatura <= 30.0) {
    probabilidadeVida += 25;
  }

  if (umidade >= 40.0 && umidade <= 70.0) {
    probabilidadeVida += 25;
  }

  if (luz > LIMITE_LUZ) {
    probabilidadeVida += 20;
  }

  if (presenca) {
    probabilidadeVida += 30;
  }

  if (probabilidadeVida > 100) {
    probabilidadeVida = 100;
  }
}


// ============================================================
// ALERTA
// ============================================================

void verificarAlerta() {

  if (probabilidadeVida > 75) {

    // Envia uma única mensagem
    if (!alertaEnviado) {

      Serial.println();
      Serial.println("********************************");
      Serial.println("             ALERTA!");
      Serial.println("********************************");
      Serial.println("ALERTA! Alta probabilidade de vida detectada!");

      sendMessage("Alerta! Alta probabilidade de vida detectada no planeta.");

      alertaEnviado = true;
    }
  }
  else {
    // Permite um novo alerta futuramente
    alertaEnviado = false;
  }
}


// ============================================================
// LEDs
// ============================================================

void atualizarLEDs() {

  // Robô desligado
  if (!roboLigado) {
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_VERMELHO, HIGH);
    return;
  }

  // Alerta
  if (probabilidadeVida > 75) {
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_VERMELHO, HIGH);
    return;
  }

  // Normal
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_VERMELHO, LOW);
}


// ============================================================
// MONITOR SERIAL
// ============================================================

void mostrarInformacoes() {

  Serial.println();
  Serial.println("========================================");
  Serial.println("             STATUS DO ROBÔ");
  Serial.println("========================================");

  Serial.print("Temperatura: ");
  Serial.print(temperatura, 1);
  Serial.println(" °C");

  Serial.print("Umidade: ");
  Serial.print(umidade, 1);
  Serial.println(" %");

  Serial.print("Intensidade da luz: ");
  Serial.println(luz);

  Serial.print("Presença: ");

  if (presenca) {
    Serial.println("Presença detectada");
  }
  else {
    Serial.println("Sem presença");
  }

  Serial.print("Estado do robô: ");

  if (!roboLigado) {
    Serial.println("DESLIGADO");
  }
  else if (probabilidadeVida > 75) {
    Serial.println("ALERTA");
  }
  else {
    Serial.println("LIGADO");
  }

  Serial.print("Probabilidade de vida: ");
  Serial.print(probabilidadeVida);
  Serial.println("%");

  if (probabilidadeVida > 75) {
    Serial.println("ALERTA! Alta probabilidade de vida detectada!");
  }
  else {
    Serial.println("Exploração normal. Nenhum indício relevante detectado.");
  }

  Serial.println("========================================");
}


// ============================================================
// JOYSTICK
// ============================================================

void controlarJoystick() {

  int valorX = analogRead(JOYSTICK_X);
  int valorY = analogRead(JOYSTICK_Y);

  String comando = "Parado";

  if (valorY > CENTRO + ZONA_MORTA) {
    comando = "Frente";
  }
  else if (valorY < CENTRO - ZONA_MORTA) {
    comando = "Trás";
  }
  else if (valorX > CENTRO + ZONA_MORTA) {
    comando = "Direita";
  }
  else if (valorX < CENTRO - ZONA_MORTA) {
    comando = "Esquerda";
  }
  else {
    comando = "Parado";
  }

  if (comando != ultimoComando) {

    Serial.print("Comando: ");
    Serial.println(comando);

    ultimoComando = comando;
  }
}


// ============================================================
// SETUP
// ============================================================

void setup() {

  Serial.begin(115200);

  pinMode(JOYSTICK_X, INPUT);
  pinMode(JOYSTICK_Y, INPUT);
  pinMode(JOYSTICK_SEL, INPUT_PULLUP);
  pinMode(BOTAO_DESLIGAR, INPUT_PULLUP);
  pinMode(PIR_PIN, INPUT);

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);

  analogReadResolution(12);

  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_VERMELHO, LOW);

  conectarWiFi();

  Serial.println("=================================");
  Serial.println("       ROBÔ DE EXPLORAÇÃO");
  Serial.println("=================================");
  Serial.println("Robô ligado.");
  Serial.println("Sensores inicializados.");
  Serial.println("Controle pronto.");
  Serial.println();
}


// ============================================================
// LOOP
// ============================================================

void loop() {

  // ==========================================================
  // BOTÃO
  // ==========================================================

  bool estadoBotao = digitalRead(BOTAO_DESLIGAR);

  if (ultimoEstadoBotao == HIGH && estadoBotao == LOW) {

    roboLigado = !roboLigado;

    if (roboLigado) {
      Serial.println();
      Serial.println("Comando enviado: LIGAR");
      Serial.println("Robô ligado.");
      ultimoComando = "";
    }
    else {
      Serial.println();
      Serial.println("Comando enviado: DESLIGAR");
      Serial.println("Robô desligado.");
      ultimoComando = "";
    }

    delay(200);
  }

  ultimoEstadoBotao = estadoBotao;


  // ==========================================================
  // LEITURA A CADA 2 SEGUNDOS
  // ==========================================================

  if (millis() - ultimoMonitoramento >= INTERVALO) {

    ultimoMonitoramento = millis();

    lerSensores();
    calcularProbabilidadeVida();
    verificarAlerta();
    atualizarLEDs();
    mostrarInformacoes();
  }


  // ==========================================================
  // JOYSTICK
  // ==========================================================

  if (roboLigado && probabilidadeVida <= 75) {
    controlarJoystick();
  }

  delay(20);
}