#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h>

// config el ethernet
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 0, 20);
char url[] = "https://icontrol-web2023-production.up.railway.app";

// config http
EthernetClient ether;
HttpClient client = HttpClient(ether, url, 80);

// config json body
StaticJsonDocument<64> doc;

// variables
bool posting = false;

void setup()
{

  Serial.begin(9600);

  pinMode(10, INPUT);
  pinMode(11, INPUT);

  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0)
  {
    Serial.println("Failed to configure Ethernet using DHCP");
    if (Ethernet.hardwareStatus() == EthernetNoHardware)
    {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true)
      {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF)
    {
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

/* 
void get()
{
  Serial.println("=========GET=========");

  client.beginRequest();
  client.get("/api/v1/public/getdatos");
  client.sendHeader("Host: ", url);
  client.println();
  client.endRequest();

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  if (statusCode == 200)
  {
    DeserializationError error = deserializeJson(doc, response);
    Serial.println(response);
  }
  else
  {
    Serial.println("Ocurrio un error");
  }

  delay(1000);
} */

void post(String data)
{
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

  if (statusCode == 200)
  {
    DeserializationError error = deserializeJson(doc, response);
    Serial.println(response);
  }
  else
  {

    Serial.println("Ocurrio un error");
  }

  delay(1000);
}

void loop()
{

  if (posting == true)
  {
    if((digitalRead(10) == 1)||(digitalRead(11) == 1))
    {
      Serial.println('!');
    }
    else{
      float lecture = analogRead(A0);
      Serial.println(lecture);
      delay(1);
      String data = "lect=" + String(lecture);
      post(data);
      posting = false;
    }
  }
  else
  {
    posting = true;
  }

  delay(1000);
}
