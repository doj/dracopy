/* -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * vi: set shiftwidth=2 tabstop=2 expandtab:
 * :indentSize=2:tabSize=2:noTabs=true:
 */
/*
 * defines.h
 * Created on: 22.04.2018
 * Author: bader++
 * DraBrowse/Copy (dbc*) is a simple copy and file browser.
 * Since make use of kernal routines they shall be able to work with most file oriented IEC devices.
 * Created 2009-2018 by Sascha Bader and updated by Me
 * The code can be used freely as long as you retain a notice describing original source and author.
 * THE PROGRAMS ARE DISTRIBUTED IN THE HOPE THAT THEY WILL BE USEFUL, BUT WITHOUT ANY WARRANTY. USE THEM AT YOUR OWN RISK!
 * Newer versions might be available here: http://www.sascha-bader.de/html/code.html
 */

#ifndef DEFINES_H
#define DEFINES_H

#define BUFFERSIZE 4096
#define CBM_SEQ 2
#define CBM_T_FREE 100
#define MAXLEVEL 12

#define COLOR_SIGNAL COLOR_GREEN
#define COLOR_BORDER COLOR_BLACK
#define COLOR_BACKGROUND COLOR_BLACK
#define COLOR_TEXT COLOR_CYAN
#define COLOR_FRAME COLOR_WHITE
#define COLOR_MENU COLOR_LIGHTGREEN
#define COLOR_ERROR COLOR_LIGHTRED

#define MENUW 11
#define MENUH 23

#define MENUX 29
#define MENUY 0

#define SCREENW 40
#define SCREENH 25
#define BOTTOM (SCREENH - 1)

#define DIRW 27
#define DIRH 22

#ifdef __C128__
#define KBCHARS 0x034a
#define KBLENGTH 0x00d0
#define TIME 24000
#define EMD c128_reu
#endif

#ifdef __C64__
#define KBCHARS 0x0277
#define KBLENGTH 0x00c6
#define TIME 12000
#define EMD c64_reu
#undef CH_ESC
#define CH_ESC 95
#endif

#endif
