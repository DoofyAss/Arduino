#include <OneWire.h>	// v2.3.5

int led = 13;			// PIN
int one = 2;			// PIN
OneWire b (one);

byte tmp[8];			// temp key
byte key[8];			// read key



void setup() {
	Serial.begin(9600);
}



void loop() {
	
	Console();
	
	if (!b.search (key)) {
		
		b.reset_search();
		delay(200);
		return;
	}
	
	if (tmp[0] == 0) {
		
		for (int i=0; i<8; i++) { tmp[i] = key[i]; }
		digitalWrite(led, HIGH);
		debug(key, "  key");
		Serial.print(" set to tmp");
	}
	
	if (!compare(tmp, key)) {
		
		debug(key, "  key");
		Serial.print(" recording ");
		rewrite(tmp);
		Serial.print(" done");
	} else {
		debug(tmp, "  tmp");
	}
}



void debug(byte *arr, String type) {
	
	String message = "\n" + type + ": {";
	
	for (int i=0; i<8; i++) {
		
		String hex = String(arr[i], HEX);
		hex.toUpperCase();
		
		message = message
		+ (arr[i] < 16 ? "0x0" : "0x") + hex
		+ (i < 7 ? ", " : "}");
	}
	
	Serial.print(message);
}



boolean compare(byte *arr1, byte *arr2) {
	
	for (int i=0; i<8; i++) if (arr1[i] != arr2[i]) return false;
	return true;
}










int rewrite(byte *arr) {
	
	b.skip();
	b.reset();
	b.write(0xD1);
	sendLogical(0);
	
	b.skip();
	b.reset();
	b.write(0xD5);
	
	for (int i=0; i<8; i++) {
		rewriteByte(arr[i]);
		digitalWrite(led, LOW);
		delay(100);
		Serial.print('*');
		digitalWrite(led, HIGH);
	}
	
	b.reset();
	b.write(0xD1);
	sendLogical(1);
	
	return 0;
}



int rewriteByte(byte Byte) {
	
	for (int j=0; j<8; j++) {
		
		if (Byte & 1) {
			sendLogical(0);
		} else {
			digitalWrite(one, LOW);
			pinMode(one, OUTPUT);
			pinMode(one, INPUT);
			digitalWrite(one, HIGH);
		}
		
		delay(10);
		Byte = Byte >> 1;
	}
	
	return 0;
}



int sendLogical(int value) {
	
	digitalWrite(one, LOW);
	pinMode(one, OUTPUT);
	delayMicroseconds(value == 1 ? 10 : 60);
	pinMode(one, INPUT);
	digitalWrite(one, HIGH);
	delay(10);
	
	return 0;
}










void Console() {
	
	if (!Serial.available()) return;
	String cmd = Serial.readString();
	
	cmd.replace("\n", "");
	cmd.replace("\r", "");
	
	
	
	if (cmd == "help") {
		
		Serial.println();
		Serial.println("\ncmd list:");
		Serial.println("  clean: set tmp to {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}");
		Serial.println("  write: set tmp to {0x01, 0xF6, 0xB2, 0xA3, 0x01, 0x00, 0x00, 0xBB}");
		Serial.println("  rewrite: force to {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}");
	}
	
	
	
	if (cmd == "clean") {
		
		for (int i=0; i<8; i++) { tmp[i] = i == 0 ? 1 : 0; }
		debug(tmp, "  tmp");
		Serial.print(" changed");
	}
	
	
	
	if (cmd == "write") {
		
		byte arr[8] = {0x01, 0xF6, 0xB2, 0xA3, 0x01, 0x00, 0x00, 0xBB};
		for (int i=0; i<8; i++) { tmp[i] = arr[i]; }
		debug(tmp, "  tmp");
		Serial.print(" changed");
	}
	
	
	
	if (cmd == "rewrite") {
		
		tmp[0] = 0;
		byte arr[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
		rewrite(arr);
	}
}