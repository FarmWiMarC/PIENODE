
#include <AuthClient.h>
#include <MicroGear.h>
#include <MQTTClient.h>
#include <SHA1.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <MicroGear.h>
#include <string.h>
//#include <Wire.h>
#include <Adafruit_GFX.h>
//#include <WiFiManager.h>
//#include <Adafruit_SSD1306.h>
//#include <ESP_Adafruit_SSD1306.h>
//#include <Adafruit_SSD1306.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include "Adafruit_SHT31.h"
LiquidCrystal_I2C lcd(0x27, 20, 4);

Adafruit_SHT31 sht31 = Adafruit_SHT31();
float temp,humid;

#define OLED_RESET 4
//Adafruit_SSD1306 display(OLED_RESET);
#define PIENODE "/PIENODE01"
#define ssid  "PIENODE01"
#define PIENODEid  "PIENODE01"
const char* host = "xxxx.freehost.in.th";
 const int httpPort = 80;
String urltemp = "/PIENODEtraining/insert_PIENODE01.php?";
int v1,v2,v3,v4,v5,v6,v7, v8;

const char HTTP_SCRIPT[] PROGMEM = "<script>function c(l){document.getElementById('ssid').value=l.innerText||l.textContent;document.getElementById('pass').focus();}</script>";
const char HTTP_ITEM[] PROGMEM = "<div><a href='#' onclick='c(this)'>{v}</a>&nbsp;<span class='q {i}'>{r}%</span></div>";

int           _minimumQuality         = 80;


 
int tempOffset1 = 6700;//tmec01 6920 tmec03
int tempOffset2 = 6700;//tmec01 6920 tmec03
int tempOffset3 = 6700;//tmec01 6920 tmec03
//int tempOffset4 = 7170;//tmec01 64920 tmec03
int value;
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define sensor1Pin 13
#define sensor2Pin 12
#define sensor3Pin 14

char InputRS232[20];
    const char*   configssid                 = "no-net";
    const char*   configpass             = NULL;
    String ssidset,pass;


MDNSResponder mdns;

int onlinecount;
int indexchar;
WiFiServer server(80);
String st;
//#define APPID       "tmecsensor"
//#define GEARKEY     "aLMxs9YOePCw5JF"
//#define GEARSECRET  "MVGEvHavYGrGWrTIgXrqx2GyHU3tjr"
#define APPID       "zzz"
#define GEARKEY     "xxxxxxxxxxxxxxxxxxx"
#define GEARSECRET  "yyyyyyyyyyyyyyyyyyyy"


#define SCOPE       ""
//int tempOffset = 7000;//tmec01 6920 tmec03
long tempvalue=300; 
float val1,val2,val3,val4;
int ANALOG_PIN = A0;
int pinOn =16;
int count=0;
WiFiClient client;
AuthClient *authclient;
MicroGear microgear(client);
String tempStyle;
//char inputString[30];         // a string to hold incoming data
String inputString;
boolean stringComplete = false;  // whether the string is complete
boolean flagREQ=false;
boolean FlagUpdate = true;
char outputString[30];
char output[20];
//int count;
///////////////////////////////////http
String page;

int getRSSIasQuality(int RSSI) {
  int quality = 0;

  if (RSSI <= -100) {
    quality = 0;
  } else if (RSSI >= -50) {
    quality = 100;
  } else {
    quality = 2 * (RSSI + 100);
  }
  return quality;
}


//////time parameter/////////////////////////////////////////////

//////////////////time NTP

unsigned int localPort = 8888;      // local port to listen for UDP packets
IPAddress timeServerIP; // time.nist.gov NTP server address
//const char* ntpServerName = "time.nist.gov";

// NTP Servers:
static const char ntpServerName[] = "us.pool.ntp.org";
//static const char ntpServerName[] = "ntp.ku.ac.th";
//static const char ntpServerName[] = "time-a.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-b.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-c.timefreq.bldrdoc.gov";

const int timeZone = +7;  // Pacific Daylight Time (BKK)

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
int hh,mm,ss,yy,mo,dd,lastminute,FlagSave;
////////////////////////////////////////////////////////////

////////////////////////////////
time_t prevDisplay = 0; // when the digital clock was displayed
time_t getNtpTime();
void digitalClockDisplay();
void printDigits(int digits);
void sendNTPpacket(IPAddress &address);

void digitalClockDisplay()
{
  String displaytime;
  // digital clock display of the time
  Serial.print(hour());
  hh=hour();
  printDigits(minute());
  mm=minute();
  printDigits(second());
  ss=second();
  Serial.print(" ");
  Serial.print(day());
  dd=day();
  Serial.print(".");
  Serial.print(month());
  mo=month();
  Serial.print(".");
  Serial.print(year());
  yy=year();
  Serial.println();

displaytime = dd;
displaytime += " ";
displaytime += mo;
displaytime += " ";
displaytime += yy;
displaytime += " ";
displaytime += hh;
displaytime += ":";
displaytime += mm;
displaytime += ":";
displaytime += ss;

lcd.setCursor(0, 0);
lcd.print("                    ");
lcd.setCursor(0, 0);
lcd.print(displaytime);
//////display.println(displaytime);
//////display.display();

}

void printDigits(int digits)
{
  // utility for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}









////////////////////////////////////////////////////////////////////////////////////////////
////////////////////LINE config
#define LINE_TOKEN "cBCdIhQ1gtem6ud4MXRPleYPwE05lKLHrZ3Bet2PU0c"
//#define LINE_TOKEN "phyWbLXcPClTOOOk123vlnVqwGVzN2UmVC7LoPYkCHq" //PIEbot001
////////////////////////////////



void Line_Notify(String message) {
  WiFiClientSecure client;

  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("connection failed");
    return;   
  }

  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Authorization: Bearer " + String(LINE_TOKEN) + "\r\n";
  req += "Cache-Control: no-cache\r\n";
  req += "User-Agent: ";
  req += "PieNode";
  req += "\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Content-Length: " + String(String("message=" + message).length()) + "\r\n";
  req += "\r\n";
  req += "message=" + message;
  // Serial.println(req);
  client.print(req);
  String ipstring; 
  delay(20);

  // Serial.println("-------------");
  while(client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
    //Serial.println(line);
  }
  // Serial.println("-------------");
}


//////////////////////////NETPIE////////////////////////////////////////////////////////////////////


void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  Serial.print("Incoming message --> ");
  msg[msglen] = '\0';
  Serial.println((char *)msg);
}

void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Found new member --> ");
  for (int i=0; i<msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();  
}

void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Lost member --> ");
  for (int i=0; i<msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
 
  microgear.setName(PIENODEid);
}



void convertDataToNeTPIE()
{ //if inputString[0]=
  inputString.toCharArray(outputString,15);
if (outputString[0] == 'P')
    {Serial.println (outputString);
    }
    else
    { Serial.println ("No RF input");
    }


  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

int timer = 3000;

int testWifi(void) {
  int c = 0;
 
  Serial.println("Waiting for Wifi to connect");  
  lcd.setCursor(0, 2);
  while ( c < 30 ) {

      lcd.print(".");  
     ////display.print(".");
     ////display.display();
    if (WiFi.status() == WL_CONNECTED) { return(20); } 
    digitalWrite(pinOn,LOW);
      
    delay(500);
    digitalWrite(pinOn,HIGH);
    Serial.print(WiFi.status());  
    ////display.print(WiFi.status());
     ////display.display();
      
    c++;
  }
  Serial.println("Connect timed out, opening AP");
   lcd.setCursor(0, 1);
  lcd.print("opening AP");
  return(10);
} 
////////////////////////////////////////////////////////////////////////////////////////////////////



int mdns1(int webtype)
{
  // Check for any mDNS queries and send responses
//  mdns.update();
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
  // Serial.println("No client"); 
    return(20);
  }
  Serial.println("");
  Serial.println("New client");
    digitalWrite(pinOn,LOW);
    delay(1000);
   digitalWrite(pinOn,HIGH);
 // count =0;
  // Wait for data from client to become available
  while(client.connected() && !client.available()){
    delay(1);
   }
  
  // Read the first line of HTTP request
  String req = client.readStringUntil('\r');
  
  // First line of HTTP request looks like "GET /path HTTP/1.1"
  // Retrieve the "/path" part by finding the spaces
  int addr_start = req.indexOf(' ');
  int addr_end = req.indexOf(' ', addr_start + 1);
  if (addr_start == -1 || addr_end == -1) {
    Serial.print("Invalid request: ");
    Serial.println(req);
    return(20);
   }
  req = req.substring(addr_start + 1, addr_end);
  Serial.print("Request: ");
  Serial.println(req);
  client.flush(); 
  String s;
  if ( webtype == 1 ) {
      count =0;
      if (req == "/")
      {  
        IPAddress ip = WiFi.softAPIP();
        String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
        s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>";
        s+=" <head>  <meta charset=\"utf-8\">   <title>NECTEC PIE NODE</title>  </head>";
          s +=tempStyle; 
       s+="  <body>\r\n";

     
   s+="   <h1 class=\"logo\">\r\n";
   s+="     NECTEC <br> PIE NODE\r\n";
   
   s+="   </h1>\r\n";

   s+="   <h3 >ID NAME :";
     s +=PIENODEid;   
    s+=" </h3>\r\n";     
        
  s+="     <section>\r\n"; 
        s+=" <div class = \"button-text\" ><input class=\"button-blue\" type=\"button\" value=\"";
   
        char      charvalue[3];
     dtostrf(val1, 3, 1, charvalue);
        s += charvalue;
        
     
        s+="\">\r\n"; 
 s+="  <label>T2</label>\r\n"; 
 
 s+=" </div>\r\n"; 

   s+=" <div class = \"button-text\" ><input class=\"button-blue\" type=\"button\" value=\"";
   
          
     dtostrf(val2, 3, 1, charvalue);
        s += charvalue;
        
     
        s+="\">\r\n"; 
 s+="  <label>T1</label>\r\n"; 
 
 s+=" </div>\r\n"; 
    s+=" <div class = \"button-text\" ><input class=\"button-blue\" type=\"button\" value=\"";
   
          
     dtostrf(val3, 3, 1, charvalue);
        s += charvalue;
        
     
        s+="\">\r\n"; 
 s+="  <label>T3</label>\r\n"; 
 
 s+=" </div>\r\n"; 


 
 s+=" </section>   \r\n";     
          
        
         s += "<p>";
        s += page;

 

 s += " <div class = \"button-text\" > \r\n";   
 s += "   <form method='get' action='a'> \r\n";   
 s += "   <label>SSID: </label> \r\n";   
  s += "   <input   style=\"font-size:25px\" id='ssid' name='ssid' length=32> \r\n";   
 s += "    <label>Password: </label> \r\n";   
 s += "   <input style=\"font-size:25px\"  id = 'pass' name='pass' length=64> \r\n";   
    
 s += "   <input style=\"font-size:40px; width: 200px; height: 50px;\"  type='submit'></form> \r\n";   
 s += "     </div> \r\n";   

    
s += " </div></body> \r\n";   

        
       
        s += "</html>\r\n\r\n";
        Serial.println("Sending 200");
         Serial.println(s);
      }
      else if ( req.startsWith("/a?ssid=") ) {
        // /a?ssid=blahhhh&pass=poooo
        Serial.println("clearing eeprom");
      //  for (int i = 0; i < 96; ++i) { EEPROM.write(i, 0); }
        String qsid; 
        qsid = req.substring(8,req.indexOf('&'));
        Serial.println(qsid);
        Serial.println("");
        String qpass;
        qpass = req.substring(req.lastIndexOf('=')+1);
        Serial.println(qpass);
        Serial.println("");
       // WiFiManager::handleWifiSave();
         configssid = qsid.c_str();
         configpass = qpass.c_str();

 WiFi.begin(configssid,configpass);
 int loopconnect=0;
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
       digitalWrite(pinOn,LOW);
     loopconnect++; if (loopconnect>50) break;
    delay(200);
    digitalWrite(pinOn,HIGH);
    delay(200);
        
    }
    if (loopconnect < 50)
    {Serial.println("");
    Serial.println("WiFi connected");

    // Start the server
    server.begin();
    Serial.println("Server started");

    // Print the IP address
    Serial.print("ESP8266 IP: ");
    Serial.println(WiFi.localIP());
         
Serial.println("writing config"); 
        
        Serial.println("writing eeprom ssid:");
        for (int i = 0; i < qsid.length(); ++i)
          {
            EEPROM.write(i, qsid[i]);
            Serial.print("Wrote: ");
            Serial.println(qsid[i]); 
          }
        Serial.println("writing eeprom pass:"); 
        for (int i = 0; i < qpass.length(); ++i)
          {
            EEPROM.write(32+i, qpass[i]);
            Serial.print("Wrote: ");
            Serial.println(qpass[i]); 
          }    
        EEPROM.commit();
        s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>";
        s +="ID=";
        s +=PIENODEid;
        s += "Found ";
        s += req;
        s += "<p> saved to eeprom... reset to boot into new wifi</html>\r\n\r\n";
        delay(2000);
        count =70;
      }
      else
      {
        s = "HTTP/1.1 404 Not Found\r\n\r\n";
        Serial.println("Sending 404");
      }
      }
    else
    {
      Serial.println ("reconnect.") ;
    }
  } 
  else
  {
         if (req == "/")
      {
        IPAddress ip = WiFi.softAPIP();
      s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>";
        s+=" <head>  <meta charset=\"utf-8\">   <title>NECTEC PIE NODE</title>  </head>";
          s +=tempStyle; 
   
    
    s+="<meta http-equiv=\"refresh\" content=\"3\">";

     s+="  <body>\r\n";

     
   s+="   <h1 class=\"logo\">\r\n";
   s+="     NECTEC <br> PIE NODE\r\n";
    s+="     <br> for \r\n";
     s+="     <br> Sensing\r\n";
      s+="      <br> Management\r\n";
       s+="      <br> and\r\n";
        s+="      <br> Remote \r\n";
           s+="      <br>  Controlling\r\n";
   s+="   </h1>\r\n";

   s+="   <h3 >ID NAME :";
     s +=PIENODEid;   
    s+=" </h3>\r\n";     
        
  s+="     <section>\r\n"; 
        s+=" <div class = \"button-text\" ><input class=\"button-blue\" type=\"button\" value=\"";
   
        char      charvalue[3];
     dtostrf(val1, 3, 1, charvalue);
        s += charvalue;
        
     
        s+="\">\r\n"; 
 s+="  <label>T2</label>\r\n"; 
 
 s+=" </div>\r\n"; 

   s+=" <div class = \"button-text\" ><input class=\"button-blue\" type=\"button\" value=\"";
   
          
     dtostrf(val2, 3, 1, charvalue);
        s += charvalue;
        
     
        s+="\">\r\n"; 
 s+="  <label>T1</label>\r\n"; 
 
 s+=" </div>\r\n"; 
    s+=" <div class = \"button-text\" ><input class=\"button-blue\" type=\"button\" value=\"";
   
          
     dtostrf(val3, 3, 1, charvalue);
        s += charvalue;
        
     
        s+="\">\r\n"; 
 s+="  <label>T3</label>\r\n"; 
 
 s+=" </div>\r\n"; 


 
 s+=" </section>   \r\n";     
          
        
         s += "<p>";
        s += page;

  s += " <div class = \"button-text\" > \r\n";   
 s += "   <form method='get' action='scan'> \r\n";   
 
 
    
 s += "   <input style=\"font-size:40px; width: 500px; height: 50px;\"  type='submit' value=\"scan new network\"></form> \r\n";   
 s += "     </div> \r\n";   




    
s += " </div></body> \r\n";   

        
       
        s += "</html>\r\n\r\n";


      
       // Serial.println(s);
        Serial.println("Sending 200");
      }

 else if ( req.startsWith("/scan") ) {
 // scanAP(); 
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    {Serial.println("no networks found");
     ////display.setTextSize(1);
  ////display.setTextColor(WHITE);
  ////display.setCursor(10,10);
  ////display.println("no networks found");
   ////display.display();
    }
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
 
  

   
    for (int i = 0; i < n; ++i)
     {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      //////display.println(WiFi.SSID(i));
      
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
     }
  
   ////display.display();
  
  }
  Serial.println(""); 
  int indices[n];

      for (int i = 0; i < n; i++) {
        indices[i] = i;
      }

   //display networks in page
      for (int i = 0; i < n; i++) {
       // DEBUG_WM(WiFi.SSID(indices[i]));
       // DEBUG_WM(WiFi.RSSI(indices[i]));
        int quality = getRSSIasQuality(WiFi.RSSI(indices[i]));
         
        //if (_minimumQuality == -1 || _minimumQuality < quality) {
           if ( _minimumQuality < quality) {
          String item = FPSTR(HTTP_ITEM);
          String rssiQ;
          rssiQ += quality;
          item.replace("{v}", WiFi.SSID(indices[i]));
          item.replace("{r}", rssiQ);
          if (WiFi.encryptionType(indices[i]) != ENC_TYPE_NONE) {
            item.replace("{i}", "l");
          } else {
            item.replace("{i}", "");
          }
          //DEBUG_WM(item);
          page += item;
          delay(0);
        } else {
          Serial.println("Skipping due to quality");
        }

      }
      page += "<br/>";
    

  
    s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>";
        s+=" <head>  <meta charset=\"utf-8\">   <title>NECTEC PIE NODE</title>  </head>";
          s +=tempStyle; 
       s+="  <body>\r\n";

     
   s+="   <h1 class=\"logo\">\r\n";
   s+="     NECTEC <br> PIE NODE\r\n";
    s+="     <br> for \r\n";
     s+="     <br> Sensing\r\n";
      s+="      <br> Management\r\n";
       s+="      <br> and\r\n";
        s+="      <br> Remote \r\n";
           s+="      <br>  Controlling\r\n";
   s+="   </h1>\r\n";

   s+="   <h3 >ID NAME :";
     s +=PIENODEid;   
    s+=" </h3>\r\n";     
        
  s+="     <section>\r\n"; 
        s+=" <div class = \"button-text\" ><input class=\"button-blue\" type=\"button\" value=\"";
   
        char      charvalue[3];
     dtostrf(val1, 3, 1, charvalue);
        s += charvalue;
        
     
        s+="\">\r\n"; 
 s+="  <label>T1</label>\r\n"; 
 
 s+=" </div>\r\n"; 

   s+=" <div class = \"button-text\" ><input class=\"button-blue\" type=\"button\" value=\"";
   
          
     dtostrf(val2, 3, 1, charvalue);
        s += charvalue;
        
     
        s+="\">\r\n"; 
 s+="  <label>T2</label>\r\n"; 
 
 s+=" </div>\r\n"; 
    s+=" <div class = \"button-text\" ><input class=\"button-blue\" type=\"button\" value=\"";
   
          
     dtostrf(val3, 3, 1, charvalue);
        s += charvalue;
        
     
        s+="\">\r\n"; 
 s+="  <label>T3</label>\r\n"; 
 
 s+=" </div>\r\n"; 


 
 s+=" </section>   \r\n";     
          
        
         s += "<p>";
        s += page;

 

 s += " <div class = \"button-text\" > \r\n";   
 s += "   <form method='get' action='a'> \r\n";   
 s += "   <label>SSID: </label> \r\n";   
 s += "   <input   style=\"font-size:25px\" id='ssid' name='ssid' length=32> \r\n";   
 s += "    <label>Password: </label> \r\n";   
 s += "   <input style=\"font-size:25px\"  id = 'pass' name='pass' length=64> \r\n";   
    
 s += "   <input style=\"font-size:40px; width: 200px; height: 50px;\"  type='submit'></form> \r\n";   
 s += "     </div> \r\n";   

    
s += " </div></body> \r\n";   

        
       
        s += "</html>\r\n\r\n";
        Serial.println("Sending 200");
         Serial.println(s);
  
 }


  
      else if ( req.startsWith("/cleareeprom") ) {
        s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Hello from NECTEC SensorNode1";
        s += "<p>Clearing the EEPROM<p>";
        s += "</html>\r\n\r\n";
        Serial.println("Sending 200");  
        Serial.println("clearing eeprom");
        for (int i = 0; i < 96; ++i) { EEPROM.write(i, 0); }
        EEPROM.commit();
      }
      else
      {
        s = "HTTP/1.1 404 Not Found\r\n\r\n";
        Serial.println("Sending 404");
      }       
  }
  client.print(s);
  Serial.println("Done with client");
  return(20);
}

//////////////////Serial/////////////////////////////////////////////////////

void serialEvent() {
  
 //   char inChar = (char)Serial.read(); 
 inputString = Serial.readStringUntil('\n');
    stringComplete=true;
  Serial.println(inputString);
   
  
}

//////////////////AP setting//////////////////////////////////////

void setupAP(void) {
  
IPAddress apIP(10, 10, 10, 1);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    {Serial.println("no networks found");
           lcd.setCursor(0, 1);
  lcd.print("no networks found");
 
    }
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
  lcd.setCursor(0, 1);
  lcd.print("networks found");
  

   
    for (int i = 0; i < n; ++i)
     {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      //////display.println(WiFi.SSID(i));
      
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
     }
  
   ////display.display();
  
  }
  Serial.println(""); 
  int indices[n];

      for (int i = 0; i < n; i++) {
        indices[i] = i;
      }

   //display networks in page
      for (int i = 0; i < n; i++) {
       // DEBUG_WM(WiFi.SSID(indices[i]));
       // DEBUG_WM(WiFi.RSSI(indices[i]));
        int quality = getRSSIasQuality(WiFi.RSSI(indices[i]));
         
        //if (_minimumQuality == -1 || _minimumQuality < quality) {
           if ( _minimumQuality < quality) {
          String item = FPSTR(HTTP_ITEM);
          String rssiQ;
          rssiQ += quality;
          item.replace("{v}", WiFi.SSID(indices[i]));
          item.replace("{r}", rssiQ);
          if (WiFi.encryptionType(indices[i]) != ENC_TYPE_NONE) {
            item.replace("{i}", "l");
          } else {
            item.replace("{i}", "");
          }
          //DEBUG_WM(item);
          page += item;
          delay(0);
        } else {
          Serial.println("Skipping due to quality");
        }

      }
      page += "<br/>";
    




  
  delay(100);
  WiFi.softAP(ssid);
  Serial.println("softap");
  Serial.println("");
    Serial.println(WiFi.localIP());
          Serial.println(WiFi.softAPIP());
        
          server.begin();
          Serial.println("Server started");   
          int b = 20;
          int c = 0;
          while(b == 20) { 
            
             b = mdns1(1);
 
            count++;
            Serial.print("count=");
            Serial.println(count);
            Serial.println("C000");
            Serial.print("input=");
            serialEvent();


temp = sht31.readTemperature();
humid = sht31.readHumidity();

  if (! isnan(temp)) {  // check if 'is not a number'
    Serial.print("Temp *C = "); Serial.println(temp);
  } else { 
    Serial.println("Failed to read temperature");
  }
  
  if (! isnan(humid)) {  // check if 'is not a number'
    Serial.print("Hum. % = "); Serial.println(humid);
  } else { 
    Serial.println("Failed to read humidity");
  }
  Serial.println();
  delay(1000);



  
 if (count > 30)
 {
Serial.println("RESET...................................");
  pinMode(2,OUTPUT);
digitalWrite(2,LOW);
  digitalWrite(pinOn,LOW);
 //  pinMode(resetPin,INPUT);
  //digitalWrite(resetPin,LOW);
  }

//////////////////////////////////////////////////////







value = analogRead( ANALOG_PIN )*10;
for (int thisloop = 0; thisloop < 5; thisloop++) {
value = ((analogRead( ANALOG_PIN )*10)+value)/2; 
delay(100);
}

Serial.print("value1=");
  Serial.println(value);
value = (tempOffset1-(value))/2;
//value = (tempvalue+value)/2;
//tempvalue=value;
val1 = value;
val1 /=10;  
 
 char charvalue[3];
     dtostrf(val1, 3, 1, charvalue);
Serial.print("temp1=");
  Serial.println(charvalue);

 dtostrf(val1, 3, 1, charvalue);
  inputString=charvalue;
  
  dtostrf(val2, 3, 1, charvalue);
  inputString +=',';
inputString +=charvalue;
 dtostrf(val3, 3, 1, charvalue);
  inputString +=',';
inputString +=charvalue;





        
           }
  Serial.println("over");
  
}



void setup() {

 

//pinMode(sensor2Pin,INPUT);
//digitalWrite(sensor2Pin,HIGH);


pinMode(pinOn,OUTPUT);
digitalWrite(pinOn,HIGH);
// pinMode(resetPin,OUTPUT);
//digitalWrite(resetPin,LOW);

//pinMode(reset1Pin,OUTPUT);
//digitalWrite(reset1Pin,HIGH);
tempStyle = "\r\n";
 tempStyle += FPSTR(HTTP_SCRIPT);
tempStyle += "<style>\r\n";

tempStyle+= " body {\r\n";
tempStyle+= "  background: rgba(41, 63, 80, 0.44);\r\n";
  
tempStyle+= "  font: 300 16px/22px \"Lato\", \"Open Sans\", \"Helvetica Neue\", Helvetica, Arial, sans-serif;\r\n";
tempStyle+= "}\r\n";

tempStyle+= ".logo {\r\n";
  
tempStyle+= "  float: left;\r\n";
tempStyle+= "  font-size: 40px;\r\n";
tempStyle+= "  font-weight: bold;\r\n";
tempStyle+= "  letter-spacing: .5px;\r\n";
tempStyle+= "  line-height: 44px;\r\n";
tempStyle+= "  padding: 40px 40px 22px 0;\r\n";
tempStyle+= "  text-transform: uppercase;\r\n";
tempStyle+= "}\r\n";

tempStyle+= "div.button-text\r\n";
tempStyle+= "{width: 15%;\r\n";
tempStyle+= "display: inline-block;\r\n";
 
tempStyle+= "}\r\n";

tempStyle+= "input {\r\n";
tempStyle+= "font-size: 200%;\r\n";
tempStyle+= "  font-weight: bold;\r\n";
tempStyle+= "}\r\n";

tempStyle+= ".button-text label {\r\n";
tempStyle+= " padding: 40px 40px 22px 0;\r\n";
tempStyle+= " display:block;\r\n";
tempStyle+= "text-align:left;\r\n";
tempStyle+= "font-size: 200%;\r\n";
tempStyle+= "  font-weight: bold;\r\n";
tempStyle+= "}\r\n";
tempStyle +="</style>\r\n";


////display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
 ////display.display();

  delay(3000);  
  count =0;
	/* disable esp8266 watch dog, otherwise the board will keep rebooting */
    //ESP.wdtDisable();
   
     Serial.begin(9600);
     lcd.begin();
  // Turn on the blacklight and print a message.
  lcd.backlight();
//     mySerial.begin(2400);
     EEPROM.begin(512);
    /* Event listener */
    microgear.on(MESSAGE,onMsghandler);
 //   microgear.on(PRESENT,onFoundgear);
 //   microgear.on(ABSENT,onLostgear);
    microgear.on(CONNECTED,onConnected);





 if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }

lcd.print("PIENODE-TMECA-SHT31");

lcd.setCursor(0, 1);

lcd.print("Connect:");

    WiFi.begin();
      
      //WiFi.begin(esid.c_str(), epass.c_str());
    //   WiFi.begin(ssidfix,passwordfix);
      if ( testWifi() == 20 ) 
     {  
         
          Serial.println("Server started");  
   Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  lcd.setCursor(0, 1);
  lcd.print("Server: ");
  lcd.print(ssid);
   lcd.setCursor(0, 2);
  lcd.print("IP:");
  lcd.print(WiFi.localIP());

  
 if (!mdns.begin("nectec1", WiFi.localIP())) {
    Serial.println("Error setting up MDNS responder!");
    while(1) { 
      delay(1000);
    }
  }
  
  Serial.println("mDNS responder started");
  
  // Start TCP (HTTP) server
  server.begin();
  Serial.println("TCP server started");

Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
   Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  setSyncInterval(300);

 digitalClockDisplay();




  
 server.begin();
          
          microgear.init(GEARKEY,GEARSECRET,SCOPE);
          microgear.connect(APPID);
          microgear.setName(PIENODEid); 
          return;
      
     }
 
  
  


setupAP(); 
        
    
}





void loop() {



  
if (WiFi.status() != WL_CONNECTED) {
    delay(1);

    return;
}
  
 int webtype;
          int b = 20;
          int c = 0;
          while((b == 20)&&(c<1000)) { 
             b = mdns1(webtype);

 
         //   Serial.println("wait for client"); 
         //   Serial.println(c);
             c++;
           }
          c=0;

 onlinecount++;


if (onlinecount > 5000)

{
  
  pinMode(2,OUTPUT);
digitalWrite(2,LOW);
  digitalWrite(pinOn,LOW);
 //  pinMode(resetPin,INPUT);
  //digitalWrite(resetPin,LOW); 
  
  }

Serial.print(onlinecount);
Serial.println("count");

if((onlinecount%5 ==0))
{
 
if ((timeStatus() != timeNotSet)) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
     
      digitalClockDisplay();
  
    }
  }
else
{
   setSyncProvider(getNtpTime);
  setSyncInterval(300);
  //////display.println("try to connect NTP");
  //////display.display();
  }
}


  
 if (lastminute == mm)
{Serial.print ("Waiting for saving in ");

int waitTime = lastminute % 10;
Serial.print(10-waitTime);
Serial.println(" minute");
}
else
{
lastminute = mm;
FlagSave = lastminute % 10;

}

//Serial.print(lastminute);
//Serial.print(":");
//Serial.println(mm);

delay(1000);


if (FlagSave == 0)
  {

String feed;
feed = "{temp:";
 char charvalue[3];
 
  dtostrf(temp, 3, 1, charvalue);

feed += charvalue;
feed = ",humid:";

 
dtostrf(humid, 3, 1, charvalue);

feed += charvalue;
feed = ",temp3:";
feed += val3;
feed = "}";

//Serial.print("Line Notify: ");
//Serial.println(url);
//Line_Notify(url);




   microgear.writeFeed("PIE2",feed);
  // if(microgear.writeFeed("PIE2","{\"temp\":25.7,\"humid\":62.8,\"light\":8.5}"))
   Serial.println("write feed ok....................");


//delay(5000);
  
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    //return;
  }
 else
  { 

   

  // We now create a URI for the request
 // url = "/NECTECTemp/insert_PIENODE01.php?";
 String url = urltemp;
  url += "date=";
  url += yy;url += "-";
  url += mo;url += "-";
  url += dd;
  url += "&time=";
  url += hh;url += ":";
  url += mm;url += ":";
  url += ss;
   url += "&A=";
  
  dtostrf(temp, 3, 1, charvalue);
  url += charvalue;
   url += "&B=";
 
  dtostrf(humid, 3, 1, charvalue);
  url += charvalue;
   url += "&C=";
  url += val1;
   url += "&D=";
  url += val2;
   url += "&E=";
  url += val3;
   url += "&F=";
  url += v6;
   url += "&G=";
  url += v7;
   url += "&H=";
  url += v8;
  

  
   
  Serial.print("Requesting URL: ");
  Serial.println(url);
  delay(1000);
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");

          delay(1000);    
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
 String line = client.readStringUntil('\n');
   Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
               
delay(5000);
client.stop();
FlagSave = 1;
//////display.println("Save...");
//////display.display();

 
  }


 //microgear.resetToken();


  } // FlagSave

  
  



///////////////NETPIE//////////////////////////////////////////



 ///////////////////////////////////////////
 convertDataToNeTPIE();
  // Serial.println(inputString); 
// Serial.println(outputString); 
 // delay(2000);
 if (microgear.connected()) {
    Serial.println("connected");
////////////////////////////////////////
   microgear.loop();
 if (outputString[0]== 'P')
 {
    microgear.publish(PIENODE,outputString);
 }
//////////////////////////////////////////////////////



value = analogRead( ANALOG_PIN )*10;
for (int thisloop = 0; thisloop < 5; thisloop++) {
value = ((analogRead( ANALOG_PIN )*10)+value)/2; 
delay(50);
}

Serial.print("value1=");
  Serial.println(value);
//value = (tempOffset1-(value))/2;
//value = (tempvalue+value)/2;
//tempvalue=value;
val1 = value;
val1 /=10;  
 
 char charvalue[3];
     dtostrf(val1, 3, 1, charvalue);
Serial.print("temp1=");
  Serial.println(charvalue);

 dtostrf(val1, 3, 1, charvalue);
  inputString=charvalue;
  
  dtostrf(val2, 3, 1, charvalue);
  inputString +=',';
inputString +=charvalue;
 dtostrf(val3, 3, 1, charvalue);
  inputString +=',';
inputString +=charvalue;


lcd.setCursor(0, 1);
  lcd.print("value1:");
  lcd.print(inputString);

  /////////////////////
 temp = sht31.readTemperature();
 humid = sht31.readHumidity();


  dtostrf(temp, 3, 1, charvalue);
 Serial.print("Temp *C = "); Serial.println(charvalue);
   
  dtostrf(humid, 3, 1, charvalue);
Serial.print("Humid %RH = "); Serial.println(charvalue);


  if (! isnan(temp)) {  // check if 'is not a number'
    Serial.print("Temp *C = "); Serial.println(temp);
  } else { 
    Serial.println("Failed to read temperature");
  }
  
  if (! isnan(humid)) {  // check if 'is not a number'
    Serial.print("Hum. % = "); Serial.println(humid);
  } else { 
    Serial.println("Failed to read humidity");
  }
  Serial.println();

 dtostrf(temp, 3, 1, charvalue);
  inputString +=',';
inputString +=charvalue;
 dtostrf(humid, 3, 1, charvalue);
  inputString +=',';
inputString +=charvalue;




  
        Serial.print("Publish...");  Serial.println(inputString);
    inputString.toCharArray(outputString,30); 
        microgear.publish(PIENODE,outputString);


   
  
  
  }
  else {
      Serial.println("connection lost, reconnect...");

  //reconnect 
     digitalWrite(pinOn,LOW);
    delay(100);
   digitalWrite(pinOn,HIGH);
 delay(100);
 digitalWrite(pinOn,LOW);
    delay(100);
   digitalWrite(pinOn,HIGH);
   /////////////////////////
      
      if (timer >= 500) {
    //  microgear.resetToken();
      delay(100);
      microgear.connect(APPID);
      
      timer = 0;
    }
    else timer += 100;
  }

  delay(100);
}







