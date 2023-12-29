/**
  ******************************************************************************
  * @file    VFO_LED_DISPLAY-RX-TX-Beta2.ino
  * @author  Stefano Homebrew
  * @version V1.0
  * @date    05-12-2023
  *
  ******************************************************************************
  *
  *                     VFO_LED_DISPLAY-RX-TX-Beta2 - PROJECT
  *
  * This project "VFO_LED_DISPLAY-RX-TX with si5351", define an experimental open platform to build
  * a RX/TX Vfo for Radio's Homebrewing 
  * 
  * Library: 
  * Led_Control https://www.pjrc.com/teensy/td_libs_LedControl.html
  * Rotary Encoder https://github.com/brianlow/Rotary
  * Si5351 https://github.com/etherkit/Si5351Arduino
  * 
  * My projects videos https://www.youtube.com/@stefanohomebrew
  *
  * NOTE: this is an experimental project and the functions can be changed
  * without any advise.
  * The SI5351 VFO_LED_DISPLAY uses examples and projects freeware and available in the opensource
  * community.
  *
  * The SI5351 VFO_LED_DISPLAY-RX-TX openSource software is released under the license:
  *              Common Creative - Attribution 3.0
  ******************************************************************************
*/

#include "LedControl.h"
#include <Rotary.h>
#include "Wire.h"
#include <si5351.h>

unsigned long freq_old = 0;
unsigned long freq = 9610000; 
unsigned long freq2 = 9610000;
unsigned long freq3 = (freq);
unsigned long freqSi = 455000;
unsigned long freqPLL;

int passo = 10000;
byte pass= 1;
byte band = 0;
byte mdfrq = 0;
int var1= 0;
byte var3= 0;
byte var5= 0;
byte var6= 0;

int bottoneStep = A0;
int bottoneBand = A1;
int bottoneMfrq = A2;
int bottoneRxTx = A3;
//int releRxTx = 9;
/*
 pin 1 is connected to the DataIn 6
 pin 2 is connected to the CLK 8
 pin 3 is connected to CS 7
*/
LedControl lc=LedControl(6,8,7,1);
Rotary r = Rotary(3, 2);
Si5351 si5351;


void setup() {
    Serial.begin (9600);
    pinMode(bottoneStep,INPUT_PULLUP);
    pinMode(bottoneBand,INPUT_PULLUP);
    pinMode(bottoneMfrq,INPUT_PULLUP);
    pinMode(bottoneRxTx,INPUT_PULLUP);
    pinMode(5,OUTPUT);

    digitalWrite(5, LOW);
    
    PCICR |= (1 << PCIE2);
    PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);
    sei();
//////////////////////////////////////////////SI5351 INIT VALUE
    si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0,0);
    si5351.set_correction(100000, SI5351_PLL_INPUT_XO);
    si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_2MA);
    si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);
    si5351.output_enable(SI5351_CLK1, 1);///////////////TX OUT (OPTIONAL)
    si5351.output_enable(SI5351_CLK0, 0);///////////////RX OUT
    
//////////////////////////////////////////////LED INIT VALUE    
    lc.shutdown(0,false);// turn off power saving, enables display
    lc.setIntensity(0,10);// sets brightness (0~15 possible values)
    lc.clearDisplay(0);// clear screen

    printNumber(00000001);////PRINT LED DISPLAY VERSION NUMBER/////////////////////////////////
    delay(2000);
    printNumber(freq3);  
 
    inviapll();  
}
 
/////////////////////////////////////////////////////  
ISR(PCINT2_vect) {
  unsigned char result = r.process();
  if (result == DIR_NONE) {
    // do nothing
  }
  else if (result == DIR_CW) {  
    freq = (freq + passo);
    freq3 = (freq);
   printNumber(freq3);



  }
  else if (result == DIR_CCW) {   
    freq = (freq - passo);
    freq3 = (freq);
   printNumber(freq3);


  } 
  }
//////////////////////////////////////////////////////////////////
void loop() {
////////////////////////////BOTTONE RTX PORT A3
if (digitalRead(bottoneRxTx) == LOW){  
    var5= 1; //memorizza sulla variabile il bottone 
    if (var5 != var6){   /////////riconosce se viene premuto il tasto
    trasmetti(); 
    var6 = var5;  
  }
}
  
if (digitalRead(bottoneRxTx) == HIGH){
    var5 = 0;
    if (var5!= var6){   /////////riconosce se viene premuto il tasto     
    ricevi();
    var6 = var5; 
   }
}
//////////////////////////////END BOTTONE RTX  
    if (digitalRead(bottoneStep) == LOW) {
    delay(200); 
    Step();
    }
    ////////////////////////////////START OPTIONAL BANDS SWITCH
 
    if (digitalRead(bottoneBand) == LOW) {
    delay(200); 
    Bands();
    }
   
    ////////////////////////////////END OPTIONAL BANDS SWITCH
    if (digitalRead(bottoneMfrq) == LOW) {
    delay(200); 
    MediaFrq();
    }
    /////////////////////////////////////IF ROTARY ENCODER ROTATION         
     if (freq != freq_old)  {   
    inviapll();
     freq_old = freq;
     }
}
////////////////////////////////SEND TO SI5351
void inviapll() {
  //  Serial.println(freq);
    freqPLL = freq - freqSi;
    si5351.set_freq ((freqPLL * 100), SI5351_CLK0);
}

void printNumber(long num)
{
  long terzo;
  long quinto;
  long quarto;
  long sesto;
  long settimo;
  long ottavo;
  long var4=(long)num;
  long secondo,primo; 
  primo=var4%10;
  var4=var4/10;
  secondo=var4%10;
  var4=var4/10;
  terzo=var4%10;
  var4=var4/10;
  quarto=var4%10;
  var4=var4/10;
  quinto=var4%10; 
  var4=var4/10; 
   sesto=var4%10; 
  var4=var4/10; 
  settimo=var4%10; 
  var4=var4/10; 
  ottavo=var4%10; 
  var4=var4/10; 
  
  //Now print the number digit by digit
  lc.setDigit(0,7,(byte)ottavo,false);
  lc.setDigit(0,6,(byte)settimo,true);
  lc.setDigit(0,5,(byte)sesto,false);
  lc.setDigit(0,4,(byte)quinto,false);
  lc.setDigit(0,3,(byte)quarto,true);
  lc.setDigit(0,2,(byte)terzo,false);
  lc.setDigit(0,1,(byte)secondo,false);
  lc.setDigit(0,0,(byte)primo,false);
  delay (300);  
}
////////////////////////////////STEP     
void Step(){   
    pass++;
    if (pass > 3) {
      (pass = 1);
    }
    if (pass < 1) {
      (pass = 1);
    }
   if (pass == 1) {
   printNumber(1000);
    
      passo = 1000;
    }
   if (pass == 2) {
   printNumber(10000);
      
      passo = 10000;
    }
   if (pass == 3) {
   printNumber(100);
     
      passo = 100;    
    }        
 Serial.println(passo);   
}

////////////////////////////////OPTIONAL BANDS SWITCH
void Bands(){  
   band++;
   if (band == 1) {
    freq = 1860000;
    inviapll();
    freq3 = (freq);
   printNumber(freq3);
     
      passo = 1000;
    }
    if (band == 2) {
    freq = 3600000;
    inviapll();
    freq3 = (freq);
   printNumber(freq3);
    
      passo = 1000;
    }   
   if (band == 3) {
    freq = 7100000;
    inviapll();
    freq3 = (freq);
   printNumber(freq3);
     
      passo = 1000;
    }   
   if (band == 4) {
    freq = 27125000;
    inviapll();
    freq3 = (freq);
   printNumber(freq3);
      
      passo = 1000;
    }
       if (band == 4) {
        band = 0;
       }
   //    Serial.println(band);
}
////////////////////////////////END BANDS 
/////////////////////////////////START MEDIA FRQ
void MediaFrq(){  
   mdfrq++;
   if (mdfrq == 1) {
    freqSi = 0;
    inviapll();
   
   printNumber(0);
     

    }   
   if (mdfrq == 2) {
    freqSi = 455000;
    inviapll();
   printNumber(freqSi);
      
      passo = 1000;
    }   
   if (mdfrq == 3) {
    freqSi = 450000;
    inviapll();

   printNumber(freqSi);
    

    }
       if (mdfrq == 3) {
        mdfrq = 0;
       }
      // Serial.println(mdfrq);
}
///////////////////////////R-TX PLL CHANGE VALUE & SI5351 CLK OUTPUT
void trasmetti(){
    freqPLL = (freq);/// TX FREQUENCY 
    si5351.output_enable(SI5351_CLK1, 0);//rx
    si5351.output_enable(SI5351_CLK0, 1);//(OPTIONAL)
    si5351.set_freq ((freqPLL * 100), SI5351_CLK0);  // SEND TO PLL
    digitalWrite(5, HIGH); 
    delay(100);
    Serial.println(freqPLL); 
  }
void ricevi(){
   
    freqPLL = (freq - freqSi);//////////////////////I.F. - or +
    si5351.output_enable(SI5351_CLK1, 0);//rx
    si5351.output_enable(SI5351_CLK0, 1);//(OPTIONAL)
    si5351.set_freq ((freqPLL * 100), SI5351_CLK0);  // SEND TO PLL
    digitalWrite(5, LOW);
    delay(100); 
    Serial.println(freqPLL);
    
     }
