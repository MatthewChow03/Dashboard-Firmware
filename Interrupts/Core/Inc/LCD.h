#include "stm32f1xx_hal.h"

/******************************
 ** DECLARES BUS CONNECTIONS **
 ******************************/

/* MACROS ---------------------------------------------*/

// OLD LCD VARS
#define BYTEPERBITMAP 16
#define LOOKUP_COLUMN 4
#define LOOKUP_ROW 41
#define OFFSET_NUMERIC 22
#define OFFSET_CAPITAL 65
#define OFFSET_LOWCASE 97

// MESSAGE ID
#define BATT_FAULTS 0x622
#define BATT_VOLTAGE 0x623
#define BATT_PACK_HEALTH 0x626
#define BATT_TEMP 0x627

#define MDU_STATUS_INFORMATION 0x501
#define MDU_VELOCITY 0x503
#define MDU_MOTOR_TEMP 0x50B

// DISPLAY TEMPLATE
#define ROW0 0
#define ROW1 5
#define ROW2 10
#define ROW3 15
#define ROW4 20
#define ROW5 25

/**
 * PAGE TEMPLATE
 *
 * Values are multiplied by 2 because bitmaps are 16 bits wide instead of 8, hence 16 = (2 * uint_8)
 *
 * VAR_LEN: max length of field + 1 for colon
 * DATA_LEN: max length of the data being displayed (include decimal place and negative sign for vectors, do not include units)
 * XPOS: starting x position for name of data (e.g., 'SPEED: ')
 * DATA_XPOS: starting x position for the value (e.g., "99.9")
 * UNIT_XPOS: starting x position for units (e.g., 'V')
 * YPOS: starting y position for the row (no UNIT_YPOS because it is at the same y position)
 *
 * Default for copying:
 *   #define ?_VAR_LEN (? + 1) * 2
 *   #define ?_DATA_LEN ? * 2
 *   #define ?_XPOS ?
 *   #define ?_DATA_XPOS ?_XPOS + ?_VAR_LEN
 *   #define ?_UNIT_XPOS ?_XPOS + ?_VAR_LEN + ?_DATA_LEN
 *   #define ?_YPOS ROW?
 */

// STATIC
#define PAGE_NUM_MAX 2
#define PAGE_NUM_VAR_LEN 0
#define PAGE_NUM_DATA_LEN 1 * 2
#define PAGE_NUM_XPOS 34
#define PAGE_NUM_DATA_XPOS PAGE_NUM_XPOS + PAGE_NUM_VAR_LEN
#define PAGE_NUM_UNIT_XPOS PAGE_NUM_XPOS + PAGE_NUM_VAR_LEN + PAGE_NUM_DATA_LEN
#define PAGE_NUM_YPOS ROW5

// PAGE 1
#define SPEED_ID 0x503
#define SPEED_VAR_LEN (5 + 1) * 2
#define SPEED_DATA_LEN 3 * 2
#define SPEED_XPOS 0
#define SPEED_DATA_XPOS SPEED_XPOS + SPEED_VAR_LEN
#define SPEED_UNIT_XPOS SPEED_XPOS + SPEED_VAR_LEN + SPEED_DATA_LEN
#define SPEED_YPOS ROW0

#define CRUISE_SPEED_ID 0x400
#define CRUISE_SPEED_VAR_LEN (12 + 1) * 2
#define CRUISE_SPEED_DATA_LEN 3 * 2
#define CRUISE_SPEED_XPOS 0
#define CRUISE_SPEED_DATA_XPOS CRUISE_SPEED_XPOS + CRUISE_SPEED_VAR_LEN
#define CRUISE_SPEED_UNIT_XPOS CRUISE_SPEED_XPOS + CRUISE_SPEED_VAR_LEN + CRUISE_SPEED_DATA_LEN
#define CRUISE_SPEED_YPOS ROW1

#define BATT_VOLT_ID 0x623
#define BATT_VOLT_VAR_LEN (9 + 1) * 2
#define BATT_VOLT_DATA_LEN 3 * 2
#define BATT_VOLT_XPOS 0
#define BATT_VOLT_DATA_XPOS BATT_VOLT_XPOS + BATT_VOLT_VAR_LEN
#define BATT_VOLT_UNIT_XPOS BATT_VOLT_XPOS + BATT_VOLT_VAR_LEN + BATT_VOLT_DATA_LEN
#define BATT_VOLT_YPOS ROW2

#define BATT_CURR_ID 0x624
#define BATT_CURR_VAR_LEN (9 + 1) * 2
#define BATT_CURR_DATA_LEN 3 * 2
#define BATT_CURR_XPOS 0
#define BATT_CURR_DATA_XPOS BATT_CURR_XPOS + BATT_CURR_VAR_LEN
#define BATT_CURR_UNIT_XPOS BATT_CURR_XPOS + BATT_CURR_VAR_LEN + BATT_CURR_DATA_LEN
#define BATT_CURR_YPOS ROW3

#define BATT_TEMP_ID 0x627
#define BATT_TEMP_VAR_LEN (9 + 1) * 2
#define BATT_TEMP_DATA_LEN 3 * 2
#define BATT_TEMP_XPOS 0
#define BATT_TEMP_DATA_XPOS BATT_TEMP_XPOS + BATT_TEMP_VAR_LEN
#define BATT_TEMP_UNIT_XPOS BATT_TEMP_XPOS + BATT_TEMP_VAR_LEN + BATT_TEMP_DATA_LEN
#define BATT_TEMP_YPOS ROW4

#define MOTOR_TEMP_ID 0x50B
#define MOTOR_TEMP_VAR_LEN (10 + 1) * 2
#define MOTOR_TEMP_DATA_LEN 3 * 2
#define MOTOR_TEMP_XPOS 0
#define MOTOR_TEMP_DATA_XPOS MOTOR_TEMP_XPOS + MOTOR_TEMP_VAR_LEN
#define MOTOR_TEMP_UNIT_XPOS MOTOR_TEMP_XPOS + MOTOR_TEMP_VAR_LEN + MOTOR_TEMP_DATA_LEN
#define MOTOR_TEMP_YPOS ROW5

// PAGE 2
#define _VAR_LEN
#define _DATA_LEN
#define _XPOS 0
#define _UNIT_XPOS (_VAR_LEN + _DATA_LEN + 2) * 2
#define _YPOS ROW0

#define _VAR_LEN
#define _DATA_LEN
#define _XPOS 0
#define _UNIT_XPOS (_VAR_LEN + _DATA_LEN + 2) * 2
#define _YPOS ROW0

/* GLOBAL VARIABLES ---------------------------------------------*/

// Static Values
extern uint8_t page_num_val;

// Page 1 Values
extern uint8_t speed_val;
extern uint8_t cruise_speed_val;
extern uint32_t batt_volt_val;
extern uint32_t batt_curr_val;
extern uint32_t batt_temp_val;
extern uint32_t motor_temp_val;

// Page 2 Values



/* FUNCTION PROTOTYPES ---------------------------------------------*/

/**
 * Write a byte of data through C3 to C10(Data Buses)
 * @Param byte: 1 byte code
 * Returns: nothing
 */
void WriteByteToDataBus(uint8_t output_byte);
/**
 * Writes a byte of command code through ports C3 to C10(Data Buses)
 * @Param command_code: 1 byte command code
 * returns: nothing
 */
void TransmitCommand(uint8_t command_code);
/**
 * Writes a byte of parameter code through ports C3 to C10(Data Buses)
 * @Param parameter_code: 1 byte parameter code
 * returns: nothing
 */
void TransmitCommandParameter(uint8_t parameter_code);
 /**
  * Moves cursor to the position on the screen determined by XY Coordinates
  * @Param x: x-coordinate
  * @Param y: y-coordinate
  */
void MoveCursor(uint8_t x, uint8_t y);
 /**
  * Saves each subcharacter to the SG RAM 1
  * @Param bitmap: An array of the bitmap of that particular subcharacter
  * @Param offset: Some offset number
  * Returns: nothing
  */
void SaveCharacterToRAM(uint8_t* bitmap, uint8_t offset);
/**
 * Writes custom bitmaps to the LCD screen RAM
 */
void CharacterBitMaps(void);
/*
 *Purpose: Prints 1200 " " characters to clear the screen
 */
void ClearScreen(void);
/*
 * Displays all information in a single screen
 */
void DisplayScreen(void);
/**
 * Periodically called to update numerical values on the screen
 * @Param screen_state: The current state of the screen (Screen1 or Screen2)
 * @Param values[]:	An array of values to display on the appropriate screen
 * Returns: nothing
 */
void UpdateScreen(uint8_t screen_state, uint8_t values[]);
/**
 * Initialise screen settings and character bit maps
 */
void ScreenSetup(void);
/**
 * Initialise GPIO Pins C0 to C12 for output
 */
void InitialiseLCDPins(void);
/**
 * Erases and revalues a single value field on a screen
 * @Param integerValue: The integer value of the parameter(Between -999 to 999)
 * @Param decValue: The decimal component of the parameter
 * @Param decOn: 1 to display decimals, 0 for no decimals
 *  @Param x: The x value of the parameter on the screen
 * @Param y: The y value of the parameter on the screen
 */
void UpdateScreenParameter(int32_t integerValue, uint8_t decValue, uint8_t decOn, uint8_t x, uint8_t y);
/**
 * Periodically called to change the value of the Charge or Speed display bar
 * @Param num: The number to be represented
 * @Param max: The maximum value that the bar can display
 * @Param y: The y-coordinate of the bar
 * Returns: nothing
 */
void SetBar(uint8_t num, uint8_t max, uint8_t y);
/**
 * Outputs a string of characters to the screen
 * @Param Str[]: A string, "array of characters", whatever
 * @Param starting_x: x-coordinate to write the string to
 * @Param starting_y: y-coordinate to write the string to
 * Returns: nothing
 */
void OutputString(char Str[], uint8_t starting_x, uint8_t starting_y);
/**
 * Converts a number to a string which can be output on the screen
 * The output can take any value from '-999.9' to ' 999.9'
 * @Param num: The integer portion of the number to be displayed
 * @Param dec: The decimal portion of the number to be displayed
 * @Param decOn: Determines whether or not to show decimal value
 * @Param x: x-coordinate to write the character
 * @Param y: y-coordinate to write the character
 * Returns: nothing
 */
void OutputPaddedInteger(int32_t num, uint8_t dec, uint8_t decOn, uint8_t x, uint8_t y);
/**
 * A Delay with a value of ~1us
 * @Param counts: Number of microseconds
 * Returns: nothing
 */
void MicrosecondDelay(uint32_t counts);
/**
 * A Delay with a value of ~1ms
 * @Param counts: Number of milliseconds
 * Returns: nothing
 */
void Delay(uint32_t counts);
