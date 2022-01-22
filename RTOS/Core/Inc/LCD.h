#include "stm32f1xx_hal.h"

// OLD LCD VARS
#define BYTEPERBITMAP 16
#define LOOKUP_COLUMN 4
#define LOOKUP_ROW 41
#define OFFSET_NUMERIC 22
#define OFFSET_CAPITAL 65
#define OFFSET_LOWCASE 97

// NEW LCD VARS
#define NUM_CAN_IDS 3

/** Helps write float values into the CAN message data array.
 * 	Instead of converting 32-bit floating point values into 4 byte arrays manually,
 * 	the union automatically takes care of that conversion. This conversion is necessary
 * 	since you cannot write float values directly into the CAN message data field. The float
 * 	value must first be converted to an array of 4 bytes (uint8_t).
 *
 */
typedef union FloatBytes {
	float float_value;			/**< Float value member of the union. */
	uint8_t bytes[4];			/**< Array of 4 bytes member of union. */
} FloatBytes;

// Unparsed data that is populated immediately when CAN data is received
typedef struct DisplayRawData
{
	const uint8_t can_id;
	volatile uint8_t raw_data[8];

} DisplayRawData;

// Constant values used to create the screen layout row by row
typedef struct DisplayVar
{
	// Example: "BATT TEMP: 55*C"
	const char* name; 			// TODO: how to do memory managmeent here (this member is not changing)

	const char* unit;			// Units to be displayed at the end (e.g., KMH, %, C)

	const uint8_t name_len;		// Characters in the name of the variable being displayed
								// Does not include colon or trailing spaces

	const uint8_t data_len;		// Max length of the data
								// Space from a decimal point or negative sign must be included in the length

	const uint8_t data_xpos;	// var_len + 1, colon and space after the variable name

	const uint8_t unit_xpos;	// data_xpos + data_len


} DisplayVar;

extern DisplayVar DisplayLayout[];

extern DisplayRawData DisplayData[];


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
