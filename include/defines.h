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
#ifndef DEFINES_H_
#define DEFINES_H_

#if defined(__PET__) || defined(__CBM610__)
#define COLOR_LIGHTGREEN COLOR_WHITE
#define COLOR_LIGHTBLUE COLOR_WHITE
#define COLOR_BLUE COLOR_WHITE
#define COLOR_CYAN COLOR_WHITE
#define COLOR_GRAY1 COLOR_BLACK
#define COLOR_GRAY2 COLOR_BLACK
#define COLOR_GRAY3 COLOR_WHITE
#endif

// a C64 style lightblue on blue scheme
#if defined(COLOR_SCHEME_BLUE)
#define DC_COLOR_BG COLOR_BLUE
#define DC_COLOR_BORDER COLOR_BLUE
#define DC_COLOR_TEXT COLOR_LIGHTBLUE
#define DC_COLOR_HIGHLIGHT COLOR_WHITE
#define DC_COLOR_DIM COLOR_GRAY2
#define DC_COLOR_ERROR COLOR_RED
#define DC_COLOR_WARNING COLOR_YELLOW
#define DC_COLOR_EE COLOR_YELLOW
#define DC_COLOR_GRAY COLOR_GRAY2
#define DC_COLOR_GRAYBRIGHT COLOR_GRAY3
#define DC_COLOR_WAITKEY COLOR_VIOLET

// a SX-64 style blue on white scheme
#elif defined(COLOR_SCHEME_SX)
#define DC_COLOR_BG COLOR_WHITE
#define DC_COLOR_BORDER COLOR_CYAN
#define DC_COLOR_TEXT COLOR_BLUE
#define DC_COLOR_HIGHLIGHT COLOR_LIGHTBLUE
#define DC_COLOR_DIM COLOR_GRAY2
#define DC_COLOR_ERROR COLOR_RED
#define DC_COLOR_WARNING COLOR_YELLOW
#define DC_COLOR_EE COLOR_GRAY1
#define DC_COLOR_GRAY COLOR_GRAY2
#define DC_COLOR_GRAYBRIGHT COLOR_GRAY3
#define DC_COLOR_WAITKEY COLOR_VIOLET

// a C128 style light green on gray scheme
#elif defined(COLOR_SCHEME_128)
#define DC_COLOR_BG COLOR_GRAY1
#define DC_COLOR_BORDER COLOR_GRAY1
#define DC_COLOR_TEXT COLOR_LIGHTGREEN
#define DC_COLOR_HIGHLIGHT COLOR_WHITE
#define DC_COLOR_DIM COLOR_GREEN
#define DC_COLOR_ERROR COLOR_RED
#define DC_COLOR_WARNING COLOR_YELLOW
#define DC_COLOR_EE COLOR_GRAY2
#define DC_COLOR_GRAY COLOR_GRAY2
#define DC_COLOR_GRAYBRIGHT COLOR_GRAY3
#define DC_COLOR_WAITKEY COLOR_VIOLET

// the classic dracopy green on black color scheme
#else
#define DC_COLOR_BG COLOR_BLACK
#define DC_COLOR_BORDER COLOR_BLACK
#define DC_COLOR_TEXT COLOR_LIGHTGREEN
#define DC_COLOR_HIGHLIGHT COLOR_WHITE
#define DC_COLOR_DIM COLOR_GREEN
#define DC_COLOR_ERROR COLOR_RED
#define DC_COLOR_WARNING COLOR_YELLOW
#define DC_COLOR_EE COLOR_LIGHTBLUE
#define DC_COLOR_GRAY COLOR_GRAY2
#define DC_COLOR_GRAYBRIGHT COLOR_GRAY3
#define DC_COLOR_WAITKEY COLOR_VIOLET
#endif

typedef unsigned char BYTE;

#define OK 0
#define ERROR -1
#define ABORT +1

#define BUFFERSIZE (4*254)

// height of sceen
#define SCREENH 25
// bottom row on screen
#define BOTTOM (SCREENH-1)

// height of menu frame
#define MENUH SCREENH
// y position of menu frame
#define MENUY 0

#ifdef CHAR80
	#define SCREENW 80
// x position of menu
	#define MENUX 58
// x position of menu items
  #define MENUXT MENUX+2
// width of menu frame
  #define MENUW 15
#else
  #define SCREENW 40
// x position of menu
  #define MENUX 27
// x position of menu items
  #define MENUXT MENUX+1
// width of menu frame
  #define MENUW 13
#endif

#define GETCWD dirs[context]
#define DIRW  25
#define DIR1X 0
#define DIR1Y 0

#ifdef CHAR80
  #define DIR2X DIRW+4
  #define DIR2Y 0
#else
  #define DIR2X 0
  #define DIR2Y (DIR1Y+2+DIR1H)
#endif

#define DIRH (context?DIR2H:DIR1H)
#define DIRX (context?DIR2X:DIR1X)
#define DIRY (context?DIR2Y:DIR1Y)

extern BYTE DIR1H;
extern BYTE DIR2H;

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

#define CH_LARROW 0x5f
#define CH_UARROW 0x5e
#define CH_POUND  0x5c

#endif
