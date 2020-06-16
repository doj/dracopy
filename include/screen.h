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
#ifndef SCREEN_H
#define SCREEN_H
#include "defines.h"
void clearArea(const BYTE xpos, const BYTE ypos, const BYTE xsize, const BYTE ysize);
void newscreen(const char *title);
void drawFrame(const char *title, const BYTE xpos, const BYTE ypos, const BYTE xsize, const BYTE ysize, const char *subtitle);
void initScreen(const BYTE border, const BYTE bg, const BYTE text);
void exitScreen(void);
#endif
