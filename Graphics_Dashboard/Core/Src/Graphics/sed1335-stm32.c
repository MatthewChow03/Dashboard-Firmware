#include "stm32f1xx_hal.h"

#define SED1335_PORT GPIOC

#define SED1335_A0	GPIO_PIN_0
#define SED1335_WR	GPIO_PIN_1
#define SED1335_RD	GPIO_PIN_2
#define SED1335_D0  GPIO_PIN_3
#define SED1335_CS	GPIO_PIN_11
#define SED1335_RES	GPIO_PIN_12

GPIO_InitTypeDef GPIO_InitStructure;

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//void GLCD_InitPorts(void) {
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
//	GPIO_StructInit(&GPIO_InitStructure);
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init(SED1335_PORT, &GPIO_InitStructure);
//	GPIO_Write(SED1335_PORT, SED1335_A0 | SED1335_WR | SED1335_RD | SED1335_CS);
//	asm("nop");
//	asm("nop");
//	asm("nop");
//	asm("nop");
//	asm("nop");
//	GPIO_SetBits(SED1335_PORT, SED1335_RES);
//}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void GLCD_WriteData(unsigned char dataToWrite) {
	HAL_GPIO_WritePin(SED1335_PORT, (dataToWrite << SED1335_D0), GPIO_PIN_SET);
	dataToWrite ^= 0xFF;
	HAL_GPIO_WritePin(SED1335_PORT, (dataToWrite << SED1335_D0), GPIO_PIN_RESET);

	HAL_GPIO_WritePin(SED1335_PORT, (SED1335_CS | SED1335_A0 | SED1335_WR), GPIO_PIN_RESET);
	asm("nop");
	asm("nop");
	asm("nop");
	HAL_GPIO_WritePin(SED1335_PORT, (SED1335_CS | SED1335_A0 | SED1335_WR), GPIO_PIN_SET);
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void GLCD_WriteCommand(unsigned char commandToWrite) {
	HAL_GPIO_WritePin(SED1335_PORT, (commandToWrite << SED1335_D0), GPIO_PIN_SET);
	commandToWrite ^= 0xFF;
	HAL_GPIO_WritePin(SED1335_PORT, (commandToWrite << SED1335_D0), GPIO_PIN_RESET);

	HAL_GPIO_WritePin(SED1335_PORT, (SED1335_CS | SED1335_WR), GPIO_PIN_RESET);
	asm("nop");
	asm("nop");
	asm("nop");
	HAL_GPIO_WritePin(SED1335_PORT, (SED1335_CS | SED1335_WR), GPIO_PIN_SET);
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
// TODO: Check this function, all previous code was remade with HAL functions, but they aren't
// equivilant because some funcitons are now deprecated (i.e., create a new struct with default values)
unsigned char GLCD_ReadData(void) {
	unsigned char tmp;
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	//GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStruct.Pin = 0xFF << SED1335_D0;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SED1335_PORT, &GPIO_InitStruct);

	HAL_GPIO_WritePin(SED1335_PORT, (SED1335_CS | SED1335_RD), GPIO_PIN_RESET);
	asm("nop");
	tmp = HAL_GPIO_ReadPin(SED1335_PORT, SED1335_D0); // TODO: this line might not be correct
	HAL_GPIO_WritePin(SED1335_PORT, (SED1335_CS | SED1335_RD), GPIO_PIN_SET);

	//GPIO_InitStruct = {0};
	//GPIO_InitStruct(&GPIO_InitStructure);
	GPIO_InitStruct.Pin = (0xFF << SED1335_D0);
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(SED1335_PORT, &GPIO_InitStruct);

	return tmp;
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
unsigned char GLCD_ReadByteFromROMMemory(unsigned char *ptr) {
	return *(ptr);
}
//

