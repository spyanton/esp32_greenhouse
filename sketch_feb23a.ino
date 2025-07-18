#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <Thinary_AHT_Sensor.h>
#include <ESP8266mDNS.h> 
#include <UrlEncode.h>
#include <string>


AHT_Sensor_Class AHT10;

const char *ssid = "TP-LINK_9544";
const char *password = "zacusca0101";


void sendMessage(String message){

  // Data to send with HTTP POST
  String url = "http://api.callmebot.com/whatsapp.php?phone=40737292077&text="+urlEncode(message)+"&apikey=4979715";
  WiFiClient client;    
  HTTPClient http;
  http.begin(client, url);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  // Send HTTP POST request
  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200){
    Serial.print("Message sent successfully");
  }
  else{
    Serial.println("Error sending the message");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }

  // Free resources
  http.end();
}

WiFiServer server(80);

void setup(void) {

  Serial.print("hello world!");

  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFi.setHostname("solar");

// Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Print local IP address and start web server
  Serial.print("");
  Serial.print("WiFi connected. ");
  Serial.print("IP address: ");
  Serial.print(WiFi.localIP());

  server.begin();                            // Actually start the server
  Serial.println("HTTP server started");

  Wire.begin(5, 4);
  if(AHT10.begin(eAHT_SensorAddress_Low))
    Serial.println("Init AHT10 Sucess.");
  else
    Serial.println("Init AHT10 Failure.");
}


bool trigger = true;
void loop(void) {
  float temp = readAHTTemperature();
  WiFiClient client = server.available();   // Listen for incoming clients         
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<html><head><meta http-equiv='refresh' content='4'/><meta name='viewport' content='width=device-width, initial-scale=1'><link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css' integrity='sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr' crossorigin='anonymous'><title>ESP32 WebServer</title><style>html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}h2 { font-size: 3.0rem; }p { font-size: 3.0rem; }.units { font-size: 2.5rem; }.dht-labels{ font-size: 2.5rem; vertical-align:middle; padding-bottom: 15px;}</style></head><body><h2>Statistici solar</h2><p><i class='fas fa-thermometer-half' style='color:#ca3517;'></i><span class='dht-labels'> Temperatura</span><span>");
            client.println(temp);
            client.println("</span><sup class='units'>&deg;C</sup></p><p><i class='fas fa-tint' style='color:#00add6;'></i><span class='dht-labels'> Umiditate aer</span><span>");
            client.println(readAHTHumidity());
            client.println("</span><sup class='units'>&percnt;</sup></p>");
            client.println("<p><i class='fas fa-water' style='color:#00add6;'></i><span class='dht-labels'> Umiditate sol</span><span>");
            client.println(readCHHumidity());
            client.println("</span><sup class='units'>&percnt;</sup>");
            client.println("</body></html>");
  
  if(temp > 30 || temp < 3){
    if(trigger == true){ 
      String s = String(temp);
      sendMessage("ATENTIE! Temperatura este: ");
      delay(5000);
      sendMessage(s);
      delay(5000);
      trigger = false;
    }
  }else if(trigger == false && (temp > 6 || temp < 27)){
      String s = String(temp);
      sendMessage("Temperatura a ajuns la un nivel optim:");
      delay(5000);
      sendMessage(s);
      delay(5000);
      trigger = true;
  }
}

float readCHHumidity() {
  float h = analogRead(A0);
  h = h/1024;
  h = 100 - h*100;
  Serial.println(h);
  return h;
}

float readAHTTemperature() {
  // Sensor readings may also be up to 2 seconds
  // Read temperature as Celsius (the default)
  float t = AHT10.GetTemperature();
  if (isnan(t)) {    
    Serial.println("Failed to read from AHT sensor!");
    return -1;
  }
  else {
    Serial.println(t);
    return t;
  }
}

float readAHTHumidity() {
  // Sensor readings may also be up to 2 seconds
  float h = AHT10.GetHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from AHT sensor!");
    return -1;
  }
  else {
    Serial.println(h);
    return h;
  }
}
