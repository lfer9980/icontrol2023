#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>

//config el ethernet
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; 
IPAddress ip(192, 168, 0, 10);
char url[] = "192.168.0.6";

//config DHT
#define DHTPIN 5 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//config http
EthernetClient ether;
HttpClient client = HttpClient(ether,url,80);

//config json body
StaticJsonDocument<64> doc;

//variables
bool posting = false;

void setup(){
   
   Serial.begin(9600);

   pinMode(3, OUTPUT);
   pinMode(4, OUTPUT);
   
   dht.begin();
   Serial.println("Initialize Ethernet with DHCP:");
    if (Ethernet.begin(mac) == 0) {
      Serial.println("Failed to configure Ethernet using DHCP");
      if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
        while (true) {
          delay(1); // do nothing, no point running without Ethernet hardware
        }
      }
      if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("Ethernet cable is not connected.");
      }
      Ethernet.begin(mac, ip);
    }
    Serial.println("Connecting.....");
    delay(1000);
  
    IPAddress ip = Ethernet.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
    Serial.println();
}

void leds(int hum, int temp){
  if (hum == 1 and temp == 1){
    digitalWrite(3 ,HIGH);
    digitalWrite(4 ,HIGH);
  }else if (hum == 1 and temp == 0){
    digitalWrite(3 ,HIGH);
    digitalWrite(4 ,LOW);
  }else if (hum == 0 and temp == 1){
    digitalWrite(3 ,LOW);
    digitalWrite(4 ,HIGH);
  }else{
    digitalWrite(3 ,LOW);
    digitalWrite(4 ,LOW);
  }
}

void get(){

  Serial.println("=========GET=========");
  
  client.beginRequest();
  client.get("/api/v1/public/getdatos");
  client.sendHeader("Host: ", url);
  client.println();
  client.endRequest();
  
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  if (statusCode == 200){
    DeserializationError error = deserializeJson(doc, response);

    bool ishot = doc["ishot"]; // true
    bool ishum = doc["ishum"]; // false
    
    Serial.println(response);

    leds(ishum, ishot);
  }else{

    Serial.println("Ocurrio un error");
  }

  delay(2000);
  
}

void post(String data){

  Serial.println("=========POST=========");

  Serial.println(data);
  
  client.beginRequest();
  client.post("/api/v1/public/datos");
  client.sendHeader("Host: ", url);
  client.sendHeader("Content-Type", "application/x-www-form-urlencoded");
  client.sendHeader("Content-Length", data.length());
  client.beginBody();
  client.print(data);
  client.endRequest();


  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  if (statusCode == 200){
    DeserializationError error = deserializeJson(doc, response);
    Serial.println(response);
    
  }else{

    Serial.println("Ocurrio un error");
  }

  delay(2000);
  
}

void loop() {

  if (posting == true){
    float hum = dht.readHumidity();
    float temp = dht.readTemperature();
    String data = "temp=" + String(temp) + "&hum=" + String(hum);
    post(data);
    posting = false;
  }else{
    get();
    posting = true;
  }

  delay(1000);
  
}
