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

uint16_t crc16(const unsigned char* data, unsigned char length);

#ifdef __cplusplus
 }
#endif /* __cplusplus */
#endif /* \} LIBPULL_COMMON_CRC16_H_ */
