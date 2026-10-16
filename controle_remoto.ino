// ===============================
// CONTROLE REMOTO - ROBÔ
// ESP32 + Joystick + LEDs + Botão
// ===============================

// ----- Pinos -----
#define JOYSTICK_Y 35
#define JOYSTICK_X 34
#define JOYSTICK_SEL 26

#define LED_VERDE 23
#define LED_VERMELHO 2

#define BOTAO_DESLIGAR 4

// ----- Joystick -----
#define CENTRO 2048
#define ZONA_MORTA 500

// ----- Estado do robô -----
bool roboLigado = true;

// Controle do botão
bool ultimoEstadoBotao = HIGH;

// Último comando enviado
String ultimoComando = "";

void setup() {

  Serial.begin(115200);

  pinMode(JOYSTICK_SEL, INPUT_PULLUP);
  pinMode(BOTAO_DESLIGAR, INPUT_PULLUP);

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);

  // Robô começa ligado
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_VERMELHO, LOW);

  Serial.println("=================================");
  Serial.println("     CONTROLE REMOTO DO ROBO");
  Serial.println("=================================");
  Serial.println("Robo ligado.");
  Serial.println("Controle pronto.");
  Serial.println();
}


void loop() {

  // =================================
  // LEITURA DO BOTÃO
  // =================================

  bool estadoBotao = digitalRead(BOTAO_DESLIGAR);

  // Detecta apenas o momento em que o botão é pressionado
  if (ultimoEstadoBotao == HIGH && estadoBotao == LOW) {

    // Inverte o estado do robô
    roboLigado = !roboLigado;

    if (roboLigado) {

      // -----------------------------
      // LIGAR ROBÔ
      // -----------------------------

      digitalWrite(LED_VERDE, HIGH);
      digitalWrite(LED_VERMELHO, LOW);

      Serial.println("Comando enviado: LIGAR");
      Serial.println("Robo ligado.");
      Serial.println();

      // Permite um novo comando
      ultimoComando = "";

    } else {

      // -----------------------------
      // DESLIGAR ROBÔ
      // -----------------------------

      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_VERMELHO, HIGH);

      Serial.println("Comando enviado: DESLIGAR");
      Serial.println("Robo desligado.");
      Serial.println();

      // Limpa último comando
      ultimoComando = "";
    }

    // Evita múltiplos acionamentos
    delay(200);
  }

  ultimoEstadoBotao = estadoBotao;


  // =================================
  // SE O ROBÔ ESTIVER DESLIGADO
  // =================================

  if (!roboLigado) {

    // Não permite movimentos
    delay(50);

    return;
  }


  // =================================
  // LEITURA DO JOYSTICK
  // =================================

  int valorX = analogRead(JOYSTICK_X);
  int valorY = analogRead(JOYSTICK_Y);

  String comando = "Parado";


  // =================================
  // FRENTE
  // =================================

  if (valorY > CENTRO + ZONA_MORTA) {

    comando = "Frente";
  }


  // =================================
  // TRÁS
  // =================================

  else if (valorY < CENTRO - ZONA_MORTA) {

    comando = "Trás";
  }


  // =================================
  // DIREITA
  // =================================

  else if (valorX > CENTRO + ZONA_MORTA) {

    comando = "Direita";
  }


  // =================================
  // ESQUERDA
  // =================================

  else if (valorX < CENTRO - ZONA_MORTA) {

    comando = "Esquerda";
  }


  // =================================
  // MOSTRAR COMANDO
  // =================================

  if (comando != ultimoComando) {

    Serial.print("Comando: ");
    Serial.println(comando);

    ultimoComando = comando;
  }

  delay(100);
}