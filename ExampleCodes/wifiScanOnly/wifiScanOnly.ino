/** WifiScanOnly  
Author: Mrs Christin Koss

please also check the wiki:

purpose of this sketch:
- scan in a loop 14 WifiChannels
- a basic introduction into wifiScan´s
*/

//4 the WiFiScanner:
#include <user_interface.h>		// used for e.g.  system_get_free_heap_size

#define DISABLE 0
#define ENABLE 1

// ERROR - COUNTER

#define _ESP_ERR_COUNT_LIMIT 50		// Auslöser für den Reset wenn dieser Schwellwert erreicht ist
uint16 _ESP_ERROR_COUNTER = 0;

//>>>>>>> Configure the scanner Stuff  ++  Configure the scanner Stuff  ++  Configure the scanner Stuff  <<<<<<<<<

// TimeFrames Tick Tack
unsigned long CHANNEL_HOP_INTERVAL_MS = 1000;
unsigned long CHANNEL_HOP_previousMillis = 0;

/*
Outer-Loop
10 min = 600000
20 min = 120000
*/
unsigned long OUTER_LOOP_LONG_TIMER = 60000;
unsigned long refreshTheLoopTimer_passedMillies = 0;

// you may define a buffer or a Struct Array to keep the  MAC devices - 

bool dataPackReady = false;


// 4 the scanner --the data-structure

// The setup function is called once at startup of the sketch
#define DATA_LENGTH 112
#define TYPE_MANAGEMENT 0x00
#define TYPE_CONTROL 0x01
#define TYPE_DATA 0x02
#define SUBTYPE_PROBE_REQUEST 0x04

struct RxControl
{
	signed rssi : 8; // signal intensity of packet
	unsigned rate : 4;
	unsigned is_group : 1;
	unsigned : 1;
	unsigned sig_mode : 2;       // 0:is 11n packet; 1:is not 11n packet;
	unsigned legacy_length : 12; // if not 11n packet, shows length of packet.
	unsigned damatch0 : 1;
	unsigned damatch1 : 1;
	unsigned bssidmatch0 : 1;
	unsigned bssidmatch1 : 1;
	unsigned MCS : 7; // if is 11n packet, shows the modulation and code used (range from 0 to 76)
	unsigned CWB : 1; // if is 11n packet, shows if is HT40 packet or not
	unsigned HT_length : 16; // if is 11n packet, shows length of packet.
	unsigned Smoothing : 1;
	unsigned Not_Sounding : 1;
	unsigned : 1;
	unsigned Aggregation : 1;
	unsigned STBC : 2;
	unsigned FEC_CODING : 1; // if is 11n packet, shows if is LDPC packet or not.
	unsigned SGI : 1;
	unsigned rxend_state : 8;
	unsigned ampdu_cnt : 8;
	unsigned channel : 4; // which channel this packet in.
	unsigned : 12;
};

struct scannerPacket
{
	struct RxControl rx_ctrl;
	uint8_t data[DATA_LENGTH];
	uint16_t cnt;
	uint16_t len;
};

// **********  CODE Section *********************************************************************


// device MAC without :
static void getMAC(char* addr, uint8_t* data, uint16_t offset)
{
	sprintf(addr, "%02x%02x%02x%02x%02x%02x", data[offset + 0], data[offset + 1], data[offset + 2], data[offset + 3], data[offset + 4], data[offset + 5]);
}


static void showMetadata(struct scannerPacket* scannerPacket)
{
	unsigned int frameControl = ((unsigned int)scannerPacket->data[1] << 8) + scannerPacket->data[0];
	// uint8_t version      = (frameControl & 0b0000000000000011) >> 0;
	uint8_t frameType = (frameControl & 0b0000000000001100) >> 2;
	uint8_t frameSubType = (frameControl & 0b0000000011110000) >> 4;
	// uint8_t toDS         = (frameControl & 0b0000000100000000) >> 8;
	// uint8_t fromDS       = (frameControl & 0b0000001000000000) >> 9;

	// Only look for probe request packets
	if (frameType != TYPE_MANAGEMENT || frameSubType != SUBTYPE_PROBE_REQUEST)
	{
		//Serial.println("showMetadata() - bedingung nicht erfüllt: frameType != TYPE_MANAGEMENT || frameSubType != SUBTYPE_PROBE_REQUEST --- RETURN ");
		return;
	}

	//RAMfree("showMetadata()");

	Serial.print(" RSSI: "); Serial.print(scannerPacket->rx_ctrl.rssi, DEC);
	Serial.print(" Ch: ");  Serial.print(wifi_get_channel());

	char deviceMacAddr[] = "000000000000";
	getMAC(deviceMacAddr, scannerPacket->data, 10);
	Serial.print(" MAC: "); Serial.println(deviceMacAddr);
	
	// here would be the place for some HASH 
	// here would be the place to store your MAC or RSSI  into a buffer
	
}


static void ICACHE_FLASH_ATTR scanner_callback(uint8_t* buffer, uint16_t length)
{
	struct scannerPacket* scannerPacket = (struct scannerPacket*)buffer;
	showMetadata(scannerPacket);
}

void sendDataPack(){
	
	Serial.print("This would be the place to send your data to the cloud "); 	
}


void setup() {

	Serial.begin(115200);
	Serial.println("\nHI..THIS is SCANNER-Node! start SETUP()");			
		Serial.printf("ESP8266 OWN MAC getChipId(): ESP_%08X\n", ESP.getChipId());  // https://github.com/esp8266/Arduino/issues/2309
		delay(2000);
	//----------------  The scanner SETUP SECTOR -------------------------------------------------------------------------

	Serial.println("setup(): start & set Station_Mode & wifi_promiscuous...");

	wifi_set_opmode(STATION_MODE);
	wifi_set_channel(1);
	wifi_promiscuous_enable(DISABLE);
	delay(10);
	wifi_set_promiscuous_rx_cb(scanner_callback);
	delay(10);
	wifi_promiscuous_enable(ENABLE);
	Serial.println("setup(): start & set Station_Mode wifi_promiscuous_enable(ENABLE)... - DONE!");
	delay(10);	
}



int i = 0;
uint8  curr_channel = 1;

void loop() {

	unsigned long currentMillis = millis(); // grab current time  -  check if "interval" time has passed (eg. some sec to do something )

	// CORE-LOOP The new Channel Hop Timer  --  hoping channels 1-14

	if ((unsigned long)(currentMillis - CHANNEL_HOP_previousMillis) >= CHANNEL_HOP_INTERVAL_MS) {
		curr_channel = curr_channel + 1;
		
		if (curr_channel >= 14)
		{
			wifi_promiscuous_enable(DISABLE);		// SNIFF PAUSE
			curr_channel = 1;
			dataPackReady = true;	//26.12.17 / sendData HIER stürtzte ab!  aber unten im loop ok	- Serial.print("set back to 1..channelHopCallBack(): "); Serial.print(curr_channel);	
		}
		if (curr_channel > 0 && curr_channel <= 14)   wifi_set_channel(curr_channel);
		delay(10);
		Serial.print("["); Serial.print(wifi_get_channel()); Serial.print("]");
		
		CHANNEL_HOP_previousMillis = millis();// save the "current" time
	}	

	if (dataPackReady == true) {
		dataPackReady = false;
		//can do something..PromMode is disabled ...so you can communicate to outside!
		wifi_set_channel(curr_channel);		// wichtig sonst durchläuft der nicht mehr alle channel, da die connection sich irgeinen nimmt
	}

	// If Buffer Full or refreshTheLoopTime is Finished  we send the Data and empty the buffers for a fresh loop

	bool timeToSendData = false;
	currentMillis = millis(); 
	if ((unsigned long)(currentMillis - refreshTheLoopTimer_passedMillies) >= OUTER_LOOP_LONG_TIMER) {
		refreshTheLoopTimer_passedMillies = millis();		
		timeToSendData = true;
	}
	else {
		timeToSendData = false;
	}

	//TIME over oder Buffer VOLL - WIR SENDEN zur CLOUD ( OPTION for later USE!! )
	if (timeToSendData == true )
	{		
		sendDataPack();				
		timeToSendData = false;
	}		

	wifi_promiscuous_enable(ENABLE);
}





