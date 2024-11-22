Este projeto implementa um sistema de semáforo inteligente baseado em um ESP32, capaz de ajustar o tempo de funcionamento conforme as condições de luminosidade do ambiente. O sistema também integra conectividade IoT para monitoramento remoto, utilizando a plataforma Ubidots via MQTT.

## Componentes Utilizados
- ESP32 (microcontrolador)
- LEDs (vermelho, amarelo e verde)
- Resistor de foto (LDR) para leitura da luz ambiente
- Resistores para os LEDs e o LDR

## Video demonstração: https://youtube.com/shorts/IYg-D8m0neM

#### Descrição do funcionamento:
Dois ESP32 estão conectados em uma protoboard, ambos com três LEDs nas cores vermelho, amarelo e verde. Um dos ESP32 possui um sensor LDR conectado para verificar a luminosidade ambiente e enviar os dados, por meio do protocolo MQTT, para a plataforma Ubidots.

Na plataforma, através de um painel de controle (dashboard), é possível verificar se está dia ou noite e tomar a decisão de encurtar o tempo dos ciclos dos semáforos. Além disso, o broker é utilizado para realizar a sincronização entre os semáforos. Caso ocorra algum problema na comunicação, um dos semáforos entra em modo de segurança, ficando com o LED amarelo piscando.

No momento da entrega, não foi possível demonstrar o segundo semáforo em funcionamento, devido a um problema na integração com a plataforma Ubidots.
