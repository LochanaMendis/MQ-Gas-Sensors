#include <WiFi.h>
#include <TimeLib.h>
//#include <SPI.h>
//MQTT Library
#include <PubSubClient.h>
#include "Gassensors.h"

#define DEBUG

#define         MQ2PIN                       (0)   //define which analog input channel you are going to use
//#define         MQ3PIN                       (3)
//#define         MQ4PIN                       (4)
//#define         MQ5PIN                       (1)
//#define         MQ6PIN                       (6)
//#define         MQ7PIN                       (7)
//#define         MQ8PIN                       (8)
//#define         MQ9PIN                       (2)
//#define         MQ137PIN                     (3)

int  HYDROGEN       =    0;
int  LPG           =    0;
int  METHANE        =  0;
int  CARBON_MONOXIDE =  0;
int  ALCOHOL         =   0;
int  SMOKE           =    0;
int  PROPANE        =     0;
int  BENZENE         =    0;
int  HEXANE         =     0;
int  CARBON_DIOXIDE  =     0;
int  AMMONIUM       =    0;
int  TOLUENE        =     0;
int  ACETONE        =     0;
int Gassensor_type[] = {MQ2, MQ3, MQ4, MQ5, MQ6, MQ7, MQ8, MQ9, MQ135, MQ137};
float   Ro = 0;                    //Ro is initialized to 0 kilo ohms
Gassensors gs2(MQ2PIN, Gassensor_type[0]);
//Gassensors gs3(MQ3PIN,Gassensor_type[1]);
//Gassensors gs4(MQ4PIN,Gassensor_type[2]);
//Gassensors gs5(MQ5PIN,Gassensor_type[3]);
//Gassensors gs6(MQ6PIN,Gassensor_type[4]);
//Gassensors gs7(MQ7PIN,Gassensor_type[5]);
//Gassensors gs8(MQ8PIN,Gassensor_type[6]);
//Gassensors gs9(MQ9PIN,Gassensor_type[7]);
//Gassensors gs137(MQ137PIN,Gassensor_type[9]);

char server[] = "iot.eclipse.org";
//char server[] = "103.24.77.108";
char topic[] = "lochana/gas";
//char topic[] = "CS2/UAV/development";
#define DEVICE_ID_1 "MQ2_SMOKE"
#define DEVICE_ID_2 "MQ5_LPG"
#define DEVICE_ID_3 "MQ9_CARBON_MONOXIDE"
#define DEVICE_ID_4 "MQ137_AMMONIUM"
char* mqttClientName = "GAS2"; ////mqtt client should be unique for different mobile airq

WiFiClient wifiClient;
WiFiClient wifiClientTime;

void callback(char* topic, byte* payload, unsigned int length);
PubSubClient pubsubClient(server, 1883, callback, wifiClient);

//char ssid[] = "dd-wrt";     // the name of your network
char ssid[] = "SUTD_LAB";     // the name of your network
char pass[] = "S!8DL52X";  // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status


//NTP url for time
char SITE_URL[] =  "103.24.77.108";
//IPAddress SITE_URL(103,24,77,108);
String currentTime;
char date_string[32];
char time_string[32];

unsigned const int CONNECTION_TIMEOUT = 30000;
unsigned const int UPLOAD_RATE = 10000; // rate at which data is uploaded to the backend (in milliseconds)
unsigned const int UPDATE_RATE_GAS = 3000;

long t1, t2, t3;

//String GasID="MQ2_CO";
//String date_string="2017-08-17";
//String time_string="21:15:43";
void initWiFi_WPA() {

  //WiFi.disconnect();
  //status = WiFi.status();

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if ( fv != "1.1.0" )
    Serial.println("Please upgrade the firmware");

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    Serial.println(status);
    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();

}
void initWiFi_NoEncription() {
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to open SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();

}
void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
}
void printWifiData() {
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);

  // print your subnet mask:
  IPAddress subnet = WiFi.subnetMask();
  Serial.print("NetMask: ");
  Serial.println(subnet);

  // print your gateway address:
  IPAddress gateway = WiFi.gatewayIP();
  Serial.print("Gateway: ");
  Serial.println(gateway);
}
void  retrieveNtpTime_SERVER() {
  Serial.print("enter");
  if (status != WL_CONNECTED) {
    Serial.print("bug1");
    return;
  }

  while (wifiClientTime.connect(SITE_URL, 8000) == 0) {
    delay(100);
    Serial.print("bug2");
  }

  wifiClientTime.println("GET /AirQ/time");
  wifiClientTime.print("Host: ");
  wifiClientTime.println(SITE_URL);
  wifiClientTime.println("Connection: close");
  wifiClientTime.println();

  while (!wifiClientTime.available()) {
    delay(100);
    Serial.print("bug1");
  }

  currentTime = "";

  while (wifiClientTime.available())  {
    char a = (char) wifiClientTime.read();
    if ( a != '{' && a != '}') {
      currentTime.concat(a);
    }
  }

  wifiClientTime.stop();

  long epochSystem = (long) (currentTime.toInt());  // to compensate the lag add 250. Remove this after the time server is corrected

  setTime(epochSystem);

  // format the date and time string to be published
  sprintf(date_string, "%02d-%02d-%02d", year(), month(), day());                                                          // use the UTC format for datestr
  sprintf(time_string, "%02d:%02d:%02d", hour(), minute(), second());
  Serial.print("exit");
}
float  MQGetPercentage(float rs_ro_ratio, float *pcurve) {
  //Serial.println(rs_ro_ratio);
  float test = (pow(10, (((log(rs_ro_ratio) - pcurve[1]) / pcurve[2]) + pcurve[0])));
  /* Serial.print("\t");
    Serial.print(test);
    Serial.print("\t");*/
  return test;
}
void connectToMqttBroker() {
  long t = millis();
  pubsubClient.disconnect();  // disconnect from pubsubclient before connecting to a (new) pubsubclient
  //make sure we are connected to WIFI before attemping to reconnect to MQTT

  if (WiFi.status() == WL_CONNECTED) {

    // Loop until we're reconnected to the MQTT server
    while (!pubsubClient.connected()) {

      Serial.println("Connecting to MQTT Broker");

      //if connected, subscribe to the topic(s) we want to be notified about
      if (pubsubClient.connect(mqttClientName)) {
        Serial.println("Successfully connected to MQTT Broker");
      }

      // break after 20 seconds if we cannot connect to the Mqtt broker
      if (millis() - t > CONNECTION_TIMEOUT) {

        Serial.println("Unable to connect to MQTT broker");
        break;
      }

    }

  } else {
    //initWiFi();
    initWiFi_WPA();

  }

}
void callback(char* topic, byte* payload, unsigned int length) {

  // ex payload = uomiot0023-1
  String message = "";
  for (int i = 0; i < length; i++) {
    message += char(payload[i]);
  }

  Serial.println(message);
}
void publishDataToBackend(char GasID[], int concentration) {

  String msg_tmp;                                                                                             //the final message sent to the mqtt
  msg_tmp.concat("|device_id=");
  msg_tmp.concat(GasID);
  msg_tmp.concat("|date=");
  msg_tmp.concat(date_string);
  msg_tmp.concat("|time=");
  msg_tmp.concat(time_string);
  msg_tmp.concat("|gc=");                                                                                             //Humidity
  msg_tmp.concat(concentration);
  msg_tmp.concat("|dummy=0 ");

  char msg[300];
  msg_tmp.toCharArray(msg, msg_tmp.length());

  boolean published = pubsubClient.publish(topic, msg);

  if (published) {
    Serial.println("Successfully published");
#ifdef DEBUG
    Serial.println(concentration);
#endif
  } else {
    Serial.println("Publish failed\n");

    /** store data temporarily in device memory and upload to backed once a connection is established **/

  }

}
void  ReadGasSensors() {
  SMOKE = gs2.printallgases(Gassensor_type[0]);
  Serial.println(SMOKE);
  //LPG  =gs5.printallgases(Gassensor_type[3]);
  //Serial.println(LPG);
  //CARBON_MONOXIDE =gs9.printallgases(Gassensor_type[7]);
  //Serial.println(CARBON_MONOXIDE);
  //AMMONIUM=gs137.printallgases(Gassensor_type[9]);
}
void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif
  Serial.println("START");
  initWiFi_WPA();
  //initWiFi_NoEncription();
  t1 = 0;
  t2 = 0;
  t3 = 0;
  Serial.println("START");
}
void loop() {

  //maintain MQTT connection
  pubsubClient.loop();

  if (!pubsubClient.connected() || (WiFi.status() != WL_CONNECTED)) {
    Serial.println("Disconnected");
    Serial.println("Connected");
    connectToMqttBroker();
  }
  if (millis() - t2 > UPDATE_RATE_GAS) {
    ReadGasSensors();
    t2 = millis();
  }
  if (millis() - t1 > UPLOAD_RATE) {
    retrieveNtpTime_SERVER();
    publishDataToBackend(DEVICE_ID_1, SMOKE);
    publishDataToBackend(DEVICE_ID_2, LPG);
    publishDataToBackend(DEVICE_ID_3, CARBON_MONOXIDE);
    publishDataToBackend(DEVICE_ID_4, AMMONIUM);
    t1 = millis();
  }


}
