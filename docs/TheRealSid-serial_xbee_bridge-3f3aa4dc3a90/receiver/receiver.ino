#include <Servo.h>

#define BYTE 	uint8_t
#define BOOL 	uint8_t
#define SHORT 	uint16_t
#define TRUE 	42
#define FALSE 	0

/*

|START(0x55)|START(0x55)|START(0x55)|HIGH|LOW|CHECK¦END(0x66)|END(0x66)|END(0x66)|

*/

#define PACK_START_LEN		3
#define PACK_PAY_LEN		2
#define PACK_END_LEN		3
#define BUFF_LEN			6
#define SYNC_BYTE			0x55
#define END_BYTE			0x66

#define CMD_SET_SERVO		0x01
#define SERVO_LED_PIN		12
#define SERVO_PIN			9
#define SERVO_MIN_ANGLE		0
#define SERVO_MAX_ANGLE		180

void receiveData();
void sync();
BYTE nextByte();
BOOL checkPacket();
void handleData();
void setServo(BYTE value);

BYTE buff[BUFF_LEN];
BYTE data[PACK_PAY_LEN];

//Servo servo;


void setup() {
	Serial.begin(9600);
	//servo.attach(SERVO_PIN);
	pinMode(SERVO_LED_PIN, OUTPUT);
}

void loop() { 
	receiveData();
	if(checkPacket()) {
		for(SHORT i = 0; i < PACK_PAY_LEN; i++) {
			data[i] = buff[i];
		}
		handleData();
	}
}

void handleData() {
	switch (data[0]) {
		case CMD_SET_SERVO:
			setServo(data[1]);
			break;
	}
}

void setServo(BYTE value) {
	if(value >= SERVO_MIN_ANGLE && value <= SERVO_MAX_ANGLE) {
		//servo.write(value);
		digitalWrite(SERVO_LED_PIN, HIGH);
		delay(500);
		digitalWrite(SERVO_LED_PIN, LOW);
	}
}

void receiveData() {
	sync();
	for (SHORT i = 0; i < BUFF_LEN; i++) {
		buff[i] = nextByte();
	}
}

void sync() {
	while(TRUE) {
		for(SHORT i = 0; i < PACK_START_LEN; i++) {
			if (nextByte() != SYNC_BYTE) {
				break;
			}
			if (i == PACK_START_LEN-1) {
				return;
			}
		}
	}
}

BYTE nextByte() {
	while(TRUE) {
		if (Serial.available()) {
			return Serial.read();
		}
	}
}

BOOL checkPacket() {
	for(SHORT i = 0; i < PACK_END_LEN; i++) {
		if(buff[BUFF_LEN - 1 - i] != END_BYTE) {
			return FALSE;
		}
	}
	BYTE checksum = 0x00;
	for(SHORT i = 0; i < PACK_PAY_LEN; i++) {
		checksum ^= buff[i];
	}
	if(buff[PACK_PAY_LEN] != checksum) {
		return FALSE;
	}
	return TRUE;
}