
#include "arduinoFFT.h"
#include <SoftwareSerial.h>

#define rxPin 2
#define txPin 3

/* 
 *  
 *
 * Vieil Thierry 22/08/2022
 * Audio Spectrum Analyzer
 * FFT Analyze  use ADC on A0
 * 
 */
 
arduinoFFT FFT = arduinoFFT(); /* Create FFT object */
/*
These values can be changed in order to evaluate the functions
*/
#define CHANNEL A0
const uint16_t samples = 64; //This value MUST ALWAYS be a power of 2 //32
const double samplingFrequency = 2500; //Hz, must be less than 10000 due to ADC //2500

unsigned int sampling_period_us;
unsigned long microseconds;


double vReal[samples];
double vImag[samples];
uint16_t Transmitted_Data[32][2];

/*uint16_t Transmitted_Data[32][2] = {{0,0},{0,0},{0,0},{0,0},{0,0},{390,25},{468,156},{546,198},
*                                    {625,23},{703,47},{781,63},{859,202},{937,255},{1015,0},
*                                    {1093,300},{1171,127},{1250,200},{0,0},{0,0},{0,0},
*                                    {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
*                                    {0,0},{0,0},{0,0}};
*                                    
*/

byte Freq_Low;
byte Freq_High;

SoftwareSerial mySerial =  SoftwareSerial(rxPin, txPin);


void setup()
{

 pinMode(rxPin, INPUT);
 pinMode(txPin, OUTPUT);
 mySerial.begin(74880);
 
 sampling_period_us = round(1000000*(1.0/samplingFrequency));


}

void loop()
{
          /*SAMPLING*/
          microseconds = micros();
  
         for(int i=0; i<samples; i++)
          {
            vReal[i] = analogRead(CHANNEL);
            vImag[i] = 0;
            while(micros() - microseconds < sampling_period_us){
              //empty loop 
            }
            microseconds += sampling_period_us;
          }
  
        FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
        FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
        FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */
        /*Serial.println("Computed magnitudes:");*/


         for (uint8_t i = 0; i < 32; i++) {

            Transmitted_Data[i][0] =uint16_t( ((i * 1.0 * samplingFrequency) / samples)); //abscissa
            Transmitted_Data[i][1] = uint16_t( vReal[i]);  //tempdata
            } //end for 

if (mySerial.available() > 0) {
       mySerial.read();

       for (byte w = 18;w<28; w++) { //w = 5;w<15; w++


        if (Transmitted_Data[w][1]>255) {Transmitted_Data[w][1]=255;}
        //Transmitted_Data[w][0] = Transmitted_Data[w][0] +78; // correctif de fréquence
        Freq_High = byte(Transmitted_Data[w][0] >>8); //right shift of 8 bits 
        mySerial.write(Freq_High); // fréquence FFT High byte
        Freq_Low = byte(Transmitted_Data[w][0] & 0x00FF); //mask Freq_High
        mySerial.write(Freq_Low); // fréquence FFT low byte
        mySerial.write(Transmitted_Data[w][1]); // valeur FFT

       } //end for
             
} //end if myserial

       }

       
