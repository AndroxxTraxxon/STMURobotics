
//constants
#define	Motor1P 2
#define Motor1N 3
    
#define	Motor2P 4
#define	Motor2N 5
    
#define	m1encA 6
#define	m1encB 7

#define	m2encA 8
#define	m2encB 9

#define errorPin 13

#define	m1Offset 0
#define	m2Offset 0

#define	Motor3P 10
#define	Motor3N 11

#define sonarEcho0 22
#define sonarTrig0 23
#define sonarEcho1 24
#define sonarTrig1 25
#define sonarEcho2 26
#define sonarTrig2 27
#define sonarEcho3 29
#define sonarTrig3 28

#define colorS0 30
#define colorS1 31
#define colorS2 33
#define colorS3 32
#define colorOUT 34

    
#define	clawOpenPower 200
#define	clawPower 150

#define	clawOpenDelay 300 
#define	clawCloseDelay 150 
    
#define	cutoff 15


#define	m1rate 5 
#define	m2rate 5
#define	rotation 425
#define	maxMotorPower 100
#define	diam 8.5f
#define	wdiam 4.75f
	
//debug commands
#define DBG_NO_ERROR 0
#define DBG_TEST 		1
#define DBG_ERROR   -1
#define DBG_PRT_ENC 2

	//motor commands will work as such: 0001CCMM
	//where the C digits are the command type, selecting between target, power, and rate modes
	//and the M digits will be the motor selection, allowing for up to 4 commands and 4 motors

//claw commands
#define CLAW_OPEN 0
#define CLAW_CLOSE 1
	//base claw command is 0010XXXX

#define Motor0 0
#define Motor1 1
#define Motor2 2
#define Motor3 3

//encoder commands
//base encoder command is 0011XXXX
//sonar commands
#define SONAR_0			0
#define SONAR_1			1
#define SONAR_2			2
#define SONAR_3			3
	//base sonar command is 0100XXXX
	


//variables
int m1pwr = 0,
	m2pwr = 0,
	m1pwrset = 0,
	m2pwrset = 0,
	encoder1 = 0,
	encoder2 = 0,
	m1target = 0, 
	m2target = 0, 
	programProgress;
	
byte 
	lastEnc1,
	lastEnc2,
	cur1, 
	cur2, 
	masterMotor = Motor0;
    

enum MotorMode {follow, target, power, rate};

MotorMode motorMode =power;

bool newCMD = false, ERROR_THROWN = true;
//String cmd = "";
      
int triggerPins[] = {sonarTrig0,sonarTrig1,sonarTrig2,sonarTrig3};
int echoPins[] = {sonarEcho0,sonarEcho1,sonarEcho2,sonarEcho3};
int sensorCount = 4;


void setup(){
	//setup the IO pins
	pinSetup();
	initEncoders();
	
	
	
	
	
	
}

void loop(){
	
	switch(programProgress)
	{
		case 0:
			break;
		case 1:
			break;
		default:
		DEBUG(DBG_ERROR);
	}
	
	
	//update encoders as often as is possible
	updateMotorPower();
	UpdateEncoder(0);
}


void pinSetup()
{
	//Motor Setup
  pinMode(Motor1P, OUTPUT);
  pinMode(Motor1N, OUTPUT);
  pinMode(Motor2P, OUTPUT);
  pinMode(Motor2N, OUTPUT);
  pinMode(Motor3P, OUTPUT);
  
  //TODO: Is input-pullup really the mode that I want here?
  //Motor Encoder Setup
  pinMode(m1encA, INPUT_PULLUP);
  pinMode(m1encB,INPUT_PULLUP);
  pinMode(m2encA, INPUT_PULLUP);
  pinMode(m2encB,INPUT_PULLUP);
  
  //Sonar setup
  pinMode(sonarEcho0,INPUT); //22
  pinMode(sonarTrig0, OUTPUT); //23
  
  pinMode(sonarEcho1,INPUT); //24
  pinMode(sonarTrig1, OUTPUT); //25
  
  pinMode(sonarEcho2,INPUT); //26
  pinMode(sonarTrig2, OUTPUT); //27
  
  pinMode(sonarEcho3,INPUT); //26
  pinMode(sonarTrig3, OUTPUT); //27
  
  pinMode(errorPin, OUTPUT);
}

void initEncoders()
/*
*/
{
    lastEnc1 = getEnc(1);
    lastEnc2 = getEnc(2);
}

void TSC_Init()
{
  pinMode(colorS0, OUTPUT);
  pinMode(colorS1, OUTPUT);
  pinMode(colorS2, OUTPUT);
  pinMode(colorS3, OUTPUT);
  pinMode(colorOUT, INPUT);

  digitalWrite(colorS0, LOW);  // OUTPUT FREQUENCY SCALING 2%
  digitalWrite(colorS1, HIGH);
}

int getEnc(int motor){
	byte b = B00;
	switch(motor){
		case Motor1:
			if(digitalRead(m1encA)){
				b += B10;
				b += (digitalRead(m1encB))?B00:B01;
			}else{
				b += (digitalRead(m1encB))?B01:B00;
			}
			break;
		
		case Motor2:
			if(digitalRead(m2encA)){
				b += B10;
				b += (digitalRead(m2encB))?B00:B01;
			}else{
				b += (digitalRead(m2encB))?B01:B00;
			}
			break;
	}
	return b;
}

void resetEnc(int i){
	switch(i){
		case Motor0: 
			encoder1 = 0;
			encoder2 = 0;
			break;
		case Motor1: 
			encoder1 = 0;
			break;
		case Motor2: 
			encoder2 = 0;
			break;
		default:
			return;
	}
}

void UpdateEncoder(int motor){
	switch(motor){

		case Motor0:
			cur1 = getEnc(Motor1);
			cur2 = getEnc(Motor2);
			if(cur1 == B00 && lastEnc1 == B11){
				encoder1 += 1;  
			}
			else if(cur1 == B11 && lastEnc1 == B00){
				encoder1 -= 1;  
			}
			else{
				encoder1 += (int)(cur1 - lastEnc1);
			}
			  
			if(cur2 == B00 && lastEnc2 == B11){
				encoder2 += 1;  
			}
			else if(cur2 == B11 && lastEnc2 == B00){
				encoder2 -= 1;  
			}
			else{
				encoder2 += (int)(cur2 - lastEnc2);
			}
			lastEnc1 = cur1;
			lastEnc2 = cur2;
			break;

		case Motor1:
			cur1 = getEnc(Motor1);
			if(cur1 == B00 && lastEnc1 == B11){
				encoder1 += 1;  
			}
			else if(cur1 == B11 && lastEnc1 == B00){
				encoder1 -= 1;  
			}
			else{
				encoder1 += (int)(cur1 - lastEnc1);
			}
			lastEnc1 = cur1;
			break;

		case Motor2:
			cur2 = getEnc(Motor2);
			if(cur2 == B00 && lastEnc2 == B11){
				encoder2 += 1;  
			}
			else if(cur2 == B11 && lastEnc2 == B00){
				encoder2 -= 1;  
			}
			else{
				encoder2 += (int)(cur2 - lastEnc2);
			}
			lastEnc2 = cur2;
			break;

		default:
			DEBUG(DBG_ERROR);
	}
	
}

void updateMotorPower(){
  int a, b;
	switch(motorMode)
	{
		casefollow:
			//some kind of PID here too? set one to target the other. probably with higher P,I,and D values
			DEBUG(DBG_ERROR);//erase this error when you write this portion of code. 
			break;
		casetarget:
			if(abs(m1target-encoder1)>5){
				a = m1pwr+m1rate*sign(m1pwr);
				b = (m1target-encoder1)*4;
				m1pwr = (abs(a) < abs(b))?a:b;
			}else{
				m1pwr = 0;
			}
			
			//Motor 2
			if(abs(m2target-encoder2)>5){
				a = m2pwr+m2rate*sign(m2pwr);
				b = (encoder2-m2target)*4;
				m2pwr = (abs(a) < abs(b))?a:b;
			}else{
				m2pwr = 0;
			}
			break;
		casepower:
		
			if(m1pwr != m1pwrset){
				m1pwrset = m1pwrset;
				if(m1pwrset < -maxMotorPower)m1pwrset = -maxMotorPower;
				else if(m1pwrset > maxMotorPower)m1pwrset = maxMotorPower;
				m1pwr = m1pwrset;
			   
			}
			
			if(m2pwr != m2pwrset){
				m2pwrset = m2pwrset;
				if(m2pwrset < -maxMotorPower)m2pwrset = -maxMotorPower;
				else if(m2pwrset > maxMotorPower)m2pwrset = maxMotorPower;
				m2pwr = m2pwrset;
			   
			   
			}
			break;
		case rate:
			//here, use m1target and m2target for target rate 
			//some sort of PID is going to be needed here.
			break;
		default:
			motorMode = power;
			motor(power, Motor0, 0);
			DEBUG(DBG_ERROR);
			
	}
	
	//changing output power for Motor1
	if(m1pwr < -cutoff){
	  //when power is negative only
	   analogWrite(Motor1P, 0);
	   analogWrite(Motor1N, -m1pwrset);
   }else if (m1pwrset > cutoff){
	   analogWrite(Motor1P, m1pwrset);
	   analogWrite(Motor1N, 0);
   }else{
	 analogWrite(Motor1P, 0);
	 analogWrite(Motor1N, 0);
   }
	
	//changing output power for Motor2
	if(m2pwr < -cutoff){
	  //when power is negative only
	   analogWrite(Motor2P, 0);
	   analogWrite(Motor2N, -m1pwrset);
   }else if (m1pwrset > cutoff){
	   analogWrite(Motor2P, m1pwrset);
	   analogWrite(Motor2N, 0);
   }else{
	 analogWrite(Motor2P, 0);
	 analogWrite(Motor2N, 0);
   }
}

void DEBUG(int dbgcmd){
	
	switch(dbgcmd){
		case DBG_PRT_ENC:
			Serial.print(encoder1);
			Serial.print("  ");
			Serial.println(encoder2);
			break;
		case DBG_TEST:
			//put code here to test as a sample test.
			break;
		case DBG_ERROR:
			ERROR_THROWN = true;
			digitalWrite(errorPin, HIGH);
			//if an error is thrown, this code should be run.
			break;
		case DBG_NO_ERROR:
			ERROR_THROWN = false;
			digitalWrite(errorPin, LOW);
			//to clear the error, if the error light is on.
			break;
			
	}
	
}

void motor(int motor, int mode, int amt)
/*Changes the current mode or values relating to the motors: 
	power mode: set the motors to a constant power output
	follow mode: set one motor to 
	
*/
{
	motorMode = mode; // change the current motor mode to selection.
	switch(mode){
		case follow:
			switch(motor){
				case Motor0:
					//if command for both motors to follow,
					//signal error light and kill motors.
					motor(power, Motor0, 0);
					DEBUG(DBG_ERROR);
					break;
				case Motor1:
					break;
				case Motor2:
					break;
			}
			//for motor following
			
			break;
		case target:
			switch(motor){
				case Motor0:
					m1target = amt + m1Offset;
					m2target = amt + m2Offset;
					break;
				case Motor1:
					m1target = amt + m1Offset;
					break;
				case Motor2:
					m1target = amt + m2Offset;
					break;
				default:
				motor(power, Motor0, 0);
				DEBUG(DBG_ERROR);
			}
			break;
		case Power:
			
			switch(motor):
				case Motor0:
					m1pwrset = amt + m1Offset;
					m2pwrset = amt + m2Offset;
					break;
				case Motor1:
					m1pwrset = amt + m1Offset;
					break;
				case Motor2:
					m1pwrset = amt + m2Offset;
					break;
				default:
				motor(power, Motor0, 0);
				DEBUG(DBG_ERROR);
			break;
		case rate:
			break;
		default:
			//motor(power, Motor0, 0);
			DEBUG(DBG_ERROR);
	}
}

void claw(int mode)
/*
	Operates the claw: use CLAW_OPEN as input to open and CLAW_CLOSE as input to close.
	TODO: Check to see if claw is closed already.
*/
{
	switch(mode){
		case CLAW_OPEN:
			//claw opening sequence
			analogWrite(Motor3P, clawPower);
			analogWrite(Motor3N, 0);
			delay(clawCloseDelay);
			analogWrite(Motor3P, 0);
			analogWrite(Motor3N, 0);
			break;
		case CLAW_CLOSE:
			//claw closing sequence
			analogWrite(Motor3P, 0);
			analogWrite(Motor3N, clawPower);
			delay(clawCloseDelay);
			analogWrite(Motor3P, 0);
			analogWrite(Motor3N, 0);
			break;
		default:
			//if any other input to claw function, notify with signal light
			DEBUG(DBG_ERROR);
	}
}

short sign(int i){
	return(i<0)?-1:1;
}

short ping(byte sensor){
  long duration, inches;
  
if ((sensor >= B0) && (sensor < sensorCount))
  {
    digitalWrite(triggerPins[sensor], LOW);
    delayMicroseconds(2);
    digitalWrite(triggerPins[sensor], HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPins[sensor], LOW);
  
  //  duration = pulseIn((sonarEcho + sensor), HIGH);
    
    
    return (short)pulseIn(echoPins[sensor], HIGH, 32000);//Chris Chavez Was Here
  }
else
	DEBUG(DBG_ERROR);
    return -1;
}
