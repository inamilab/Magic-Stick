#include "accel_control.h"  // MPU control (for stick roll and acceleration)
#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include "ball_table.h"

#define CONTROL_RATE 256 // Hz, powers of 2 are most reliable

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

float acc=0; float p_acc=0;  // simulated marbel acceleration
float vel=0; float p_vel=0;  // simulated marbel velocity
float dist=1.0; float p_dist=1.0;  // simulated marbel distance
const float LEN = 0.6; //2.0; //stick lenght in m
const float LEN_MM = 600; //2000; //stick lenght in mm
const float H = 1.0/256.0;  // drag simulation teime step in seconds
const float HH = 0.4; // impact simulation time step in seconds (the smaller the larger the decay time)
// variables to control the impact decay rate
float impact_vel=0;
float impact_curve = 1;
float i_dist = 0.0;

void setup(){
  Serial.begin(115200);
  while (!Serial) delay(10); // will pause until serial console opens-
  setup_accelero();
  vel = 0.0; acc= 0.0;
  p_vel = 0.0; p_acc=0.0;
  startMozzi(CONTROL_RATE);
}

// function to calculate the marvel impact decay sine wave
void marbelImpact(){
    
  impact_curve = (4*impact_vel * exp(-(i_dist)*(i_dist)));
  i_dist = i_dist + (HH * ((impact_vel )/ 2.0)  );  // HH value was adjusted (but I cannot feel the relation velocity/impact force clearly)
  //dist = p_dist + (H *((p_vel + vel)/2.0 ) );
  if (impact_curve < 0){ impact_curve = 0;}

  // SERIAL DEBUG
  // Serial.print (impact_vel);
  // Serial.print (",");
  // Serial.print (impact_curve);
  // Serial.print (",");
  // Serial.println (i_dist/100.0);
}


// Function when the ball rolls up
void rollingUp(float roll, float az){
  acc = 7.0 * sin(roll);
  //acc = az;

  if (p_dist > LEN){
    if (vel != 0) { impact_vel = vel; i_dist = 0;  }  //impact_vel = vel; i_dist = dist;
    vel = 0;
    marbelImpact();
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
      if (vel != 0)  { impact_vel = -vel;  i_dist = 0;  }  // impact_vel = -vel;  i_dist = LEN;
      vel = 0;  
      marbelImpact();  
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
    dd = (int)(i_dist * 1000);
    index = dd % 30;
    return isinTable8[index] * impact_curve;
    //return isinTable8[0];
  }
  index = dd % 30;
  return isinTable8[index];
}

void loop() {
  audioHook(); // required to be here
}
