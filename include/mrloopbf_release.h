#ifndef MRLOOPBF_RELEASE_H
#define MRLOOPBF_RELEASE_H

#include <stdint.h>
#include "Common/mrloopsdkheader.h"

/** \ingroup asyncio
 * Initial the device and SDK first.
 * \returns If return fail, there may be error on device driver or no device is attached.
 */
bool ML_Init();

/** \ingroup asyncio
 * Close the device and leave SDK .
 * \returns no return.
 */
void ML_Close();

/** \ingroup asyncio
 * Hide the debug output message.
 * \returns no return.
 */
void ML_HiddenDebugMsg();

/** \ingroup asyncio
 * To send out packet. Timeout value is 1 second.
 * \param In_byte_ptr The buffer to be sent out. The buffer size must be multiple of 4096bytes since the unit in RF transaction is 4096bytes. The buffer will be divided into 4k packets in transaction. Each packet may be dropped or repeat in RF transaction.
 * \param In_length_ptr The size of the buffer.
 * \returns If return fail, there may be error on device driver or no device is attached.
 *
 * ML_Transfer does NOT guarantee that the packet is delivered without error. Even ML_Transfer returns true, the packet could be dropped or repeated.
 * The safe way is to put an index in the packet. Tx sends out the packet. Rx receives the packet and checks the index. Then Rx sends out one packet to note ack.
 * If Tx does not receive the ack packet, then sends out the packet again or return error.
 */
int ML_Transfer(uint8_t* In_byte_ptr, int In_length_ptr);

/** \ingroup asyncio
 * To listen to RF. Timeout value is 1 second.
 * \param In_byte_ptr A buffer to receive data. The buffer size must be multiple of 4096bytes since the unit in RF transaction is 4096bytes. The buffer will be divided into 4k packets in transaction. Each packet may be dropped or repeat in RF transaction.
 * \param In_length_ptr Input the size of the buffer and output the size  of the receive packet. When it returns fail, *In_length_ptr will be 0.
 * \returns When it returns fail, *In_length_ptr will be 0.
 *
 * ML_Transfer does NOT guarantee that the packet is delivered without error. Even ML_Transfer returns true, the packet could be dropped or repeated.
 * The safe way is to put an index in the packet. Tx sends out the packet. Rx receives the packet and checks the index. Then Rx sends out one packet to note ack.
 * If Tx does not receive the ack packet, then sends out the packet again or return error.
 */
int ML_Receiver(uint8_t* In_byte_ptr, int* In_length);

/** \ingroup asyncio
 * is set Mrloop WiGig Dongle speed.
 * \param speed It ranges between 1~7.
 * \returns False is fail.
 */
bool ML_SetSpeed(uint8_t speed);

/** \ingroup asyncio
 * is Set Mrloop WiGig Dongle RF rule
 * \param mode Set mode value "1" is Master, mode value "2" is Slave.
 * \returns False is fail.
 *  Slave can Only connect to Master. Master can Only connect to Slave.
 */
bool ML_SetMode(uint8_t mode);

/** \ingroup asyncio
 *  Get Current USB Device Generation.
 * \returns Integer value when succeeded, 0 if it fails.
 *  1 is USB 1.1
 *  2 is USB 2.0
 *  3 is USB 3.0
 *  4 is USB 3.1
 */
uint8_t ML_GetDevGen(void);

bool ML_SetTxSector(uint8_t TxSector);
bool ML_SetRxSector(uint8_t RxSector);
bool ML_SendRFStatusReq();
bool ML_DecodeRFStatusPacket(uint8_t * In_byte_ptr, ML_RF_INF* Out_Result);
bool ML_GetRFStatus(ML_RF_INF* Out_Result);

#endif // MRLOOPBF_RELEASE_H
