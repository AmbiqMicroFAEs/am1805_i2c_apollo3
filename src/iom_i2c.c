//*****************************************************************************
// SIMPLIFIED IOM I2C DRIVER,
// Apollo3 Blue
// Mariusz Lacina, Ambiq, 2023 
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "am_mcu_apollo.h"
#include "am_bsp.h"
#include "am_util.h"
#include "iom_dma.h"
#include "iom_i2c.h"
#include "uart_debug.h"
#include "amx805_device.h"

//*****************************************************************************
//  IOM MODULE USED
//*****************************************************************************
#define     IOM_MODULE              1

//*****************************************************************************
//  IOM callback context, 
//  free to use or type redefine, 
//  will be casted on *void
//*****************************************************************************
uint8_t  i2c_wr_callback_cntx = 0xaa;
uint8_t  i2c_rd_callback_cntx = 0xbb;

//*****************************************************************************
//  IOM dma handle and flags
//*****************************************************************************
void *g_IOMHandle;
volatile bool i2c_cmd_completed = false;
volatile bool i2c_cmd_error = false;
volatile bool i2c_dma_completed = false;
volatile bool i2c_cmd_no_ack_error = false;

//*****************************************************************************
//  IOM dma config structure
//*****************************************************************************
static iom_dma_config_t g_sIOMI2cConfig =
{
  .eInterfaceMode = AM_HAL_IOM_I2C_MODE,  
  .ui32ClockFreq  = AM_HAL_IOM_400KHZ,  
};


//*****************************************************************************
//  IOM GPIO_RTC_SDA config structure
//*****************************************************************************
const am_hal_gpio_pincfg_t g_GPIO_RTC_SDA =
{
    .uFuncSel            = AM_HAL_PIN_9_M1SDAWIR3,
    .eDriveStrength      = AM_HAL_GPIO_PIN_PULLUP_WEAK, 
		.eGPOutcfg           = AM_HAL_GPIO_PIN_OUTCFG_OPENDRAIN,
		.eGPInput						 = AM_HAL_GPIO_PIN_INPUT_NONE,
    .uIOMnum             = 1
};


//*****************************************************************************
//  IOM GPIO_RTC_SCL config structure
//*****************************************************************************
const am_hal_gpio_pincfg_t g_GPIO_RTC_SCL =
{
    .uFuncSel            = AM_HAL_PIN_8_M1SCL,
		.ePullup 						 = AM_HAL_GPIO_PIN_PULLUP_WEAK, 	
		.eGPOutcfg           = AM_HAL_GPIO_PIN_OUTCFG_OPENDRAIN,
		.eGPInput						 = AM_HAL_GPIO_PIN_INPUT_NONE,
    .uIOMnum             = 1
};


//*****************************************************************************
//  IOM GPIO_RTC_FOUT config structure
//*****************************************************************************
const am_hal_gpio_pincfg_t g_GPIO_RTC_FOUT =
{
    .uFuncSel            = AM_HAL_PIN_31_GPIO,  
    .eGPOutcfg           = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
		.eGPInput						 = AM_HAL_GPIO_PIN_INPUT_ENABLE,
		.ePullup 						 = AM_HAL_GPIO_PIN_PULLUP_WEAK, //AM_HAL_GPIO_PIN_PULLUP_WEAK, 					
};

//*****************************************************************************
//  IOM GPIO_RTC_EXTR config structure
//*****************************************************************************
const am_hal_gpio_pincfg_t g_GPIO_RTC_EXTR =
{
    .uFuncSel            = AM_HAL_PIN_39_GPIO,  
    .eDriveStrength      = AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA,
    .eGPOutcfg           = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL,
		.ePullup 						 = AM_HAL_GPIO_PIN_PULLUP_NONE, 					
};

//*****************************************************************************
//  IOM GPIO_RTC_CLKOUT config structure
//*****************************************************************************
const am_hal_gpio_pincfg_t g_GPIO_RTC_CLKOUT =
{
    .uFuncSel            = AM_HAL_PIN_48_GPIO,  
    .eGPOutcfg           = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
		.eGPInput						 = AM_HAL_GPIO_PIN_INPUT_NONE,
		.ePullup 						 = AM_HAL_GPIO_PIN_PULLUP_WEAK, //AM_HAL_GPIO_PIN_PULLUP_WEAK, //AM_HAL_GPIO_PIN_PULLDOWN, 				
};

//*****************************************************************************
//  IOM GPIO_RTC_EXTI config structure
//*****************************************************************************
const am_hal_gpio_pincfg_t g_GPIO_RTC_EXTI =
{
    .uFuncSel            = AM_HAL_PIN_40_GPIO,  
    .eDriveStrength      = AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA,
    .eGPOutcfg           = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL,
		.ePullup 						 = AM_HAL_GPIO_PIN_PULLUP_NONE, 					
};

//*****************************************************************************
//  IOM GPIO_RTC_PSW config structure
//*****************************************************************************
const am_hal_gpio_pincfg_t g_GPIO_RTC_PSW =
{
    .uFuncSel            = AM_HAL_PIN_13_GPIO,  
    .eGPOutcfg           = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
		.eGPInput						 = AM_HAL_GPIO_PIN_INPUT_ENABLE,
		.ePullup 						 = AM_HAL_GPIO_PIN_PULLUP_WEAK, 		
};

//*****************************************************************************
//  IOM GPIO_RTC_WDI config structure
//*****************************************************************************
const am_hal_gpio_pincfg_t g_GPIO_RTC_WDI =
{
    .uFuncSel            = AM_HAL_PIN_35_GPIO,  
	  .eDriveStrength      = AM_HAL_GPIO_PIN_DRIVESTRENGTH_2MA,
    .eGPOutcfg           = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL,
		.ePullup 						 = AM_HAL_GPIO_PIN_PULLUP_NONE, 					
};

//*****************************************************************************
//  IOM GPIO_RTC_RST config structure
//*****************************************************************************
const am_hal_gpio_pincfg_t g_GPIO_RTC_RST =
{
    .uFuncSel            = AM_HAL_PIN_44_GPIO,  
    .eGPOutcfg           = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
		.eGPInput						 = AM_HAL_GPIO_PIN_INPUT_ENABLE,
		.ePullup 						 = AM_HAL_GPIO_PIN_PULLUP_WEAK, 		 					
};

//*****************************************************************************
//  IOM GPIO_RTC_TIRQ config structure
//*****************************************************************************
const am_hal_gpio_pincfg_t g_GPIO_RTC_TIRQ =
{
    .uFuncSel            = AM_HAL_PIN_7_GPIO,  
    .eGPOutcfg           = AM_HAL_GPIO_PIN_OUTCFG_DISABLE,
		.eGPInput						 = AM_HAL_GPIO_PIN_INPUT_ENABLE,
		.ePullup 						 = AM_HAL_GPIO_PIN_PULLUP_WEAK, 		 					
};


//----------------------------------------------------------------------//
//  IOM user callback
//   -> mandatory !!!
//----------------------------------------------------------------------//
void i2c_callback(uint8_t IomModule, void *pCallbackCtxt, uint32_t transactionStatus)
{
uint8_t context;

  context = context;  
  context = *(uint8_t*)pCallbackCtxt;   //callback context
   
  if(transactionStatus)
  {
	
  if(transactionStatus & AM_HAL_IOM_INT_NAK)     //I2C NAK
    {
    i2c_cmd_error = true;
    i2c_cmd_no_ack_error = true;
    }   
		 
  if(transactionStatus & AM_HAL_IOM_INT_CMDCMP)  //Cpmmand complete interrupt
    {
    iom_i2c_dma_pop_tail(IomModule);  //Mandatory for I2C mode!!! pop the last incomplete word from fifo if necessarly
                                      //correct tranmission errors with clcok streching slaves
    i2c_cmd_completed = true;
    }

  if(transactionStatus & AM_HAL_IOM_INT_THR)     //FIFO Threshold interrupt
    {
    }

  if(transactionStatus & AM_HAL_IOM_INT_FUNDFL)  //Read FIFO underflow
    {
    }

  if(transactionStatus & AM_HAL_IOM_INT_FOVFL)   //Write FIFO overflow
    {
    }

  if(transactionStatus & AM_HAL_IOM_INT_IACC)    //Illegal FIFO access
    {
    }

  if(transactionStatus & AM_HAL_IOM_INT_ICMD)    //ILLEGAL command
    {
    }

  if(transactionStatus & AM_HAL_IOM_INT_START)  //Start command
    {
    }

  if(transactionStatus & AM_HAL_IOM_INT_STOP)  //Stop command
    {
    }

  if(transactionStatus & AM_HAL_IOM_INT_DCMP)  //DMA transfer complete
    {
		i2c_dma_completed = true;
    }

  if(transactionStatus & AM_HAL_IOM_INT_DERR)  //DMA error received
    {    
		i2c_cmd_error = true;	
    }

  if(transactionStatus & AM_HAL_IOM_INT_CQPAUSED)  //CQ operation paused
    {
    }

  if(transactionStatus & AM_HAL_IOM_INT_CQUPD)   //CQ performed a register write with the register address bit 0 set to 1.
    {
    }

  if(transactionStatus & AM_HAL_IOM_INT_CQERR)   //CQ error
    {
    } 
  }  
}

//----------------------------------------------------------------------//
//  IOM pins configuration
//----------------------------------------------------------------------//
void iom_gpio_config(void)
{
	am_hal_gpio_pinconfig(GPIO_RTC_SDA, g_GPIO_RTC_SDA);
	am_hal_gpio_pinconfig(GPIO_RTC_SCL, g_GPIO_RTC_SCL);
	am_hal_gpio_pinconfig(GPIO_RTC_TIRQ, g_GPIO_RTC_TIRQ);
	am_hal_gpio_pinconfig(GPIO_RTC_FOUT, g_GPIO_RTC_FOUT);
	am_hal_gpio_pinconfig(GPIO_RTC_EXTR, g_GPIO_RTC_EXTR);
	am_hal_gpio_pinconfig(GPIO_RTC_CLKOUT, g_GPIO_RTC_CLKOUT);
	am_hal_gpio_pinconfig(GPIO_RTC_EXTI, g_GPIO_RTC_EXTI);
	am_hal_gpio_pinconfig(GPIO_RTC_PSW, g_GPIO_RTC_PSW);
	am_hal_gpio_pinconfig(GPIO_RTC_WDI, g_GPIO_RTC_WDI);
	am_hal_gpio_pinconfig(GPIO_RTC_RST, g_GPIO_RTC_RST);
}


//----------------------------------------------------------------------//
//  IOM DMA WRITE
//----------------------------------------------------------------------//
uint32_t  iom_i2c_write(uint8_t slave_address, uint32_t *pbuf, uint32_t size, bool cont, bool block)
{
iom_dma_transfer_t Transaction;
uint32_t status;

	Transaction.uPeerInfo.ui32I2CDevAddr = slave_address;
  Transaction.eDirection      = AM_HAL_IOM_TX;
  Transaction.ui32NumBytes    = size;
  Transaction.pui32Buffer     = pbuf;
  Transaction.bContinue       = cont;
  Transaction.pCallback       = i2c_callback; 
  Transaction.pCallbackCtxt   = &i2c_wr_callback_cntx;
   
  i2c_cmd_completed = false;
	i2c_dma_completed = false;
  i2c_cmd_error = false;
	i2c_cmd_no_ack_error = false;

  status = iom_dma_start(g_IOMHandle,&Transaction);
	if(status != AM_HAL_STATUS_SUCCESS)return status;
	
	if(block)
	{
	while(((i2c_cmd_completed == false) || (i2c_dma_completed == false))&&(i2c_cmd_error == false)){};
  if(i2c_cmd_error)
		{
		return AM_HAL_STATUS_FAIL;				
		}  	
	}
	
return 	AM_HAL_STATUS_SUCCESS;
}

//----------------------------------------------------------------------//
//  IOM DMA READ
//----------------------------------------------------------------------//
uint32_t iom_i2c_read(uint8_t slave_address, uint32_t *pbuf, uint32_t size, bool cont, bool block) 
{
iom_dma_transfer_t Transaction;
uint32_t status;

	Transaction.uPeerInfo.ui32I2CDevAddr = slave_address;
  Transaction.eDirection      = AM_HAL_IOM_RX;
  Transaction.ui32NumBytes    = size;
  Transaction.pui32Buffer     = pbuf;
  Transaction.bContinue       = cont;
  Transaction.pCallback       = i2c_callback;
  Transaction.pCallbackCtxt   = &i2c_rd_callback_cntx;
  
  i2c_cmd_completed = false;
	i2c_dma_completed = false;
  i2c_cmd_error = false;
	i2c_cmd_no_ack_error = false;
  
  status = iom_dma_start(g_IOMHandle, &Transaction);
	if(status != AM_HAL_STATUS_SUCCESS)return status;
	
	if(block)
	{
	while(((i2c_cmd_completed == false) || (i2c_dma_completed == false))&&(i2c_cmd_error == false)){};
	if(i2c_cmd_error)
		{
		return AM_HAL_STATUS_FAIL;				
		} 
			
	}
	
return 	AM_HAL_STATUS_SUCCESS;
}

//----------------------------------------------------------------------//
//  IOM spi initialization
//----------------------------------------------------------------------//
uint32_t iom_i2c_init(void)
{
uint32_t status;
	
	status = am_hal_iom_initialize(IOM_MODULE, &g_IOMHandle);
	if(status != AM_HAL_STATUS_SUCCESS)return status;
	
	
  status = am_hal_iom_power_ctrl(g_IOMHandle, AM_HAL_SYSCTRL_WAKE, false);
	if(status != AM_HAL_STATUS_SUCCESS)return status;
	
	//
  // Set the required configuration settings for the IOM.
  //
  status = iom_dma_configure(g_IOMHandle, &g_sIOMI2cConfig);
	if(status != AM_HAL_STATUS_SUCCESS)return status;

  //
  // Configure the IOM pins.
  //
  iom_gpio_config();
  
  //
  // Enable interrupts
  //
  iom_dma_int_enable(IOM_MODULE);

  //
  // Enable the IOM.
  //
  status = iom_dma_enable(g_IOMHandle);
	
return status;	
}






