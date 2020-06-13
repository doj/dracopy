// code to test stuff. Not used in the dracopy programs.

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <cbm.h>

# define CBM_T_FREE 100

typedef struct direlement {
        struct cbm_dirent dirent;
        struct direlement * next;
        struct direlement * prev;
        unsigned char flags;
} DirElement;


typedef struct {
  /// 16 characters name
  /// 1 comma
  /// 2 characters ID
  /// NUL
  unsigned char name[16+1+2+1];
  struct direlement * firstelement;
  struct direlement * selected;
  const char *device_type;
  /// current cursor position
  unsigned int pos;
  /// number of free blocks
  unsigned int free;
} Directory;

typedef unsigned char BYTE;

/**
 * @return 0 upon success, @p l_dirent was set.
 * @return >0 upon error.
 */
unsigned char
myCbmReadDir(const BYTE device, struct cbm_dirent* l_dirent)
{
  unsigned char byte, i, byte2, byte3;

  if (cbm_k_chkin (device) != 0)
    {
      cbm_k_clrch();
      return 1;
    }

  if (cbm_k_readst() != 0)
    {
      return 7;
    }

  l_dirent->access = 0;

  // skip next basic line: 0x01, 0x01
  cbm_k_basin();
  cbm_k_basin();

  // read file size
  l_dirent->size = cbm_k_basin();
  l_dirent->size |= (cbm_k_basin()) << 8;

  i = 0;
  byte = cbm_k_basin();

  // handle "B" BLOCKS FREE
  if (byte == 'b')
    {
      l_dirent->type = CBM_T_FREE;
      l_dirent->name[i++] = byte;
      while ((byte = cbm_k_basin()) != '\"'  )
        {
          if (cbm_k_readst() != 0)
            {
              cbm_k_clrch();
              l_dirent->name[i] = '\0';
              return 0;
            }
          if (i < sizeof (l_dirent->name) - 1)
            {
              l_dirent->name[i] = byte;
              ++i;
            }
        }
      l_dirent->name[i] = '\0';
      return 0;
    }

  // read file name
  if (byte != '\"')
    {
      while (cbm_k_basin() != '\"')
        {
          if (cbm_k_readst() != 0)
            {
              cbm_k_clrch();
              return 3;
            }
        }
    }

  while ((byte = cbm_k_basin()) != '\"'  )
    {
      if (cbm_k_readst() != 0)
        {
          cbm_k_clrch();
          return 4;
        }

      if (i < sizeof (l_dirent->name) - 1)
        {
          l_dirent->name[i] = byte;
          ++i;
        }
    }
  l_dirent->name[i] = '\0';

  // read file type
  while ((byte=cbm_k_basin()) == ' ')
    {
      if (cbm_k_readst())
        {
          cbm_k_clrch();
          return 5;
        }
    }

  byte2 = cbm_k_basin();
  byte3 = cbm_k_basin();

#define X(a,b,c) byte==a && byte2==b && byte3==c

  if (X('p','r','g'))
    {
      l_dirent->type = CBM_T_PRG;
    }
  else if (X('s','e','q'))
    {
      l_dirent->type = CBM_T_SEQ;
    }
  else if (X('u','s','r'))
    {
      l_dirent->type = CBM_T_USR;
    }
  else if (X('d','e','l'))
    {
      l_dirent->type = CBM_T_DEL;
    }
  else if (X('r','e','l'))
    {
      l_dirent->type = CBM_T_REL;
    }
  else if (X('c','b','m'))
    {
      l_dirent->type = CBM_T_CBM;
    }
  else if (X('d','i','r'))
    {
      l_dirent->type = CBM_T_DIR;
    }
  else if (X('v','r','p'))
    {
      l_dirent->type = CBM_T_VRP;
    }
  else if (X('l','n','k'))
    {
      l_dirent->type = CBM_T_LNK;
    }
  else if (byte == '2' && byte2 == 'a')
    {
      // a directory line with an empty ID
      l_dirent->size = ' ' | (' '<<8);
      l_dirent->type = _CBM_T_HEADER;
      cbm_k_clrch();
      return 0;
    }
  else if (byte3 == ' ')
    {
      // reading the disk name line
      l_dirent->size = byte | (byte2 << 8);
      l_dirent->type = _CBM_T_HEADER;

      while (cbm_k_basin() != 0)
        {
          if (cbm_k_readst() != 0)
            {
              cbm_k_clrch();
              return 8;
            }
        }

      cbm_k_clrch();
      return 0;
    }
  else
    {
      l_dirent->type = CBM_T_OTHER;
    }

  // read access
  byte = cbm_k_basin();
  l_dirent->access = (byte == 0x3C) ? CBM_A_RO : CBM_A_RW;

  if (byte != 0)
    {
      while (cbm_k_basin() != 0)
        {
          if (cbm_k_readst() != 0)
            {
              cbm_k_clrch();
              return 6;
            }
        }
    }

  cbm_k_clrch();
  return 0;
}

int main()
{
  int i, cnt;
  const BYTE device = 9;
  clrscr();
  cprintf("start\n\r");
  i=cbm_opendir(device, device);
  cprintf("opendir %i\n\r", i);
  assert(i == 0);

  while(1)
    {
      DirElement * de = (DirElement *) calloc(1, sizeof(DirElement));
      assert(de);
      i = myCbmReadDir(device, &(de->dirent));
      cprintf("read %i\n\r", i);
      if (i < 0)
        break;
      if (i != 0)
        {
          cgetc();
        }
      cprintf("%s %u %u %u\n\r", de->dirent.name, de->dirent.size, de->dirent.type, de->dirent.access);
      //cgetc();
      if (++cnt == 8)
        {
          clrscr();
          cnt=0;
        }
    }

  return 0;
}
