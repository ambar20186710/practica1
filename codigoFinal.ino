#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define ledVerde 8
#define ledAzul 7
#define ledBlanco 6
#define ledAmarillo 5
#define ledRojo 4
#define ledNaranja 9
#define sensor A0
#define tonePin 13
#define boton 11
#define bomba 12

LiquidCrystal_I2C lcd(0x27,16,2);

bool banderaBomba = false; 
  

byte gota[8] = {
B00100,			
B01110,			
B01110,			
B11111,			
B11111,			
B11111,			
B01110,			
B00100			
};

byte rayo[8]{
B00001,			
B00011,			
B00110,			
B01110,			
B11111,			
B01110,			
B11000,			
B10000			

};

// Estado anterior del botón


//control de los leds
void controlLed(int a,int b,int c){
	  digitalWrite(ledVerde, a);
    digitalWrite(ledAzul, b);
    digitalWrite(ledRojo, c);
}


//Obtiene la lectura del sensor y retorna el porcentaje.
uint8_t getSoilmoisture ( void ){
  uint16_t valor =  analogRead(sensor);
  uint8_t porcentaje = map(valor, 0, 1023, 100, 0 );
  return porcentaje;
}

//Obtiene cómo parámetro el resultado de getSoilmoisture 
//y realiza la lógica necesaria para cambiar de color los LEDs.
void ledIndicator ( uint8_t data ){
	
  if(data <= 100 && data > 75){
    //verde
  	controlLed(255,0,0);
  }
  else if(data <= 75 && data > 50){
    //azul
  	controlLed(0,255,0);
  }
  else if(data <= 50 && data > 25){
    //blanco
  	controlLed(255,255,255);
  }
  else if(data <= 25 && data > 0){
    //amarillo
  	controlLed(255,255,0);
  }
  else if(data ==0 ){
    //rojo
  	controlLed(0,0,255);
  }
}



//Realiza lo mismo que la función anterior solo que 
//en lugar de cambiar los LEDs de color, 
//muestra en la pantalla el porcentaje de humedad.
void showValue( uint8_t data ){
	  lcd.setCursor(0,0);
	  lcd.print("Humedad: ");
    lcd.setCursor(9,0);
    lcd.print("    ");
    lcd.setCursor(9,0);
    lcd.print(data);
  	lcd.print("%");
  	lcd.setCursor(13, 0);
  	lcd.write(byte(0));
  	Serial.print("Humedad: ");
  	Serial.print(data);
  	Serial.println("%");
  	
  	int analogo = analogRead(sensor);
  
  	lcd.setCursor(0,1);
	  lcd.print("Analogo: ");
    lcd.setCursor(9,1);
    lcd.print("    ");
    lcd.setCursor(9,1);
    lcd.print(analogo);
  	lcd.setCursor(13, 1);
  	lcd.write(byte(1));
  	Serial.print("Analogo: ");
  	Serial.println(analogo);
}

//indicar que melodía va a tocar.
void playSong ( uint8_t song ){
  
  if(song == LOW){
    controlLed(0,0,0);
    controlLed(0,255,128);
    tone(tonePin, 262, 500);
    delay(500);
    tone(tonePin, 262, 500);
    delay(500); 
    tone(tonePin, 349, 500);
    delay(500); 
    tone(tonePin, 392, 500);
    delay(500);  
    return;
  }
  else{
    controlLed(0,255,128);
  	tone(tonePin, 293, 608.1075); 
    delay(675.675);
    delay(225.225);
    tone(tonePin, 293, 66.5117578125);
    delay(73.901953125);
    delay(38.710546875);
    tone(tonePin, 293, 66.5117578125);
    delay(73.901953125);
    return;
  }
} 

//Activa la bomba cuando la humedad está por debajo del 80%.
void drainWater ( bool humidityFlag ){
  if(humidityFlag == true ){
    digitalWrite(bomba, LOW);   // poner el Pin en HIGH
    Serial.println("bomba");
  }else{
    digitalWrite(bomba, HIGH);  
  }
}
  

void setup()
{
   lcd.init();
   lcd.backlight();
   pinMode(ledVerde, OUTPUT);
   pinMode(ledAzul, OUTPUT);
   pinMode(ledBlanco, OUTPUT);
   pinMode(ledAmarillo, OUTPUT);
   pinMode(ledRojo, OUTPUT);
   pinMode(ledNaranja, OUTPUT);
   pinMode(sensor, INPUT);
   pinMode(bomba, OUTPUT);
   Serial.begin(9600);
   lcd.createChar(0,gota);
   lcd.createChar(1,rayo);
   pinMode(boton, INPUT);
   pinMode(tonePin, OUTPUT);
   digitalWrite(bomba, HIGH);
}

void loop()
{
  
  int humedad = getSoilmoisture();
  showValue(humedad);
  delay(250);

  if(digitalRead(boton)){
    banderaBomba = !banderaBomba;
    delay(250);
  }
 
  // Verifica si el botón ha sido presionado
  while (banderaBomba != 0) {
        playSong(true);
        drainWater(true);
        delay(250);
        controlLed(0,255,0);
        Serial.println("estoy en auto");
        if(digitalRead(boton)){
          banderaBomba = !banderaBomba;
          delay(250);
        }
  }
  
        playSong(false);
        drainWater(false);
        ledIndicator(humedad);
        Serial.println("estoy en manual");
        Serial.println(banderaBomba);
}