/**************************************************************************************************
  Filename:       Serial.c
  Revised:        $Date: 2016-03-24 16:45:16 +0800 (Thu, 24 Mar 2016) $
  Revision:       $Revision: 1 $

  Description:    This file contains the interface of UART for BLE


  Copyright 2016 Bupt. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED 揂S IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, 
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE, 
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact kylinnevercry@gmail.com. 
**************************************************************************************************/

/***************************************************************************************************
 *                                             INCLUDES
 ***************************************************************************************************/
#include "Serial.h"
#include "hal_uart.h"
#include "OSAL.h"

/***************************************************************************************************
 *                                             CONSTANTS
 ***************************************************************************************************/
#if !defined( SERIAL_PORT )
#define SERIAL_PORT  HAL_UART_PORT_0
#endif

#if !defined( SERIAL_BAUD )
#define SERIAL_BAUD  HAL_UART_BR_38400
#endif

// When the Rx buf space is less than this threshold, invoke the Rx callback.
#if !defined( SERIAL_THRESH )
#define SERIAL_THRESH  64
#endif

#if !defined( SERIAL_RX_SZ )
#define SERIAL_RX_SZ  128
#endif

#if !defined( SERIAL_TX_SZ )
#define SERIAL_TX_SZ  128
#endif

// Millisecs of idle time after a byte is received before invoking Rx callback.
#if !defined( SERIAL_IDLE )
#define SERIAL_IDLE  6
#endif



/***************************************************************************************************
 *                                              MACROS
 ***************************************************************************************************/

/***************************************************************************************************
 *                                              TYPEDEFS
 ***************************************************************************************************/

/**************************************************************************************************
 *                                        INNER GLOBAL VARIABLES
 **************************************************************************************************/
/* Used to indentify the application ID for osal task */
uint8 registeredSerialTaskID;


/**************************************************************************************************
 *                                        FUNCTIONS - Local
 **************************************************************************************************/
void Serial_UartProcesssData( uint8 port, uint8 event);

/**************************************************************************************************
 *                                        FUNCTIONS - API
 **************************************************************************************************/


/**************************************************************************************************
 * @fn      Serial_Init
 *
 * @brief   Initilize Serial-UART1 alt1
 *
 * @param   none
 *
 * @return  None
 **************************************************************************************************/
void Serial_Init( void )
{
  //don't care 就是表示这些参数随便设置，并没有用到，他们是在UART driver里设置的。
  //主要就是设置BAUD、硬件流控制、回调函数。
  halUARTCfg_t uartConfig;
  
  /* Initialize APP ID */
  registeredSerialTaskID = 0;
  
  /* UART Configuration */  
  uartConfig.configured           = TRUE;              // 2x30 don't care - see uart driver.
  uartConfig.baudRate             = SERIAL_BAUD;       // 38400
  uartConfig.flowControl          = FALSE;             //关闭硬件流控制
  uartConfig.flowControlThreshold = SERIAL_THRESH;     // 2x30 don't care - see uart driver.
  uartConfig.rx.maxBufSize        = SERIAL_RX_SZ;      // 2x30 don't care - see uart driver.
  uartConfig.tx.maxBufSize        = SERIAL_TX_SZ;      // 2x30 don't care - see uart driver.
  uartConfig.idleTimeout          = SERIAL_IDLE;       // 2x30 don't care - see uart driver.
  uartConfig.intEnable            = TRUE;              // 2x30 don't care - see uart driver.
  uartConfig.callBackFunc         = Serial_UartProcesssData;
  
  
  HalUARTOpen( SERIAL_PORT , &uartConfig );
}

/***************************************************************************************************
 * @fn      Serial_UartProcesssData 
 *
 * @brief   Parses the data and determine either is UART
 *          Reference to MT_UartProcessZAppData on MT_UART.c
 *
 * @param   port     - UART port
 *          event    - Event that causes the callback
 *
 *
 * @return  None
 ***************************************************************************************************/
void Serial_UartProcesssData( uint8 port, uint8 event)
{
  OSALSerialData_t *msg_ptr;
  uint8 rxBufLen = Hal_UART_RxBufLen(port);
  
  /* Verify events */
  if (event == HAL_UART_TX_FULL)
  {
    // Do something when TX if full
    return;
  }
  
  //接收处理
  if (event & ( HAL_UART_RX_FULL | HAL_UART_RX_ABOUT_FULL | HAL_UART_RX_TIMEOUT))
  {
    if ( registeredSerialTaskID )
    {
      if( rxBufLen != 0 )
      {
        /* 2 more bytes are added, 1 for CMD type, other for length */
        msg_ptr = (OSALSerialData_t *)osal_msg_allocate( rxBufLen + sizeof( OSALSerialData_t ) );
        if( msg_ptr )
        {
          msg_ptr->hdr.event = CMD_SERIAL_UART_MSG;
          msg_ptr->hdr.status = rxBufLen;
          msg_ptr->msg = (uint8 *)(msg_ptr + 1);
          
          /* Read the data of Rx buffer */
          HalUARTRead( port, msg_ptr->msg , rxBufLen );

          /* Send the raw data to application...or where ever */
          osal_msg_send( registeredSerialTaskID, (uint8 *)msg_ptr );          
        }
      }
    }
  }
}

/***************************************************************************************************
 * @fn      Serial_UartRegisterTaskID
 *
 * @brief   This function registers the taskID of the application so it knows
 *          where to send the messages whent they come in.
 *
 * @param   void
 *
 * @return  void
 ***************************************************************************************************/
void Serial_UartRegisterTaskID( uint8 taskID )
{
  registeredSerialTaskID = taskID;
}

/***************************************************************************************************
 * @fn      Serial_UartSendMsg
 *
 * @brief   This function send Msg use uart to BLE
 *
 * @param   buf  - pointer to the buffer that will be written, not freed
 *          len  - length of
 *
 * @return  length of the buffer that was sent
 ***************************************************************************************************/
uint16 Serial_UartSendMsg( uint8 *msg , uint8 dataLen )
{
  return HalUARTWrite( SERIAL_PORT , msg , dataLen);
}