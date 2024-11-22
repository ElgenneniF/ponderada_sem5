#include <WiFi.h>
#include <PubSubClient.h>

#define LED_R 13
#define LED_Y 12
#define LED_G 14

const char* SSID = "";
const char* PASSWORD = "";

const char* MQTT_SERVER = "industrial.api.ubidots.com";
const char* MQTT_CLIENT_ID = "esp32_t12_g02_2";
const char* TOPIC_STATE = "/v1.6/devices/esp32_t12_g02_1/state";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long ultimoReconnectWiFi = 0;
unsigned long ultimoReconnectMQTT = 0;
unsigned long ultimoPiscarAmarelo = 0;
unsigned long ultimoMilissegundo = 0;

bool estadoAmarelo = false;
bool conectado = false;
bool cicloIniciado = false;
int estadoSemaforo = 0;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String mensagem;
    for (int i = 0; i < length; i++) {
        mensagem += (char)payload[i];
    }

    int estado = mensagem.toInt();
    if (!cicloIniciado) {
        cicloIniciado = true;
    }

    if (estado == 0) {
        estadoSemaforo = 2;
    } else if (estado == 1) {
        estadoSemaforo = 1;
    } else if (estado == 2) {
        estadoSemaforo = 0;
    }
}

void reconnectWiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        unsigned long agora = millis();
        if (agora - ultimoReconnectWiFi >= 3000) {
            ultimoReconnectWiFi = agora;
            WiFi.disconnect();
            WiFi.begin(SSID, PASSWORD);
        }
    }
}

void reconnectMQTT() {
    if (!client.connected()) {
        unsigned long agora = millis();
        if (agora - ultimoReconnectMQTT >= 5000) {
            ultimoReconnectMQTT = agora;
            if (client.connect(MQTT_CLIENT_ID)) {
                client.subscribe(TOPIC_STATE);
                conectado = true;
            } else {
                conectado = false;
            }
        }
    }
}

void piscarAmareloFalha() {
    unsigned long agora = millis();
    if (agora - ultimoPiscarAmarelo >= 500) {
        ultimoPiscarAmarelo = agora;
        estadoAmarelo = !estadoAmarelo;
        digitalWrite(LED_Y, estadoAmarelo ? HIGH : LOW);
    }
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, LOW);
}

void executarCicloSemaforo() {
    unsigned long millisAtual = millis();

    switch (estadoSemaforo) {
        case 0:
            if (millisAtual - ultimoMilissegundo >= 6000) {
                ultimoMilissegundo = millisAtual;
                estadoSemaforo = 1;
            }
            digitalWrite(LED_R, HIGH);
            digitalWrite(LED_Y, LOW);
            digitalWrite(LED_G, LOW);
            break;

        case 1:
            if (millisAtual - ultimoMilissegundo >= 2000) {
                ultimoMilissegundo = millisAtual;
                estadoSemaforo = 2;
            }
            digitalWrite(LED_R, LOW);
            digitalWrite(LED_Y, HIGH);
            digitalWrite(LED_G, LOW);
            break;

        case 2:
            if (millisAtual - ultimoMilissegundo >= 6000) {
                ultimoMilissegundo = millisAtual;
                estadoSemaforo = 0;
            }
            digitalWrite(LED_R, LOW);
            digitalWrite(LED_Y, LOW);
            digitalWrite(LED_G, HIGH);
            break;
    }
}

void setup() {
    pinMode(LED_R, OUTPUT);
    pinMode(LED_Y, OUTPUT);
    pinMode(LED_G, OUTPUT);
    Serial.begin(115200);

    WiFi.begin(SSID, PASSWORD);
    client.setServer(MQTT_SERVER, 1883);
    client.setCallback(mqttCallback);
}

void loop() {
    reconnectWiFi();
    reconnectMQTT();

    if (WiFi.status() == WL_CONNECTED && client.connected()) {
        conectado = true;
        client.loop();
    } else {
        conectado = false;
    }

    if (!conectado || !cicloIniciado) {
        piscarAmareloFalha();
    } else {
        executarCicloSemaforo();
    }
}
