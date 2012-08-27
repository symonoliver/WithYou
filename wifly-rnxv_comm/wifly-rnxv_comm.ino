/*
 * WiFlyHQ Example udpclient.ino
 *
 * This sketch implements a simple UDP client that sends a UDP packet
 * to a UDP server every second.
 *
 * This sketch is released to the public domain.
 *
 */


#include <SoftwareSerial.h>
SoftwareSerial wifiSerial(8,9);

//#include <AltSoftSerial.h>
//AltSoftSerial wifiSerial(8,9);

#include <WiFlyHQ.h>

/* Change these to match your WiFi network */
const char mySSID[] = "SOLM";
const char myPassword[] = "zero0and";

int led = 13;

void terminal();

WiFly wifly;

void setup()
{
  pinMode(led, OUTPUT);
  
  char buf[32];

  Serial.begin(115200);
  Serial.println("Starting");
  Serial.print("Free memory: ");
  Serial.println(wifly.getFreeMemory(),DEC);

  wifiSerial.begin(9600);

  if (!wifly.begin(&wifiSerial, &Serial)) {
    Serial.println("Failed to start wifly");
    terminal();
  }

  if (wifly.getFlushTimeout() != 10) {
    Serial.println("Restoring flush timeout to 10msecs");
    wifly.setFlushTimeout(10);
    wifly.save();
    wifly.reboot();
  }

  /* Join wifi network if not already associated */
  if (!wifly.isAssociated()) {
    /* Setup the WiFly to connect to a wifi network */
    Serial.println("Joining network");
    wifly.setSSID(mySSID);
    wifly.setPassphrase(myPassword);
    wifly.enableDHCP();
    wifly.setPort(2000);

    if (wifly.join()) {
      Serial.println("Joined wifi network");
    } 
    else {
      Serial.println("Failed to join wifi network");
      terminal();
    }
  } 
  else {
    Serial.println("Already joined network");
  }

  /* Ping the gateway */
  wifly.getGateway(buf, sizeof(buf));

  Serial.print("ping ");
  Serial.print(buf);
  Serial.print(" ... ");
  if (wifly.ping(buf)) {
    Serial.println("ok");
  } 
  else {
    Serial.println("failed");
  }

  /*
    Serial.print("ping google.com ... ");
   if (wifly.ping("google.com")) {
   	Serial.println("ok");
   } else {
   	Serial.println("failed");
   }
   */

  /* Setup for UDP packets, sent automatically */
  wifly.setIpProtocol(WIFLY_PROTOCOL_UDP);
  wifly.setHost("192.168.100.106", 8000);	// Send UDP packet to this server and port

  Serial.print("MAC: ");
  Serial.println(wifly.getMAC(buf, sizeof(buf)));
  Serial.print("IP: ");
  Serial.println(wifly.getIP(buf, sizeof(buf)));
  Serial.print("Netmask: ");
  Serial.println(wifly.getNetmask(buf, sizeof(buf)));
  Serial.print("Gateway: ");
  Serial.println(wifly.getGateway(buf, sizeof(buf)));

  wifly.setDeviceID("Wifly-UDP");
  Serial.print("DeviceID: ");
  Serial.println(wifly.getDeviceID(buf, sizeof(buf)));

  wifly.setHost("192.168.100.106", 8000);	// Send UPD packets to this server and port
  Serial.println(wifly.getPort());

  Serial.println("WiFly ready");
}

uint32_t lastSend = 0;
uint32_t count=0;

void loop()
{
  //wifly.print(2);
  if (wifly.available() > 0) {
    int match = wifly.multiMatch_P(100, 3,
    F("button"), F("slider="), F("switch="));
    switch (match) {
    case 0: /* button */
      {
        Serial.print(F("button: pressed"));
        digitalWrite(led, HIGH);
        break;
      }
    case 1: /* slider */
      {
        float slider = wifly.parseFloat();
        Serial.print(F("slider: "));
        Serial.println(slider);
        break;
      }
    case 2: /* switch */
      {
        char ch = wifly.read();
        Serial.print(F("switch: "));
        Serial.println(ch);
        break;
      }
    default: /* timeout */
      {
        break;
      }
    }
  }
}

void terminal()
{
  Serial.println("Terminal ready");
  while (1) {
    if (wifly.available() > 0) {
      Serial.write(wifly.read());
    }

    if (Serial.available()) {
      wifly.write(Serial.read());
    }
  }
}





