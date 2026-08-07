#pragma once
#include <stdint.h>
#include <stddef.h>

namespace Hardware
{
	void Initialise();

	void SetLedOn(bool state);
	void FlashLed(int8_t count);

	void FlashingStop(int8_t count);

	int32_t DivModRem(int32_t a, int32_t b, int32_t* rem);

	void* PsramBase();
	size_t PsramSize();
}
