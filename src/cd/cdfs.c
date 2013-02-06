/*  Copyright 2007 Theo Berkau

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

#include <string.h>
#include "../iapetus.h"

//////////////////////////////////////////////////////////////////////////////

typedef struct
{
  u16 groupid;
  u16 userid;
  u16 attributes;
  u16 signature;
  u8 filenumber;
  u8 reserved[5];
} xarec_struct;

typedef struct
{
  u8 recordsize;
  u8 xarecordsize;
  u32 lba;
  u32 size;
  u8 dateyear;
  u8 datemonth;
  u8 dateday;
  u8 datehour;
  u8 dateminute;
  u8 datesecond;
  u8 gmtoffset;
  u8 flags;
  u8 fileunitsize;
  u8 interleavegapsize;
  u16 volumesequencenumber;
  u8 namelength;
  char name[32];
  xarec_struct xarecord;
} dirrec_struct;

u8 *dirtbl;
u8 *sectbuf;
int sectbuffered;
int dirtblsize;
int rootlba;
int rootsize;

//////////////////////////////////////////////////////////////////////////////

void CopyDirRecord(u8 *buffer, dirrec_struct *dirrec)
{
  u8 *temp_pointer;

  temp_pointer = buffer;

  memcpy(&dirrec->recordsize, buffer, sizeof(dirrec->recordsize));
  buffer += sizeof(dirrec->recordsize);

  memcpy(&dirrec->xarecordsize, buffer, sizeof(dirrec->xarecordsize));
  buffer += sizeof(dirrec->xarecordsize);

  buffer += sizeof(dirrec->lba);
  memcpy(&dirrec->lba, buffer, sizeof(dirrec->lba));
  buffer += sizeof(dirrec->lba);

  buffer += sizeof(dirrec->size);
  memcpy(&dirrec->size, buffer, sizeof(dirrec->size));
  buffer += sizeof(dirrec->size);

  dirrec->dateyear = buffer[0];
  dirrec->datemonth = buffer[1];
  dirrec->dateday = buffer[2];
  dirrec->datehour = buffer[3];
  dirrec->dateminute = buffer[4];
  dirrec->datesecond = buffer[5];
  dirrec->gmtoffset = buffer[6];
  buffer += 7;

  dirrec->flags = buffer[0];
  buffer += sizeof(dirrec->flags);

  dirrec->fileunitsize = buffer[0];
  buffer += sizeof(dirrec->fileunitsize);

  dirrec->interleavegapsize = buffer[0];
  buffer += sizeof(dirrec->interleavegapsize);

  buffer += sizeof(dirrec->volumesequencenumber);
  memcpy(&dirrec->volumesequencenumber, buffer, sizeof(dirrec->volumesequencenumber));
  buffer += sizeof(dirrec->volumesequencenumber);

  dirrec->namelength = buffer[0];
  buffer += sizeof(dirrec->namelength);

  memset(dirrec->name, 0, sizeof(dirrec->name));
  memcpy(dirrec->name, buffer, dirrec->namelength);
  buffer += dirrec->namelength;

  // handle padding
  buffer += (1 - dirrec->namelength % 2);

  memset(&dirrec->xarecord, 0, sizeof(dirrec->xarecord));

  // Sadily, this is the best way I can think of for detecting XA records
  if ((dirrec->recordsize - (buffer - temp_pointer)) == 14)
  {
     memcpy(&dirrec->xarecord.groupid, buffer, sizeof(dirrec->xarecord.groupid));
     buffer += sizeof(dirrec->xarecord.groupid);

     memcpy(&dirrec->xarecord.userid, buffer, sizeof(dirrec->xarecord.userid));
     buffer += sizeof(dirrec->xarecord.userid);

     memcpy(&dirrec->xarecord.attributes, buffer, sizeof(dirrec->xarecord.attributes));
     buffer += sizeof(dirrec->xarecord.attributes);

     memcpy(&dirrec->xarecord.signature, buffer, sizeof(dirrec->xarecord.signature));
     buffer += sizeof(dirrec->xarecord.signature);

     memcpy(&dirrec->xarecord.filenumber, buffer, sizeof(dirrec->xarecord.filenumber));
     buffer += sizeof(dirrec->xarecord.filenumber);

     memcpy(dirrec->xarecord.reserved, buffer, sizeof(dirrec->xarecord.reserved));
     buffer += sizeof(dirrec->xarecord.reserved);
  }
}

//////////////////////////////////////////////////////////////////////////////

int CDFSInit(void *workdirtbl, int size)
{
   int ret;
   dirrec_struct dirrec;

   dirtbl = workdirtbl;
   dirtblsize = size;
   if (size < 4096)
      return LAPETUS_ERR_INVALIDARG;

   sectbuf = workdirtbl+2048;
   sectbuffered = 0;

   // Read in lba 16
   if ((ret = CDReadSector(dirtbl, 166, SECT_2048, 2048)) != LAPETUS_ERR_OK)
      return ret;

   CopyDirRecord(dirtbl+0x9C, &dirrec);

   // Ok, now we have the root directory(that's good enough for now)
   rootlba = dirrec.lba;
   rootsize = (dirrec.size / 2048);

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int CDFSOpen(const char *path, file_struct *file)
{
   char *p;
   char dirname[13];
   dirrec_struct dirrec;
   u32 lba;
   u32 size;
   int sectorsleft;
   u8 *workbuffer;
   int done=0;
   int ret;

   // Starting with the root directory, go through each subdirectory,
   // read the directory table, and find the next subdirectory. Once we're
   // in the correct level, parse through the table and find the file.

   if ((ret = CDReadSector(dirtbl, 150+rootlba, SECT_2048, 2048)) != LAPETUS_ERR_OK)
      return ret;

   lba = rootlba + 1;
   sectorsleft = rootsize - 1;
   workbuffer = dirtbl;

   if (path == NULL)
   {
      int i;
      // Get first file(I may remove this feature yet)
      for (i = 0; i < 3; i++)
      {
         if (workbuffer[0] == 0)
            return LAPETUS_ERR_FILENOTFOUND;

         CopyDirRecord(workbuffer, &dirrec);
         workbuffer += dirrec.recordsize;
      }

      // We're done.
      file->lba = dirrec.lba;
      file->size = dirrec.size;
      file->sectpos = 0;
      file->pos = 0;

      return LAPETUS_ERR_OK;
   }

   while(!done)
   {
      if ((p = strchr(path, '\\')) == NULL)
      {
         // It's just the filename now
         done = 1;
         size = strlen(path);
      }
      else
         size = p - path;

      strncpy(dirname, path, size);
      dirname[size] = '\0';
      path += size;

      // Now that we've got the current subdirectory's name, let's find its
      // table.
      for (;;)
      {
         CopyDirRecord(workbuffer, &dirrec);
         workbuffer += dirrec.recordsize;

         if (strcmp(dirrec.name, dirname) == 0 ||
             strncmp(dirrec.name, dirname, strlen(dirname)) == 0)
         {
            lba = dirrec.lba;
            sectorsleft = dirrec.size / 2048;
            break;
         }
           
         // If record size of the next entry is zero, it means we're at the end
         // of the directory record data in the current sector
         if (workbuffer[0] == 0)
         {
            // Let's see if we can read in another sector yet
            if (sectorsleft > 0)
            {
               // Read in new sector
               if ((ret = CDReadSector(dirtbl, 150+lba, SECT_2048, 2048)) != LAPETUS_ERR_OK)
                  return ret;
               lba++;
               sectorsleft--;
               workbuffer = dirtbl;
            }
            else
               // We can't, let's bail
               return LAPETUS_ERR_FILENOTFOUND;
         }        
      }

      if (done)
         break;

      // Ok, we've found the next directory table, time to read it
      if ((ret = CDReadSector(dirtbl, 150+lba, SECT_2048, 2048)) != LAPETUS_ERR_OK)
         return ret;
      lba++;
      sectorsleft--;
      workbuffer = dirtbl;
   }

   // Ok, time to fill out the file structure
   file->lba = lba;
   file->size = dirrec.size;
   file->sectpos = 0;
   file->pos = 0;

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int CDFSSeek(file_struct *file, int offset, int seek_type)
{
   switch(seek_type)
   {
      case CDFS_SEEK_SET:
         file->sectpos = offset / 2048;
         file->pos = offset % 2048;
         break;
      case CDFS_SEEK_CUR:
         file->sectpos = file->sectpos + ((file->pos + offset) / 2048);
         file->pos = (file->pos + offset) % 2048;
         break;
      case CDFS_SEEK_END:
         file->sectpos = (file->size - 1 - offset) / 2048;
         file->pos = (file->size - 1 - offset) % 2048;
         break;
      default:
         return LAPETUS_ERR_INVALIDARG;
   }

   // Change the CD read position here if need be
   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int CDFSRead(u8 *buffer, int size, int num, file_struct *file)
{
   int ret;

   // Only sector-aligned transfers supported for now
   if (file->pos == 0)
   {
      // Straight sectors reads. Nice and fast
      if ((ret = CDReadSector(buffer, 150+file->lba+file->sectpos, SECT_2048, size * num)) == LAPETUS_ERR_OK)
         file->sectpos += (size * num);
      return ret;
   }
   else
      return LAPETUS_ERR_UNSUPPORTED;

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int CDFSClose(file_struct *file)
{
   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////
