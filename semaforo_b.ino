#include <WiFi.h>
#include <PubSubClient.h>

#define LED_R 21
#define LED_Y 19
#define LED_G 18

const char* SSID = "SHARE-RESIDENTE";
const char* PASSWORD = "Share@residente23";

const char* MQTT_SERVER = "industrial.api.ubidots.com";
const char* MQTT_CLIENT_ID = "esp32_t12_g02_2";
const char* TOPIC_STATE = "/v1.6/devices/esp32_t12_g02_1/state";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long ultimoReconnectWiFi = 0;
unsigned long ultimoReconnectMQTT = 0;
unsigned long ultimoPiscarAmarelo = 0;
bool estadoAmarelo = false;
bool conectado = false;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String mensagem;
    for (int i = 0; i < length; i++) {
        mensagem += (char)payload[i];
    }

    int estado = mensagem.toInt();

    if (estado == 0) {
        digitalWrite(LED_R, LOW);
        digitalWrite(LED_Y, LOW);
        digitalWrite(LED_G, HIGH);
    } else if (estado == 1) {
        digitalWrite(LED_R, LOW);
        digitalWrite(LED_Y, HIGH);
        digitalWrite(LED_G, LOW);
    } else if (estado == 2) {
        digitalWrite(LED_R, HIGH);
        digitalWrite(LED_Y, LOW);
        digitalWrite(LED_G, LOW);
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

    if (!conectado) {
        piscarAmareloFalha();
    }
}
