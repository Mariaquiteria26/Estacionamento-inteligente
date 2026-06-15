// Biblioteca responsável pelo controle do display LCD
#include <LiquidCrystal.h>

// Biblioteca responsável pelo controle dos LEDs NeoPixel
#include <Adafruit_NeoPixel.h>

// Cria o objeto LCD e define os pinos utilizados na comunicação
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Pino conectado aos LEDs NeoPixel
#define C_LED 6

// Quantidade de LEDs NeoPixel utilizados
#define CONT 3

// Brilho máximo dos LEDs (0 a 255)
#define LED_BR 255

// Cria o objeto NeoPixel
Adafruit_NeoPixel neo = Adafruit_NeoPixel(CONT, C_LED, NEO_GRB + NEO_KHZ800);

// Pinos dos sensores ultrassônicos das vagas
#define s1 10
#define s2 9
#define s3 8

// =====================
// SISTEMA DE COBRANÇA
// =====================

// Armazena o instante de entrada do veículo na vaga 1
unsigned long entrada1 = 0;

// Armazena o instante de entrada do veículo na vaga 2
unsigned long entrada2 = 0;

// Armazena o instante de entrada do veículo na vaga 3
unsigned long entrada3 = 0;

// Indica se a vaga 1 está ocupada
bool ocupado1 = false;

// Indica se a vaga 2 está ocupada
bool ocupado2 = false;

// Indica se a vaga 3 está ocupada
bool ocupado3 = false;

// Valor cobrado por minuto estacionado
float precoPorMinuto = 1.00;

void setup() {

  // Inicializa o LCD com 16 colunas e 2 linhas
  lcd.begin(16, 2);

  // Posiciona o cursor na primeira coluna da primeira linha
  lcd.setCursor(0, 0);

  // Inicializa a comunicação serial
  Serial.begin(9600);

  // Inicializa os LEDs NeoPixel
  neo.begin();

  // Apaga todos os LEDs
  neo.clear();

  // Define o brilho dos LEDs
  neo.setBrightness(LED_BR);

  // Atualiza os LEDs
  neo.show();
}

// =====================
// LEITURA DOS SENSORES
// =====================

long distancia(int triggerPin, int echoPin) {

  // Define o pino trigger como saída
  pinMode(triggerPin, OUTPUT);

  // Garante que o trigger começa desligado
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);

  // Envia um pulso ultrassônico
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);

  // Finaliza o pulso
  digitalWrite(triggerPin, LOW);

  // Define o pino echo como entrada
  pinMode(echoPin, INPUT);

  // Retorna o tempo que o som levou para voltar
  return pulseIn(echoPin, HIGH);
}

// =====================
// LEDS
// =====================

long cor_led (int a, int b, int c) {

  // Apaga todos os LEDs antes da atualização
  neo.clear();

  // LED da vaga 1
  if (a == 0)
    // Verde = vaga livre
    neo.setPixelColor(0, 0, 255, 0);
  else
    // Vermelho = vaga ocupada
    neo.setPixelColor(0, 255, 0, 0);

  // LED da vaga 2
  if (b == 0)
    neo.setPixelColor(1, 0, 255, 0);
  else
    neo.setPixelColor(1, 255, 0, 0);

  // LED da vaga 3
  if (c == 0)
    neo.setPixelColor(2, 0, 255, 0);
  else
    neo.setPixelColor(2, 255, 0, 0);

  // Atualiza os LEDs com as novas cores
  neo.show();
}

// =====================
// LOOP PRINCIPAL
// =====================

void loop() {

  // Converte o tempo retornado pelo sensor em distância (cm)
  float d1 = 0.01723 * distancia(s1, s1);
  float d2 = 0.01723 * distancia(s2, s2);
  float d3 = 0.01723 * distancia(s3, s3);

  // =====================
  // CONTROLE DA VAGA 1
  // =====================

  // Se houver um veículo próximo e a vaga ainda não estiver marcada como ocupada
  if (d1 < 100 && ocupado1 == false) {

    // Armazena o instante em que o veículo entrou
    entrada1 = millis();

    // Marca a vaga como ocupada
    ocupado1 = true;
  }

  // Se o veículo saiu da vaga
  if (d1 > 100 && ocupado1 == true) {

    // Calcula o tempo de permanência em minutos
    float minutos = (millis() - entrada1) / 60000.0;

    // Calcula o valor a pagar
    float preco = minutos * precoPorMinuto;

    // Limpa o LCD
    lcd.clear();

    // Exibe a vaga utilizada
    lcd.setCursor(0,0);
    lcd.print("Vaga 1");

    // Exibe o valor cobrado
    lcd.setCursor(0,1);
    lcd.print("R$ ");
    lcd.print(preco,2);

    // Mantém a informação visível por 4 segundos
    delay(4000);

    // Libera novamente a vaga
    ocupado1 = false;
  }

  // =====================
  // CONTROLE DA VAGA 2
  // =====================

  if (d2 < 100 && ocupado2 == false) {

    entrada2 = millis();
    ocupado2 = true;
  }

  if (d2 > 100 && ocupado2 == true) {

    float minutos = (millis() - entrada2) / 60000.0;
    float preco = minutos * precoPorMinuto;

    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("Vaga 2");

    lcd.setCursor(0,1);
    lcd.print("R$ ");
    lcd.print(preco,2);

    delay(4000);

    ocupado2 = false;
  }

  // =====================
  // CONTROLE DA VAGA 3
  // =====================

  if (d3 < 100 && ocupado3 == false) {

    entrada3 = millis();
    ocupado3 = true;
  }

  if (d3 > 100 && ocupado3 == true) {

    float minutos = (millis() - entrada3) / 60000.0;
    float preco = minutos * precoPorMinuto;

    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("Vaga 3");

    lcd.setCursor(0,1);
    lcd.print("R$ ");
    lcd.print(preco,2);

    delay(4000);

    ocupado3 = false;
  }

  // =====================
  // MONITORAMENTO DAS VAGAS
  // =====================

  // Caso todas as vagas estejam livres
  if (d1 > 100 && d2 > 100 && d3 > 100) {

    lcd.setCursor(0, 0);
    lcd.print("3 ESP LIVRES   ");

    lcd.setCursor(0, 1);
    lcd.print("1 2 3 LIVRES ");

    // Todos os LEDs ficam verdes
    cor_led(0, 0, 0);

    delay(500);
  }

  // Caso existam duas vagas livres
  else if ((d1 > 100 && d2 > 100) ||
           (d2 > 100 && d3 > 100) ||
           (d3 > 100 && d1 > 100)) {

    lcd.setCursor(0, 0);
    lcd.print("2 ESP LIVRES ");

    lcd.setCursor(0, 1);

    // Verifica quais são as vagas livres

    if (d1 > 100 && d2 > 100) {
 
      lcd.print("VAGAS 1 E 2     ");

      // Vagas 1 e 2 livres
      cor_led(0, 0, 1);
    }

    else if (d1 > 100 && d3 > 100) {

      lcd.print("VAGAS 1 E 3     ");

      // Vagas 1 e 3 livres
      cor_led(0, 1, 0);
    }

    else {

      lcd.print("VAGAS 2 E 3      ");

      // Vagas 2 e 3 livres
      cor_led(1, 0, 0);
    }

    delay(500);
  }

  // Caso todas as vagas estejam ocupadas
  else if (d1 < 100 && d2 < 100 && d3 < 100) {

    // Todos os LEDs ficam vermelhos
    cor_led(1, 1, 1);

    lcd.setCursor(0, 0);
    lcd.print("SEM VAGAS ");

    lcd.setCursor(0, 1);
    lcd.print("LOTADO ");

    delay(500);
  }

  // Caso exista apenas uma vaga livre
  else if ((d1 < 100 && d2 < 100) ||
           (d2 < 100 && d3 < 100) ||
           (d3 < 100 && d1 < 100)) {

    lcd.setCursor(0, 0);
    lcd.print("1 ESP LIVRE ");

    lcd.setCursor(0, 1);

    // Identifica qual vaga está disponível

    if (d1 > 100) {

      lcd.print("VAGA 1       ");

      cor_led(0, 1, 1);
    }

    else if (d2 > 100) {

      lcd.print("VAGA 2        ");

      cor_led(1, 0, 1);
    }

    else {

      lcd.print("VAGA 3        ");

      cor_led(1, 1, 0);
    }

    delay(500);
  }

  // Pequena pausa antes da próxima leitura dos sensores
  delay(100);
}
