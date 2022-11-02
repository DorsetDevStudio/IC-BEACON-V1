/**
 * @file main.cpp
 * @author G5STU - https://station-master.online
 * @brief  Simple icom ham radio frequency sniffer
 * @version 0.1
 * @date 2022-11-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <Arduino.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>

using namespace std;

#define CIV_PIN_RX 37
#define DEBUG_SERIAL Serial

HardwareSerial CIV_SERIAL(1);

struct s_data {
	uint32_t device_id			=	ESP.getEfuseMac() & 0xFFFFFFFF;
	String previous_mode 		=	"0";	// last mode reported to cloud
	String mode					=	"0";	// will be reported raw i.e 0203 == CW
	String current_frequency 	=	"0";	// will be reported in Hz
	String previous_frequency 	=	"0";		// last frequency report to cloud	
	} s_data;

auto *ps_data = &s_data;

//uint32_t device_id = ESP.getEfuseMac() & 0xFFFFFFFF; 	// get 32 bit device id

/**
 * @brief call `function` every `interval` miliseconds
 * 
 * @param function 
 * @param interval 
 */
void run_periodically(function<void(void)> function, int interval)
{
    thread([function, interval]() {
        while (true){
			delay(1); // keep watchdog happy
            function();
            auto ms = chrono::steady_clock::now() + chrono::milliseconds(interval);
            this_thread::sleep_until(ms);
        }
    }).detach();
}
 
 /**
  * @brief TODO: add code to ping the device_id, current_frequency, and mode to the cloud
  * 
  */
void reportToCloud() 
{
	if (strcmp((*ps_data).previous_frequency.c_str(), (*ps_data).current_frequency.c_str()) != 0
	|| strcmp((*ps_data).previous_mode.c_str(), (*ps_data).mode.c_str()) != 0){
		(*ps_data).previous_frequency = (*ps_data).current_frequency;
		(*ps_data).previous_mode = (*ps_data).mode;
		DEBUG_SERIAL.println((String) "Device ID " + (*ps_data).device_id + " Reported to cloud Frequency = " + (*ps_data).current_frequency + " Hz , MODE = " + (*ps_data).mode );
	}	
}

/**
 * @brief return a formatted string
 */
char *storePrintf(const char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    size_t sz = snprintf(NULL, 0, fmt, arg);
    char *buf = (char *)malloc(sz + 1);
    vsprintf(buf, fmt, arg);
    va_end (arg);
    return buf;
}

/**
 * @brief monitor the serial port for C-IV messages from the radio
 * 
 */
void monitor_radio_bg_task()
{
	thread([]() {
        while (true){
			delay(1); // keep watchdog happy
			if(CIV_SERIAL.available())
			{
				auto available_bytes = CIV_SERIAL.available();	
				uint8_t buff[available_bytes] = {0}; 
				CIV_SERIAL.read(buff, available_bytes);						// read incomming message

				if(available_bytes == 11) {									// frequency change			
					(*ps_data).current_frequency = storePrintf("%02x%02x%02x%02x%02x", 
						buff[9], buff[8], buff[7], buff[6], buff[5]);  		// parse the current frequency from the C-IV message
					}
				else if(available_bytes == 8) 								// report mode as raw values so we can support any modes from the server side
					(*ps_data).mode = storePrintf("%02x%02x", buff[6], buff[5]); 		// parse the current mode from the C-IV message

				else														// other message we don't care about
					DEBUG_SERIAL.printf("Unknown command length = %i\n",available_bytes);	
			}		   
        }
    }).detach();
}

/**
 * @brief on load one-time events
 * 
 */
void setup() 
{
	DEBUG_SERIAL.begin(115200);								// start debugging via usb
	CIV_SERIAL.begin(9600, SERIAL_8N1, CIV_PIN_RX, false);	// connect to serial port, 8 bits, no parity , 1 stop bit
	run_periodically(reportToCloud, 3000);					// background timer pings server if freq / mode changed 
	monitor_radio_bg_task();						    	// background thread to constantly monitor radio C-IV port for messsages
}

/**
 * @brief main process loop
 * 
 */
void loop() {
	digitalWrite(LED_BUILTIN, HIGH);
	delay(2000);
	digitalWrite(LED_BUILTIN, LOW);
	delay(2000);
}