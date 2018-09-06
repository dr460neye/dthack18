/** WifiScan + OLED +  cloud
Author: Mrs Christin Koss

please also check the wiki:

Challenge:

- setup and cofigure the LIBs in Arduino Properly
- check the serial Monitor
- build / deploy the application to the NodeMcu
- Understand the basic Concept - What  the methods do?
- make some basic tests concerning OLED Display

*/


//All u NEED for the OLED DISPLAY I2C 
#include <Wire.h>		// 
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>	// Original .h  zZ ein 64 Bit Display  - https://github.com/adafruit/Adafruit_SSD1306/issues/57
#include <user_interface.h>


int oledRowCount = 0;
#define I2C_ADDRESS 0x3C		// Define proper RST_PIN if required.
#define RST_PIN -1
SSD1306AsciiWire oled;


void setup() {

	Serial.begin(115200);		// OLED
	Wire.begin(D1, D4);			// sda, scl ok: (D1,D2)
	Wire.setClock(400000L);
	oled.begin(&Adafruit128x64, I2C_ADDRESS);
	oled.setFont(Adafruit5x7);
	oled.clear();
	oled.set1X();
	oledRowCount = 0;
	oled.clear();
	
}

void CheckOledMessage() {
	//Display  ** Display  ** Dosplay ************************
	if (oledRowCount >= 8) {	//bei 128 x 32  sind das 4   or bei 128 x 64  sind das 8
		oledRowCount = 0;
		oled.clear();		//oled.set1X();  //  alles ist auf 1 ..	
		oled.print("dthack18 ");  oled.print(0, 2); oled.print(" H"); oled.println(system_get_free_heap_size());	
	}
	++oledRowCount;	// jedes CRLF ist eine Zeile und zählt als rowCount	
}

void loop() {

	oled.print("dthack");  oled.print(18);	 oled.print(" * ");  oled.print(random(1,100), DEC); oled.print(" H"); oled.println(system_get_free_heap_size()); 
	CheckOledMessage();
	delay(4000);	
}



