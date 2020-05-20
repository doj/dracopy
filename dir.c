/* -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * vi: set shiftwidth=2 tabstop=2 expandtab:
 * :indentSize=2:tabSize=2:noTabs=true:
 */
/*
 * dir.c
 * Created on: 22.04.2018
 * Author: bader++
 * DraBrowse/Copy (dbc*) is a simple copy and file browser.
 * Since make use of kernal routines they shall be able to work with most file oriented IEC devices.
 * Created 2009-2018 by Sascha Bader and updated by Me
 * The code can be used freely as long as you retain a notice describing original source and author.
 * THE PROGRAMS ARE DISTRIBUTED IN THE HOPE THAT THEY WILL BE USEFUL, BUT WITHOUT ANY WARRANTY. USE THEM AT YOUR OWN RISK!
 * Newer versions might be available here: http://www.sascha-bader.de/html/code.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <dir.h>
#include <cbm.h>
#include <defines.h>

Directory *readDir(Directory *dir, unsigned char device){

	DirElement *previous = NULL;
	DirElement *current = NULL;
	unsigned char stat = 0;

	/* free old dir */
	if(dir) freeDir(&dir);
	cbm_opendir(8, device);

	do{
		DirElement *current = (DirElement *) malloc(sizeof(DirElement));
		memset(current, 0, sizeof(DirElement));
		stat = cbmReadDir(8, &current->dirent);

		if(!stat){
			if(!dir){
				/* initialize directory */
				dir = (Directory *) malloc(sizeof(Directory));
				dir->name = current->dirent.name;
				dir->firstelement = NULL;
				dir->selected = NULL;
				dir->pos = 0;
			}
			else if(current->dirent.type == CBM_T_FREE){
				/* blocks free entry */
				dir->free = current->dirent.size;
				free(current);
			}
			else if(dir->firstelement == NULL){
				/* first element */
				dir->firstelement = current;
				dir->selected = current;
				previous = current;
			}
			else{
				/* all other elements */
				current->previous = previous;
				previous->next = current;
				previous = current;
			}
		}
	}
	while(!stat);
	cbm_closedir(8);
	return dir;
}

unsigned char cbmReadDir(unsigned char lfn, register struct cbm_dirent *l_dirent){

	unsigned char byte;
	unsigned char i = 0;

	if(!cbm_k_chkin(lfn)){
		if(!cbm_k_readst()){
			/* 0x01, 0x01, next basic line */
			cbm_k_basin();
			cbm_k_basin();
			/* file-size */
			l_dirent->size = cbm_k_basin() | (cbm_k_basin() << 8);

			byte = cbm_k_basin();
			/* Blocks free */
			if(byte == 0x42){
				l_dirent->type = CBM_T_FREE;
				while(!cbm_k_readst()){
					cbm_k_basin();
				}
				cbm_k_clrch();	/* prevent endless sort */
				return 0;
			}
			while(byte != 0x22){
				if(cbm_k_readst()){
					cbm_k_clrch();	/* prevent endless sort */
					return 3;
				}
				byte = cbm_k_basin();
			}
			while((byte = cbm_k_basin()) != 0x22){
				if(cbm_k_readst()){
					cbm_k_clrch();	/* prevent endless sort */
					return 4;
				}
				if(i < sizeof(l_dirent->name) - 1){
					l_dirent->name[i] = byte;
					++i;
				}
			}
			l_dirent->name[i] = 0x00;

			while((byte = cbm_k_basin()) == 0x20){
				if(cbm_k_readst()){
					cbm_k_clrch();	/* prevent endless sort */
					return 5;
				}
			}
			/* file type */
			switch (byte){
				case 0x44:
					cbm_k_basin() == 0x49 ? l_dirent->type = CBM_T_DIR : l_dirent->type = CBM_T_DEL;
					break;
				case 0x53:
					l_dirent->type = CBM_T_SEQ;
					break;
				case 0x50:
					l_dirent->type = CBM_T_PRG;
					break;
				case 0x55:
					l_dirent->type = CBM_T_USR;
					break;
				case 0x52:
					l_dirent->type = CBM_T_REL;
					break;
				case 0x43:
					l_dirent->type = CBM_T_CBM;
					break;
				case 0x56:
					l_dirent->type = CBM_T_VRP;
					break;
				default:
					l_dirent->type = CBM_T_OTHER;
			}
			cbm_k_basin ();
			/* locked files */
			byte = cbm_k_basin();
			l_dirent->access = byte == 0x3c ? CBM_A_RO : CBM_A_RW;
			if(byte){
				while(cbm_k_basin()){
					if(cbm_k_readst()){
						cbm_k_clrch();	/* prevent endless sort */
						return 6;
					}
				}
			}
			cbm_k_clrch();
			return 0;	/* line read successfuly */
		}
	}
	cbm_k_clrch();
	return 1;
}

void freeDir(Directory ** dir){

	struct direlement *acurrent;

	if(*dir){
		acurrent = (*dir)->firstelement;
		while(acurrent){
			DirElement *next = acurrent->next;
			free(acurrent);
			acurrent = next;
		}
		free(*dir);
	}
	*dir = NULL;
}

Directory *sortDir(Directory *dir){

	struct cbm_dirent tmpdirent;
	unsigned char tmpflags;
	DirElement *sort, *current;

	for(sort = dir->firstelement; sort->next; sort = sort->next){
		for(current = dir->firstelement; current->next; current = current->next){
			if(stricmp(current->dirent.name, current->next->dirent.name) > 0){

				tmpdirent = current->dirent;
				current->dirent = current->next->dirent;
				current->next->dirent = tmpdirent;

				tmpflags = current->flags;
				current->flags = current->next->flags;
				current->next->flags = tmpflags;
			}
		}
	}
	return dir;
}
