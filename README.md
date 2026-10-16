# iot-esp32-robo-espacial


# Controle Remoto de Robô Espacial com ESP32

## Sobre o Projeto

Este projeto faz parte de uma atividade prática da disciplina de Internet das Coisas (IoT), com o objetivo de desenvolver e simular um controle remoto para um robô utilizando a placa ESP32 e a plataforma Wokwi.

O sistema utiliza um joystick analógico para controlar a movimentação do robô em quatro direções, além de um botão para enviar o comando de desligamento. Dois LEDs indicam o estado da comunicação com o robô, enquanto o Monitor Serial exibe os comandos enviados.

## Objetivo

Desenvolver e simular um sistema de controle remoto baseado em ESP32, aplicando conceitos de programação embarcada, leitura de sinais analógicos, entradas digitais e sinalização por LEDs.

## Componentes Utilizados

- **ESP32:** microcontrolador responsável pelo processamento dos comandos.
- **Joystick analógico:** utilizado para selecionar a direção de movimentação do robô.
- **LED verde:** indica o funcionamento normal do controle e o estado de conexão, conforme a lógica implementada.
- **LED vermelho:** indica que o robô está desligado ou desconectado, conforme a lógica implementada.
- **Botão de desligamento:** utilizado para enviar o comando `DESLIGAR`.
- **Resistores:** utilizados para limitar a corrente dos LEDs, quando necessários.
- **Wokwi:** plataforma utilizada para a montagem e simulação do circuito.

## Funcionalidades

### Controle de Movimentação

O joystick permite selecionar quatro direções:

| Direção | Comportamento esperado |
|---|---|
| Frente | Acionamento dos motores para frente |
| Trás | Acionamento dos motores para trás |
| Esquerda | Acionamento do motor direito |
| Direita | Acionamento do motor esquerdo |

O ESP32 realiza a leitura dos eixos X e Y do joystick e identifica a direção selecionada para gerar o comando correspondente.

### Desligamento Remoto

Ao pressionar o botão de desligamento, o sistema envia o comando `DESLIGAR`, que deverá ser interpretado pelo robô para interromper imediatamente o acionamento dos motores.

O Monitor Serial deve exibir a mensagem:

`Comando enviado: DESLIGAR`

### Indicadores de Status

- **LED verde:** indica o estado normal de funcionamento e conexão, conforme a implementação.
- **LED vermelho:** indica o estado de desligamento ou desconexão do robô.

### Monitor Serial

O Monitor Serial permite acompanhar os comandos gerados pelo controle remoto, incluindo mensagens como:

- `Comando: Frente`
- `Comando: Trás`
- `Comando: Esquerda`
- `Comando: Direita`
- `Comando enviado: DESLIGAR`

As mensagens de status também podem indicar se o robô está ligado ou desligado.

## Tecnologias Utilizadas

- ESP32
- Linguagem C/C++ para Arduino
- Arduino IDE ou ambiente de programação compatível
- [Wokwi](https://wokwi.com/) — simulador de circuitos eletrônicos
- Git e GitHub para versionamento e compartilhamento do projeto

## Como Executar no Wokwi

1. Acesse a plataforma [Wokwi](https://wokwi.com/).
2. Abra o projeto pelo link disponibilizado neste repositório.
3. Inicie a simulação clicando no botão **Start Simulation**.
4. Utilize o joystick virtual para testar as quatro direções de movimentação.
5. Observe os comandos exibidos no Monitor Serial.
6. Pressione o botão de desligamento e verifique se a mensagem `Comando enviado: DESLIGAR` aparece.
7. Confira o comportamento dos LEDs de status durante os testes.

## Link da Simulação

**Projeto no Wokwi:** https://wokwi.com/projects/477045930258680833

## Estrutura do Repositório

```text
iot-esp32-robo-espacial/
├── README.md
└── controle_remoto.ino
```

## Etapa da Atividade

**Etapa 01 — Controle Remoto no Wokwi**

Atividade prática voltada ao desenvolvimento e à simulação de um controle remoto para robô espacial utilizando ESP32, joystick analógico, botão de desligamento e indicadores luminosos.

Desenvolvido como atividade acadêmica de Internet das Coisas (IoT).
