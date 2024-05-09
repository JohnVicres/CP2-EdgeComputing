#include <LiquidCrystal.h>

#define ESCURO    0
#define MEIA_LUZ  1
#define CLARO     2

#define TEMP_OK   0
#define FRIO      1
#define QUENTE    2

#define UMID_OK    0
#define UMID_BAIXA 1
#define UMID_ALTA  2

int luz = ESCURO;
int temperaturaAmbiente = TEMP_OK;
int umidadeAmbiente = UMID_OK;

int ledVermelhoPin = 11;
int ledAmareloPin  = 12;
int ledVerdePin    = 13;
int ldrPin         = A1;
int buzzerPin      = 10;
int sensorTempPin  = A2;
int sensorUmidPin  = A0;

LiquidCrystal LCD(9, 8, 7, 6, 5, 4);

int numLeituras = 5;
int leiturasTemperatura[5];
int leiturasUmidade[5];
int indexLeitura = 0;

void setup() {
  LCD.begin(16, 2);
  pinMode(ledVermelhoPin, OUTPUT);
  pinMode(ledAmareloPin, OUTPUT);
  pinMode(ledVerdePin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  ler_sensores();
  verifica_sensores();
  aciona_alarmes();
  mostra_mensagem();
  delay(5000);
}

void ler_sensores() {
  int valorLDR = analogRead(ldrPin);

  int valorSensorTemp = analogRead(sensorTempPin);
  float tensao = valorSensorTemp * 5.0 / 1024.0;
  float temperatura = (tensao - 0.5) * 100.0;

  int valorSensorUmi = analogRead(sensorUmidPin);
  int umidade = map(valorSensorUmi, 0, 1023, 0, 100);

  leiturasTemperatura[indexLeitura] = temperatura;
  leiturasUmidade[indexLeitura] = umidade;
  indexLeitura = (indexLeitura + 1) % numLeituras;
}

void verifica_sensores() {
  float mediaTemperatura = calcula_media(leiturasTemperatura, numLeituras);
  if (mediaTemperatura < 10) {
    temperaturaAmbiente = FRIO;
  } else if (mediaTemperatura > 15) {
    temperaturaAmbiente = QUENTE;
  } else {
    temperaturaAmbiente = TEMP_OK;
  }

  float mediaUmidade = calcula_media(leiturasUmidade, numLeituras);
  if (mediaUmidade < 50) {
    umidadeAmbiente = UMID_BAIXA;
  } else if (mediaUmidade > 70) {
    umidadeAmbiente = UMID_ALTA;
  } else {
    umidadeAmbiente = UMID_OK;
  }
}

float calcula_media(int valores[], int tamanho) {
  float soma = 0;
  for (int i = 0; i < tamanho; i++) {
    soma += valores[i];
  }
  return soma / tamanho;
}

void aciona_alarmes() {
  digitalWrite(ledVerdePin, luz == ESCURO);
  digitalWrite(ledAmareloPin, luz == MEIA_LUZ);
  digitalWrite(ledVermelhoPin, luz == CLARO);

  digitalWrite(buzzerPin, luz == CLARO);

  switch (luz) {
    case ESCURO:
      LCD.print("Ambiente: ESCURO");
      break;
    case MEIA_LUZ:
      LCD.print("Ambiente: MEIA LUZ");
      break;
    case CLARO:
      LCD.print("Ambiente: CLARO");
      break;
  }

  if (temperaturaAmbiente == TEMP_OK) {
    LCD.print("Temperatura: OK");
    LCD.print("Temp. = ");
    LCD.print(calcula_media(leiturasTemperatura, numLeituras));
    LCD.print("C");
  } else if (temperaturaAmbiente == FRIO) {
    LCD.print("Temperatura: BAIXA");
    LCD.print("Temp. = ");
    LCD.print(calcula_media(leiturasTemperatura, numLeituras));
    LCD.print("C");
  } else if (temperaturaAmbiente == QUENTE) {
    LCD.print("Temperatura: ALTA");
    LCD.print("Temp. = ");
    LCD.print(calcula_media(leiturasTemperatura, numLeituras));
    LCD.print("C");
  }
}

void mostra_mensagem() {
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Umidade Ambiente:");
  LCD.setCursor(0, 1);
  LCD.print(umidadeAmbiente);
  LCD.print("%");
  delay(1000);
}
