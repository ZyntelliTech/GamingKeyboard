/******************** (C) COPYRIGHT 2017 SONiX *******************************
* COMPANY:			SONiX
* DATE:					2017/07
* AUTHOR:				SA1
* IC:						SN32F240B
* DESCRIPTION:	GPIO related functions.
*____________________________________________________________________________
* REVISION	Date				User		Description
*	1.0				2017/07/07	SA1			1. First release
*
*____________________________________________________________________________
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS TIME TO MARKET.
* SONiX SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL 
* DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH SOFTWARE
* AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN 
* IN CONNECTION WITH THEIR PRODUCTS.
*****************************************************************************/

/*_____ I N C L U D E S ____________________________________________________*/
#include "GPIO.h"


/*_____ D E C L A R A T I O N S ____________________________________________*/


/*_____ D E F I N I T I O N S ______________________________________________*/


/*_____ M A C R O S ________________________________________________________*/


/*_____ F U N C T I O N S __________________________________________________*/


/*****************************************************************************
* Function		: GPIO_Mode
* Description	: set GPIO as input or output
* Input			: port_number - GPIO0, GPIO1, GPIO2, GPIO3
							pin_number	- GPIO_PIN0, 1, 2, ...,15 
							mode				- 0 as Input
														1 as output					
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void GPIO_Mode(uint32_t port_number, uint32_t pin_number, uint32_t mode)
{
		uint32_t wGpiomode=0;
		switch(port_number){
			case 0:
				wGpiomode=(uint32_t)SN_GPIO0->MODE;
				wGpiomode&=~(1<<(uint32_t) pin_number);				
				wGpiomode|=(mode<<(uint32_t) pin_number);
				SN_GPIO0->MODE=wGpiomode;				
				wGpiomode=SN_GPIO0->MODE;		//for check
			break;
			case 1:
				wGpiomode=(uint32_t)SN_GPIO1->MODE;
				wGpiomode&=~(1<<(uint32_t) pin_number);				
				wGpiomode|=(mode<<(uint32_t) pin_number);
				SN_GPIO1->MODE=wGpiomode;				
				wGpiomode=SN_GPIO1->MODE;		//for check
			break;
			case 2:
				wGpiomode=(uint32_t)SN_GPIO2->MODE;
				wGpiomode&=~(1<<(uint32_t) pin_number);				
				wGpiomode|=(mode<<(uint32_t) pin_number);
				SN_GPIO2->MODE=wGpiomode;				
				wGpiomode=SN_GPIO2->MODE;		//for check
			break;
			case 3:
				wGpiomode=(uint32_t)SN_GPIO3->MODE;
				wGpiomode&=~(1<<(uint32_t) pin_number);				
				wGpiomode|=(mode<<(uint32_t) pin_number);
				SN_GPIO3->MODE=wGpiomode;				
				wGpiomode=SN_GPIO3->MODE;		//for check
			break;
			default:
			break;		
		}
	return;
}

/*****************************************************************************
* Function		: GPIO_Set
* Description	: set GPIO high
* Input			: port_number - GPIO0, GPIO1, GPIO2, GPIO3
							pin_number	- GPIO_PIN0, 1, 2, ...,15 
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void GPIO_Set(uint32_t port_number, uint32_t pin_number)
{
		switch(port_number){
			case 0:
				SN_GPIO0->BSET|=(1<<pin_number);
			break;
			case 1:
				SN_GPIO1->BSET|=(1<<pin_number);
			break;
			case 2:
				SN_GPIO2->BSET|=(1<<pin_number);
			break;			
			case 3:
				SN_GPIO3->BSET|=(1<<pin_number);
			break;
			default:
			break;			
		}
	return;
}

/*****************************************************************************
* Function		: GPIO_Clr
* Description	: set GPIO low
* Input			: port_number - GPIO0, GPIO1, GPIO2, GPIO3
							pin_number	- GPIO_PIN0, 1, 2, ...,15 
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void GPIO_Clr(uint32_t port_number, uint32_t pin_number)
{
		switch(port_number){
			case 0:
				SN_GPIO0->BCLR|=(1<<pin_number);
			break;
			case 1:
				SN_GPIO1->BCLR|=(1<<pin_number);
			break;
			case 2:
				SN_GPIO2->BCLR|=(1<<pin_number);
			break;			
			case 3:
				SN_GPIO3->BCLR|=(1<<pin_number);
			break;
			default:
			break;			
		}
	return;
}

/*****************************************************************************
* Function		: GPIO_P0Trigger
* Description	: set GPIO as edge or level trigger
* Input			: port_number - GPIO0, GPIO1, GPIO2, GPIO3
							pin_number	- GPIO_PIN0, 1, 2, ...,15 
							is					-	0: edge sensitive
														1: event sensitive
							ibs					-	0: edge trigger
														1: both edge trigger
							iev					-	0:  Rising edges or HIGH level trigger
														1:  Falling edges or LOW level trigger						
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void GPIO_P0Trigger(uint32_t pin_number, uint32_t is, uint32_t ibs,uint32_t iev)
{
	uint32_t	wGpiovalue=0;	
	
	wGpiovalue=SN_GPIO0->IS;	
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(is<<pin_number);
	SN_GPIO0->IS=wGpiovalue;			
	wGpiovalue=SN_GPIO0->IBS;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(ibs<<pin_number);			
	SN_GPIO0->IBS=wGpiovalue;			
	wGpiovalue=SN_GPIO0->IEV;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(iev<<pin_number);
	SN_GPIO0->IEV=wGpiovalue;
}

/*****************************************************************************
* Function		: GPIO_P1Trigger
* Description	: set GPIO as edge or level trigger
* Input			: port_number - GPIO0, GPIO1, GPIO2, GPIO3
							pin_number	- GPIO_PIN0, 1, 2, ...,15 
							is					-	0: edge sensitive
														1: event sensitive
							ibs					-	0: edge trigger
														1: both edge trigger
							iev					-	0:  Rising edges or HIGH level trigger
														1:  Falling edges or LOW level trigger						
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void GPIO_P1Trigger(uint32_t pin_number, uint32_t is, uint32_t ibs,uint32_t iev)
{
	uint32_t	wGpiovalue=0;	

	wGpiovalue=SN_GPIO1->IS;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(is<<pin_number);
	SN_GPIO1->IS=wGpiovalue;			
	wGpiovalue=SN_GPIO1->IBS;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(ibs<<pin_number);			
	SN_GPIO1->IBS=wGpiovalue;			
	wGpiovalue=SN_GPIO1->IEV;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(iev<<pin_number);
	SN_GPIO1->IEV=wGpiovalue;
}

/*****************************************************************************
* Function		: GPIO_P2Trigger
* Description	: set GPIO as edge or level trigger
* Input			: port_number - GPIO0, GPIO1, GPIO2, GPIO3
							pin_number	- GPIO_PIN0, 1, 2, ...,15 
							is					-	0: edge sensitive
														1: event sensitive
							ibs					-	0: edge trigger
														1: both edge trigger
							iev					-	0:  Rising edges or HIGH level trigger
														1:  Falling edges or LOW level trigger				
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void GPIO_P2Trigger(uint32_t pin_number, uint32_t is, uint32_t ibs,uint32_t iev)
{
	uint32_t	wGpiovalue=0;	

	wGpiovalue=SN_GPIO2->IS;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(is<<pin_number);
	SN_GPIO2->IS=wGpiovalue;			
	wGpiovalue=SN_GPIO2->IBS;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(ibs<<pin_number);			
	SN_GPIO2->IBS=wGpiovalue;			
	wGpiovalue=SN_GPIO2->IEV;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(iev<<pin_number);
	SN_GPIO2->IEV=wGpiovalue;
}

/*****************************************************************************
* Function		: GPIO_P3Trigger
* Description	: set GPIO as edge or level trigger
* Input			: port_number - GPIO0, GPIO1, GPIO2, GPIO3
							pin_number	- GPIO_PIN0, 1, 2, ...,15 
							is					-	0: edge sensitive
														1: event sensitive
							ibs					-	0: edge trigger
														1: both edge trigger
							iev					-	0:  Rising edges or HIGH level trigger
														1:  Falling edges or LOW level trigger				
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void GPIO_P3Trigger(uint32_t pin_number, uint32_t is, uint32_t ibs,uint32_t iev)
{
	uint32_t	wGpiovalue=0;	

	wGpiovalue=SN_GPIO3->IS;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(is<<pin_number);
	SN_GPIO3->IS=wGpiovalue;			
	wGpiovalue=SN_GPIO3->IBS;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(ibs<<pin_number);			
	SN_GPIO3->IBS=wGpiovalue;			
	wGpiovalue=SN_GPIO3->IEV;
	wGpiovalue&=~(1<<pin_number);
	wGpiovalue|=(iev<<pin_number);
	SN_GPIO3->IEV=wGpiovalue;		
}
/*****************************************************************************
* Function		: GPIO_int_clr
* Description	: set clear interrupt
* Input			: port_number - GPIO0, GPIO1, GPIO2, GPIO3
							pin_number	- GPIO_PIN0, 1, 2, ...,11 	
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void GPIO_IntClr(uint32_t port_number, uint32_t pin_number)
{
	switch(port_number){
		case 0:
			SN_GPIO0->IC=(1<<pin_number);
			break;
		case 1:
			SN_GPIO1->IC=(1<<pin_number);
			break;
		case 2:
			SN_GPIO2->IC=(1<<pin_number);
			break;
		case 3:
			SN_GPIO3->IC=(1<<pin_number);
			break;
		default:
			break;		
	}	
}

/*****************************************************************************
* Function		: GPIO_Config
* Description	: set GPIO as pull-up, pull-down, inactive or repeater
* Input			: port_number - GPIO0, GPIO1, GPIO2, GPIO3
							pin_number	- GPIO_PIN0, 1, 2, ...,11 
							value				-	0: Pull-up enable 
														1: Pull-down enable
														2: Inactive
														3: Repeate mode
* Output		: None
* Return		: None
* Note			: None
*****************************************************************************/
void GPIO_Config(uint32_t port_number, uint32_t pin_number, uint32_t value)
{
	uint32_t wTemp=0;

	wTemp=pin_number*2;

	switch(port_number){
		case 0:
				SN_GPIO0->CFG&=~(3<<wTemp);
				SN_GPIO0->CFG|=(value<<wTemp);
				break;
		case 1:
				SN_GPIO1->CFG&=~(3<<wTemp);
				SN_GPIO1->CFG|=(value<<wTemp);		
				break;
		case 2:
				SN_GPIO2->CFG&=~(3<<wTemp);
				SN_GPIO2->CFG|=(value<<wTemp);		
				break;
		case 3:
				SN_GPIO3->CFG&=~(3<<wTemp);
				SN_GPIO3->CFG|=(value<<wTemp);	
				break;
		default:
				break;
	}
	return;
}

/*****************************************************************************
* Function		: GPIO_IntStatus
* Description	: Check GPIO interrupt status
* Input			: port_number - GPIO0, GPIO1, GPIO2, GPIO3
							pin_number	- GPIO_PIN0, 1, 2, ...,11 
* Output		: None
* Return		: 0 or 1
* Note			: None
*****************************************************************************/
uint32_t GPIO_IntStatus(uint32_t port_number, uint32_t pin_number)
{
	uint32_t wreturn_value=0;
	switch(port_number)
	{
		case 0:
			wreturn_value=(SN_GPIO0->RIS >>pin_number);
		break;
		case 1:
			wreturn_value=(SN_GPIO1->RIS >>pin_number);
		break;
		case 2:
			wreturn_value=(SN_GPIO2->RIS >>pin_number);
		break;
		case 3:
			wreturn_value=(SN_GPIO3->RIS >>pin_number);
		break;
		default:
		break;
	}
	wreturn_value&=0x01;
	return wreturn_value;
}

