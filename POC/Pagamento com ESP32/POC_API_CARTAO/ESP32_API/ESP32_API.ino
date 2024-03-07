#include <Arduino.h>
#include <MFRC522.h>
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define RST_PIN   27  // Pino de reset
#define SDA_PIN   5   // Pino SS
#define LED_PIN   2   // Pino do LED
MFRC522 mfrc522(SDA_PIN, RST_PIN);  // Crie o objeto MFRC522

// Configurações Wi-Fi
const char* ssid = "Marcus.net";
const char* password = "qkyo8876";
const char* url = "https://c52720bb-4947-4565-942b-be80644a2c57-00-uroxns5y0nuh.janeway.replit.dev/saldo";

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  pinMode(LED_PIN, OUTPUT);
  conectarWiFi();
}

void loop() {
  // Procure por cartões RFID
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.println("Cartão detectado!");

    // Obtenha o UID lido
    String uidString = obterUIDString();

    // Compare o UID lido com o UID associado a "Marcus"
    if (verificarUID(uidString)) {
      Serial.println("Cartão do Marcus detectado!");

      // Efetua a requisição POST com os dados
      float saldo = obterSaldoDaAPI(uidString);
      if (saldo >= 0) {
        // Se houver saldo suficiente, realiza o desconto
        float valor = 10.0; // Defina o valor a ser descontado
        if (saldo >= valor) {
          saldo =saldo- valor;
          atualizarSaldoNaAPI(uidString, saldo);
          Serial.println("Desconto efetuado com sucesso. Novo saldo: " + String(saldo));
        } else {
          Serial.println("Saldo insuficiente para efetuar o desconto.");
        }
      } else {
        Serial.println("Erro ao obter saldo da API.");
      }

    } else {
      Serial.println("Cartão não associado a uma pessoa conhecida.");
    }

    mfrc522.PICC_HaltA();  // Pare a leitura do cartão
    delay(1000);  // Aguarda 1 segundo antes de procurar o próximo cartão
    Serial.println("Aguardando cartão RFID...");
  }
}

bool verificarUID(String uid) {
  // Compare o UID lido com o UID associado a "Marcus"
  // Neste exemplo, apenas um UID específico está associado a "Marcus"
  return uid == "43 71 3E 25 ";
}

String obterUIDString() {
  // Obtenha o UID lido
  String uidString = "";
  for (byte i = 0; i < mfrc522.uid.size; ++i) {
    uidString += String(mfrc522.uid.uidByte[i], HEX);
    uidString.toUpperCase();
    uidString += " ";
  }
  return uidString;
}

float obterSaldoDaAPI(String uid) {
  HTTPClient http;

  // Construa o corpo da requisição POST
  String dados = "nome_usuario=" + uid;

  // Inicializa a comunicação HTTP
  http.begin(url);

  // Define o cabeçalho para indicar o tipo de conteúdo
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  // Realiza a requisição POST com os dados
  int httpCode = http.POST(dados);

  // Verifica o status da resposta
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    Serial.println("Resposta da API: " + payload);
    DynamicJsonDocument doc(128); 
    deserializeJson(doc, payload);

    // Extraia o saldo
    float saldo = doc["saldo"];

    // Neste exemplo, vamos apenas retornar um valor fictício para teste
    return saldo; // Saldo fictício
  } else {
    Serial.println("Erro ao obter saldo da API. Código HTTP: " + String(httpCode));
    Serial.println("Corpo da resposta: " + http.getString());
    return -1.0; // Indica um erro
  }

  // Encerra a comunicação HTTP
  http.end();
}

void atualizarSaldoNaAPI(String uid, float saldo) {
  // Aqui você implementaria a lógica para atualizar o saldo na sua API
  // Pode ser uma nova requisição POST ou PUT, dependendo da sua API
  // Implemente de acordo com a sua necessidade
}

void conectarWiFi() {
  Serial.println("Conectando ao Wi-Fi...");

  // Coloque sua lógica de conexão Wi-Fi aqui
  WiFi.begin(ssid, password);

  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 30) {
    digitalWrite(LED_PIN, HIGH);  // Acende o LED durante a tentativa de conexão
    delay(500);
    digitalWrite(LED_PIN, LOW);   // Apaga o LED durante a tentativa de conexão
    delay(500);

    tentativas++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Conectado ao Wi-Fi.");
    digitalWrite(LED_PIN, HIGH);  // Mantém o LED aceso quando a conexão é estabelecida
  } else {
    Serial.println("Falha ao conectar ao Wi-Fi. Verifique as credenciais e reinicie.");
    digitalWrite(LED_PIN, LOW);  // Garante que o LED esteja apagado em caso de falha
  }
}
