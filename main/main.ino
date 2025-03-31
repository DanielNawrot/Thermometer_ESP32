// Required Libraries
#include <Adafruit_BME280.h>
#include <esp_now.h>
#include <WiFi.h>

// Other ESP32 address
uint8_t broadCastAddress[] = {0xa0, 0xdd, 0x6c, 0x10, 0x5f, 0x44};

// Data structure sent to other ESP board
typedef struct struct_message {
  float temp;
  float hum;
  float pres;
} struct_message;

struct_message message;
esp_now_peer_info_t peerInfo;

// Function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10


Adafruit_BME280 bme; // I2C

void setup() {
  Serial.begin(115200);
  while(!Serial);

  // ESP communication wifi mode
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);

  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Setting up the communication between the two ESPs
  memcpy(peerInfo.peer_addr, broadCastAddress, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // ______STARTUP FOR BME280 SENSOR________
  unsigned status;

  status = bme.begin();
  if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
  }
}

void loop() {
  // Continuously read the sensor data
  message.hum = bme.readHumidity();
  message.pres = (bme.readPressure() / 100.0F) + 14;
  message.temp = bme.readTemperature();

  // Send data to other ESP to be displayed
  esp_err_t result = esp_now_send(0, (uint8_t *) &message, sizeof(struct_message));
  if (result == ESP_OK) {
    Serial.println("Sent with Success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(2000);
}
