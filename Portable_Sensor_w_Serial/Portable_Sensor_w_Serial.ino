#include <LiquidCrystal.h>
//                BS E D4  D5  D6  D7
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

int DHT_PIN = 2;
bool swtch;
bool scale;
bool err;
int actStart;
bool serialAtch;
int serialIN;

void setup() {
  // put your setup code here, to run once:
  err = false;
  swtch = true;
  scale = true;

  lcd.begin(16,2);
  lcd.setCursor(4,0);
  lcd.print("!-Code-!");
  lcd.setCursor(3,1);
  lcd.print("!-Loaded-!");
  delay(1500);
  clearLCD();
  Serial.begin(9600);

  lcd.setCursor(0,0);
  lcd.print("TEMP - ");
  lcd.setCursor(0,1);
  lcd.print("HUMD - ");
  
}

void dhtERROR(){
  err = true;
  
  clearLCD();
  lcd.setCursor(3,0);
  lcd.print("!-SENSOR-!");
  lcd.setCursor(3,1);
  lcd.print("!-ERROR-!");
}

void clearLCD()
{
  lcd.setCursor(0,0);
  lcd.print("                      ");
  lcd.setCursor(0,1);
  lcd.print("                      ");
  delay(100);
}

void dec2bin(int n)
{
  int c, k;

  for (c=15; c>=0; c--)
  {
    k=n >> c;

    if (k&1)
      Serial.print("1");
     else
      Serial.print("0");
  }
}

void dec2bin8(int n)
{
  int c, k;

  for (c=7;c >=0; c--)
  {
    k=n >> c;

    if (k&1)
      Serial.print("1");
    else
      Serial.print("0"); 
  }
}

void wait_for_dht11()
{
  delay(100);
}

void start_signal(uint8_t dht11_pin)
{
  pinMode(dht11_pin, OUTPUT);
  digitalWrite(dht11_pin, LOW); 
  delay(18);
  digitalWrite(dht11_pin, HIGH);
  pinMode(dht11_pin, INPUT);
  digitalWrite(dht11_pin, HIGH); 
}

double read_dht_temp (bool scale)
{
  uint16_t rawTemperature = 0;
  uint8_t checkSum = 0;
  uint16_t data = 0;

  uint8_t tempi;
  uint8_t tempd;
  double full_temp;
  double full_temp_f;

  unsigned long startTime;
  
  for ( int8_t i = -3 ; i < 80; i++ ) {
    byte live;
    startTime = micros();

    do {
      live = (unsigned long)(micros() - startTime);
      if ( live > 90 ) {
        Serial.println("ERROR_TIMEOUT");
        dhtERROR();
        return 0;
      }
    }
    while ( digitalRead(DHT_PIN) == (i & 1) ? HIGH : LOW );

    if ( i >= 0 && (i & 1) ) {
      data <<= 1;

      // TON of bit 0 is maximum 30 usecs and of bit 1 is at least 68 usecs.
      if ( live > 30 ) {
        data |= 1; // we got a one
      }
    }

    switch ( i ) {
      case 63:
        rawTemperature = data;
      case 79: 
        checkSum = data;
        data = 0;
        break;
    }
  }

  tempi = rawTemperature >> 8;
  rawTemperature = rawTemperature << 8;
  tempd = rawTemperature >> 8;

  full_temp = tempd;
  full_temp = tempi + (full_temp/10);

  if(scale){
    full_temp_f = (full_temp * 1.8) + 32;
    return full_temp_f;
  } else {
    return full_temp;
  }
}

int read_dht_humid ()
{
  uint16_t rawHumidity = 0;
  uint16_t rawTemperature = 0;
  uint8_t checkSum = 0;
  uint16_t data = 0;

  uint8_t humi;
  uint8_t humd;
  int full_humid;

  unsigned long startTime;

  for ( int8_t i = -3 ; i < 80; i++ ) {
    byte live;
    startTime = micros();

    do {
      live = (unsigned long)(micros() - startTime);
      if ( live > 90 ) {
        Serial.println("ERROR_TIMEOUT");
        dhtERROR();
        return 0;
      }
    }
    while ( digitalRead(DHT_PIN) == (i & 1) ? HIGH : LOW );

    if ( i >= 0 && (i & 1) ) {
      data <<= 1;

      // TON of bit 0 is maximum 30 usecs and of bit 1 is at least 68 usecs.
      if ( live > 30 ) {
        data |= 1; // we got a one
      }
    }

    switch ( i ) {
      case 31:
        rawHumidity = data;
        break;
      case 79: 
        checkSum = data;
        data = 0;
        break;
    }
  }

   humi = rawHumidity >> 8;
   rawHumidity = rawHumidity << 8;
   humd = rawHumidity >> 8;

   full_humid = humd;
   full_humid = humi + (full_humid/10);
  
   return(full_humid);
}

void printTemp(bool scale){
  if (!err){
    //set cursor for Temp
    lcd.setCursor(7,0);
    
    //ready DHT11   
    wait_for_dht11();
    start_signal(DHT_PIN);
    
    //print temp
    lcd.print(read_dht_temp(scale));
    lcd.setCursor(12,0);
    if(scale){
      lcd.print("  F"); 
    } else {
      lcd.print("  C");
    }
    lcd.setCursor(13,0);
    lcd.print((char)223);
  }
  
}

void printHumid(){
  if (!err){
    
    //ready DHT11
    wait_for_dht11();
    start_signal(DHT_PIN);
    
    //read humidity
    int hm = 0;
    hm = read_dht_humid();
    if (hm < 10) {
      lcd.setCursor(7,1);
      lcd.print("0");
      lcd.setCursor(8,1);
    } else {
    //set cursor for Humidity >=10
    lcd.setCursor(7,1);
    }
    lcd.print(hm);
    lcd.setCursor(9,1);
    lcd.print("%");
  }
  
}

void serialPrint(){
  if (!err){
    //get humidity
    wait_for_dht11();
    start_signal(DHT_PIN);
    int hum = read_dht_humid();

    //get temp farenheight
    wait_for_dht11();
    start_signal(DHT_PIN);
    double Tf = read_dht_temp(true);

    //get temp celsius
    wait_for_dht11();
    start_signal(DHT_PIN);
    double Tc = read_dht_temp(false);
    
    //print to serial
    Serial.print(hum);
    Serial.print(",");
    Serial.print(Tf);
    Serial.print(",");
    Serial.println(Tc);
  }
}

void printBaseActivity(int ctr) {

 delay(1000);
  int cursorLoc = 12 + ctr;
  lcd.setCursor(cursorLoc,1);
  if(swtch){
    lcd.print("_");
    swtch = false; 
  } else {
    lcd.print("-");
    swtch = true;
  }
  
}

void printSerialActivity(int ctr) {
  
  switch (ctr) {
    case 0:
      lcd.setCursor(13,1);
      lcd.print("   ");
  
      break;
    case 1:
      lcd.setCursor(13,1);
      lcd.print("-  ");
  
      break;
    case 2:
      lcd.setCursor(13,1);
      lcd.print("-- ");
  
      break;
    case 3:
      lcd.setCursor(13,1);
      lcd.print("-->");
      serialPrint();
  
      break;
  }
  
}

void loop() {

  if (err){
    //ensure error holds screen
    dhtERROR();
    delay(10000);
  } else {
    // Activity monitor
    if(Serial.readString() == ""){
      actStart = 1;
      serialAtch = false;
      //Serial.end();
      //Serial.begin(9600);
    } else {
      actStart = 0;
      serialAtch = true;
    }
    for (int ctr = actStart; ctr <= 3; ctr++){
        //print temperature
        printTemp(scale);
            
        //print humidity
        printHumid();


        
        //Activity ID (lower right)
        if(!serialAtch){
          printBaseActivity(ctr);
        } else {
          printSerialActivity(ctr);
        }
      
    } // end activity monitor

    //print to serial

  } // end error check
  
}// end main loop
