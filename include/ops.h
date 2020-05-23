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
void updateScreen(const BYTE context, BYTE num_dirs);
void updateMenu(void);
void about(const char *progname);
void showDir(BYTE context, const Directory * cwd, const BYTE mycontext);
void mainLoop(void);
void clrDir(BYTE context);
void refreshDir(const BYTE context);
void printDir(const BYTE context, const Directory *dir, const BYTE xpos, const BYTE ypos);
void printElement(const BYTE context, const Directory *dir, const BYTE xpos, const BYTE ypos);
const char* fileTypeToStr(BYTE ft);
unsigned char dosCommand(unsigned char lfn, unsigned char drive, unsigned char sec_addr, char *cmd);
void changeDir(const BYTE context, const BYTE device, const char *dirname);
void changeDeviceID(BYTE device);
void drawDirFrame(BYTE context, const Directory *dir, const BYTE mycontext);
void debugs(const char *s);
const char* getDeviceType(BYTE context);
void initDirWindowHeight();

enum drive_e {NONE=0, D1540, D1541, D1551, D1570, D1571, D1581, SD2IEC, CMD, VICE, LAST_DRIVE_E};

extern BYTE devicetype[];
extern const char* drivetype[];

#endif
