#include <WiFi.h>
#include <PubSubClient.h>

#define LED_R 13
#define LED_Y 12
#define LED_G 14

#define LDR_PIN 34
int limiarLuz = 500;

unsigned long ultimoMilissegundo = 0;
int estadoSemaforo = 0;
int tempoBase;
bool modoNoturno = false;

const char* SSID = "";
const char* PASSWORD = "";

const char* MQTT_SERVER = "industrial.api.ubidots.com";
const char* MQTT_CLIENT_ID = "esp32_t12_g02_1";
const char* MQTT_USER = "BBUS-QO9k65Ha5kiIEWMMYADwHXxW6zcljy";
const char* MQTT_PASSWORD = "";
const char* TOPIC_MODO = "/v1.6/devices/esp32_t12_g02_1/modo";
const char* TOPIC_ESTADO = "/v1.6/devices/esp32_t12_g02_1/estado";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long ultimoReconnectWiFi = 0;
unsigned long ultimoReconnectMQTT = 0;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String mensagem;
    for (int i = 0; i < length; i++) {
        mensagem += (char)payload[i];
    }
    if (mensagem == "0") {
        modoNoturno = true;
    } else if (mensagem == "1") {
        modoNoturno = false;
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
            if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
                client.subscribe(TOPIC_MODO);
            }
        }
    }
}

void sendStateToMQTT(int state) {
    if (client.connected()) {
        String payload = String(state);
        client.publish(TOPIC_ESTADO, payload.c_str());
    }
}

void definirTempoBase() {
    if (modoNoturno) {
        tempoBase = 3000;
    } else {
        int valorLDR = analogRead(LDR_PIN);
        tempoBase = (valorLDR < limiarLuz) ? 3000 : 6000;
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

    while (!client.connected()) {
        reconnectMQTT();
    }
    client.subscribe(TOPIC_MODO);
}

void loop() {
    reconnectWiFi();
    reconnectMQTT();
    client.loop();

    unsigned long millisAtual = millis();
    definirTempoBase();

    switch (estadoSemaforo) {
        case 0:
            if (millisAtual - ultimoMilissegundo >= tempoBase) {
                ultimoMilissegundo = millisAtual;
                estadoSemaforo = 1;
            }
            digitalWrite(LED_R, HIGH);
            digitalWrite(LED_Y, LOW);
            digitalWrite(LED_G, LOW);
            sendStateToMQTT(0);
            break;

        case 1:
            if (millisAtual - ultimoMilissegundo >= tempoBase / 3) {
                ultimoMilissegundo = millisAtual;
                estadoSemaforo = 2;
            }
            digitalWrite(LED_R, LOW);
            digitalWrite(LED_Y, HIGH);
            digitalWrite(LED_G, LOW);
            sendStateToMQTT(1);
            break;

        case 2:
            if (millisAtual - ultimoMilissegundo >= tempoBase) {
                ultimoMilissegundo = millisAtual;
                estadoSemaforo = 0;
            }
            digitalWrite(LED_R, LOW);
            digitalWrite(LED_Y, LOW);
            digitalWrite(LED_G, HIGH);
            sendStateToMQTT(2);
            break;
    }
}
