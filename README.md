Este projeto implementa um sistema de semáforo inteligente baseado em um ESP32, capaz de ajustar o tempo de funcionamento conforme as condições de luminosidade do ambiente. O sistema também integra conectividade IoT para monitoramento remoto utilizando a plataforma Ubidots via MQTT.

## Componentes Utilizados
- ESP32 (microcontrolador)
- LEDs (vermelho, amarelo e verde)
- Resistor de foto (LDR) para leitura da luz ambiente
- Resistores para os LEDs e o LDR

## Video demonstração: https://youtube.com/shorts/IYg-D8m0neM

#### Descrição do funcionamento:
Dois esps32 estão conectados em uma protoboard, ambos tem 3 leds, das cores vermelho, amarelo e verde. um esp32 tem um sensor ldr conectado para verificar a luminosidade ambiente e enviar, por meio de um protocolo MQTT, para a platafrma ubidots. Dentro da plataforma, no dashboard, é possivel verificar se está dia ou noite e tomar a decisão de encurtar o tempo dos ciclos dos semaforos. Alem disso o broker é utilizado para relaizar a sincronização entre os semáforos, mas caso de errado um dos semaforos fica piscando o led amarelo.
No momento da entrega não é possivel ver o segundo semaforo funcionando, pois estamos com problema no ubidots.
