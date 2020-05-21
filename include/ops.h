/* -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * vi: set shiftwidth=2 tabstop=2 expandtab:
 * :indentSize=2:tabSize=2:noTabs=true:
 */

#ifndef OPS__H
#define OPS__H

#include "defines.h"
#include "dir.h"

int cmd(unsigned char lfn, unsigned char * cmd);
void execute(char * prg, BYTE device);
void updateScreen(BYTE num_dirs);
void updateMenu(void);
void about(const char *progname);
void showDir(Directory * cwd, BYTE context);
void mainLoop(void);
void clrDir(BYTE context);
void refreshDir(void);
void printDir(Directory * dir,int xpos, int ypos);
void printElement(Directory * dir,int xpos, int ypos);
const char* fileTypeToStr(BYTE ft);
void showDir(Directory * dir, BYTE mycontext);

extern BYTE DIRH;

#endif
