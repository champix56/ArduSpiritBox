/**
 * Config file
 */

#ifndef _CONFIG_H
#define _CONFIG_H
#include "includer.h"
 /**
  * visual Output messages
  */
#define SERIAL_OUTPUT
//#define LCD_20_04

/**
* frequecies save
**/
#define FREQ_SAVE_SDCARD //selector & omitor for scanned frequecies by csv SDCARD file
#define FREQ_SAVE_EEPROM //selector & omitor for scanned frequecies by EEPROM save

/**
* frequencies selectors
**/
#define SELECTOR_ROTARY //selector for frequecy positionning by rotary encoder
#define SELECTOR_BUTTON //selector for frequecy positionning by push buttons
#define SELECTOR_UART //selector for frequecy positionning by UART
/**
* recorder
*/
#define RECORDER_SD //recod wav file fetures
#define RECORDER_MULTI_OUPUT //activate multi audio outputfor external recording


#endif
