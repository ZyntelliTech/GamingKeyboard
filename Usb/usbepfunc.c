/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 * Name:    usbepfunc.c
 * Purpose: USB Custom User Module
 * Version: V1.00
 * Date:		2017/07
 *------------------------------------------------------------------------------*/
#include	<SN32F240B.h>
#include	"..\type.h"
#include	"..\Utility\Utility.h"

#include	"usbhw.h"
#include	"usbram.h"
#include	"usbdesc.h"
#include	"usbuser.h"
#include	"usbepfunc.h"

#include	"..\UsbHid\hid.h"
#include	"..\UsbHid\hiduser.h"


/*****************************************************************************
* Function		: USB_EPnINFunction
* Description	: SET EP1~EP4 IN token RAM
* Input				: 1. wEPNum: EP1~EP4
*								2. pData: transaction Data buffer
*								3. wBytecnt: Byte conuter Number of transaction
* Output			: None
* Return			: by EPn_RETURN_VALUE
* Note				: None
*****************************************************************************/
uint32_t	USB_EPnINFunction(uint32_t	wEPNum, uint32_t *pData, uint32_t	wBytecnt)
{
	volatile	uint32_t	*pUsbReg;
	uint32_t	i;
	
	pUsbReg = (&SN_USB->EP0CTL) + wEPNum;
	
	if (!((*pUsbReg) & mskEPn_ENDP_EN))
			return	EPn_RETURN_DISABLE;
	
	// Only support EP1~EP4 and Byte counter < 64
	if ((wEPNum == USB_EP0) || (wEPNum > USB_EP4) || wBytecnt > 64)
		return EPn_RETURN_NOT_SUPPORT;			
	
	if ((*pUsbReg & (mskEPn_ENDP_EN|mskEPn_ENDP_STATE)) == (mskEPn_ENDP_EN|mskEPn_ENDP_STATE_NAK))
	{
		if (wUSB_EndpHalt[wEPNum] == USB_EPn_NON_HALT)
		{
			pUsbReg = &wUSB_EPnPacketsize[wEPNum];
			if (wBytecnt > *pUsbReg)
				return EPn_RETURN_OVER_MAX_SIZE;		//wBytecnt > EPn packet size

			//set EPn FIFO offset value to i
			i = wUSB_EPnOffset[wEPNum-1];
		
			fnUSBMAIN_WriteFIFO( i , *pData);
			fnUSBMAIN_WriteFIFO( i+4 , *(pData + 1));	
			
			USB_EPnAck(wEPNum,wBytecnt);	// ACK hwByteCnt byte
			return EPn_RETURN_ACK_OK;
		}
		else//Halt = 1, return STALL
		{
			USB_EPnStall(wEPNum);					// EPn STALL
			return EPn_RETURN_STALL;
		}
	}
	else
	{
		return EPn_RETURN_ACK_BUSY;
	}
}


