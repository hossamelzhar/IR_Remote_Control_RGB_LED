#include "BIT_MATH.h"
#include "STD_Types.h"

#include "IRReciever_interface.h"

#include "RCC_interface.h"
#include "GPIO_interface.h"
#include "NVIC_interface.h"
#include "EXTI_interface.h"
#include "SYSTIC_interface.h"


void APP_voidRGBFunc(u8 Copy_u8PressedButton);

void main(void)
{
	u8 Local_u8ReturnValue = 0;
	u8 Local_u8PressedButton = 0;

	HIRReciever_voidInit();
	MGPIO_voidSetPinMode(GPIOA, PIN1, OUTPUT);
	MGPIO_voidSetPinMode(GPIOA, PIN2, OUTPUT);
	MGPIO_voidSetPinMode(GPIOA, PIN3, OUTPUT);

	while(1)
	{
		Local_u8ReturnValue = HIRReciever_u8IsDataRecieved();
		if(Local_u8ReturnValue == RECIEVED)
		{
			HIRReciever_u8DecodeFrame(&Local_u8PressedButton);
			HIRReciever_voidGetPressedKey(&Local_u8PressedButton);
			APP_voidRGBFunc(Local_u8PressedButton);
		}
		else{}
	}
}

void APP_voidRGBFunc(u8 Copy_u8PressedButton)
{
	static u8 Local_u8Pin1Flag = 0;
	static u8 Local_u8Pin2Flag = 0;
	static u8 Local_u8Pin3Flag = 0;

	switch(Copy_u8PressedButton)
	{
		case 1:
			Local_u8Pin1Flag ^= 1;
			MGPIO_voidSetPinValue(GPIOA, PIN1, Local_u8Pin1Flag);
			break;
		case 4:
			Local_u8Pin2Flag ^= 1;
			MGPIO_voidSetPinValue(GPIOA, PIN2, Local_u8Pin2Flag);
			break;
		case 7:
			Local_u8Pin3Flag ^= 1;
			MGPIO_voidSetPinValue(GPIOA, PIN3, Local_u8Pin3Flag);
			break;
	}
}
