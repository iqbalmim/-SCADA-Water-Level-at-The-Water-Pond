//===================================================================
#include <modbus.h>
#include <modbusDevice.h>
#include <modbusRegBank.h>
#include <modbusSlave.h>
modbusDevice regBank;
modbusSlave slave;

//===================================================================
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); //atau 0x3F

//===================================================================
int trig = 2;         // Pin Trigger
int echo = 3;         // Pin Echo

long duration;
int distance;
int AI1, AI0;
int jarak;

//===================================================================
void setup() {
  regBank.setId(1);
  regBank.add(30001); // Potensio (untuk percobaan)
  regBank.add(30006); // Sensor Sltrasonik
  regBank.add(40004); // Led Maintenance
  regBank.add(40009); // Led Mandatory
  regBank.add(40012); // Relay
  
  pinMode (12,OUTPUT);        // Relay
  pinMode (5, OUTPUT);        // Led Maintenance
  pinMode (4, INPUT_PULLUP);  // Saklar Maintenance
  pinMode (6, OUTPUT);        // Led Mandatory
  pinMode (trig, OUTPUT);     // Set trigger menjadi OUTPUT
  pinMode (echo, INPUT);      // Set echo menjadi INPUT
  Serial.begin (9600);        // Komunikasi Serial Arduino

  slave._device = &regBank;  
  slave.setBaud(9600);   

  lcd.init();                 // Inisiasi LCD
  lcd.backlight();            // Menghidupkan backlight LCD
  lcd.clear();
  
}

//===================================================================
void loop() {
  
int DO12 = regBank.get(40012);            // Inisiasi relay opc
  if (DO12 == 1 ) digitalWrite(12 ,HIGH); // Indikator relay 
  if (DO12 == 0 ) digitalWrite(12 ,LOW);  // Indikator relay 

int DO9 = regBank.get(40009);             // Inisiasi mandatory opc
  if (DO9 == 1 ) digitalWrite(9 ,HIGH);   // Led indikator mandatory
  if (DO9 == 0 ) digitalWrite(9 ,LOW);    // Led indikator mandatory

//===================================================================
byte DI4 = digitalRead(4);  // Input saklar maintenance
  if (DI4 == 1){            // Jika saklar maintenance off
   regBank.set(40004,0);    // Nilai di opc 0
   digitalWrite(5, LOW);}   // Led maintenance off
  if (DI4 == 0){            // Jika saklar maintenance on
   regBank.set(40004,1);    // Nilai di opc 1
   digitalWrite(5, HIGH);}  // Led maintenance on
      
//===================================================================      
  // Koding jarak menggunakan sensor ultrasonik
  digitalWrite(trig, LOW);   
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH); 
  distance = duration * 0.034 / 2;
  jarak = 36 - distance;
  
//===================================================================
  AI0 = analogRead(A0);            // Inisiasi potensio (percobaan)
  regBank.set(30001, (word) AI0);
  
  AI1 = jarak;                     // Inisiasi sensor ultrasonik opc
  regBank.set(30006, (word) AI1); 
  
  slave.run(); 

//===================================================================
  lcd.setCursor(2, 0);      // Set posisi baris 1 LCD
  lcd.print("Water Level"); // Set judul
  lcd.setCursor(1, 1);      // Set posisi baris 2 LCD
  lcd.print("Tinggi : ");   // Set kondisi
  lcd.print(jarak);         // Inisiasi kondisi
  lcd.print(" cm");         // Satuan kondisi
  delay(100);               // Delay pembacaan sensor
}
//===================================================================
