
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
    
#define	clawOpenPower 200
#define	clawPower 150

#define	clawOpenDelay 300 
#define	clawCloseDelay 150 
    
#define	cutoff 15
#define	amt 1   

#define	m1rate 5 
#define	m2rate 5
#define	rotation 425
#define	maxMotorPower 100
#define	diam 8.5f
#define	wdiam 4.75f


//comm commands
#define DBG_COMMAND 	B00000000
#define MOTOR_COMMAND	B00010000
#define CLAW_COMMAND 	B00100000
#define ENCODER_COMMAND	B00110000
#define SONAR_COMMAND	B01000000
//the base commands will have form XXXX0000
#define COMMAND_PREFIX_OVERLAY	B11110000
//comm-specific commands will have the form 1011XXXX
#define COMMAND_BEGIN	B11011111
#define COMMAND_END		B11010000
	
//debug commands
#define DBG_TEST 		B00000001
#define DBG_ERROR 		B00000010
	//base debug command is 0000XXXX
//motor commands
#define MOTOR_FOLLOW	B00010000
#define MOTOR_TARGET 	B00010100 
#define MOTOR_POWER 	B00011000
#define MOTOR_RATE 		B00011100
	//base motor command is 0001XXXX
#define motor0 B00
#define motor1 B01
#define motor2 B10
	//motor commands will work as such: 0001CCMM
	//where the C digits are the command type, selecting between target, power, and rate modes
	//and the M digits will be the motor selection, allowing for up to 4 commands and 4 motors

//claw commands
#define CLAW_OPEN B00100001
#define CLAW_CLOSE B00100010
	//base claw command is 0010XXXX
	
//encoder commands
#define ENC_RESET		B00110001
#define ENC_GET			B00110010
//base encoder command is 0011XXXX
//sonar commands
#define SONAR_GET		B01000100
#define SONAR_0			B00
#define SONAR_1			B01
#define SONAR_2			B10
#define SONAR_3			B11
	//base sonar command is 0100XXXX
	


//variables
int m1pwr = 0,
	m2pwr = 0,
	m1pwrset = 0,
	m2pwrset = 0,
	encoder1 = 0,
	encoder2 = 0,
	m1target = 0, 
	m2target = 0;
	
byte 
	lastEnc1,
	lastEnc2,
	cur1, 
	cur2, 
	motorMode = MOTOR_POWER, 
	masterMotor = motor0;
    
bool newCMD = false, ERROR_THROWN = true;
//String cmd = "";
      
int triggerPins[] = {sonarTrig0,sonarTrig1,sonarTrig2,sonarTrig3};
int echoPins[] = {sonarEcho0,sonarEcho1,sonarEcho2,sonarEcho3};
byte sensorCount = B100;

void setup(){
	//setup the IO pins
	pinSetup();
	//open the serial line at 115200 baud.
	Serial1.begin(115200);
	
	//initialize current encoder values
	initEncoders();
	
	//TODO: Interrupt command attachments
	
}

void loop(){
	//put code here to be run continuously
	while(Serial1.available()>0){
		byte in = Serial1.read();
		
		if(in == COMMAND_BEGIN)
			processCMD();
		else{
			DEBUG(DBG_ERROR);
		}
		UpdateMotorPower()
		UpdateEncoder(0);
	}
	//update encoders as often as is possible
	updateMotorPower();
	UpdateEncoder(0);
}

void processCMD()
/*David: Done	
*/
{
	byte in;
	in = Serial1.read();
	switch(in & COMMAND_PREFIX_OVERLAY){
		case DBG_COMMAND:
			DEBUG(in);
			break;
		case MOTOR_COMMAND:
			if(Serial1.available()>1){
				int val = Serial1.read();
				motor(in&B00001100, in&B00000011, val);
			}
			else 
				DEBUG(DBG_ERROR);
			break;
		case CLAW_COMMAND:
			claw(in);
			break;
		case PING_COMMAND:
			//this will return the time that it took for the signal to return to 
			short dur = ping(in & B00000011);
			Serial1.print(dur);
			break;
		case COMMAND_END:
			Serial1.print(COMMAND_END);
		
		default:
			DEBUG(DBG_ERROR);
	}
	
}

void pinSetup()
/* David: Done	
*/
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

int getEnc(int motor){
	byte b = B00;
	switch(motor){
		case motor1:
			if(digitalRead(m1encA)){
				b += B10;
				b += (digitalRead(m1encB))?B00:B01;
			}else{
				b += (digitalRead(m1encB))?B01:B00;
			}
			break;
		
		case motor2:
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
		case motor0: 
			encoder1 = 0;
			encoder2 = 0;
			break;
		case motor1: 
			encoder1 = 0;
			break;
		case motor2: 
			encoder2 = 0;
			break;
		default:
			return;
	}
}

void UpdateEncoder(int motor){
	switch(motor){

		case motor0:
			cur1 = getEnc(motor1);
			cur2 = getEnc(motor2);
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

		case motor1:
			cur1 = getEnc(motor1);
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

		case motor2:
			cur2 = getEnc(motor2);
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
	switch(motorMode)
	{
		case MOTOR_FOLLOW:
			//some kind of PID here too? set one to target the other. probably with higher P,I,and D values
			DEBUG(DEBUG_ERROR);//erase this error when you write this portion of code. 
			break;
		case MOTOR_TARGET:
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
		case MOTOR_POWER:
		
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
		case MOTOR_RATE:
			//here, use m1target and m2target for target rate 
			//some sort of PID is going to be needed here.
			break;
		default:
			motorMode = MOTER_POWER;
			motor(MOTOR_POWER, motor0, 0);
			DEBUG_ERROR();
			
	}
	
	//changing output power for motor1
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
	
	//changing output power for motor2
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

void DEBUG(byte dbgcmd){
	
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

void motor(byte mode, byte motor, int amt)
/*Changes the current mode or values relating to the motors: 
	power mode: set the motors to a constant power output
	follow mode: set one motor to 
	
*/
{
	motorMode = mode; // change the current motor mode to selection.
	switch{mode}{
		case MOTOR_FOLLOW:
			switch(motor){
				case motor0:
					//if command for both motors to follow,
					//signal error light and kill motors.
					motor(MOTOR_POWER, motor0, 0);
					DEBUG(DEBUG_ERROR);
					break;
				case motor1:
					break;
				case motor2:
					break;
			}
			//for motor following
			
			break;
		case MOTOR_TARGET:
			switch(motor){
				case motor0:
					m1target = amt + m1Offset;
					m2target = amt + m2Offset;
					break;
				case motor1:
					m1target = amt + m1Offset;
					break;
				case motor2:
					m1target = amt + m2Offset;
					break;
				default:
				motor(MOTOR_POWER, motor0, 0);
				DEBUG(DEBUG_ERROR);
			}
			break;
		case MOTOR_POWER:
			
			switch(motor):
				case motor0:
					m1pwrset = amt + m1Offset;
					m2pwrset = amt + m2Offset;
					break;
				case motor1:
					m1pwrset = amt + m1Offset;
					break;
				case motor2:
					m1pwrset = amt + m2Offset;
					break;
				default:
				motor(MOTOR_POWER, motor0, 0);
				DEBUG(DEBUG_ERROR);
			break;
		case MOTOR_RATE:
			break;
		default:
			motor(MOTOR_POWER, motor0, 0);
			DEBUG(DBG_ERROR);
	}
}

void claw(byte mode)
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
