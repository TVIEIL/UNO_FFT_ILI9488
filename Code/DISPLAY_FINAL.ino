
         /*
         * Vieil Thierry 22/08/2022
         * Audio Spectrum Analyzer
         * ILI9488 Display Results of FFT Analyze 
         *
         * Exemple des 12 données transférées :
         * 
         * 390 Hz; 0
         * 468 Hz; 0
         * 546 Hz; 0
         * 625 Hz; 1
         * 703 Hz; 0
         * 781 Hz; 0
         * 859 Hz; 2
         * 937 Hz; 43
         * 1015 Hz; 70
         * 1093 Hz; 21
         * 1171 Hz; 0
         * 1250 Hz; 0
         */


#include "SPI.h"
#include <Adafruit_GFX.h>
#include <ILI9488.h>
#include <SoftwareSerial.h>

#define rxPin 2
#define txPin 3

#define TFT_CS         9
#define TFT_DC         8
#define TFT_RST        7
#define TFT_LED        6
#define TFT_WIDTH     480
#define TFT_HEIGHT    295 //320



const String TitreStr1 = "AUDIO SPECTRUM ANALYZER";
const String TitreStr2 = "Thierry VIEIL - F4HRB";


uint16_t buffer1[36]; //received data
uint16_t Freq_value_Data[10][2];
uint8_t Index_Freq_value_Data =0;
bool EN_Display = false;
uint16_t Freq_Low;
uint16_t Freq_High;
int Posi =0;
uint16_t xmin =85;
uint16_t x =xmin;
uint16_t y =TFT_HEIGHT;
uint8_t xwide_value_FFT =3;
bool WriteFrequenciesOnLCD = true;
bool debug_serial= true;
int test_valeur =0;



// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
ILI9488 tft = ILI9488(TFT_CS, TFT_DC, TFT_RST);

SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);

void setup() {
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  mySerial.begin(74880); 

  
  tft.begin();
  tft.setRotation(1);
  StartPage();
}


void loop(void) {
while (mySerial.available()>0){mySerial.read();}


     mySerial.write(10);
     delayMicroseconds(150); 


    for (int n=0;n<30;n++) { 
      if(mySerial.available()>0) {buffer1[n] = mySerial.read();} else {break;}
      if (n==29) EN_Display = true;
      } //end for



  if (EN_Display == true) {


    for (uint8_t q =0; q<30;q++) {

    if (q==0 || q==3 || q==6 || q==9 || q==12 || q==15 || q==18 || q==21 || q==24 || q==27 )
         {
         Freq_High = buffer1[q];
        }
    else {
         if (q==1 || q==4 || q==7 || q==10 || q==13 || q==16 || q==19 || q==22 || q==25 || q==28 ) 
           {
            Freq_Low = buffer1[q];
            Freq_value_Data[Index_Freq_value_Data][0] = (Freq_High *256)+Freq_Low;
          }
          else
          {
            Freq_value_Data[Index_Freq_value_Data][1]=buffer1[q];
            Index_Freq_value_Data++;
          }
        }


}// end for 


//Displayed Frequencies
/* 390 Hz; 468 Hz; 546 Hz; 625 Hz; 703 Hz; 781 Hz; 859 Hz; 937 Hz; 1015 Hz; 1093 Hz; 1171 Hz; 1250 Hz; */
if (WriteFrequenciesOnLCD == true) {

    tft.setCursor(10, 300);
    tft.setTextColor(ILI9488_WHITE);  tft.setTextSize(2);
    tft.print(Freq_value_Data[0][0]);
    tft.setTextColor(ILI9488_WHITE);  tft.setTextSize(2);
    tft.println(" Hz");

    tft.setTextColor(ILI9488_WHITE);  tft.setTextSize(2);
    tft.setCursor(10, 274);
    tft.println(Freq_value_Data[1][0]);
    
    tft.setCursor(10, 248);
    tft.println(Freq_value_Data[2][0]);
    
    tft.setCursor(10, 223);
    tft.println(Freq_value_Data[3][0]);

    tft.setCursor(10, 198);
    tft.println(Freq_value_Data[4][0]);
    
    tft.setCursor(10, 171);
    tft.println(Freq_value_Data[5][0]);
    
    tft.setCursor(10, 144);
    tft.println(Freq_value_Data[6][0]);
    
    tft.setCursor(10, 120);
    tft.println(Freq_value_Data[7][0]);
    
    tft.setCursor(10, 92);
    tft.println(Freq_value_Data[8][0]);
    
    tft.setCursor(10, 68);
    tft.println(Freq_value_Data[9][0]);
    

    
 WriteFrequenciesOnLCD = false;
}

//Display Data on LCD
      
      for ( y = TFT_HEIGHT; y>0; y-=26){


        test_valeur = (unsigned int)Freq_value_Data[Posi][1];
        
        if (test_valeur <=42) {tft.drawRect(x, y,xwide_value_FFT,25,ILI9488_BLACK);}
        if (test_valeur >=43 && test_valeur <=84) {tft.drawRect(x, y,xwide_value_FFT,25,ILI9488_BLACK);} 
        if (test_valeur>=85 && test_valeur<=126)  {tft.drawRect(x, y,xwide_value_FFT,25, ILI9488_BLACK);}
        if (test_valeur>=127 && test_valeur<=200) {tft.drawRect(x, y,xwide_value_FFT,25,ILI9488_BLACK);} //ILI9488_DARKGREY);}
        if (test_valeur>=201 && test_valeur<=249) {tft.drawRect(x, y,xwide_value_FFT,25,ILI9488_RED);}
        if (test_valeur>=250 && test_valeur<=253) {tft.drawRect(x, y,xwide_value_FFT,25,ILI9488_RED);}
        if (test_valeur>=254 && test_valeur<=255) {tft.drawRect(x, y,xwide_value_FFT,25,ILI9488_RED);}


          Serial.print(" Valeur y : ");
          Serial.println(y);
          Serial.print(" Valeur FFT : ");
          Serial.println(test_valeur);
        
        Posi++;
        if (Posi ==10 ) {Posi = 0; y=TFT_HEIGHT; break;}
       }//end for y
       

x=x+xwide_value_FFT +1;
if (x>480) x=xmin;

// Display data with 'Serial.'
if (debug_serial == true) {
  for (int r =0;r<10;r++) {
    Serial.print(Freq_value_Data[r][0]);
    Serial.print(" ");
    Serial.print(Freq_value_Data[r][1]);
    Serial.print(";");
    Serial.print(" ");
  }
  Serial.println("");
}
   
// nettoyage buffer1
for (uint8_t p=0;p<30;p++) { //p=0;p<30;p++
 buffer1[p] = 0;
} // end for

// nettoyage Freq_value_Data =0;
for (uint8_t p=0;p<10;p++) { 
 Freq_value_Data[p][0] = 0;
 Freq_value_Data[p][1] = 0;
}


//Reset Index_Freq_value_Data
Index_Freq_value_Data =0;

EN_Display = false;

} //end if EN_Display

  
} // end void

void StartPage(){
  
  tft.setCursor(30, 120);
  tft.fillRect(20, 110, 440, 90, ILI9488_BLUE);
  tft.setTextColor(ILI9488_WHITE);  tft.setTextSize(3);
  tft.println(TitreStr1);
  tft.setCursor(50, 170);
  tft.setTextColor(ILI9488_RED);  tft.setTextSize(3);
  tft.println(TitreStr2);
  delay(2000);
  tft.fillScreen(ILI9488_BLACK);
}



