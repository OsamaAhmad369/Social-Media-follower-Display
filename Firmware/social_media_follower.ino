/*
 * Maximum Characters 
facebook page Channel id=75   (90)
youtube Channel ID=60         (70)
twitter =30                   (50)
Instagram User name=35        (55)
SSID=32;
Password=64;*/

#include <WiFi.h>
#include <HTTPClient.h>
#include "InstagramStats.h"         // https://github.com/witnessmenow/arduino-instagram-stats
#include <YoutubeApi.h>
#include <TwitterApi.h>
#include <ArduinoJson.h>
#include "JsonStreamingParser.h"    // https://github.com/squix78/json-streaming-parser
#include <WiFiClientSecure.h>
#include <EEPROM.h>
#include <SPI.h>
#include "LedMatrix.h"

#define NUMBER_OF_DEVICES 4 //number of led matrix connect in series
#define CS_PIN 15
#define CLK_PIN 14
#define MISO_PIN 2 //we do not use this pin just fill to match constructor
#define MOSI_PIN 12
#define CHAR_WIDTH 7
#define INC 22 //Increment the number 
#define DECR 23 // Decrement the number

int num=0,num_2=0;
bool _debug=false;
int y_followers;
#define BEARER_TOKEN "AAAAAAAAAAAAAAAAAAAAAEMaLgEAAAAA3tWeN7%2BxcNZF8tefUfHbXvXSYJY%3D69Nq4iuLtb8exK2GYqOSpi5J694RKKyH0qFgxh1JODzpLAXoue"
#define API_KEY "AIzaSyAkOUjXX7who8qzr84mTgTDoXCHB4mJ6Wk"  // your google apps API Token



String FACEBOOK_ACCESS_TOKEN = "EAAFN0yUCTokBABVahP16PKNgrtceH4vM0wHvvYY9OIZAIbASJzZC2Bwwzsy2nqka1OH2HF0twhty5QUcwGQvbFwjH1ExfUoAfBvMarE5buojvlAOTRbuRoLqbZCBZCFjL5jsYDHvLeIUtUrLLk13DNVLyBMCH2Rm1OhEhh0eV3yVplCpLr10geZA6cfipjty5IXuAQ2Rd9wZDZD";    // not needed for the page fan count
String ipStr;

LedMatrix ledMatrix = LedMatrix(NUMBER_OF_DEVICES, CLK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
WiFiClientSecure secureClient;
TwitterApi twitterapi(secureClient);
YoutubeApi api(API_KEY, secureClient);
InstagramStats instaStats(secureClient);

String read_eeprom(int start_address, int end_address,int data_len)
{
  String estring = "";
  for (int i = start_address; i < EEPROM.read(data_len)+start_address; i++)
  {
    estring += char(EEPROM.read(i));
   
  }
  return  estring;
  }
String read_eeprom2(int start_address, int end_address)
{
  String estring = "";
  for (int i = start_address; i <=end_address; i++)
  {
    estring += char(EEPROM.read(i));
   
  }
  return  estring;
  }
String stats,a;

void setup() 
{ 
  pinMode(INC,INPUT);
  pinMode(DECR,INPUT);
   EEPROM.begin(512); //Initialasing EEPROM
     delay(10);
  Serial.begin(9600);
  connectWiFi();  
  ledMatrix.init();
  _debug=true;

}

void loop() 
{ 
  if(Serial.available())
  {
    a=Serial.readStringUntil(',');
    }
    if(a=="wifi"){
      num_2=1;
      a="0";
      clear_eeprom(0,96);
      Serial.println("Enter the SSID:");
      }
      else if(a=="facebook")
      {
        num_2=2;
         a="0";
         clear_eeprom(97,187);
        Serial.println("Enter the Facebook Page ID:");
        }
        else if(a=="youtube")
        {
          num_2=3;
           a="0";
           clear_eeprom(188,258);
           Serial.println("Enter the Youtube Channel ID:");
          }
          else if(a=="twitter")
          {
            num_2=4;
             a="0";
             clear_eeprom(259,309);
           Serial.println("Enter the twitter Screen Name:");
            }
            else if(a=="instagram")
            { 
               num_2=5;
               a="0";
               clear_eeprom(310,365);
               Serial.println("Enter the Instagram User Name:");
              }
       if(num_2==1&& a!="0")
       {
        write_eeprom(0,32,511,a); //SSID
        a="0";
        num_2=10;
       Serial.println("Enter the Password:");
        }
        else if(num_2==10 && a!="0")
        {
            Serial.println(a);
        write_eeprom(33,96,510,a); //Password
        num_2=0;
        Serial.println("Save Sucessfully");
        ESP.restart();
          }
          else if(num_2==5 && a!="0")
          {
           write_eeprom(310,365,509,a); //Instagram User name
            num_2=0;
             Serial.println("Save Sucessfully");
             ESP.restart(); 
            }
            else if(num_2==4 && a!="0")
          {
           write_eeprom(259,309,508,a); //Twitter Account
            num_2=0;
             Serial.println("Save Sucessfully");
             ESP.restart(); 
            }
            else if(num_2==2 && a!="0")
          {
           write_eeprom(97,187,507,a); //FaceBook Page ID
            num_2=0;
             Serial.println("Save Sucessfully");
             ESP.restart(); 
            }
             else if(num_2==3 && a!="0")
          {
           write_eeprom(188,258,506,a); //Youtube
            num_2=0;
             Serial.println("Save Sucessfully");
             ESP.restart(); 
            }
            
  if(digitalRead(INC)==LOW)
  {
    delay(700);
    if(num<4){
    num=num+1;}
   
    }
    if(digitalRead(DECR)==LOW)
  {
    delay(700);
    if(num>0){
    num=num-1;}
    
    }
    if(num==1)
    {
   //Instagram followers
    InstagramUserStats response = instaStats.getUserStats(read_eeprom(310,365,509));
    scrollValueTilPassed("Instagram:");
    scrollValueAndStop(formatData(response.followedByCount));
    }
    //Youtube followers
    if(num==2){
     if(api.getChannelStatistics(read_eeprom(188,258,506)))
    {
    y_followers=api.channelStats.subscriberCount;
    scrollValueTilPassed("YouTube:");
    scrollValueAndStop(formatData(y_followers));
    }
    }
      //facebook page follower
      if(num==3){
     int pageLikes =getPageFanCount(read_eeprom(97,187,507),FACEBOOK_ACCESS_TOKEN);
    if(pageLikes >= 0) {
      scrollValueTilPassed("Facebook:");
      scrollValueAndStop(formatData(pageLikes));
    }
    }
    //Twitter followers
    if(num==4)
    {
      twitterapi.setBearerToken(BEARER_TOKEN);
      int twitterFollowers=twitterapi.getTwitterFollowers(read_eeprom(259,309,508));
      if(twitterFollowers>=0){
      scrollValueTilPassed("Twitter:");
      scrollValueAndStop(formatData(twitterFollowers));}
    }
   
}
int getPageFanCount(String pageId,String Access_token)
{
  HTTPClient http;

  http.begin("https://graph.facebook.com/"+ pageId+"?fields=name,fan_count&access_token="+Access_token);

  int httpCode = http.GET();

  if(httpCode <= 0)
  {
    //Serial.println("Error : no HTTP code");
    return -1;
  }

  String answer = http.getString();
  http.end();
  
 DynamicJsonDocument jsonBuffer(8096);
         // Parse JSON object
        DeserializationError error = deserializeJson(jsonBuffer,answer);
        Serial.println(answer);
  if (!error) {
    if (jsonBuffer.containsKey("fan_count"))
    {
      return jsonBuffer["fan_count"].as<int>();
    }
    
   // Serial.println("Incompatible JSON");
  }
  else
  {
   // Serial.println("Failed to parse JSON");
  }
  
  return -1;
}
void connectWiFi(void)
{
  int i;
 Serial.println();
 Serial.print("Connecting to: ");

  String ssid=read_eeprom2(0,32);
  String pass=read_eeprom2(33,96);
 Serial.println(ssid);
  Serial.println(pass);
  WiFi.begin(ssid.c_str(), pass.c_str());
  while ((WiFi.status() != WL_CONNECTED) && i < 20)
  {
    i ++;
    delay(500);
    Serial.print(".");
  }
  if ( WiFi.status() == WL_CONNECTED)
  {
    IPAddress ip = WiFi.localIP();
    ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
//    Serial.println("");
//    Serial.println("WiFi Connected.");
//    Serial.println("IP address: ");
//    Serial.println(WiFi.localIP());
//    Serial.print("Subnet Mask: ");
//    Serial.println(WiFi.subnetMask());
//    Serial.print("Gateway: ");
//    Serial.println(WiFi.gatewayIP());
  }
  else
  {
    ipStr = "NOT CONNECTED";
//    Serial.println("");
//    Serial.println("WiFi not connected.");
   }
  delay(1000);
}
void clear_eeprom(int start_address, int end_address)
{
  for (int i = start_address; i <= end_address; i++)
  {
    EEPROM.write(i,0);
  }
 }
 void write_eeprom(int start_address, int end_address,int data_len,String data_in)
{ 
  int r=0; 
  EEPROM.write(data_len,data_in.length());
  EEPROM.commit();
  
  for (int i = start_address; i <= end_address; i++)
  {
    EEPROM.write(i,data_in[r]);
    EEPROM.commit();
    r++;
  }
 }

void scrollValueAndStop(String value){
  int lengthToScroll = value.length() * CHAR_WIDTH + 1;
  ledMatrix.setText(value);
  for (int i = 0; i < lengthToScroll; i ++){
    ledMatrix.clear();
    ledMatrix.scrollTextLeft();
    ledMatrix.drawText();
    ledMatrix.commit();
    delay(50);
  }
}

void scrollValueTilPassed(String value){
  int lengthToScroll = (value.length() * CHAR_WIDTH) + (NUMBER_OF_DEVICES * 8);
  ledMatrix.setText(value);
  for (int i = 0; i < lengthToScroll; i ++){
    ledMatrix.clear();
    ledMatrix.scrollTextLeft();
    ledMatrix.drawText();
    ledMatrix.commit();
    delay(50);
  }
}
String formatData(int data){
  int beforeDecimal;
  int afterDecimal;
  if(data < 10000) {
    return String(data);
  }
  if(data < 1000000) {
    return String(data/1000) + "K";
  } else {
    return String(data/1000000) + "M";
  }
}
