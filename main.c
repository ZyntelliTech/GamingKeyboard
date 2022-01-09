/******************** (C) COPYRIGHT 2018 SONiX *******************************
* COMPANY: SONiX
* DATE:		 2018/02
* AUTHOR:	 SA1
* IC:			 SN32F240B
*____________________________________________________________________________
* REVISION	Date				User		Description
* 1.00			2017/07/07	Tim		1. First release
* 1.01			2017/07/17	Tim		1. Modified the BL jump address
*															2. Added the EMC protect
* 1.02			2018/02/06	Tim		1. Add NotPinOut_GPIO_init to set the status of the GPIO which are NOT pin-out to input pull-up.
*															2. Add the CodeOption_SN32F240B.s to modify Code option, please modify with Configuration Wizard, 
*																 and Strongly recommand to keep CS0 for debugging with SN-LINK.
*															3. Enable the EFT protect process.
* 1.03			2018/09/27	Tim		1. Modify the usbhw.c.
* 1.04			2019/12/03	Tim		1. Modify usbhw.c
*															2. Modify the SOF judging grammar in USB IRQ_handler flow
*															3. Modify the HID Descriptor for USB ISP
*___
*_________________________________________________________________
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS TIME TO MARKET.
* SONiX SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL 
* DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH SOFTWARE
* AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN 
* IN CONNECTION WITH THEIR PRODUCTS.
*****************************************************************************/

/*_____ I N C L U D E S ____________________________________________________*/
#include	"SN32F240B.h"
#include	"SN32F200_Def.h"
#include	".\Utility\Utility.h"
#include	".\Usb\usbram.h"
#include	".\Usb\usbdesc.h"
#include	".\Usb\usbhw.h"
#include	".\Usb\usbuser.h"
#include	".\Usb\usbepfunc.h"
#include	".\UsbHid\hid.h"
#include	".\UsbHid\hiduser.h"
#include <stdio.h>
#include <string.h>
#include "usb_hid_keycode.h"
#include "GPIO.h"
#include "SysTick.h"
#define CLICKED	 1
#define RELEASED 0
/*_____ D E C L A R A T I O N S ____________________________________________*/
void	NotPinOut_GPIO_init(void);

/*_____ D E F I N I T I O N S ______________________________________________*/
#ifndef	SN32F248B					//Do NOT Remove or Modify!!!
	#error Please install SONiX.SN32F2_DFP.1.2.9.pack or version >= 1.2.9
#endif
#define	PKG		SN32F248B		//User SHALL modify the package on demand (SN32F248B, SN32F247B, SN32F246B, SN32F2451B) 

/*_____ M A C R O S ________________________________________________________*/

typedef struct
{
	uint8_t MODIFIER;
	uint8_t RESERVED;
	uint8_t KEYCODE1;
	uint8_t KEYCODE2;
	uint8_t KEYCODE3;
	uint8_t KEYCODE4;
	uint8_t KEYCODE5;
	uint8_t KEYCODE6;
}keyboardHID;

/* USB HID variables */
keyboardHID keyboardhid = {0,0,0,0,0,0,0,0};
uint32_t keyboardhid_report[2];

/* Key Press Flag */
uint8_t key_pressed = 0;
uint8_t i =  0;

#define CL_NUM 19
uint8_t ckeyMapper[CL_NUM][2] = {
	{GPIO_PORT3, GPIO_PIN4},		//CL0
	{GPIO_PORT3, GPIO_PIN3}, 		//CL1
	{GPIO_PORT1, GPIO_PIN15},		//CL2
	{GPIO_PORT1, GPIO_PIN14},		//CL3
	{GPIO_PORT1, GPIO_PIN13},		//CL4
	{GPIO_PORT1, GPIO_PIN6},		//CL5
	//{GPIO_PORT3, GPIO_PIN6},		//CL6
	//{GPIO_PORT3, GPIO_PIN5},		//CL7
	{GPIO_PORT0, GPIO_PIN8},		//CL8
	{GPIO_PORT0, GPIO_PIN9},		//CL9
	{GPIO_PORT0, GPIO_PIN10},		//CL10
	{GPIO_PORT0, GPIO_PIN11},		//CL11
	{GPIO_PORT0, GPIO_PIN12},		//CL12
	{GPIO_PORT0, GPIO_PIN13},		//CL13
	{GPIO_PORT0, GPIO_PIN14},		//CL14
	{GPIO_PORT1, GPIO_PIN0},		//CL15
	{GPIO_PORT1, GPIO_PIN1},		//CL16
	{GPIO_PORT1, GPIO_PIN2},		//CL17
	{GPIO_PORT1, GPIO_PIN3},		//CL18
	{GPIO_PORT1, GPIO_PIN4},		//CL19
	{GPIO_PORT1, GPIO_PIN5},		//CL20
};
uint8_t rKeyMapper[6][2] = {
	{GPIO_PORT2, GPIO_PIN15},		//R0
	{GPIO_PORT3, GPIO_PIN11}, 	//R1
	{GPIO_PORT3, GPIO_PIN10},		//R2
	{GPIO_PORT3, GPIO_PIN9},		//R3
	{GPIO_PORT3, GPIO_PIN8},		//R4
	{GPIO_PORT3, GPIO_PIN7},		//R5
};


/*_____ F U N C T I O N S __________________________________________________*/
void SysTick_Init(void);
void NDT_Init(void);

void SendKeyCode(keyboardHID key_hid, uint32_t delay_ms)
{
	keyboardhid_report[0] = (key_hid.KEYCODE2<<24) | (key_hid.KEYCODE1<<16) | (key_hid.RESERVED << 8) | key_hid.MODIFIER;
	keyboardhid_report[1] = (key_hid.KEYCODE6<<24) | (key_hid.KEYCODE5<<16) | (key_hid.KEYCODE4<<8) | (key_hid.KEYCODE3);
	USB_EPnINFunction(USB_EP1, keyboardhid_report, 8);
	if(delay_ms > 0) UT_MAIN_DelayNms(delay_ms);
}
void releaseKeyCode(uint32_t delay_ms)
{
	memset(&keyboardhid, 0x00, 8);
	keyboardhid_report[0] = 0;
	keyboardhid_report[1] = 0;
	USB_EPnINFunction(USB_EP1, keyboardhid_report, 8);
	if(delay_ms > 0) UT_MAIN_DelayNms(delay_ms);
}

void GPIO_Configuration(void)
{
	// Input/Output mode
	SN_GPIO0->MODE = 0xFFFFFFFF;
	SN_GPIO1->MODE = 0xFFFFFFFF;
	SN_GPIO2->MODE = 0xFFF7FFF;
	SN_GPIO3->MODE = 0x0000F078;
	
	// Configure Pullup
	//SN_GPIO0->CFG = 0x00000000;
	//SN_GPIO1->CFG = 0x00000000;
	//SN_GPIO2->CFG = 0x00000000;	
	//SN_GPIO3->CFG = 0x00000000;
	// Data reset
	SN_GPIO0->DATA = 0x00000000;
	SN_GPIO1->DATA = 0x00000000;
	SN_GPIO2->DATA = 0x00000000;
	SN_GPIO3->DATA = 0x00000000;
}

void resetAllGpio()
{
	SN_GPIO0->DATA = 0x00000000;
	SN_GPIO1->DATA = 0x00000000;
	SN_GPIO2->DATA = 0x00000000;
	SN_GPIO3->DATA = 0x00000000;
}

void readKeyboard(void)
{
	resetAllGpio();
	for(i = 0; i < 4; i++)
	{
		GPIO_Set(ckeyMapper[i][0], ckeyMapper[i][1]);
		switch (i){
			case 0:
				if(SN_GPIO2->DATA_b.DATA15 == CLICKED){
					keyboardhid.KEYCODE1 = KEY_A; 
					key_pressed = 0x01;
				}
				else if(SN_GPIO3->DATA_b.DATA11 == CLICKED){
					keyboardhid.KEYCODE1 = KEY_B; 
					key_pressed = 0x01;
				}
				else if(SN_GPIO3->DATA_b.DATA10 == CLICKED){
					keyboardhid.KEYCODE1 = KEY_C; 
					key_pressed = 0x01;
				}
				else if(SN_GPIO3->DATA_b.DATA9 == CLICKED){
					keyboardhid.KEYCODE1 = KEY_D;
					key_pressed = 0x01;
				}					
				break;
			case 1:
				if(SN_GPIO2->DATA_b.DATA15 == CLICKED){ 
					keyboardhid.KEYCODE1 = KEY_E;
					key_pressed = 0x01;
				}
				else if(SN_GPIO3->DATA_b.DATA11 == CLICKED){
					keyboardhid.KEYCODE1 = KEY_F;
					key_pressed = 0x01;
				}
				else if(SN_GPIO3->DATA_b.DATA10 == CLICKED){
					keyboardhid.KEYCODE1 = KEY_G;
					key_pressed = 0x01;
				}
				else if(SN_GPIO3->DATA_b.DATA9 == CLICKED){
					keyboardhid.KEYCODE1 = KEY_H;		
					key_pressed = 0x01;
				}
				break;
			case 2:
				if(SN_GPIO2->DATA_b.DATA15 == CLICKED){
					keyboardhid.KEYCODE1 = KEY_I;
					key_pressed = 0x01;
				}
				else if(SN_GPIO3->DATA_b.DATA11 == CLICKED){
					keyboardhid.KEYCODE1 = KEY_J;
					key_pressed = 0x01;
				}
				else if(SN_GPIO3->DATA_b.DATA10 == CLICKED){
					keyboardhid.KEYCODE1 = KEY_K;
					key_pressed = 0x01;
				}
				else if(SN_GPIO3->DATA_b.DATA9 == CLICKED){
					keyboardhid.KEYCODE1 = KEY_L;		
					key_pressed = 0x01;
				}
				break;
			case 3:
				if(SN_GPIO2->DATA_b.DATA15 == CLICKED){
					keyboardhid.KEYCODE1 = KEY_M;
					key_pressed = 0x01;
				}
				else if(SN_GPIO3->DATA_b.DATA11 == CLICKED){
					keyboardhid.KEYCODE1 = KEY_N;
					key_pressed = 0x01;
				}
				else if(SN_GPIO3->DATA_b.DATA10 == CLICKED){
					keyboardhid.KEYCODE1 = KEY_O;
					key_pressed = 0x01;
				}
				else if(SN_GPIO3->DATA_b.DATA9 == CLICKED){
					keyboardhid.KEYCODE1 = KEY_P;		
					key_pressed = 0x01;
				}
				break;
		}
		if(key_pressed > 0) break;
	}
}
/*****************************************************************************
* Function		: main
* Description	: USB HID demo code 
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
int	main (void)
{
	SystemInit();
	SystemCoreClockUpdate();

	//1. User SHALL define PKG on demand.
	//2. User SHALL set the status of the GPIO which are NOT pin-out to input pull-up.
	NotPinOut_GPIO_init();
	
	NDT_Init();								/* NDT Initialization */
	//****************USB Setting START******************//
	USB_Init();								/* USB Initialization */
	//*******************USB Setting END*****************//
	GPIO_Configuration();
	SysTick_Init();	//init SysTick
	
	while (1)
	{
#if SYSTICK_IRQ == POLLING_METHOD
		if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
		{
			// Cycle Processing by SysTick
			readKeyboard();
			if(key_pressed == 1){
				keyboardhid.KEYCODE2 = keyboardhid.KEYCODE1;	
				//keyboardhid.KEYCODE3 = keyboardhid.KEYCODE1;	
				//keyboardhid.KEYCODE4 = keyboardhid.KEYCODE1;	
				//keyboardhid.KEYCODE5 = keyboardhid.KEYCODE1;	
				//keyboardhid.KEYCODE6 = keyboardhid.KEYCODE1;	
				SendKeyCode(keyboardhid, 100);		
				releaseKeyCode(100);
				key_pressed = 0;
			}
			
			__SYSTICK_CLEAR_COUNTER_AND_FLAG;
		}
#endif	
		/*
		GPIO_Set(ckeyMapper[0][0], ckeyMapper[0][1]);
#ifdef BUS_SUSPEND
		if (sUSB_EumeData.wUSB_Status & mskBUSSUSPEND)	//** Check BusSuspend
		{
			USB_Suspend();
		}
#endif
		
#if (USB_LIBRARY_TYPE == USB_KB_MOUSE_TYPE1) 
		//readKeyboard();
		//if(key_flag > 0){ 
		//	SendKeyCode(keyboardhid, 1);
		//	releaseKeyCode(1);
		//	key_flag = 0;
		//}
#elif (USB_LIBRARY_TYPE == USB_MOUSE_TYPE)
			USB_EPnINFunction(USB_EP1,&wUSB_MouseData,4);
#else
			USB_EPnINFunction(USB_EP2,&wUSB_MouseData,5);
#endif
*/
	}
}

/*****************************************************************************
* Function		 : NDT_Init
* Description	 : Noise Detect Init
* Input	(Global) : None
* Input	(Local)	 : None
* Output (Global): None
* Return (Local) : None
*****************************************************************************/
void NDT_Init(void)
{
	SN_SYS0->NDTCTRL = 0x2;					//** Enable NDT 5V
	NVIC_EnableIRQ(NDT_IRQn);
	SN_SYS0->ANTIEFT = 0x04;	
}


/*****************************************************************************
* Function		: NDT_IRQHandler
* Description	: ISR of NDT interrupt
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
__irq void NDT_IRQHandler(void)
{
	NVIC_ClearPendingIRQ(NDT_IRQn);
	if (SN_SYS0->NDTSTS)				//** Check NDT 5V detected
	{
		bNDT_Flag = 1;
	}
	SN_SYS0->NDTSTS = 0x3;				//** Clear NDT 5V flag
}


/*****************************************************************************
* Function		: NotPinOut_GPIO_init
* Description	: Set the status of the GPIO which are NOT pin-out to input pull-up. 
* Input				: None
* Output			: None
* Return			: None
* Note				: 1. User SHALL define PKG on demand.
*****************************************************************************/
void	NotPinOut_GPIO_init(void)
{
	//(SN32F248B, SN32F247B, SN32F246B, SN32F2451B) 
#if (PKG == SN32F248B)
	//set P3.0~P3.2 to input pull-up
	SN_GPIO3->CFG = 0x00AAAA80;
#elif (PKG == SN32F247B)	
	//set P0.8~P0.9, P0.12~P0.15 to input pull-up
	SN_GPIO0->CFG = 0x00A0AAAA;
	//set P1.0~P1.2, P1.14~P1.15 to input pull-up
	SN_GPIO1->CFG = 0x0AAAAA80;
	//set P2.12~P2.14 to input pull-up
	SN_GPIO2->CFG = 0x80AAAAAA;
	//set P3.0~P3.4 to input pull-up
	SN_GPIO3->CFG = 0x00AAA800;
#elif (PKG == SN32F246B)
	//set P0.8~P0.9, P0.12~P0.15 to input pull-up
	SN_GPIO0->CFG = 0x00A0AAAA;
	//set P1.0~P1.2, P1.14~P1.15 to input pull-up
	SN_GPIO1->CFG = 0x0AAAAA80;
	//set P2.10~P2.14 to input pull-up
	SN_GPIO2->CFG = 0x800AAAAA;
	//set P3.0~P3.4 to input pull-up
	SN_GPIO3->CFG = 0x00AAA800;
#elif (PKG == SN32F2451B)
	//set P0.0~P0.1, P0.8~P0.15 to input pull-up
	SN_GPIO0->CFG = 0x0000AAA0;
	//set P1.0~P1.4, P1.12~P1.15 to input pull-up
	SN_GPIO1->CFG = 0x00AAA800;
	//set P2.6~P2.15 to input pull-up
	SN_GPIO2->CFG = 0x00000AAA;
	//set P3.0~P3.4, P3.10~P3.11 to input pull-up
	SN_GPIO3->CFG = 0x000AA800;
#endif
}


/*****************************************************************************
* Function		: HardFault_Handler
* Description	: ISR of Hard fault interrupt
* Input			: None
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
__irq void HardFault_Handler(void)
{
	NVIC_SystemReset();
}



