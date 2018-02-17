#include <dht.h>
#include<stdlib.h>
#define dhtpin 7
#define ssid "Drago"
#define pass "drago1234"
#define api "acet.ac.in"
String GET="GET /firesafty/requestData/request.php?field=M1&field1=";
//String Field2="&field2=";
bool updated;
dht x;
//int BuzzerPin=13;
int AnalogPin=A0;
//int AnalogPin1=A1;
//int thresholdvalue1=400;
//int thresholdvalue2=400;

bool ConnectedWifi()
{
  delay(2000);
  Serial.println("AT+CWMODE=1\n");
  delay(5000);
  String cmd=("AT+CWJAP=\"");
    cmd+=ssid;
    cmd+="\",\"";
    cmd+=pass;
    cmd+="\"\n";
    Serial.println(cmd);
    delay(5000);
    if(Serial.find("OK")){
      return true;
    }
    else
    {
    return false;
    }
}


bool updatevalues(String temp)//,String analogvalue0)
{
  String cmd="AT+CIPSTART=\"TCP\",\"";
  cmd+=api;
  cmd+="\",80";
  Serial.println(cmd);
  delay(2000);
  if(Serial.find("ERROR"))
  {
    return false;
  }
  cmd=GET;
  cmd+=temp;
  //cmd+=Field2;
  //cmd+=analogvalue0;
  //cmd+=Field3;
  //cmd+=analogvalue1;
  cmd+="\r\n";
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  if(Serial.println(">")){
    Serial.print(cmd);
  }
  else{
     Serial.println("AT+CIPCLOSE");
      
  }
  delay(5000);
  if(Serial.find("OK"))
  {
    return true;
  }
  else{
    return false;
  }
}
void setup() {
  // put your setup code here, to run once:
//pinMode(BuzzerPin,OUTPUT);
pinMode(AnalogPin,INPUT);
//pinMode(AnalogPin1,INPUT);
//Serial.begin(9600);
Serial.begin(115200);
Serial.println("AT");
delay(5000);
 if(Serial.find("OK"))
 {
    bool connected=ConnectedWifi();
    if(!connected)
    {
      Serial.println("WIFI is not connected");
    }
 }
    else
    {
      Serial.println("Wifi is connected");
    }
  
//    else
//    {
//      Serial.println("Wifi module is not responding");
//    }
}

void loop() {
  // put your main code here, to run repeatedly:
x.read11(dhtpin);
float temp,hum;
temp=x.temperature;
//hum=x.humidity;
//float analogvalue0=analogRead(AnalogPin);
//int analogvalue1=analogRead(AnalogPin1);
//if(analogvalue0>thresholdvalue1 || temp>40){
//  digitalWrite(BuzzerPin,HIGH);
//  Serial.println("the building is set on fire alert the staff and sendng the messages to the fire department and traffic police");
//}
//else if(analogvalue0==thresholdvalue1 || temp==40){
//  digitalWrite(BuzzerPin,HIGH);
//  Serial.println("the building is on fire and issue is minor alert the staff members to control");
//  }
//  else{
//    
//  
//  digitalWrite(BuzzerPin,LOW);
//  Serial.println("the building is in normal condition");
//  }
  updated=updatevalues(String(temp));//,String(analogvalue0)); //,String(analogvalue1));
if(updated){
  Serial.println("values are being updated ");
}
else{
  Serial.println("updatation of data is failed");
}
  delay(5000);

}

