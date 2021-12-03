#include "accel_control.h"  // MPU control (for stick roll and acceleration)
#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include "ball_table.h"

#define CONTROL_RATE 1024 // Hz, powers of 2 are most reliable

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

float impact_vel=0;
float acc=0; float p_acc=0;
float vel=0; float p_vel=0;
float dist=1.0; float p_dist=1.0; float pp_dist=1.0;
const float LEN = 2.0; //stick LEN in m
const float LEN_MM = 2000; //stick LEN in mm
const float H = 1.0/256.0;  // in seconds

void setup(){
  Serial.begin(115200);
  while (!Serial) delay(10); // will pause until serial console opens
  setup_accelero();
  vel = 0.0; acc= 0.0;
  p_vel = 0.0; p_acc=0.0;
  startMozzi(CONTROL_RATE);
}


// Function when the ball rolls up
void rollingUp(float roll, float az){
  acc = 7.0 * sin(roll);
  //acc = az;

  if (p_dist > LEN){
    if (vel != 0) { 
      impact_vel = vel; 
      
    }
    vel = 0;
  }else{
    vel = p_vel + (H * ( (p_acc + acc ) / 2.0 )   ) ;
  }

  dist = p_dist + (H * ((p_vel + vel )/ 2.0)  );
   
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

   if (p_dist < 0){
      if (vel != 0) { impact_vel = vel; }
      vel = 0;
   }else{
      vel = p_vel + (H * ( (p_acc + acc ) / 2.0 )   ) ;
   }

   dist = p_dist + (H *((p_vel + vel)/2.0 ) ); 
   
   p_acc = acc;
   p_vel = vel;
   p_dist = dist;
}


float TILT_LIMIT = DEG_TO_RAD * 1;  
void updateControl(){
  getAccInfo();  // gets the accelerometer info 
  
  if ((tilt_y < 2*PI-TILT_LIMIT) && (tilt_y > PI)){
    rollingDown(tilt_y, accy);
    //Serial.print ("down ");
  }else if ((tilt_y > TILT_LIMIT) && (tilt_y < PI)){
    rollingUp(tilt_y, accy);  
    //Serial.print ("up ");
  }

//  Serial.print (tilt_y);
//  Serial.print (",");
//  Serial.print (acc);
//  Serial.print (",");
//  Serial.print (vel);
//  Serial.print (",");
//  Serial.print (impact_vel);
//  Serial.print (",");
//  Serial.println (dist);

}

int dd = 0;
int index = 0;
int updateAudio(){
  dd =  (int)(dist * 1000);
  if ((dd < 0) || (dd > LEN_MM)){
    return isinTable8[0];
  }
  index = dd % 30;
  return isinTable8[index];
}

void loop() {
  audioHook(); // required to be here
}
