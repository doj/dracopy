/* -*- c-basic-offset: 2; tab-width: 2; indent-tabs-mode: nil -*-
 * vi: set shiftwidth=2 tabstop=2 expandtab:
 * :indentSize=2:tabSize=2:noTabs=true:
 */
/*
 * defines.h
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

#ifndef DEFINES_H_
#define DEFINES_H_

#define OK 0
#define ERROR -1
#define ABORT +1

#if defined(__C64__)
	#define BUFFERSIZE 16*1024
#elif defined(__C128__)
	#define BUFFERSIZE 4*1024
#elif defined(__PET__)
  #define BUFFERSIZE 1024
#else
  #define BUFFERSIZE 10*1024
#endif

#ifdef NOCOLOR
  #define COLOR_SIGNAL COLOR_WHITE
#else
  #define COLOR_SIGNAL COLOR_VIOLET
#endif

#define SCREENH 25
#define BOTTOM (SCREENH-1)

#define MENUW 13
#define MENUH SCREENH

#ifdef CHAR80
	#define SCREENW 80
	#define MENUX 58
#else
  #define SCREENW 40
  #define MENUX 27
#endif
#define MENUY 0


#define GETCWD dirs[context]
#define DIRW  25
#define DIR1X 0
#define DIR1Y 0

#ifdef CHAR80
  #define DIR2X DIRW+4
  #define DIR2Y 0
#else
  #define DIR2X 0
  #define DIR2Y 12
#endif

#define SPLITTER 12
#define ERROR -1
#define OK 0

// keyboard buffer
#if defined(__PLUS4__)
#define KBCHARS 1319
#define KBNUM 239
#elif defined(__C128__)
#define KBCHARS 842
#define KBNUM 208
#else
#define KBCHARS 631
#define KBNUM 198
#endif


// define function keys to something on PET
#ifdef __PET__
#define CH_F1 0xf1
#define CH_F2 0xf2
#define CH_F3 0xf3
#define CH_F4 0xf4
#define CH_F5 0xf5
#define CH_F6 0xf6
#define CH_F7 0xf7
#define CH_F8 0xf8
#endif

#define DIRTYPE 6

typedef unsigned char BYTE;

#endif
