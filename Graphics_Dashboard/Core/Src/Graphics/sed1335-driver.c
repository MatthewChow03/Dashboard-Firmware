//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------
#include "Graphics/sed1335-driver.h"

//extern void GLCD_InitializePorts(void);

//-------------------------------------------------------------------------------------------------
// Funkcja inicjalizacji wy�wietlacza
//-------------------------------------------------------------------------------------------------
void GLCD_Initialize(void) {
//	GLCD_InitializePorts();

	GLCD_WriteCommand(SED1335_SYSTEM_SET);
	GLCD_WriteData(SED1335_SYS_P1);
	GLCD_WriteData(SED1335_SYS_P2);
	GLCD_WriteData(SED1335_FY);
	GLCD_WriteData(SED1335_CR);
	GLCD_WriteData(SED1335_TCR);
	GLCD_WriteData(SED1335_LF);
	GLCD_WriteData(SED1335_APL);
	GLCD_WriteData(SED1335_APH);

	GLCD_WriteCommand(SED1335_SCROLL);
	GLCD_WriteData(SED1335_SAD1L);
	GLCD_WriteData(SED1335_SAD1H);
	GLCD_WriteData(SED1335_SL1);
	GLCD_WriteData(SED1335_SAD2L);
	GLCD_WriteData(SED1335_SAD2H);
	GLCD_WriteData(SED1335_SL2);
	GLCD_WriteData(SED1335_SAD3L);
	GLCD_WriteData(SED1335_SAD3H);
	GLCD_WriteData(SED1335_SAD4L);
	GLCD_WriteData(SED1335_SAD4H);

	GLCD_WriteCommand(SED1335_CSRFORM);
	GLCD_WriteData(SED1335_CRX);
	GLCD_WriteData(SED1335_CSRF_P2);

	GLCD_WriteCommand(SED1335_CGRAM_ADR);
	GLCD_WriteData(SED1335_SAGL);
	GLCD_WriteData(SED1335_SAGH);

	GLCD_WriteCommand(SED1335_CSRDIR_R);

	GLCD_WriteCommand(SED1335_HDOT_SCR);
	GLCD_WriteData(SED1335_SCRD);

	GLCD_WriteCommand(SED1335_OVLAY);
	GLCD_WriteData(SED1335_OVLAY_P1);

	GLCD_WriteCommand(SED1335_DISP_ON);
	GLCD_WriteData(SED1335_FLASH);
}
//-------------------------------------------------------------------------------------------------
// Funkcja zapalaj�ca piksel
//-------------------------------------------------------------------------------------------------
void GLCD_SetPixel(unsigned int x, unsigned int y, int color) {
	unsigned char tmp = 0;
	unsigned int address = SED1335_GRAPHICSTART + (40 * y) + (x / 8);
	GLCD_SetCursorAddress(address);

	GLCD_WriteCommand(SED1335_MREAD);
	tmp = GLCD_ReadData();

	if (color)
		tmp |= (1 << (SED1335_FX - (x % 8)));
	else
		tmp &= ~(1 << (SED1335_FX - (x % 8)));

	GLCD_SetCursorAddress(address);
	GLCD_WriteCommand(SED1335_MWRITE);
	GLCD_WriteData(tmp);
}
//-------------------------------------------------------------------------------------------------
// Funkcja wyswietlajaca tekst
//-------------------------------------------------------------------------------------------------
void GLCD_WriteText(char *tekst) {
	GLCD_WriteCommand(SED1335_MWRITE);
	while (*tekst)
		GLCD_WriteData(*tekst++);
}
//-------------------------------------------------------------------------------------------------
// Funkcja wy�wietlaj�ca tekst z pami�ci programu (AVR)
//-------------------------------------------------------------------------------------------------
//void GLCD_WriteText(char *tekst) {
//	GLCD_WriteCommand(SED1335_MWRITE);
//	while (GLCD_ReadByteFromROMMemory(tekst))
//		GLCD_WriteData(GLCD_ReadByteFromROMMemory(tekst++));
//}
//-------------------------------------------------------------------------------------------------
// Funkcja ustawiaj�ca adres kursora
//-------------------------------------------------------------------------------------------------
void GLCD_SetCursorAddress(unsigned int address) {
	GLCD_WriteCommand(SED1335_CSRW);
	GLCD_WriteData((unsigned char) (address & 0xFF));
	GLCD_WriteData((unsigned char) (address >> 8));
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void GLCD_TextGoTo(unsigned char x, unsigned char y) {
	GLCD_SetCursorAddress((y * 40) + x);
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void GLCD_GraphicGoTo(unsigned int x, unsigned int y) {
	GLCD_SetCursorAddress(SED1335_GRAPHICSTART + (y * 40) + x / 8);
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void GLCD_ClearText(void) {
	int i;
	GLCD_TextGoTo(0, 0);
	GLCD_WriteCommand(SED1335_MWRITE);
	for (i = 0; i < 1200; i++)
		GLCD_WriteData(' ');
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void GLCD_ClearGraphic(void) {
	unsigned int i;
	GLCD_SetCursorAddress(SED1335_GRAPHICSTART);
	GLCD_WriteCommand(SED1335_MWRITE);
	for (i = 0; i < (40 * 240); i++)
		GLCD_WriteData(0x00);
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void GLCD_Bitmap(char *bmp, int x, int y, int width, int height) {
	unsigned int i, j;
	for (i = 0; i < height; i++) {
		GLCD_GraphicGoTo(x, y + i);
		GLCD_WriteCommand(SED1335_MWRITE);
		for (j = 0; j < width / 8; j++)
			GLCD_WriteData(GLCD_ReadByteFromROMMemory(bmp + j + (40 * i)));
	}
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//Write text using a fixed 16-pixel width, 24 pixel height bitmap font, each character is 48 bytes or 24 ints
//first param is pointer to the string to be written
//last param is pointer to the font
void GLCD_WriteBitmapText(char * text,int x, int y, unsigned int * font)
{
	unsigned int * fontPointer;
	char i;


	while(*text)
	{

		//First get the pointer to the first int of the font letter
		//Font starts with space which is ascii char #32
		fontPointer = font + ((*text - 32) * 24); // each char 24 ints long
		for (i=0;i<24;i++)
		{
			//X is reset on the page boundry (every 8 pixels) for speed and ease of implementation
			//Move LCD ram cursor to appropriate location
			//40 is the number of bytes in a horizontal row
			GLCD_SetCursorAddress(SED1335_GRAPHICSTART + (40 * (y+i)) + (x/8));
			//send command to write to LCD RAM
			GLCD_WriteCommand(SED1335_MWRITE);
			//Write Data
//			GLCD_WriteData(pgm_read_word_near(fontPointer)); //lowbyte or first 8 pixels of this row
//			GLCD_WriteData(pgm_read_word_near(fontPointer) >> 8); //highbyte or last 8 pixels

			GLCD_WriteData((unsigned char)(fontPointer)); //lowbyte or first 8 pixels of this row
			GLCD_WriteData((unsigned char)(fontPointer) >> 8); //highbyte or last 8 pixels

			fontPointer++;
		}
		text++;
		x+=16; //move the cursor 16 pixels for the next char
	}
}

