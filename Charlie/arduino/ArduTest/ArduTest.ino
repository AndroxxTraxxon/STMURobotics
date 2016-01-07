

byte in;

void setup(){
	Serial.begin(9600);
	Serial.write("Serial Initiated");
}

void loop(){
	
}

void serialEvent(){
	in = Serial.read();
	Serial.write(in);	
	Serial.write(":");
	Serial.println((int)in);
	
}