/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_cc2538
 * @{
 *
 * @file
 * @brief           Linker script for the CC2538SF53
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

/* Memory Space Definitions: */
MEMORY
{
    rom   (rx ) : ORIGIN = (OTA_OFFSET), LENGTH = (OTA_LENGTH)
#ifndef CCFG_DISABLE
    cca   (rx ) : ORIGIN = 0x0027ffd4, LENGTH = 44
#endif
    ram   (rwx) : ORIGIN = 0x20000000, LENGTH = 32K
}

/* MCU Specific Section Definitions */
SECTIONS
{
#ifndef CCFG_DISABLE
    .flashcca :
    {
        KEEP(*(.flashcca))
    } > cca
#else
    /DISCARD/ :
    {
        *(.flashcca)
    }
#endif
}

INCLUDE cortexm_base.ld

/* @} */
