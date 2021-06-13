
#include "DRV8825.h"
#include "SyncDriver.h"

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
#define STEPS_PER_TURN 343

#define RPM_INDIVIDUAL 80
#define RPM_SYNC 20
/*#define LEDG 26
#define LEDR 14
#define LEDB 33*/

#define DIR 7
#define STEP 8

//#define RATIO ((float)76 / (float)21)

#define STEPS_CLOSE 1600
//if it exceed 3 moves the claw need to be recalibrate
int needtocalibrate_l = 0;
int needtocalibrate_r = 0;
//first is for opening or closing, second is for inclinaison (0 = 0° & 1=90°)


int stateClawLeft[2] = {1,0}; 
int stateClawRight[2] = {1,0};

DRV8825 leftClawOpener(MOTOR_STEPS, 25, 26);     // DIR=6, STEP=7
DRV8825 leftClawRotator(MOTOR_STEPS, 27, 14);    // DIR=8, STEP=9
DRV8825 rightClawRotator(MOTOR_STEPS, 32, 33); // DIR=10, STEP=11
DRV8825 rightClawOpener(MOTOR_STEPS, 12, 13);  // DIR=12, STEP=13

SyncDriver leftClaw(leftClawOpener, leftClawRotator);
SyncDriver rightClaw(rightClawOpener, rightClawRotator);

void setup()
{
  pinMode(0, OUTPUT);
  //0 for low, if low motors are activated
  MotorActivation(1);
  
  Serial.begin(115200);

  leftClawRotator.begin(RPM_SYNC, 32);
  leftClawRotator.enable();

  leftClawOpener.begin(RPM_SYNC , 32);
  leftClawOpener.enable();

  rightClawRotator.begin(RPM_SYNC, 32);
  rightClawRotator.enable();

  rightClawOpener.begin(RPM_SYNC , 32);
  rightClawOpener.enable();

}

int currAngle = 0;

typedef struct
{
  char instruction;
  uint8_t side;
} Instruction;

Instruction *instructions = (Instruction*)malloc(10 * sizeof(Instruction));
uint8_t instructionsCount = 0;
uint8_t currentInstruction = 0;

void loop()
{
  if (Serial.available())
  {
    char peaked = Serial.peek();
    if (peaked == 'S')
    {
      process();
      Serial.read();
      return;
    }
    if (peaked == 'D')
    {
      
      Serial.read();
      return;
    }

    if (Serial.peek() == ',')
    {
      Serial.read();
    }

    uint8_t side = Serial.parseInt();
    char instruction = Serial.read();

    if (instruction != 'R' && instruction != 'T' && instruction != 'O' && instruction != 'F')
    {
      return;
    }

    if (side != 1 && side != 2)
    {
      //Serial.println("Invalid instruction (side != 1 && side != 2). Resetted instruction stack");
      instructionsCount = 0;
      return;
    }
    /*
    Serial.print("Instruction = ");
    Serial.print(instruction);
    Serial.print(", side = ");
    Serial.println(side);*/

    memcpy(&instructions[instructionsCount].instruction, &instruction, 1);
    memcpy(&instructions[instructionsCount].side, &side, 1);

    instructionsCount++;

  }
  
}

void process()
{
  MotorActivation(0);
  //Serial.println("Starting processing instructions");
  while (currentInstruction < instructionsCount)
  {
    Instruction *instruction = &instructions[currentInstruction];
   /* Serial.print(instruction->instruction);
    Serial.print(", ");
    Serial.println(instruction->side);*/
    if (instruction->instruction == 'R')
    {
      //digitalWrite(LEDB, HIGH);
      rotateClaw(STEPS_PER_TURN / 4, instruction->side);
    }
    else if (instruction->instruction == 'T')
    {
      //digitalWrite(LEDB, LOW);
      rotateClaw((-STEPS_PER_TURN) / 4, instruction->side);
    }
    else if (instruction->instruction == 'O')
    {
     // digitalWrite(LEDR, HIGH);
      openClaw(instruction->side);
    }
    else if (instruction->instruction == 'F')
    {
     // digitalWrite(LEDR, LOW);
      closeClaw(instruction->side);
    }

    if (instruction->instruction == 'R' || instruction->instruction == 'T')
    {
      rotateOpener(STEPS_CLOSE/50, instruction->side);
    }

    //Serial.println("INSTRUCTION DONE");
    currentInstruction++;
    delay(250);
    
  }
  MotorActivation(1);
  currentInstruction = 0;
  instructionsCount = 0;
   Serial.println('E');
   
   
}



void closeClaw(int side)
{
  
  if (side == 1)
  {
    if(stateClawLeft[0]==0){
      rotateOpener(STEPS_CLOSE+60, side);
      stateClawLeft[0]=1;
    }
    
  }
  else
  {
    if(stateClawRight[0]==0){
      rotateOpener(-STEPS_CLOSE-60, side);
      stateClawRight[0]=1;
    }
  }
}

void openClaw(int side)
{
  
  if (side == 1)
  {
    if(stateClawLeft[0]==1){
      rotateOpener(-STEPS_CLOSE, side);
      stateClawLeft[0]=0;
    }
  }
  else
  {
   if(stateClawRight[0]==1){
      rotateOpener(STEPS_CLOSE, side);
      stateClawRight[0]=0;
    }
  }
}

void rotateOpener(int angle, int side)
{
  
  leftClawRotator.setRPM(RPM_INDIVIDUAL);
  leftClawOpener.setRPM(RPM_INDIVIDUAL );
  rightClawRotator.setRPM(RPM_INDIVIDUAL);
  rightClawOpener.setRPM(RPM_INDIVIDUAL );

  if (side == 1)
  {
    if(stateClawLeft[0]==0){
      
    leftClaw.rotate(angle, 0);
      
    }else{
    leftClaw.rotate(angle, 0);
  }
  }
  else
  {
   // digitalWrite(LEDG, LOW);
   if(stateClawLeft[0]==0){
    rightClaw.rotate(angle, 0 );
  }else{
    rightClaw.rotate(angle , 0 );
  }
}
}
void rotateClaw(long angle, int side)
{
  leftClawRotator.setRPM(RPM_SYNC );
  leftClawOpener.setRPM(RPM_SYNC );
  rightClawRotator.setRPM(RPM_SYNC);
  rightClawOpener.setRPM(RPM_SYNC );

  if (side == 1)
  {
    leftClaw.rotate(-angle*3.6 , -angle *3.65);
    if(stateClawLeft[1]==0){
     
      stateClawLeft[1]=1;
    }
    else{
      stateClawLeft[1]=0;
    }
    needtocalibrate_l++;
    
      //MotorCorrection(1);
    
    
  }
  else
  {
    rightClaw.rotate(-angle*3.6 , -angle*3.6);
    if(stateClawRight[1]==0){
     
      stateClawRight[1]=1;
    }
    else{
      stateClawRight[1]=0;
    }
    needtocalibrate_r++;
    
   //   MotorCorrection(2);
    
  }
}

void MotorActivation(int active){
  //HYPER IMPORTANT c'est le enable qui doit être à low
  
  digitalWrite(0,active);
  
}

void MotorCorrection(int side){


  MotorActivation(0);
  if(stateClawLeft[0]==1&&side==1){
    Serial.print("correction, left");
    if(needtocalibrate_l%2==0){
      leftClaw.rotate(60 , -0);
      Serial.print("correction, left forte");
    }
    else{
      leftClaw.rotate(40 , 0);
      Serial.print("correction, left,faible");
    }
    
    //print("correction, left");
  }
  if(stateClawRight[0]==1&&side==2){
    if(needtocalibrate_r%2==0){
      Serial.print("correction, right");
      rightClaw.rotate(-60 , 0);
      Serial.print("correction, right, forte");
    }
    else{
      rightClaw.rotate(-40 , 0);
      Serial.print("correction, right, faible");
    }
    
    
  }

}
