/******************** (C) COPYRIGHT 2018 SONiX *******************************
* COMPANY: Freelancer
* DATE:		 2022/01/25
* AUTHOR:	 Ming
* IC:			 SN32F240B
*____________________________________________________________________________
* The Firmware implement the keyboard with RGB led.
* Scan rate: 1ms
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
#include "CT16.h"
#include "CT16B1.h"
//Button Status
#define CLICKED	 1
#define RELEASED 0
// PWM Constant for LED driver
#define PWM_STEP	10     // PWM resolution
#define PWM_CYCLE	12000  //the cycle register value, which is corresonded to 1ms
#define MAX_STEPS 1200	 // this is MAXIMUM step size, this value = PWM_CYCLE / PWM_STEP
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
}keyboardHID; // keyboard report structure

keyboardHID keyboardhid = {0,0,0,0,0,0,0,0};

uint32_t keyboardhid_report[2]; // 32bit array variable to send the report to PC

/* Keycode stack array */
uint8_t key_stack[6] = {0,0,0,0,0,0};

/* Key variables */
uint8_t key_pressed = 0; //set when key pressed
uint8_t key_released = 0;//set when key released
uint8_t mod_flag = 0; //set if the special key is pressed, SHIFT, CTRL, ALT, and WIN
uint8_t error_flag = 0; // set if error occured
uint8_t i =  0; // index variable

/* LED PWM variables */
uint16_t red_duty = 0;  // PWM Duty for RED led
uint16_t blue_duty = 0; // PWM Duty for BLUE led
uint16_t green_duty = 0;// PWM Duty for GREEN led
uint8_t counting_flag = 0; /* 0: Timer 1 is set as upcounting, 1: Timer 1 is set as downcounting */

//key matrix columns
#define CL_NUM 21
uint8_t ckeyMapper[CL_NUM][2] = {
	{GPIO_PORT3, GPIO_PIN4},		//CL0
	{GPIO_PORT3, GPIO_PIN3}, 		//CL1
	{GPIO_PORT1, GPIO_PIN15},		//CL2
	{GPIO_PORT1, GPIO_PIN14},		//CL3
	{GPIO_PORT1, GPIO_PIN13},		//CL4
	{GPIO_PORT1, GPIO_PIN6},		//CL5
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
	{GPIO_PORT3, GPIO_PIN6},		//CL6
	{GPIO_PORT3, GPIO_PIN5},		//CL7
};
//key matrix rows
uint8_t rKeyMapper[6][2] = {
	{GPIO_PORT2, GPIO_PIN15},		//R0
	{GPIO_PORT3, GPIO_PIN11}, 	//R1
	{GPIO_PORT3, GPIO_PIN10},		//R2
	{GPIO_PORT3, GPIO_PIN9},		//R3
	{GPIO_PORT3, GPIO_PIN8},		//R4
	{GPIO_PORT3, GPIO_PIN7},		//R5
};

//key layout
uint8_t fr_key_layout[21][6] = {
	{KEY_ESC, FR_SUP2, KEY_TAB, KEY_CAPSLOCK, KEY_MOD_LSHIFT, KEY_MOD_LCTRL}, //CL0
	{KEY_NONE, KEY_1, KEY_Q, KEY_A, FR_LABK, KEY_MOD_LMETA}, //CL1
	{KEY_F1, KEY_2, KEY_W, KEY_S, KEY_Z, KEY_MOD_LALT}, //CL2
	{KEY_F2, KEY_3, KEY_E, KEY_D, KEY_X, KEY_NONE}, //CL3
	{KEY_F3, KEY_4, KEY_R, KEY_F, KEY_C, KEY_NONE}, //CL4
	{KEY_F4, KEY_5, KEY_T, KEY_G, KEY_V, KEY_NONE}, //CL5
	{KEY_F7, KEY_8, KEY_I, KEY_K, KEY_M, KEY_NONE}, //CL8
	{KEY_F8, KEY_9, KEY_O, KEY_L, KEY_COMMA, KEY_NONE}, //CL9
	{KEY_F9, KEY_0, KEY_P, KEY_SEMICOLON, KEY_DOT, KEY_MOD_RALT}, //CL10
	{KEY_F10, KEY_MINUS, KEY_LEFTBRACE, KEY_APOSTROPHE, KEY_SLASH, KEY_MOD_RMETA}, //CL11
	{KEY_F11, KEY_EQUAL, KEY_RIGHTBRACE, KEY_APOSTROPHE, KEY_NONE, KEY_COMPOSE}, //CL12
	{KEY_F12, KEY_BACKSPACE, KEY_NONE, KEY_ENTER, KEY_MOD_RSHIFT, KEY_MOD_RCTRL},//CL13
	{KEY_SYSRQ, KEY_INSERT, KEY_DELETE, KEY_NONE, KEY_NONE, KEY_LEFT}, //CL14
	{KEY_SCROLLLOCK, KEY_HOME, KEY_END, KEY_NONE, KEY_UP, KEY_DOWN}, //CL15
	{KEY_PAUSE, KEY_PAGEUP, KEY_PAGEDOWN, KEY_NONE, KEY_NONE, KEY_RIGHT}, //CL16
	{KEY_NONE, KEY_NUMLOCK, KEY_KP7, KEY_KP4, KEY_KP1, KEY_NONE}, //CL17
	{KEY_NONE, KEY_KPSLASH, KEY_KP8, KEY_KP5, KEY_KP2, KEY_KP0}, //CL18
	{KEY_NONE, KEY_KPASTERISK, KEY_KP9, KEY_KP6, KEY_KP3, KEY_KPDOT}, //CL19
	{KEY_NONE, KEY_KPMINUS, KEY_KPPLUS, KEY_NONE, KEY_KPENTER, KEY_NONE}, //CL20
	{KEY_F5, KEY_6, KEY_Y, KEY_H, KEY_B, KEY_SPACE}, //CL6
	{KEY_F6, KEY_7, KEY_U, KEY_J, KEY_N, KEY_NONE} //CL7	
};

/*_____ F U N C T I O N S __________________________________________________*/
void SysTick_Init(void);
void NDT_Init(void);

void RGB_Line0_Driver(uint8_t val) //bit 0: Red, bit1: Blue, bit2: Green, if bit is set, turn on, else, turn off
{
	//SN_GPIO2->DATA = SN_GPIO2->DATA & 0xFFF4; //line 0
	if(val & 0x01) GPIO_Set(GPIO_PORT2, GPIO_PIN0); // turn on the RED
	if(val & 0x02) GPIO_Set(GPIO_PORT2, GPIO_PIN1); // turn on the BLUE
	if(val & 0x04) GPIO_Set(GPIO_PORT2, GPIO_PIN3); // Turn on the GREEN	
}
void RGB_Line1_Driver(uint8_t val) //bit 0: Red, bit1: Blue, bit2: Green, if bit is set, turn on, else, turn off
{
	//SN_GPIO2->DATA = SN_GPIO2->DATA & 0xFF8F; //line 1
	if(val & 0x01) GPIO_Set(GPIO_PORT2, GPIO_PIN4); // turn on the RED
	if(val & 0x02) GPIO_Set(GPIO_PORT2, GPIO_PIN5); // turn on the BLUE
	if(val & 0x04) GPIO_Set(GPIO_PORT2, GPIO_PIN6); // Turn on the GREEN
}
void RGB_Line2_Driver(uint8_t val) //bit 0: Red, bit1: Blue, bit2: Green, if bit is set, turn on, else, turn off
{
	//SN_GPIO2->DATA = SN_GPIO2->DATA & 0xFC7F; //line 2
	if(val & 0x01) GPIO_Set(GPIO_PORT2, GPIO_PIN7); // turn on the RED
	if(val & 0x02) GPIO_Set(GPIO_PORT2, GPIO_PIN8); // turn on the BLUE
	if(val & 0x04) GPIO_Set(GPIO_PORT2, GPIO_PIN9); // Turn on the GREEN
}
void RGB_Line3_Driver(uint8_t val) //bit 0: Red, bit1: Blue, bit2: Green, if bit is set, turn on, else, turn off
{
	//SN_GPIO2->DATA = SN_GPIO2->DATA & 0xE3FF; //line 3
	if(val & 0x01) GPIO_Set(GPIO_PORT2, GPIO_PIN10); // turn on the RED
	if(val & 0x02) GPIO_Set(GPIO_PORT2, GPIO_PIN11); // turn on the BLUE
	if(val & 0x04) GPIO_Set(GPIO_PORT2, GPIO_PIN12); // Turn on the GREEN
}
void RGB_Line4_Driver(uint8_t val) //bit 0: Red, bit1: Blue, bit2: Green, if bit is set, turn on, else, turn off
{
	//SN_GPIO1->DATA = SN_GPIO1->DATA & 0xFC7F; //line 4
	if(val & 0x01) GPIO_Set(GPIO_PORT1, GPIO_PIN7); // turn on the RED
	if(val & 0x02) GPIO_Set(GPIO_PORT1, GPIO_PIN8); // turn on the BLUE
	if(val & 0x04) GPIO_Set(GPIO_PORT1, GPIO_PIN9); // Turn on the GREEN
}
void RGB_Line5_Driver(uint8_t val) //bit 0: Red, bit1: Blue, bit2: Green, if bit is set, turn on, else, turn off
{
	//SN_GPIO1->DATA = SN_GPIO1->DATA & 0xE3FF; //line 5
	if(val & 0x01) GPIO_Set(GPIO_PORT1, GPIO_PIN10); // turn on the RED
	if(val & 0x02) GPIO_Set(GPIO_PORT1, GPIO_PIN11); // turn on the BLUE
	if(val & 0x04) GPIO_Set(GPIO_PORT1, GPIO_PIN12); // Turn on the GREEN
}
void led_all_off()
{
	SN_GPIO2->DATA = SN_GPIO2->DATA & 0xFFF4; //line 0
	SN_GPIO2->DATA = SN_GPIO2->DATA & 0xFF8F; //line 1
	SN_GPIO2->DATA = SN_GPIO2->DATA & 0xFC7F; //line 2
	SN_GPIO2->DATA = SN_GPIO2->DATA & 0xE3FF; //line 3
	SN_GPIO1->DATA = SN_GPIO1->DATA & 0xFC7F; //line 4
	SN_GPIO1->DATA = SN_GPIO1->DATA & 0xE3FF; //line 5
}
// set CL0-CL20 as LOW
void cl_all_on(void)
{
	SN_GPIO0->DATA = SN_GPIO0->DATA & 0x80FF;
	SN_GPIO1->DATA = SN_GPIO1->DATA & 0x1F80;
	SN_GPIO3->DATA = SN_GPIO3->DATA & 0xFF87;
}

void set_pwm_red(uint16_t val) // set the pwm duty for LED color
{
	SN_CT16B1->MR8 = PWM_STEP * val; //LR0
	SN_CT16B1->MR12 = PWM_STEP * val;//LR1
	SN_CT16B1->MR15 = PWM_STEP * val;//LR2
	SN_CT16B1->MR18 = PWM_STEP * val;//LR3
	SN_CT16B1->MR23 = PWM_STEP * val;//LR4
	SN_CT16B1->MR2 = PWM_STEP * val; //LR5	
}
void set_pwm_blue(uint16_t val){ // set the pwm duty for BLUE color
	SN_CT16B1->MR9 = PWM_STEP * val; //LB0
	SN_CT16B1->MR13 = PWM_STEP * val;//LB1
	SN_CT16B1->MR16 = PWM_STEP * val;//LB2
	SN_CT16B1->MR19 = PWM_STEP * val;//LB3
	SN_CT16B1->MR0 = PWM_STEP * val; //LB4
	SN_CT16B1->MR3 = PWM_STEP * val; //LB5
}
void set_pwm_green(uint16_t val){ // set the pwm duty for GREEN color
	SN_CT16B1->MR11 = PWM_STEP * val;//LG0
	SN_CT16B1->MR14 = PWM_STEP * val;//LG1
	SN_CT16B1->MR17 = PWM_STEP * val;//LG2
	SN_CT16B1->MR20 = PWM_STEP * val;//LG3
	SN_CT16B1->MR1 = PWM_STEP * val; //LG4
	SN_CT16B1->MR4 = PWM_STEP * val; //LG5
}
void MN_CtDemoCase12(void) //Configured the Timer 1(PWM timer)
{
	CT16B1_Init(); //Timer enable
	//Set MR10 value for 1ms PWM period ==> count value = 1000*12 = 12000
	SN_CT16B1->MR10 = PWM_CYCLE;
	
	//Set MR1 value for 30% duty ==> count value = 12000 - (30%*12000) = 8400
	SN_CT16B1->MR8 = 0; //LR0
	SN_CT16B1->MR9 = 0; //LB0
	SN_CT16B1->MR11 = 0;//LG0
	SN_CT16B1->MR12 = 0;//LR1
	SN_CT16B1->MR13 = 0;//LB1
	SN_CT16B1->MR14 = 0;//LG1
	SN_CT16B1->MR15 = 0;//LR2
	SN_CT16B1->MR16 = 0;//LB2
	SN_CT16B1->MR17 = 0;//LG2
	SN_CT16B1->MR18 = 0;//LR3
	SN_CT16B1->MR19 = 0;//LB3
	SN_CT16B1->MR20 = 0;//LG3
	SN_CT16B1->MR23 = 0;//LR4
	SN_CT16B1->MR0 = 0; //LB4	
	SN_CT16B1->MR1 = 0; //LG4
	SN_CT16B1->MR2 = 0; //LR5
	SN_CT16B1->MR3 = 0; //LB5
	SN_CT16B1->MR4 = 0; //LG5
	
	//Enable PWM function, IOs and select the PWM modes
	SN_CT16B1->PWMENB = (mskCT16_PWM0EN_EN | mskCT16_PWM1EN_EN | mskCT16_PWM2EN_EN | mskCT16_PWM3EN_EN | mskCT16_PWM4EN_EN | mskCT16_PWM8EN_EN | mskCT16_PWM9EN_EN
											| mskCT16_PWM11EN_EN | mskCT16_PWM12EN_EN | mskCT16_PWM13EN_EN | mskCT16_PWM14EN_EN | mskCT16_PWM15EN_EN | mskCT16_PWM16EN_EN | mskCT16_PWM17EN_EN | mskCT16_PWM18EN_EN
											| mskCT16_PWM19EN_EN | mskCT16_PWM20EN_EN | mskCT16_PWM23EN_EN);//Enable PWM0/1/2 function
	
	SN_CT16B1->PWMIOENB = (mskCT16_PWM0IOEN_EN | mskCT16_PWM1IOEN_EN | mskCT16_PWM2IOEN_EN | mskCT16_PWM3IOEN_EN | mskCT16_PWM4IOEN_EN | mskCT16_PWM8IOEN_EN | 
												mskCT16_PWM9IOEN_EN | mskCT16_PWM11IOEN_EN | mskCT16_PWM12IOEN_EN | mskCT16_PWM13IOEN_EN | mskCT16_PWM14IOEN_EN | mskCT16_PWM15IOEN_EN |
												mskCT16_PWM16IOEN_EN | mskCT16_PWM17IOEN_EN | mskCT16_PWM18IOEN_EN | mskCT16_PWM19IOEN_EN | mskCT16_PWM20IOEN_EN | mskCT16_PWM23IOEN_EN);//Enable PWM0/1/2 IO
	
	SN_CT16B1->PWMCTRL = (mskCT16_PWM0MODE_2 | mskCT16_PWM1MODE_2 | mskCT16_PWM2MODE_2 | mskCT16_PWM3MODE_2 | mskCT16_PWM4MODE_2 | mskCT16_PWM8MODE_2 | mskCT16_PWM9MODE_2 | 
												mskCT16_PWM11MODE_2 | mskCT16_PWM12MODE_2 | mskCT16_PWM13MODE_2 | mskCT16_PWM14MODE_2 | mskCT16_PWM15MODE_2);	//PWM0/1/2  select as PWM mode 2
	SN_CT16B1->PWMCTRL2 = (mskCT16_PWM16MODE_2 | mskCT16_PWM17MODE_2 | mskCT16_PWM18MODE_2 | mskCT16_PWM19MODE_2 | mskCT16_PWM20MODE_2 | mskCT16_PWM23MODE_2);
  SN_PFPA->CT16B1 = 0x001FFB1F;
	
	//Set MR23 match interrupt and TC rest 
	SN_CT16B1->MCTRL2 = (mskCT16_MR10RST_EN);						

	//Set CT16B0 as the up-counting mode.
	SN_CT16B1->TMRCTRL = (mskCT16_CRST);

	//Wait until timer reset done.
	while (SN_CT16B1->TMRCTRL & mskCT16_CRST);			
	
	//Let TC start counting.
	SN_CT16B1->TMRCTRL |= mskCT16_CEN_EN;						
	
	//Enable CT16B0's NVIC interrupt.
	CT16B1_NvicDisable();
}
// add the pressed/released keycode to stack
void push_key(uint8_t keycode)
{
	key_stack[5] = key_stack[4];
	key_stack[4] = key_stack[3];
	key_stack[3] = key_stack[2];
	key_stack[2] = key_stack[1];
	key_stack[1] = key_stack[0];
	key_stack[0] = keycode;
}
//Send the pressed keycode to PC
 uint32_t SendKeyCode(keyboardHID key_hid, uint32_t delay_ms)
{
	uint32_t ret;
	keyboardhid_report[0] = (key_hid.KEYCODE2<<24) | (key_hid.KEYCODE1<<16) | (key_hid.RESERVED << 8) | key_hid.MODIFIER;
	keyboardhid_report[1] = (key_hid.KEYCODE6<<24) | (key_hid.KEYCODE5<<16) | (key_hid.KEYCODE4<<8) | (key_hid.KEYCODE3);
	ret = USB_EPnINFunction(USB_EP1, keyboardhid_report, 8);
	if(delay_ms > 0) UT_MAIN_DelayNms(delay_ms);
	return ret;
}
// send the key release to PC
uint32_t releaseKeyCode(uint32_t delay_ms)
{
	uint32_t ret;
	memset(&keyboardhid, 0x00, 8);
	keyboardhid_report[0] = 0;
	keyboardhid_report[1] = 0;
	ret = USB_EPnINFunction(USB_EP1, keyboardhid_report, 8);
	if(delay_ms > 0) UT_MAIN_DelayNms(delay_ms);
	return ret;
}
// init the GPIO ports
void GPIO_Configuration(void)
{
	// Input/Output mode
	SN_GPIO0->MODE = 0xFFFFFFFF;
	SN_GPIO1->MODE = 0xFFFFFFFF;
	SN_GPIO2->MODE = 0xFFF7FFF; //P2.15 is input
	SN_GPIO3->MODE = 0x0000F078; //P3.7~P3.11 is input.
	
	// Data reset
	SN_GPIO0->DATA = 0x00000000;
	SN_GPIO1->DATA = 0x00000000;
	SN_GPIO2->DATA = 0x00000000;
	SN_GPIO3->DATA = 0x00000000;
}
// Reset ALL GPIO data register
void resetAllGpio()
{
	SN_GPIO0->DATA = 0x00000000;
	SN_GPIO1->DATA = 0x00000000;
	SN_GPIO2->DATA &= 0x00000004;
	SN_GPIO3->DATA = 0x00000000;
}
//Led driver function
void led_blink()
{
	cl_all_on();
	RGB_Line0_Driver(0x04);
	RGB_Line1_Driver(0x02);
	RGB_Line2_Driver(0x01);
	RGB_Line3_Driver(0x04);
	RGB_Line4_Driver(0x02);
	RGB_Line5_Driver(0x01);
}
// scan the keygen
void readKeyboard(void)
{
	key_pressed = 0;
	i = 0;
	while(i < CL_NUM)
	{
		resetAllGpio();
		GPIO_Set(ckeyMapper[i][0], ckeyMapper[i][1]);
		UT_MAIN_DelayNx10us(1);//10us
		// scan the keyboard
			if(SN_GPIO2->DATA_b.DATA15 == CLICKED){ //R0
				key_pressed = 0x01;
				push_key(fr_key_layout[i][0]);
				break;
			}
			if(SN_GPIO3->DATA_b.DATA11 == CLICKED){ //R1
				push_key(fr_key_layout[i][1]);
				key_pressed = 0x01;
				break;
			}
			if(SN_GPIO3->DATA_b.DATA10 == CLICKED){ //R2
				push_key(fr_key_layout[i][2]); 
				key_pressed = 0x01;
				break;
			}
			if(SN_GPIO3->DATA_b.DATA9 == CLICKED){ //R3	
				push_key(fr_key_layout[i][3]);
				key_pressed = 0x01;
				break;
			}
			if(SN_GPIO3->DATA_b.DATA8 == CLICKED){ //R4
				if((fr_key_layout[i][4] == KEY_MOD_LSHIFT) || (fr_key_layout[i][4] == KEY_MOD_RSHIFT))
				{
					keyboardhid.MODIFIER |= fr_key_layout[i][4];
				}
				else{
					key_pressed = 0x01;
					push_key(fr_key_layout[i][4]);
					break;
				}		
			}					
			if(SN_GPIO3->DATA_b.DATA7 == CLICKED){ //R5
				if((fr_key_layout[i][5] == KEY_MOD_LCTRL) || 
				 (fr_key_layout[i][5] == KEY_MOD_RCTRL) || 
			   (fr_key_layout[i][5] == KEY_MOD_LALT) || 
			   (fr_key_layout[i][5] == KEY_MOD_RALT) || 
				 (fr_key_layout[i][5] == KEY_MOD_LMETA) ||
				 (fr_key_layout[i][5] == KEY_MOD_RMETA))
				{
					 keyboardhid.MODIFIER |= fr_key_layout[i][5];
				}
				else{
					push_key(fr_key_layout[i][5]);
					key_pressed = 0x01;
					break;
				}
			}
		//end
		i++;
	}
	if(key_pressed == 0) {
		key_released = 1; 
		push_key(KEY_NONE);
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
	
	SystemInit(); // Init the system register
	SystemCoreClockUpdate(); //enable the peripheral clocks

	//1. User SHALL define PKG on demand.
	//2. User SHALL set the status of the GPIO which are NOT pin-out to input pull-up.
	NotPinOut_GPIO_init();
	
	NDT_Init();								/* NDT Initialization */
	
	USB_Init();								/* USB Initialization */
	
	GPIO_Configuration();     /* GPIO Initaialization */
	
	SysTick_Init();						/* init SysTick, make the 1ms timer */

	memset(key_stack, 0x00, 6); /* init key stack */
	
	MN_CtDemoCase12(); /* Initi the PWM Timer for LED driver*/
	while (1)
	{
		cl_all_on(); // set CL0-CL20 as LOW
		if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) //if 1ms flag is set 
		{
			// clear systick flag
			__SYSTICK_CLEAR_COUNTER_AND_FLAG;

				if(counting_flag == 0){
					if(red_duty <  MAX_STEPS){
						red_duty++;	
						set_pwm_red(red_duty);
					}else{
						if(green_duty < MAX_STEPS){
							green_duty++;
							set_pwm_green(green_duty);
						}else{
							if(blue_duty < MAX_STEPS){
								blue_duty++;
								set_pwm_blue(blue_duty);
							}else{
								counting_flag = 1;
							}
						}
					}
				}
				else{
					if(red_duty > 0){
						red_duty--;	
						set_pwm_red(red_duty);
					}else{
						if(green_duty > 0){
							green_duty--;
							set_pwm_green(green_duty);
						}else{
							if(blue_duty > 100){
								blue_duty--;
								set_pwm_blue(blue_duty);
							}else{
								blue_duty = 0;
								set_pwm_blue(0);
								red_duty = 100;
								counting_flag = 0;
							}
						}
					}
				}
			//}
			//	led_all_off();
				readKeyboard();
				if(key_pressed == 1){				
					keyboardhid.KEYCODE1 = key_stack[0];
					if( SendKeyCode(keyboardhid, 0) != 0){
						error_flag = 1;
					}
				}
				if(key_released == 1){
					releaseKeyCode(0);
					key_released = 0;
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



