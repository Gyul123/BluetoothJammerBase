#include <Arduino.h>
#include "RF24.h"
#include "esp_bt.h"
#include "esp_wifi.h"

//Some variables
constexpr int SPI_SPEED = 16000000;
int bluetooth_channels[] = {32, 34, 46, 48, 50, 52, 0, 1, 2, 4, 6, 8, 22, 24, 26, 28, 30, 74, 76, 78, 80};
int ble_channels[] = {2, 26, 80};

//VSIP pins
#define VSPI_CE_PIN 22
#define VSPI_CSN_PIN 21
SPIClass *spiVSPI = nullptr;

//HSIP pins
#define HSPI_CE_PIN 16
#define HSPI_CSN_PIN 15
SPIClass *spiHSPI = nullptr;

//Setting up the antenas
RF24 radio_vspi(VSPI_CE_PIN, VSPI_CSN_PIN, SPI_SPEED);
RF24 radio_hspi(HSPI_CE_PIN, HSPI_CSN_PIN, SPI_SPEED);

//Functions
void configureRadio(RF24 &radio, int channel, SPIClass *spi);
void jamBluetooth();
void jamBle();
void jamAll();

void setup() {
  
    esp_bt_controller_deinit();
    esp_wifi_stop();
    esp_wifi_deinit();
    esp_wifi_disconnect();

  
  spiVSPI = new SPIClass(VSPI);
  spiVSPI->begin();
  configureRadio(radio_vspi, ble_channels[0], spiVSPI);

  spiHSPI = new SPIClass(HSPI);
  spiHSPI->begin();
  configureRadio(radio_hspi, bluetooth_channels[0], spiHSPI);
}

void loop() {
  jamAll();
}

void configureRadio(RF24 &radio, int channel, SPIClass *spi) {
    if (radio.begin(spi)) {
        radio.setAutoAck(false);
        radio.stopListening();
        radio.setRetries(0, 0);
        radio.setPALevel(RF24_PA_MAX, true);
        radio.setDataRate(RF24_2MBPS);
        radio.setCRCLength(RF24_CRC_DISABLED);
        radio.startConstCarrier(RF24_PA_HIGH, channel);
    }
}

void jamBluetooth() {
    int randomIndex = random(0, sizeof(bluetooth_channels) / sizeof(bluetooth_channels[0]));
    int channel = bluetooth_channels[randomIndex];
    radio_vspi.setChannel(channel);
    radio_hspi.setChannel(channel);
}

void jamBle() {
    int randomIndexBle = random(0, sizeof(ble_channels) / sizeof(ble_channels[0]));
    int channelBle = ble_channels[randomIndexBle];
    radio_vspi.setChannel(channelBle);
    radio_hspi.setChannel(channelBle);
}

void jamAll() {
    if (random(0, 2)) {
        jamBluetooth();        
    } else {
        jamBle();
    }
}
