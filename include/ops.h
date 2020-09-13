/* -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * vi: set shiftwidth=2 tabstop=2 expandtab:
 * :indentSize=2:tabSize=2:noTabs=true:
 */
/** @file
 * \date 10.01.2009
 * \author bader
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
 * https://github.com/doj/dracopy
 */

#ifndef OPS__H
#define OPS__H

#include "defines.h"
#include "dir.h"

int cmd(const BYTE device, const char *cmd);
void execute(char * prg, BYTE device);
void updateScreen(const BYTE context, BYTE num_dirs);
void updateMenu(void);
void about(const char *progname);
void showDir(BYTE context, const BYTE mycontext);
void mainLoop(void);
void clrDir(BYTE context);
void refreshDir(const BYTE context, const BYTE sorted, const BYTE mycontext);
void printDir(const BYTE context, const BYTE xpos, const BYTE ypos);
void printElement(const BYTE context, const Directory *dir, const BYTE xpos, const BYTE ypos);
const char* fileTypeToStr(BYTE ft);
BYTE dosCommand(const BYTE lfn, const BYTE drive, const BYTE sec_addr, const char *cmd);
int changeDir(const BYTE context, const BYTE device, const char *dirname, const BYTE sorted);
void changeDeviceID(BYTE device);
void drawDirFrame(BYTE context, const BYTE mycontext);
const char* getDeviceType(const BYTE device);
void initDirWindowHeight(void);
int textInput(const BYTE xpos, const BYTE ypos, char *str, const BYTE size);
void doDOScommand(const BYTE context, const BYTE sorted, const BYTE use_linebuffer);

enum drive_e {NONE=0, D1540, D1541, D1551, D1570, D1571, D1581, D1001, D2031, D8040, SD2IEC, CMD, VICE, LAST_DRIVE_E};

extern BYTE devicetype[];
extern const char* drivetype[];
extern char linebuffer[SCREENW+1];
extern BYTE devices[];
extern char linebuffer2[];
extern Directory* dirs[];

void debugs(const char *s);
void debugu(const unsigned u);

#endif
