#include "accel_control.h"  // MPU control (for stick roll and acceleration)
//#include "sound_contro.h"  

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator

#define CONTROL_RATE 256 // Hz, powers of 2 are most reliable
//Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);
//float gain;

uint8_t isinTable8[] = { 
   0,  26,  52,  78, 102, 125, 147, 167, 185, 201, 215, 226, 234, 240,
 243, 243, 240, 234, 226, 215, 201, 185, 167, 147, 125, 102,  78,  52,
  26,   0
}; 

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup(){
  Serial.begin(115200);
  while (!Serial) delay(10); // will pause until serial console opens
  setup_accelero();
  startMozzi(CONTROL_RATE);
  //aSin.setFreq(50);
  //gain = 255;
}

float acc=0; float p_acc=0;
float vel=0; float p_vel=0;
float dist=1.0; float p_dist=1.0; float pp_dist=1.0;
const int LEN = 2; //stick LEN in m
const float H = 1.0/256.0;  // in seconds

// Function when the ball rolls down
void rollingUp(float roll, float az){
  acc = 7.0 * sin(roll);
  //acc = az;

  if ((p_dist < 0) || (p_dist > LEN)){
    vel = 0;
  }else{
    vel = p_vel + (H * ( (p_acc + az ) / 2.0 )   ) ;
  }

  dist = p_dist + (H * ((p_vel + vel )/ 2.0)  );
  if (dist < 0.0){
     dist = 0.0; //acc=0.0; vel = 0.0; 
   }else if (dist > LEN){
     dist = LEN; //acc=0.0; vel = 0.0;
   }
   
  p_acc = acc;
  p_vel = vel;
  p_dist = dist;
}

// Function when the ball rolls DOWN
void rollingDown(double roll, float az){
   float sak = sin(roll);
   if (sak*sak < 0.2) {
    acc = 0;
   }else{
    acc = (9.8*sak - 1.96*(sak*-1)) * sqrt (1- (sak*sak) ) ;
   }

   if ((p_dist < 0) || (p_dist > LEN)){
      vel = 0;
   }else{
      vel = p_vel + (H * ( (p_acc + az ) / 2.0 )   ) ;
   }

   dist = p_dist + (H *((p_vel + vel)/2.0 ) ); 
   if (dist < 0.0){
     dist = 0.0; //acc=0.0; vel = 0.0;  
   }else if (dist > LEN){
     dist = LEN; //acc=0.0; vel = 0.0;
   }
   
   p_acc = acc;
   p_vel = vel;
   p_dist = dist;
}


float TILT_LIMIT = DEG_TO_RAD * 5;  
void updateControl(){
  getAccInfo();  // gets the accelerometer info 

  if ((tilt_x > 2*PI-TILT_LIMIT) && (tilt_x > TILT_LIMIT)){
     vel = 0.0; acc= 0.0;
     p_vel = 0.0; p_acc=0.0;
     //Serial.print("zero ");
  }else if ((tilt_x < 2*PI-TILT_LIMIT) && (tilt_x > PI)){
    rollingDown(tilt_x, accx);
    //Serial.print ("down ");
  }else if ((tilt_x > TILT_LIMIT) && (tilt_x < PI)){
    rollingUp(tilt_x, accx);  
    //Serial.print ("up ");
  }

//  Serial.print (tilt_x);
//  Serial.print (",");
//  Serial.print (acc);
//  Serial.print (",");
//  Serial.print (vel);
//  Serial.print (",");
//  Serial.println (dist);

}

//int updateAudio(){
//  if (dist != pp_dist){
//    pp_dist = dist;
//    return aSin.next() * gain; 
//  }
//  return 0;
//}

int dd = 0;
int index = 0;
int updateAudio(){
  dd =  (int)(dist * 1000);
  index = dd % 30;
  return isinTable8[index];
}

void loop() {
  audioHook(); // required to be here
}
