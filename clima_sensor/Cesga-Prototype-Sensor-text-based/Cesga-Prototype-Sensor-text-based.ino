/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  

  Modificado por Alvaro Caride
*********/

// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>

//uncomment the following lines if you're using SPI
/*#include <SPI.h>
#define BME_SCK 14
#define BME_MISO 12
#define BME_MOSI 13
#define BME_CS 15*/



Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

// Replace with your network credentials
const char* ssid     = "invitado";
const char* password = "cafecafe0000#";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

void setup() {
  Serial.begin(115200);
  bool status;

  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  //status = bme.begin();  
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            //client.println("Content-type:text/html");
            client.println("Content-type:application/json");
            client.println("Connection: close");
            client.println();
            // Pruebas para la publicación y lectura más cómodas
            // Respuesta en json
            StaticJsonDocument<200> doc;
            float temp = bme.readTemperature() + 0.16;
            float pressure = bme.readPressure() / 100.0F;
            float humidity = bme.readHumidity() + 8.3;
            //float dewpoint = (237.7*((17.27*bme .readTemperature()/(237.7+bme.readTemperature()))+(log(bme.readHumidity()/100)/log(2.718))))/(17.27-((17.27*bme.readTemperature()/(237.7+bme.readTemperature()))+(log(bme.readHumidity()/100)/log(2.718))));
            float dewpoint = (237.7*((17.27*temp/(237.7+temp))+(log(humidity/100)/log(2.718))))/(17.27-((17.27*temp/(237.7+temp))+(log(humidity/100)/log(2.718))));            
            
            doc["temp"] = temp;
            doc["pressure"] = pressure;
            doc["humidity"] = humidity;
            doc["dewpoint"] = dewpoint;

            char buffer[100];
            serializeJson(doc, buffer);
            Serial.println(buffer);           
            client.println(buffer);




            
            // Display the HTML web page
            //client.println("<!DOCTYPE html><html>"); 
            // Web Page Heading
            //client.println("<main>Temperatura Celsius ");
            //client.println(bme.readTemperature() + 0.16);      // Valor extraido de la calibración 
            //client.println(" Presion hPa ");
            //client.println(bme.readPressure() / 100.0F);
            //client.println(" Humedad relativa % ");
            //client.println(bme.readHumidity() + 8.3);           // Calibrado con un punto con NaCl
            //client.println(" Punto de Rocio Celsius ");
            //client.println ((237.7*((17.27*bme .readTemperature()/(237.7+bme.readTemperature()))+(log(bme.readHumidity()/100)/log(2.718))))/(17.27-((17.27*bme.readTemperature()/(237.7+bme.readTemperature()))+(log(bme.readHumidity()/100)/log(2.718)))));
            //client.println("</main></body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
