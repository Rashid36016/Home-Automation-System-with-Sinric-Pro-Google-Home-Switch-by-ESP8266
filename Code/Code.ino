#define BLYNK_TEMPLATE_ID "TMPL6gJ3yTVLH" // Your BLYNK_TEMPLATE_ID
#define BLYNK_TEMPLATE_NAME "My home" // Your BLYNK_TEMPLATE_NAME
#define BLYNK_AUTH_TOKEN "ot_SnzsovwsIgkUbz0Rj7Gf7bfGYk4F7" // Your BLYNK_AUTH_TOKEN


#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>


char auth[] = BLYNK_AUTH_TOKEN;


char ssid[] = "Legacy Room";   // Your WiFi name (2.4GHz only)
char pass[] = "202336018";     // Your WiFi password


// Relay Control
BLYNK_WRITE(V1) {
  int value = param.asInt();
  Serial.print("V1 is clicked. Value: ");
  Serial.println(value);
  digitalWrite(D0, value ? LOW : HIGH);
}

BLYNK_WRITE(V2) {
  int value = param.asInt();
  Serial.print("V2 is clicked. Value: ");
  Serial.println(value);
  digitalWrite(D1, value ? LOW : HIGH);
}

BLYNK_WRITE(V3) {
  int value = param.asInt();
  Serial.print("V3 is clicked. Value: ");
  Serial.println(value);
  digitalWrite(D2, value ? LOW : HIGH);
}

BLYNK_WRITE(V4) {
  int value = param.asInt();
  Serial.print("V4 is clicked. Value: ");
  Serial.println(value);
  digitalWrite(D3, value ? LOW : HIGH);
}


void setup()
{
  Serial.begin(115200);
  delay(2000); // Delay for serial monitor

  Serial.println("NodeMCU Starting...");
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);


  // Connection start with Wi-Fi 
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(2000);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n WiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n WiFi Connection FAILED!");
  }


  // Connection start with Blynk
  Serial.println("Connecting to Blynk...");
  Blynk.begin(auth, ssid, pass);

  pinMode(D0, OUTPUT);
  digitalWrite(D0, HIGH);  // Initially OFF
  pinMode(D1, OUTPUT);
  digitalWrite(D1, HIGH);  // Initially OFF
  pinMode(D2, OUTPUT);
  digitalWrite(D2, HIGH);  // Initially OFF
  pinMode(D3, OUTPUT);
  digitalWrite(D3, HIGH);  // Initially OFF
}

void loop()
{
  Blynk.run();


  // Blynk & Wi-Fi status print in Serial monitor. Delay 2sec. 
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 2000) {  
    lastCheck = millis();

    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("Wi-Fi Connected. Running IP: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("WiFi Disconnected!");
    }

    if (Blynk.connected()) {
      Serial.println("Blynk Connected");
    } else {
      Serial.println("Blynk Not Connected");
    }
  }
}
