/*  Copyright 2005-2007,2013 Theo Berkau

    This file is part of Iapetus.

    Iapetus is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Iapetus is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Iapetus; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef BIOS_H
#define BIOS_H

#define INDIRECT_CALL(addr, return_type, ...)       (**(return_type(**)(__VA_ARGS__)) addr)

#define bios_run_cd_player              INDIRECT_CALL(0x0600026C, void,     void                            )
#define bios_is_mpeg_card_present       INDIRECT_CALL(0x06000274, int,      int filtno                      )
#define bios_get_mpeg_rom               INDIRECT_CALL(0x06000298, int,      u32 index, u32 size, u32 addr   )
#define bios_check_cd_auth              INDIRECT_CALL(0x06000270, int,      int mode                        )   //!< mode 0 -> check, 1 -> do auth
#define bios_set_scu_interrupt          INDIRECT_CALL(0x06000300, void,     u32 vector, void *func          )
#define bios_get_scu_interrupt          INDIRECT_CALL(0x06000304, void *,   u32 vector                      )
#define bios_set_sh2_interrupt          INDIRECT_CALL(0x06000310, void,     u32 vector, void *func          )
#define bios_get_sh2_interrupt          INDIRECT_CALL(0x06000314, void *,   u32 vector                      )
#define bios_set_clock_speed            INDIRECT_CALL(0x06000320, void,     u32 mode                        )
#define bios_get_clock_speed()          (*(volatile u32*)0x06000324)
#define bios_set_scu_interrupt_mask     INDIRECT_CALL(0x06000340, void,     u32 bits                        )
#define bios_change_scu_interrupt_mask  INDIRECT_CALL(0x06000344, void,     u32 mask, u32 bits              )
#define bios_get_scu_interrupt_mask()   (*(volatile u32*)0x06000348)

//! Prepare for Load CD operation.
/*!
 *  A Saturn CD may be read and booted using the bios_loadcd_ calls in order init, read, boot.
 *  Negative return values indicate failures.
 *
 *  bios_loadcd_init():
 *      - resets selectors and the usual things
 *      - resets some BIOS state variables
 *      - starts a disc authentication
 */
#define bios_loadcd_init                INDIRECT_CALL(0x0600029c, int,      void                            )
//! Start reading for Load CD.
/*!
 *  bios_loadcd_read():
 *      - checks the CD auth result
 *      - starts a Play command for the first 16 disc sectors, into selector 0
 *      - sets some BIOS flags
 */
#define bios_loadcd_read                INDIRECT_CALL(0x060002cc, int,      void                            )
//! Attempt to boot loaded disc
/*!
 *  Must not be called until the disc read is complete.
 *  bios_loadcd_boot():
 *      - reads the sector data from the CDB to 0x06002000
 *      - checks the SEGA SEGASATURN header
 *      - checks the IP size, security code
 *      - checks the region
 *      - boots the disc if checks pass
 *
 *  Return values include:
 *      -1 - bad header
 *      -4 - bad security code
 *      -8 - bad region
 *      +1 - not a Saturn disc
 */
#define bios_loadcd_boot                INDIRECT_CALL(0x06000288, int,      void                            )


typedef struct
{
	u16 dev_id;					// ID of device
	u16 part;					// Number of Partitions
} bup_cfg_struct;

typedef struct
{
	u32 total_bytes;			// Size of Backup Ram (in bytes)
	u32 total_blocks;			// Size of Backup Ram (in blocks)
	u32 block_size;			// Size of Block(in bytes)
	u32 free_bytes;			// Free Space(in bytes)
	u32 free_blocks;			// Free Space(in blocks)
	u32 write_block_size;	// Writable block size
} bup_stat_struct;

typedef struct
{
	u8 filename[12];			// File name
	u8 comment[11];			// Comment
	u8 language;				// Language of Comment
	u32 date;					// Date Stamp of File
	u32 byte_size;				// Size of Data(in bytes)
	u16 block_size;			// Size of Data(in blocks)
} bup_dir_struct;

typedef struct
{
	u8 year;						// Year minus 1980
	u8 month;					// Month
	u8 day;						// Day
	u8 hour;						// Hour
	u8 min;						// Minute
	u8 week;						// Week
} bup_date_struct;

#define bios_bup_vect_addr *(volatile u32 *)(0x6000354))

//! Initialize Bios Backup Library code
/*!
 *  bios_bup_init():
 *      - Copies bios backup library code to address specified in lib_addr
 *      - Detects whether cartridge and floppy disk drive are connected
 *      - Fills cfg with detected device information
 *      - cfg must an array bup_cfg_struct[3]
 */
#define bios_bup_init                   INDIRECT_CALL(0x6000358,             void,    volatile u32 *lib_addr, u32 *work_addr, bup_cfg_struct *cfg)

//! Backup Ram Select Partition. Only used for the floppy disk drive.
/*!
 *  bios_bup_sel_part():
 *      - Selects partition for the specified device
 *  Return values include:
 *      0 - No error
 *      1 - Device Error or doesn't exist
 *      2 - Not formatted
 *      3 - Write protected
 *      4 - Not enough memory
 *      5 - Not Found
 *      7 - No Match
 *      8 - Broken
 */
#define bios_bup_sel_part               INDIRECT_CALL(bios_bup_vect_addr+4,  s32,     u32 dev, u16 num)

//! Backup Ram Format.
/*!
 *  bios_bup_format():
 *      - Erases and formats specified device
 *  Return values include:
 *      0 - No error
 *      1 - Device Error or doesn't exist
 *      2 - Not formatted
 *      3 - Write protected
 *      4 - Not enough memory
 *      5 - Not Found
 *      7 - No Match
 *      8 - Broken
 */
#define bios_bup_format                 INDIRECT_CALL(bios_bup_vect_addr+8,  s32,     u32 dev)

//! Backup Ram Status.
/*!
 *  bios_bup_stat():
 *      - Fetches the used and free space of the current device
 *  Return values include:
 *      0 - No error
 *      1 - Device Error or doesn't exist
 *      2 - Not formatted
 *      3 - Write protected
 *      4 - Not enough memory
 *      5 - Not Found
 *      7 - No Match
 *      8 - Broken
 */
#define bios_bup_stat                   INDIRECT_CALL(bios_bup_vect_addr+12, s32,     u32 dev, u32 stat_size, bup_stat_struct *stat)

//! Backup Ram Write.
/*!
 *  bios_bup_write():
 *      - Checks to see if filename already exists. If overwrite is set to 1 it continues with next step
 *      - Writes data specified in data variable to specified device
 *  Return values include:
 *      0 - No error
 *      1 - Device Error or doesn't exist
 *      2 - Not formatted
 *      3 - Write protected
 *      4 - Not enough memory
 *      5 - Not Found
 *      7 - No Match
 *      8 - Broken
 */
#define bios_bup_write                  INDIRECT_CALL(bios_bup_vect_addr+16, s32,     u32 dev, bup_dir_struct *dir, volatile u8 *data, u8 overwrite)

//! Backup Ram Read.
/*!
 *  bios_bup_read():
 *      - Checks to see if filename exists
 *      - Reads data from specified device and filename to memory pointed to by data
 *  Return values include:
 *      0 - No error
 *      1 - Device Error or doesn't exist
 *      2 - Not formatted
 *      3 - Write protected
 *      4 - Not enough memory
 *      5 - Not Found
 *      7 - No Match
 *      8 - Broken
 *
 */
#define bios_bup_read                   INDIRECT_CALL(bios_bup_vect_addr+20, s32,     u32 dev, u8 *filename, volatile u8 *data)

//! Backup Ram Delete.
/*!
 *  bios_bup_del():
 *      - Checks to see if filename exists
 *      - Deletes data associated with filename and frees blocks
 *  Return values include:
 *      0 - No error
 *      1 - Device Error or doesn't exist
 *      2 - Not formatted
 *      3 - Write protected
 *      4 - Not enough memory
 *      5 - Not Found
 *      7 - No Match
 *      8 - Broken
 */
#define bios_bup_del                    INDIRECT_CALL(bios_bup_vect_addr+24, s32,     u32 dev, u8 *filename)

//! Backup Ram Get Directory List.
/*!
 *  bios_bup_dir():
 *      - Looks for saves based on specified device and filename
 *      - Stores results in memory pointed to by dir pointer
 *  Return values include:
 *      0 - No Error
 *      1 - Device Error or doesn't exist
 *      2 - Not formatted
 *      3 - Write protected
 *      4 - Not enough memory
 *      5 - Not Found
 *      7 - No Match
 *      8 - Broken
 */
#define bios_bup_dir                    INDIRECT_CALL(bios_bup_vect_addr+28, s32,     u32 dev, u8 *filename, u16 dir_size, bup_dir_struct *dir)

//! Backup Ram Verify.
/*!
 *  bios_bup_verify:
 *      - Compares data specified in data variable to data already stored on specified device and filename
 *  Return values include:
 *      0 - Match
 *      1 - Device Error or doesn't exist
 *      2 - Not formatted
 *      3 - Write protected
 *      4 - Not enough memory
 *      5 - Not Found
 *      7 - No Match
 *      8 - Broken
 */
#define bios_bup_verify                 INDIRECT_CALL(bios_bup_vect_addr+32, s32,     u32 dev, u8 *filename, volatile u8 *data)

//! Backup Ram Convert Date Stamp variable to structure.
/*!
 *  bios_bup_get_date:
 *      - Converts value specified in date(also see date variable in bup_dir_struct) to bup_date_struct format
 */
#define bios_bup_get_date               INDIRECT_CALL(bios_bup_vect_addr+36, void,    u32 date, bup_date_struct *date_data)

//! Backup Ram Convert structure to Date Stamp variable.
/*!
 *  bios_bup_set_date:
 *      - Converts data in date_data and returns 32-bit date stamp value(see date variable in bup_dir_struct).
 */
#define bios_bup_set_date               INDIRECT_CALL(bios_bup_vect_addr+40, u32,     bup_date_struct *date_data)

#endif
