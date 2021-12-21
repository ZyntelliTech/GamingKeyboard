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

/*_____ D E C L A R A T I O N S ____________________________________________*/
void	NotPinOut_GPIO_init(void);

/*_____ D E F I N I T I O N S ______________________________________________*/
#ifndef	SN32F248B					//Do NOT Remove or Modify!!!
	#error Please install SONiX.SN32F2_DFP.1.2.9.pack or version >= 1.2.9
#endif
#define	PKG		SN32F248B		//User SHALL modify the package on demand (SN32F248B, SN32F247B, SN32F246B, SN32F2451B) 

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/
void SysTick_Init(void);
void NDT_Init(void);

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
	
	//** Initial GPIO
	SN_GPIO0->CFG = 0x00000000;  //** Enable P0 internal pull-up resistor
	SN_GPIO1->CFG = 0x00000000;  //** Enable P1 internal pull-up resistor
	SN_GPIO2->CFG = 0x00000000;  //** Enable P2 internal pull-up resistor
	SN_GPIO3->CFG = 0x00000000;  //** Enable P3 internal pull-up resistor
	
	SN_GPIO0->MODE = 0x00000000; //** P0 Input mode			
	SN_GPIO1->MODE = 0x00000000; //** P1 Input mode
	SN_GPIO2->MODE = 0x00000000; //** P2 Input mode
	SN_GPIO3->MODE = 0x00000000; //** P3 Input mode
	
	NDT_Init();								/* NDT Initialization */
	//****************USB Setting START******************//
	USB_Init();								/* USB Initialization */
	//*******************USB Setting END*****************//
	
	while (1)
	{	
		if (sUSB_EumeData.wUSB_Status & mskBUSSUSPEND)	//** Check BusSuspend
		{
			USB_Suspend();
		}
		/*#if (USB_LIBRARY_TYPE == USB_MOUSE_TYPE)
		USB_EPnINFunction(USB_EP1,&wUSB_MouseData,4);
		#else
		USB_EPnINFunction(USB_EP2,&wUSB_MouseData,5);
		#endif*/
	}
}


/*****************************************************************************
* Function		 : SysTick_Init
* Description	 : For EFT Protection
* Input	(Global) : None
* Input	(Local)	 : None
* Output (Global): None
* Return (Local) : None
*****************************************************************************/
void	SysTick_Init (void)
{
	SysTick->LOAD = 0x000752FF;		//RELOAD = (system tick clock frequency ? 10 ms)/1000 -1
	SysTick->VAL = 0xFF; //__SYSTICK_CLEAR_COUNTER_AND_FLAG;
	SysTick->CTRL = 0x7;			//Enable SysTick timer and interrupt	
}


/*****************************************************************************
* Function		 : SysTick_Handler
* Description	 : For EFT Protection
* Input	(Global) : None
* Input	(Local)	 : None
* Output (Global): None
* Return (Local) : None
*****************************************************************************/
__irq void SysTick_Handler(void)
{
	if(bNDT_Flag)			//** Check NDT
	{
		if(SN_SYS0->NDTSTS_b.NDT5V_DET == 1)
		{
			dbNDT_Cnt = 0;
		}
		else
		{
		  dbNDT_Cnt++;
			if(dbNDT_Cnt == 90)//** 900ms
			{
				bNDT_Flag = 0;
			}
		}
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



