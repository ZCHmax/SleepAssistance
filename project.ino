// This #include statement was automatically added by the Particle IDE.
#include <HttpClient.h>

// This #include statement was automatically added by the Particle IDE.
#include <SparkFunRHT03.h>


// This #include statement was automatically added by the Particle IDE.
#include <Grove_LCD_RGB_Backlight.h>

#include <Adafruit_CAP1188.h>
#include <Wire.h>
#include "application.h"


#define CAP1188_MOSI  A5
#define CAP1188_MISO  D0
#define CAP1188_CLK  D1

rgb_lcd lcd;
const int colorR = 255;
const int colorG = 0;
const int colorB = 0;
Adafruit_CAP1188 cap = Adafruit_CAP1188();

unsigned int nextTime = 0;  

HttpClient http;

// Headers currently need to be set at init, usefulfor API keys etc.
http_header_t headers[] = {
    //  { "Content-Type", "application/json" },
    //  { "Accept" , "application/json" },
    { "Accept" , "*/*"},
    { NULL, NULL } // NOTE: Always terminate headerswill NUL
};

http_request_t request;
http_response_t response;

int hum;
int temp;
int light;
int recHum = 50;
int recTemp = 28;
int recLight = 5;
int tempChange;
int humChange;
int lightChange;
int analogValue;
int stage = 0;

const int RHT03_DATA_PIN = D3;
RHT03 rht;

void setup() {
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.setRGB(colorR, colorG, colorB);
    delay(1000);
    rht.begin(RHT03_DATA_PIN);
    cap.begin(0x28);
}

void loop() {

    if (nextTime > millis()) return;

    // Temperature and Humidity
    int updateRet = rht.update();
    if (updateRet == 1) {
		float h = rht.humidity();
		float t = rht.tempC();
		hum = round(h);
		temp = round(t);
	}
    String strHum = String(hum);
    String strTemp = String(temp);
    
    // Light
    analogValue = analogRead(A0);
    light = map(analogValue, 0, 2500, 0, 100);
    String strLight = String(light);

	//Print temp and humidity values to serial monitor
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print("%, Temperature: ");
    Serial.print(temp);
    Serial.print(" Celsius,");
    Serial.print(" Light: ");
    Serial.print(light);
    Serial.print("%");
    Serial.println();
    nextTime = millis() + 10000;
    
    
    // send data to cloud  
    request.hostname = "34.235.140.143";
    request.port = 5000;
    request.path = "/?temperature=" + strTemp + "&humidity=" + strHum + "&light=" + strLight;
    
    Serial.println("Application>\tStart of Loop.");
    // Get request
    http.get(request, response, headers);
    Serial.print("Application>\tResponse status: ");
    Serial.println(response.status);
    // Serial.print("Application>\tHTTP Response Body:");
    // Serial.println(response.body);
    
    // Show REcommandation
    if (stage == 0) {
        // Recommendation
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Recommandation");
        lcd.setCursor(0, 1);
        lcd.print("H:");
        lcd.print(recHum);
        lcd.print("%");
        lcd.print(" T:");
        lcd.print(recTemp);
        lcd.print("C L:");
        lcd.print(recLight);
        lcd.print("%");
        // button
        uint8_t touched = cap.touched();
        for (uint8_t i = 0; i < 8; i++) {
            if (touched & (1 << i)) {
                // Serial.print("C"); Serial.print(i+1); Serial.print("\t");
                stage += 1;
            }
        }
    }
    
    // Show Room Condition
    if (stage == 1) {
        // LCD Message
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("H: ");
        lcd.print(hum);
        lcd.print("%, T: ");
        lcd.print(temp);
        lcd.print(" C");
        lcd.setCursor(0, 1);
        lcd.print("L: ");
        lcd.print(light);
        lcd.print("%");
        // button
        uint8_t touched = cap.touched();
        for (uint8_t i = 0; i < 8; i++) {
            if (touched & (1 << i)) {
                stage += 1;
            }
        }
    }
    
    // Change Recommand Condition
    if (stage == 2) {
        stage = 0;
        // Temp
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Temperature");
        lcd.setCursor(0, 1);
        lcd.print("1-UP 2-DOWN 3-NO");
        uint8_t touched = cap.touched();
        for (uint8_t i = 0; i < 8; i++) {
            if (touched & (1 << i)) {
                Serial.print("C"); Serial.print(i+1); Serial.print("\t");
                tempChange += 1;
            }
        }
        delay(4000);
        
        // Hum
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Humidity");
        lcd.setCursor(0, 1);
        lcd.print("1-UP 2-DOWN 3-NO");
        for (uint8_t i = 0; i < 8; i++) {
            if (touched & (1 << i)) {
                Serial.print("C"); Serial.print(i+1); Serial.print("\t");
                humChange += 1;
            }
        }
        delay(4000);
        
        // Light
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Light");
        lcd.setCursor(0, 1);
        lcd.print("1-UP 2-DOWN 3-NO");
        for (uint8_t i = 0; i < 8; i++) {
            if (touched & (1 << i)) {
                Serial.print("C"); Serial.print(i+1); Serial.print("\t");
                lightChange += 1;
            }
        }
        delay(4000);
        
        switch (tempChange) {
            case 1: 
                recTemp += 1;
                break;
            case 2:
                recTemp -= 1;
                break;
            case 3:
                break;
        }
        switch (humChange) {
            case 1: 
                recHum += 1;
                break;
            case 2:
                recHum -= 1;
                break;
            case 3:
                break;
        }
        switch (lightChange) {
            case 1: 
                recLight += 1;
                break;
            case 2:
                recLight -= 1;
                break;
            case 3:
                break;
        }
    }
}
