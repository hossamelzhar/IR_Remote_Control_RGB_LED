/**********************************************************************************************
 * Author:          Hossam M. Elzhar
 * Creation Data:   25 MAY, 2023
 * Version:         v1.0
 * Compiler:        GNU ARM-GCC
 *********************************************************************************************/


#include "BIT_MATH.h"
#include "STD_Types.h"

#include "GPIO_interface.h"
#include "SYSTIC_interface.h"
#include "RCC_interface.h"
#include "NVIC_interface.h"
#include "EXTI_interface.h"

#include "IRReciever_interface.h"
#include "IRReciever_private.h"
#include "IRReciever_config.h"

volatile u32 IRReciever_u32FrameArray[40];
volatile u8 IRReciever_u8StartOfFrameFlag;
volatile u8 IRReciever_u8ArrayCounter;
volatile u8 IRReciever_u8RecievingDoneFlag;


void HIRReciever_voidInit(void)
{
    MRCC_voidInit();
    MRCC_voidEnablePeripheralClock(RCC_AHB1, GPIOA_ID);

    MGPIO_voidSetPinMode(GPIOA, PIN0, INPUT);
    MGPIO_voidSetInputConnectionType(GPIOA, PIN0, PULLUP);

    MNVIC_voidInit();
    MNVIC_voidEnablePeripheralInterrupt(MNVIC_EXTI0);

    MEXTI_voidInit();
    MEXTI_voidChangeSenseMode(MEXTI_LINE_0, MEXTI_FALLING_EDGE);
    MEXTI_voidSetCallBack(MEXTI_LINE_0, &HIRReciever_voidRecieveData);
    MEXTI_voidEnableInterrupt(MEXTI_LINE_0);

    MSYSTICK_voidInit();
    MSYSTICK_voidStop();
    MSYSTICK_voidSetCallBack(&Frame_Completed);
    MSYSTICK_voidInterruptDisnable();
}


static void HIRReciever_voidRecieveData(void)
{
    if(IRReciever_u8StartOfFrameFlag == FLAG_NOT_ACTIVE)//First Falling Edge//First Interrupt Enterance
    {
        MSYSTICK_voidSetPreloadValue(START_BIT_TIME);
        MSYSTICK_voidInterruptEnable();
        IRReciever_u8StartOfFrameFlag = FLAG_ACTIVE;
    }
    else
    {
        IRReciever_u32FrameArray[IRReciever_u8ArrayCounter] = MSYSTICK_u32GetElpasedTime();
        MSYSTICK_voidStop();
        MSYSTICK_voidSetPreloadValue(FRAME_BITS_TIME);        
        IRReciever_u8ArrayCounter++;
    }
}

static void Frame_Completed(void)
{
    IRReciever_u8RecievingDoneFlag = FLAG_ACTIVE;
    MSYSTICK_voidInterruptDisnable();
    //Stop SYSTICK
    MSYSTICK_voidStop();
}

u8 HIRReciever_u8IsDataRecieved(void)
{
    u8 Local_u8RecievingStatus = NOT_RECIEVED_YET;

    if(IRReciever_u8RecievingDoneFlag == FLAG_ACTIVE)
    {
        Local_u8RecievingStatus = RECIEVED;
        IRReciever_u8RecievingDoneFlag = FLAG_NOT_ACTIVE;
    }else{}

    return Local_u8RecievingStatus;
}

void HIRReciever_u8DecodeFrame(u8 * Copy_pu8RecievedData)
{
    u8 Local_u8Counter;
    (*Copy_pu8RecievedData) = 0;

    for(Local_u8Counter = DATA_START_BIT; Local_u8Counter<=DATA_END_BIT; Local_u8Counter++)
    {
        if((IRReciever_u32FrameArray[Local_u8Counter] >= ZERO_BIT_TIME_BEGIN) && (IRReciever_u32FrameArray[Local_u8Counter] <= ZERO_BIT_TIME_END))
        {
            CLEAR_BIT((*Copy_pu8RecievedData), (Local_u8Counter - DATA_START_BIT));
        }
        else if((IRReciever_u32FrameArray[Local_u8Counter] >= ONE_BIT_TIME_BEGIN) && (IRReciever_u32FrameArray[Local_u8Counter] <= ONE_BIT_TIME_END))
        {
            SET_BIT((*Copy_pu8RecievedData), (Local_u8Counter - DATA_START_BIT));
        }else{}
        
    }

    // Reset The Start Of Frame Flag
    IRReciever_u8StartOfFrameFlag = FLAG_NOT_ACTIVE;
    //Reset Array Counter
    IRReciever_u8ArrayCounter = RESET;
}

void HIRReciever_voidGetPressedKey(u8 * Copy_pu8ButtonPressed)
{
    switch( (*Copy_pu8ButtonPressed) )
    {
        case 12:
            *Copy_pu8ButtonPressed = 1;
            break;
        case 24:
            *Copy_pu8ButtonPressed = 2;
            break;
        case 94:
            *Copy_pu8ButtonPressed = 3;
            break;
        case 8:
            *Copy_pu8ButtonPressed = 4;
            break;
        case 28:
            *Copy_pu8ButtonPressed = 5;
            break;
        case 90:
            *Copy_pu8ButtonPressed = 6;
            break;
        case 66:
            *Copy_pu8ButtonPressed = 7;
            break;
        case 82:
            *Copy_pu8ButtonPressed = 8;
            break;
        case 74:
            *Copy_pu8ButtonPressed = 9;
            break;
        case 22:
            *Copy_pu8ButtonPressed = 10;
            break;
    }
}
