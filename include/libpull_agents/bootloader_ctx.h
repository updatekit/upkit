/** \file bootloader_ctx.h
 * \brief Structure of the bootloader context.
 * \author Antonio Langiu
 * \defgroup ag_bl_ctx
 * \{
 */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#ifdef __cplusplus
}
#endif /* __cplusplus */
#ifndef LIBPULL_AGENTS_BOOTLOADER_CTX_H_
#define LIBPULL_AGENTS_BOOTLOADER_CTX_H_

#define FIRST_BOOT 0x1

/** 
 * \brief  Structure of the bootloader context. It is used to
 * store data used by the bootloader.
 */
typedef struct {
    uint8_t vendor_key[64];
    uint8_t buffer[63];
    uint8_t startup_flags;
} bootloader_ctx;

#endif /* LIBPULL_AGENTS_BOOTLOADER_CTX_H_ */
