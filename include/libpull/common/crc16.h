/** \file crc16.h
  * \author Antonio Langiu
  * \defgroup com_crc16
  * \{
  */
#ifndef LIBPULL_COMMON_CRC16_H_
#define LIBPULL_COMMON_CRC16_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** 
 * \brief  Function to calculate the CRC16
 * 
 * \param data The data for which we want to calculate the CRC.
 * \param length The length of the data.
 * 
 * \returns The 16bits of the CRC 16.
 */
uint16_t libpull_crc16(const unsigned char* data, unsigned char length);

#ifdef __cplusplus
 }
#endif /* __cplusplus */
#endif /* \} LIBPULL_COMMON_CRC16_H_ */
