#include "displayUtility.h"

// Конвертирует 8-битный компонент цвета в 5-битное значение
uint8_t convert8to5(uint8_t x) 
{
	return (x * 249 + 1014) >> 11;
}

// Конвертирует 8-битный компонент цвета в 6-битное значение
uint8_t convert8to6(uint8_t x)
{
	return (x * 63 + 127) >> 8;
}

// Конвертирует RGB цвет в формат RGB565
uint16_t convertRGBtoTFT(uint8_t r, uint8_t g, uint8_t b)
{
	uint16_t r5 = convert8to5(r);
	uint16_t g6 = convert8to6(g);
	uint16_t b5 = convert8to5(b);
	return (r5 << 11) | (g6 << 5) | b5;
}