#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <Wire.h>

/* Set these to your desired credentials. */
const char *ssid = "Andy";
const char *password = "internetispower";
WebSocketsClient ws = WebSocketsClient();

int offset = 0;
int pos = 0;

int motorEnablePin = 14;
// int reading = analogRead(A0); 
unsigned long lastTime = micros();
String bat1 = "bat1,";

void setup()
{ 
  //pinMode(0, OUTPUT);
  Serial.begin(115200);
  Serial.println();
  Serial.println("Connecting to access point...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  ws.begin("192.168.2.1", 8080);
  Serial.println("Connecting to socket!");
  
  ws.onEvent(wsEvent);

  Wire.begin(); // join i2c bus (address optional for master)

  pinMode(motorEnablePin, OUTPUT);
}

void wsEvent(WStype_t type, uint8_t * payload, size_t length) {
  int num = 0;
  switch (type) {
    case WStype_DISCONNECTED:
    break;
    case WStype_CONNECTED:
      {
        ws.sendTXT("id,MOTOR");
        Serial.println("Connected");
        sendPosition(0); // Send to default position
        digitalWrite(motorEnablePin, HIGH); // wait for motor start
      }
      break;
    case WStype_TEXT: // not in use in normal circumstances
      { 
        
        char *dot = strchr((char *) payload, ',');
        int dotIndex = (int)(dot - (char *) payload);
        
        char command[dotIndex+1];
        memcpy( command, &payload[0], dotIndex );
        command[dotIndex] = '\0';

        Serial.println(command);

        int valueLength = length-dotIndex-1;
        char value[valueLength+1];
        memcpy( value, &payload[dotIndex+1], valueLength );
        value[valueLength] = '\0';


        if (strcmp(command,"state") == 0) {
          if (strcmp(value, "start") == 0) {
            //digitalWrite(0, HIGH);
          } 
          if (strcmp(value, "stop") == 0) {
            //digitalWrite(0, LOW);
          }
        }

        
        if (strcmp(command,"motor") == 0) {
          if (strcmp(value, "zero") == 0) {
            offset += pos;
            
          } 
          if (strcmp(value, "on") == 0) {
            digitalWrite(motorEnablePin, LOW);
          }

          if (strcmp(value, "off") == 0) {
            digitalWrite(motorEnablePin, HIGH);
          }
        }
        
        Serial.println(value);
      }
      break;
    case WStype_BIN:
      {
        int * vector = (int*) payload;
        pos = vector[0];
        sendPosition(pos + offset);
      }
      break;
    }
}

void loop()
{
  ws.loop();
  checkSendAnalogRead();
}


void sendPosition(int x) {
  Wire.beginTransmission(8); // transmit to device #8
  Wire.write(x); // sends one byte
  Wire.write((x >> 8));
  Wire.endTransmission();    // stop transmitting
}


void checkSendAnalogRead() {
  if (micros() - lastTime > 1000000) {
    lastTime = micros();

    // Define 
    String str =  bat1 + analogRead(A0);
    
    // Length (with one extra character for the null terminator)
    int str_len = str.length() + 1; 
    
    // Prepare the character array (the buffer) 
    char char_array[str_len];
    
    // Copy it over 
    str.toCharArray(char_array, str_len);
    
    ws.sendTXT( char_array );
  }
  
}

