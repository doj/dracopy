/*
 * dir.c
 *
 *  Created on: 10.01.2009
 *      Author: bader
 *
 * DraCopy (dc*) is a simple copy program.
 * DraBrowse (db*) is a simple file browser.
 *
 * Since both programs make use of kernal routines they shall
 * be able to work with most file oriented IEC devices.
 *
 * Created 2009 by Sascha Bader
 *
 * The code can be used freely as long as you retain
 * a notice describing original source and author.
 *
 * THE PROGRAMS ARE DISTRIBUTED IN THE HOPE THAT THEY WILL BE USEFUL,
 * BUT WITHOUT ANY WARRANTY. USE THEM AT YOUR OWN RISK!
 *
 * Newer versions might be available here: http://www.sascha-bader.de/html/code.html
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <dir.h>
#include <cbm.h>
#include <defines.h>


/*
 * read a devices directory
 */
Directory * readDir(Directory  * dir, BYTE device, BYTE context)
{
	//BYTE * mem;

	DirElement * previous = NULL;
	DirElement * current = NULL;

	register BYTE xpos = 0;

	unsigned char stat = 0;

	// free old dir
	if (dir!=NULL)
	{
		freeDir(&dir);
		dir=NULL;
	}

	/*
	for (mem=(BYTE*)(1024+24*40);mem<(BYTE*)2023;mem++)
	{
		*mem=32;
	}
*/

	revers(0);
	gotoxy((context==0)?DIR1X:DIR2X,(context==0) ? DIR1Y : DIR2Y );
	cputs("                         ");
	gotoxy((context==0)?DIR1X:DIR2X,(context==0) ? DIR1Y : DIR2Y );


	if (cbm_opendir(8, device) != 0) {
		cputs("could not open directory");
		//printErrorChannel(device);
		//cgetc();
		cbm_closedir(8);
		return NULL;
	}

	revers(1);

	do
	{
		DirElement * current = (DirElement *) malloc(sizeof(DirElement));
		memset (current,0,sizeof(DirElement));

		//current->previous=NULL;
		//current->next=NULL;
		//current->flags=0;


		stat=myCbmReadDir(8, &(current->dirent));
		if (stat==0)
		{
			if (++xpos < MENUX)	cputc(' ');
  		  	if (dir==NULL)
			{
				// initialize directory
				dir = (Directory *) malloc(sizeof(Directory));
				dir->name = current->dirent.name;
				dir->firstelement=NULL;
				dir->selected=NULL;
				dir->pos=0;
		    }
		    else if (current->dirent.type==CBM_T_FREE)
		    {
				// blocks free entry
		    	dir->free=current->dirent.size;
		    	free(current);
			}
		    else if (dir->firstelement==NULL)
		    {
				// first element
		    	dir->firstelement = current;
		    	dir->selected = current;
		    	previous=current;

			}
			else
		    {
				// all other elements
				current->previous=previous;
				previous->next=current;
			    previous=current;
		    }

			//cprintf("file:%s\n",current->dirent.name);
	    }
	}
	while(stat==0);

	cbm_closedir(8);

	revers(0);

	return dir;
}

unsigned char myCbmReadDir (unsigned char lfn, register struct cbm_dirent* l_dirent)
{
    unsigned char byte, i;

    if (cbm_k_chkin (lfn) == 0) {
        if (cbm_k_readst () == 0) {
            cbm_k_basin ();                    /* 0x01, 0x01, next basic line */
            cbm_k_basin ();

            byte = cbm_k_basin();             /* File-size */
            l_dirent->size = byte | ((cbm_k_basin()) << 8);

            byte = cbm_k_basin();

            i = 0;

            /* "B" BLOCKS FREE. */
            if (byte == 'b') {
                    l_dirent->type = CBM_T_FREE;
                    l_dirent->name[i++] = byte;
                          while ((byte = cbm_k_basin ()) != '\"'  )
                          {
                         		if (cbm_k_readst () != 0)
                         		{     /* ### Included to prevent */
              						cbm_k_clrch ();             /* ### Endless loop */
                                    l_dirent->name[i] = '\0';

              						return 0;                   /* ### Should be probably removed */
              					}
                              if (i < sizeof (l_dirent->name) - 1) {
                                  l_dirent->name[i] = byte;
                                  ++i;
                              }
                          }
                l_dirent->name[i] = '\0';
                return 0;
            }

            if (byte != '\"')
            {
                while (cbm_k_basin() != '\"') {
                    if (cbm_k_readst () != 0) {   /* ### Included to prevent */
                        cbm_k_clrch ();           /* ### Endless loop */
                        return 3;                 /* ### Should be probably removed */
                    }                             /* ### */
                }
            }


            while ((byte = cbm_k_basin ()) != '\"'  )
            {

					if (cbm_k_readst () != 0) {     /* ### Included to prevent */
						cbm_k_clrch ();             /* ### Endless loop */
						return 4;                   /* ### Should be probably removed */
					}

                /* ### */

                if (i < sizeof (l_dirent->name) - 1) {
                    l_dirent->name[i] = byte;
                    ++i;
                }
            }
            l_dirent->name[i] = '\0';

            while ((byte=cbm_k_basin ()) == ' ') {
                if (cbm_k_readst ()) {          /* ### Included to prevent */
                    cbm_k_clrch ();             /* ### Endless loop */
                    return 5;                   /* ### Should be probably removed */
                }                               /* ### */
            }

            switch (byte) {
                case 's':
                    l_dirent->type = CBM_T_SEQ;
                    break;
                case 'p':
                    l_dirent->type = CBM_T_PRG;
                    break;
                case 'u':
                    l_dirent->type = CBM_T_USR;
                    break;
                case 'r':
                    l_dirent->type = CBM_T_REL;
                    break;
                case 'c':
                    l_dirent->type = CBM_T_CBM;
                    break;
                case 'd':
                    l_dirent->type = CBM_T_DIR;
                    break;
                case 'v':
                    l_dirent->type = CBM_T_VRP;
                    break;
                default:
                    l_dirent->type = CBM_T_OTHER;
            }

            cbm_k_basin ();
            cbm_k_basin ();

            byte = cbm_k_basin ();

            l_dirent->access = (byte == 0x3C)? CBM_A_RO : CBM_A_RW;

            if (byte != 0) {
                while (cbm_k_basin() != 0) {
                    if (cbm_k_readst () != 0) { /* ### Included to prevent */
                        cbm_k_clrch ();         /* ### Endless loop */
                        return 6;               /* ### Should be probably removed */
                    }                           /* ### */
                }
            }

            cbm_k_clrch ();
            return 0;                         /* Line read successfuly */
        }
    }
    cbm_k_clrch ();
    return 1;
}



/*
 * free memory of directory structure
 */
void freeDir(Directory * * dir)
{
	struct direlement * acurrent;
	if (*dir!=NULL)
	{
		acurrent = (*dir)->firstelement;

		while (acurrent!=NULL)
		{
			DirElement * next = acurrent->next;
			free(acurrent);
			acurrent=next;
		}

		free(*dir);
	}
	*dir=NULL;
}


/*
 * Remove an entry from its data structure (directory)
 */
void removeFromDir(DirElement * current)
{
 	if (current!=NULL)
	{
		if (current->previous!=NULL)
		{
			current->previous->next = current->next;
	    }

		if (current->next!=NULL)
		{
			current->next->previous = current->previous;
	    }
	    free(current);
    }
}

