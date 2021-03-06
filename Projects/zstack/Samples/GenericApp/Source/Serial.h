/**************************************************************************************************
  Filename:       Serial.h
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

#ifndef SERIAL_H
#define SERIAL_H

#ifdef __cplusplus
extern "C"
{
#endif
  
/**************************************************************************************************
 *                                             INCLUDES
 **************************************************************************************************/
#include "Onboard.h"
#include "OSAL.h"


/**************************************************************************************************
 *                                              MACROS
 **************************************************************************************************/

/**************************************************************************************************
 *                                            CONSTANTS
 **************************************************************************************************/
/* Message Command IDs */
#define CMD_SERIAL_UART_MSG             0x01    

  
typedef struct
{
  osal_event_hdr_t  hdr;
  uint8             *msg;
} OSALSerialData_t;

#define START_MEASURE   0x01
#define STOP_MEASURE    0x02
#define SYNC_MEASURE    0x03
#define FIND_NWK        0x04
#define END_DEVICE      0x05
#define CLOSEING        0x06
#define CLOSE_NWK       0x07
#define DATA_START      0x33    // 数据开始校验位
#define DATA_END        0x55    // 数据结束校验位

/**************************************************************************************************
 *                                             FUNCTIONS - API
 **************************************************************************************************/
/*
 * Task Initialization for the UART1-alt1
 */
extern void Serial_Init( void );

/*
 * Register TaskID for the application
 */
extern void Serial_UartRegisterTaskID( uint8 taskID );

/*
 * Send Msg
 */
extern uint16 Serial_UartSendMsg( uint8 *msg , uint8 dataLen );

#ifdef __cplusplus
}
#endif  
#endif