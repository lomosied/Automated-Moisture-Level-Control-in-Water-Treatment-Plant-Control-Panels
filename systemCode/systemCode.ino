#include <SoftwareSerial.h>
#include <DFRobot_DHT11.h>
#include <LiquidCrystal.h>
DFRobot_DHT11 DHT;
#define DHT11_PIN 8

SoftwareSerial mySerial(11, 10); //SIM800L Tx & Rx is connected to Arduino #11 & #10  

const int rs = 12, en = 9, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int airHeater = 13; // Define input pin for relay
int inletFan = 7; // Define input pin for relay
int outletFan = 6; // Define input pin for relay

int SmsSent = 0;

String message;
float temp_reading;
float intakeTemperature;

const int Sensor = A0; //define temperature sensor pin which is connected to the analogue input pins of the arduino
byte degree_symbol[8] = 
              {
                0b00111,
                0b00101,
                0b00111,
                0b00000,
                0b00000,
                0b00000,
                0b00000,
                0b00000
              };
void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
 
   pinMode(Sensor, INPUT);// initialize temp sensor as an input
   pinMode(airHeater, OUTPUT); // Define the Relaypin as output pin
   pinMode(inletFan, OUTPUT); // Define the Relaypin as output pin
   pinMode(outletFan, OUTPUT); // Define the Relaypin as output pin

 lcd.begin(16, 2);
}

void loop() {
  // put your main code here, to run repeatedly:
  // put your main code here, to run repeatedly:
   temp_reading=analogRead(Sensor);// store the reading from the temp sensor in temp_reading
     intakeTemperature = temp_reading*(5.0/1023.0)*100;// calculate temperature 
     delay(10); 
DHT.read(DHT11_PIN);
 /*
  Serial.print("Component temp:");
  Serial.print(DHT.temperature);
  Serial.print("  Component humidity:");
  Serial.println(DHT.humidity);
  delay(1000); 

Serial.print("Intake Temperature: ");
 Serial.println(intakeTemperature);
 delay(1000);
 */
  if (DHT.humidity >= 40){
    Serial.println("Critical Humidity Levels");
    Serial.println(DHT.humidity);
    delay(1000);
    digitalWrite(airHeater, LOW); // Makes the signal low
    digitalWrite(inletFan, LOW); // Makes the signal low
    digitalWrite(outletFan, LOW); // Makes the signal low
    message = "Critical Humidity detected in Panel 1 \n Control mechanism initiated \n Fans On | Heater On";
    if(SmsSent == 0){ 
    sendMessage();
    SmsSent = 1;
    }
    
    criticalHumidityDisplay();
  
    }
  else if (intakeTemperature > 50 ){
    Serial.println("Critical Inlet Air Temp");
    digitalWrite(airHeater, HIGH); // Makes the signal low
    digitalWrite(inletFan, LOW); // Makes the signal low
    digitalWrite(outletFan, LOW); // Makes the signal low
message = "Critical Inlet Temp Detected in Panel 1 \n Control Mechanism Initiated \n Fans On";
    if (SmsSent == 0){
  sendMessage();
    SmsSent = 1;
    }
  criticalInletTempDisplay();
  }
  else if (DHT.temperature > 40){
    Serial.println("Critical component Air Temp");
    digitalWrite(airHeater, HIGH); // Makes the signal low
    digitalWrite(inletFan, LOW); // Makes the signal low
    digitalWrite(outletFan, LOW); // Makes the signal low
message = "Critical component Temp detected in Panel 1 \n Control Mechanism Initiated \n Fans on";
    if (SmsSent ==0 ){
      sendMessage();
      SmsSent = 1;
      }
    }
    else if (DHT.temperature < 20){
       Serial.println("Critical component Air Temp");
    digitalWrite(airHeater, LOW); // Makes the signal low
    digitalWrite(inletFan, LOW); // Makes the signal low
    digitalWrite(outletFan, HIGH); // Makes the signal low
message = "Critical Low component Temp detected in Panel 1 \n Control Mechanism Initiated \n In Fan on  | Heater On";
    if (SmsSent ==0 ){
      sendMessage();
      SmsSent = 1;
      }
      }
  
    
else{
  digitalWrite(airHeater, HIGH); // Sends high signal
  digitalWrite(inletFan, HIGH); // Sends high signal
  digitalWrite(outletFan, HIGH); // Sends high signal
  
  Serial.println(" ");
  Serial.print("Component temp:");
  Serial.println(DHT.temperature);
  delay(500);
  
  Serial.println(" ");
  Serial.print("Humidity:");
  Serial.println(DHT.humidity);
  delay(500); 

  Serial.println(" ");
  Serial.print("Intake Temperature: ");
  Serial.println(intakeTemperature);
  delay(500);

  lcdDisplay();
}

}

void lcdDisplay()
{
  lcd.setCursor(0,0);
  lcd.print("Normal operation");
  lcd.setCursor(0,1);
  lcd.print("Comp Temp: ");
  lcd.println(DHT.temperature);
  delay(500);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Humidity: ");
  lcd.println(DHT.humidity);
  lcd.setCursor(0,1);
  lcd.print("Inlet Temp: ");
  lcd.println(intakeTemperature);
  delay(500);
  }

 void criticalInletTempDisplay()
 {
  lcd.setCursor(0,0);
  lcd.print("Critical In temp");
  lcd.setCursor(0,1);
  lcd.print("Heater OFF");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Inlet Fan ON");  
  lcd.setCursor(0,1);
  lcd.print("Inlet Fan ON");
  delay(1000);
  }
  void criticalHumidityDisplay()
  {
    lcd.setCursor(0,0);
    lcd.print("Critical Humi");
  lcd.setCursor(0,1);
  lcd.print("Heater ON");
    delay(1000);
     lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Inlet Fan ON");
   lcd.setCursor(0,1);
  lcd.print("Outlet fan on");
  delay(1000);
    }
  void criticalComponentTempDisplay()
 {
  lcd.setCursor(0,0);
  lcd.print("High Comp Temp");
  lcd.setCursor(0,1);
  lcd.print("Heater off");
  delay(1000);
   lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Inlet Fan ON");
  lcd.setCursor(0,1);
  lcd.print("Outlet Fan ON");
   delay(1000);
  }
  void sendMessage()
  {
      //Begin serial communication with Arduino and SIM800L
  mySerial.begin(9600);
  Serial.println("Initializing..."); 
  delay(1000);
  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
 mySerial.println("AT+CMGS=\"+254798223888\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
  updateSerial();
  
  mySerial.print(message); //text content
  updateSerial();
  mySerial.write(26);
    }
 
  void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}
