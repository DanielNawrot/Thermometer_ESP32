// Needed Libraties
#include <esp_now.h>
#include <WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// Storing the I2C adress and other needed values, such as OLED Screen parameters
#define i2c_Address 0x3C

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET -1  

// Initializing the display object
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Data structure for the incoming data from transmitting ESP board
typedef struct struct_message {
  float temp;
  float hum;
  float pres;
} struct_message;

struct_message myData;

float intemp;
float inhum;
float inpres;

// Function called when data is recieved.
// Prints values to the serial monitor and stores values for siplay on OLED
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  intemp = myData.temp;
  inhum = myData.hum;  
  inpres = myData.pres;
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("temp: ");
  Serial.println(myData.temp);
  Serial.print("hum: ");
  Serial.println(myData.hum);
  Serial.print("pres: ");
  Serial.println(myData.pres);
  Serial.println();
}


void setup() {
  Serial.begin(115200);
  // Setting the wifi mode to be able to communicate between ESPs
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  delay(250); // wait for the OLED to power up
  display.begin(i2c_Address, true); 

  display.clearDisplay();
  delay(2000);

}
void loop()
{
  // Simple loop to display all measured values
  displayTemp(intemp);
  display.clearDisplay();
  displayHum(inhum);
  display.clearDisplay();
  displayPress(inpres);
  display.clearDisplay();
}

void displayTemp(float temp)
{
  // TEXT
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("Temp");
  display.setCursor(0, 20);
  display.setTextSize(4);
  display.setTextColor(SH110X_WHITE);
  display.print(temp - 0.3, 1);
  display.setTextSize(2);
  display.print(char(247));
  display.println("C");

  // GRAPHIC //
  /**
  display.drawLine(114, 41, 124, 41, SH110X_WHITE);
  int y = 42-(int)temp;
  int h = (int)temp;
  display.drawRect(116, y, 7, h, SH110X_WHITE);
  */
  display.display();
  delay(2000);

}

void displayHum(float hum)
{
  // TEXT
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("Humi");
  display.setCursor(0, 20);
  display.setTextSize(4);
  display.setTextColor(SH110X_WHITE);
  display.print(hum, 1);
  display.setTextSize(2);
  display.print("%");
  display.display();
  delay(2000);
}

void displayPress(float press)
{
  // TEXT
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("Press");
  display.setCursor(0, 20);
  display.setTextSize(3);
  display.setTextColor(SH110X_WHITE);
  display.print((int)press);
  display.setTextSize(2);
  display.print("hPa");
  display.display();
  delay(2000);
}
