/* $Id: kconfig.c,v 1.1.1.1 2006/03/17 19:57:17 zicodxx Exp $ */
/*
THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF PARALLAX
SOFTWARE CORPORATION ("PARALLAX").  PARALLAX, IN DISTRIBUTING THE CODE TO
END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.
COPYRIGHT 1993-1999 PARALLAX SOFTWARE CORPORATION.  ALL RIGHTS RESERVED.
*/

/*
 *
 * Routines to configure keyboard, joystick, etc..
 *
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#ifdef RCS
static char rcsid[] = "$Id: kconfig.c,v 1.1.1.1 2006/03/17 19:57:17 zicodxx Exp $";
#endif

#ifdef WINDOWS
#include "desw.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "error.h"
#include "pstypes.h"
#include "gr.h"
#include "mono.h"
#include "key.h"
#include "palette.h"
#include "game.h"
#include "gamefont.h"
#include "iff.h"
#include "u_mem.h"
#include "joy.h"
#include "mouse.h"
#include "kconfig.h"
#include "gauges.h"
#include "joydefs.h"
#include "songs.h"
#include "render.h"
#include "digi.h"
#include "newmenu.h"
#include "endlevel.h"
#include "multi.h"
#include "timer.h"
#include "text.h"
#include "player.h"
#include "menu.h"
#include "automap.h"
#include "args.h"
#include "lighting.h"
#include "ai.h"
#include "cntrlcen.h"
#if defined (TACTILE)
 #include "tactile.h"
#endif

#include "collide.h"

#ifdef USE_LINUX_JOY
#include "joystick.h"
#endif

ubyte ExtGameStatus=1;

vms_vector ExtForceVec;
vms_matrix ExtApplyForceMatrix;

int ExtJoltInfo[3]={0,0,0};
int ExtXVibrateInfo[2]={0,0};
int ExtYVibrateInfo[2]={0,0};
ubyte ExtXVibrateClear=0;
ubyte ExtYVibrateClear=0;

#define TABLE_CREATION 1

// Array used to 'blink' the cursor while waiting for a keypress.
sbyte fades[64] = { 1,1,1,2,2,3,4,4,5,6,8,9,10,12,13,15,16,17,19,20,22,23,24,26,27,28,28,29,30,30,31,31,31,31,31,30,30,29,28,28,27,26,24,23,22,20,19,17,16,15,13,12,10,9,8,6,5,4,4,3,2,2,1,1 };

int invert_text[2] = { TNUM_N, TNUM_Y };

#ifndef USE_LINUX_JOY
#ifdef WINDOWS
	int joybutton_text[28] = 
	{ TNUM_BTN_1, TNUM_BTN_2, TNUM_BTN_3, TNUM_BTN_4,
	  -1, -1, -1, -1,
	  -1, -1, -1, -1,
	  -1, -1, -1, -1,
	  TNUM_HAT_L, TNUM_HAT_R, TNUM_HAT_U, TNUM_HAT_D,
	  -1, -1, -1, -1,
	  -1, -1, -1, -1
	};
	int joyaxis_text[7] = { TNUM_X1, TNUM_Y1, TNUM_Z1, TNUM_UN, TNUM_P1,TNUM_R1,TNUM_YA1 };
#elif defined(SDL_INPUT)
char *joybutton_text[JOY_MAX_BUTTONS];
char *joyaxis_text[JOY_MAX_AXES];
#else
	int joybutton_text[28] = 
	{ TNUM_BTN_1, TNUM_BTN_2, TNUM_BTN_3, TNUM_BTN_4,
	  -1, TNUM_TRIG, TNUM_LEFT, TNUM_HAT_L,
	 TNUM_RIGHT, -1, TNUM_HAT2_D, TNUM_HAT_R,
	 TNUM_MID, -1, TNUM_HAT2_R, TNUM_HAT_U,
	 TNUM_HAT2_L, -1, TNUM_HAT2_U, TNUM_HAT_D,
	 TNUM_TRIG, TNUM_LEFT, TNUM_RIGHT, -1, 
	 TNUM_UP, TNUM_DOWN, TNUM_LEFT, TNUM_RIGHT };

	int joyaxis_text[7] = { TNUM_X1, TNUM_Y1, TNUM_Z1, TNUM_UN, TNUM_P1,TNUM_R1,TNUM_YA1 };
#endif
#endif

int mouseaxis_text[3] = { TNUM_L_R, TNUM_F_B, TNUM_Z1 };
#ifndef MACINTOSH
int mousebutton_text[3] = { TNUM_LEFT, TNUM_RIGHT, TNUM_MID };
char * mousebutton_textra[13] = { "M4", "M5", "M6", "M7", "M8", "M9", "M10","M11","M12","M13","M14","M15","M16" };//text for buttons above 3. -MPM
#else
char *mousebutton_text[3] = { "Btn", "", "" };		// only one silly mouse button on the mac
#endif

#ifdef MACINTOSH
char * key_text[256] = {
"","S","D","F","H","G","Z","X","C","V","","B","Q", "W", "E", "R",
"Y","T","1","2","3","4","6","5","=","9","7","-", "8", "0", "]", "O",
"U","[","I","P","RET","L","J","'","K", ";", "\\", ",", "/", "N", "M", ".",
"TAB","SPC","`","DEL","","ESC","","APL","SHIFT","CAPSL","OPTN","CTRL","","","","A",
"","PAD.","","PAD*","","PAD+","","NMLCK","","","","PAD/","ENTER","","PAD-","",
"","PAD=","PAD0","PAD1","PAD2","PAD3","PAD4","PAD5","PAD6","PAD7","","PAD8","PAD9","","","",
"F5","F6","F7","","F8","F9","","F11","","F13","","F14","","F10","","F12",
"","PAUSE","HELP","HOME","PGUP","DEL","","END","F2","","F1","LARW","RARW","DARW","UARW","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","","",
"","","","","","","","","","","","","","","",""};
#else
#if !defined OGL && !defined SDL_INPUT
char * key_text[256] = {         \
"","ESC","1","2","3","4","5","6","7","8","9","0","-", 			\
"=","BSPC","TAB","Q","W","E","R","T","Y","U","I","O",				\
"P","[","]","","LCTRL","A","S","D","F",        \
"G","H","J","K","L",";","'","`",        \
"LSHFT","\\","Z","X","C","V","B","N","M",",",      \
".","/","RSHFT","PAD*","LALT","SPC",      \
"CPSLK","F1","F2","F3","F4","F5","F6","F7","F8","F9",        \
"F10","NMLCK","SCLK","PAD7","PAD8","PAD9","PAD-",   \
"PAD4","PAD5","PAD6","PAD+","PAD1","PAD2","PAD3","PAD0", \
"PAD.","","","","F11","F12","","","","","","","","","",         \
"","","","","","","","","","","","","","","","","","","","",     \
"","","","","","","","","","","","","","","","","","","","",     \
"","","","","","","","","","","","","","","","","","",           \
"PAD","RCTRL","","","","","","","","","","","","","", \
"","","","","","","","","","","PAD/","","","RALT","",      \
"","","","","","","","","","","","","","HOME","","PGUP",     \
"","","","","","END","","PGDN","INS",       \
"DEL","","","","","","","","","","","","","","","","","",     \
"","","","","","","","","","","","","","","","","","","","",     \
"","","","","","","" };
#endif /* OGL */
#endif

#ifdef D2X_KEYS
//added/removed by Victor Rachels for adding rebindable keys for these
// KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0
ubyte system_keys[] = { KEY_ESC, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, KEY_MINUS, KEY_EQUAL, KEY_PRINT_SCREEN };
#else
ubyte system_keys[] = { KEY_ESC, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_MINUS, KEY_EQUAL, KEY_PRINT_SCREEN };
#endif

//extern void GameLoop(int, int );

extern void transfer_energy_to_shield(fix);
extern void CyclePrimary(),CycleSecondary(),InitMarkerInput();
extern ubyte DefiningMarkerMessage;
extern char CybermouseActive;

#ifdef WINDOWS
extern int joydefsw_do_button();
extern int joydefsw_do_winjoybutton(int *axis);
extern joydefsw_win_joyselect(char *title);
#endif

control_info Controls;

fix Cruise_speed=0;

// macros for drawing lo/hi res kconfig screens (see scores.c as well)

#define LHX(x)		((x)*(MenuHires?FONTSCALE_X(2):FONTSCALE_X(1)))
#define LHY(y)		((y)*(MenuHires?FONTSCALE_Y(2.4):FONTSCALE_Y(1)))


#define BT_KEY 			0
#define BT_MOUSE_BUTTON 	1
#define BT_MOUSE_AXIS		2
#define BT_JOY_BUTTON 		3
#define BT_JOY_AXIS		4
#define BT_INVERT		5

char *btype_text[] = { "BT_KEY", "BT_MOUSE_BUTTON", "BT_MOUSE_AXIS", "BT_JOY_BUTTON", "BT_JOY_AXIS", "BT_INVERT" };

#define INFO_Y 28

typedef struct kc_item {
	short id;				// The id of this item
	short x, y;
	short w1;
	short w2;
	short u,d,l,r;
        //short text_num1;
        char *text;
	ubyte type;
	ubyte value;		// what key,button,etc
} kc_item;

int Num_items=28;
kc_item *All_items;

ubyte kconfig_settings[CONTROL_MAX_TYPES][MAX_CONTROLS];

//added on 2/4/99 by Victor Rachels to add d1x new keys
ubyte kconfig_d2x_settings[MAX_D2X_CONTROLS];
//end this section addition - VR

//----------- WARNING!!!!!!! -------------------------------------------
// THESE NEXT FOUR BLOCKS OF DATA ARE GENERATED BY PRESSING DEL+F12 WHEN
// IN THE KEYBOARD CONFIG SCREEN.  BASICALLY, THAT PROCEDURE MODIFIES THE
// U,D,L,R FIELDS OF THE ARRAYS AND DUMPS THE NEW ARRAYS INTO KCONFIG.COD
//-------------------------------------------------------------------------
/*ubyte default_kconfig_settings[CONTROL_MAX_TYPES][MAX_CONTROLS] = {
{0xc8,0x48,0xd0,0x50,0xcb,0x4b,0xcd,0x4d,0x38,0xff,0xff,0x4f,0xff,0x51,0xff,0x4a,0xff,0x4e,0xff,0xff,0x10,0x47,0x12,0x49,0x1d,0x9d,0x39,0xff,0x21,0xff,0x1e,0xff,0x2c,0xff,0x30,0xff,0x13,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf,0xff,0x1f,0xff,0x33,0xff,0x34,0xff,0x23,0xff,0x14,0xff,0xff,0xff,0x0,0x0},
{0x0,0x1,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x1,0x0,0x0,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
{0x5,0xc,0xff,0xff,0xff,0xff,0x7,0xf,0x13,0xb,0xff,0x6,0x8,0x1,0x0,0x0,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
{0x0,0x1,0xff,0xff,0x2,0xff,0x7,0xf,0x13,0xb,0xff,0xff,0xff,0x1,0x0,0x0,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0xff,0xff,0x3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
{0x3,0x0,0x1,0x2,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x1,0x0,0x0,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
{0x0,0x1,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x1,0x0,0x0,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
{0x0,0x1,0xff,0xff,0x2,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
};*/                                                                              

#ifndef MACINTOSH
ubyte default_kconfig_settings[CONTROL_MAX_TYPES][MAX_CONTROLS] = {
{0xc8,0x48,0xd0,0x50,0xcb,0x4b,0xcd,0x4d,0x38,0xff,0xff,0x4f,0xff,0x51,0xff,0x4a,0xff,0x4e,0xff,0xff,0x10,0x47,0x12,0x49,0x1d,0x9d,0x39,0xff,0x21,0xff,0x1e,0xff,0x2c,0xff,0x30,0xff,0x13,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf,0xff,0x1f,0xff,0x33,0xff,0x34,0xff,0x23,0xff,0x14,0xff,0xff,0xff,0x0,0x0},
{0x0,0x1,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x1,0x0,0x0,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x0,0x0,0x0},
{0x5,0xc,0xff,0xff,0xff,0xff,0x7,0xf,0x13,0xb,0xff,0x6,0x8,0x1,0x0,0x0,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x0},
{0x0,0x1,0xff,0xff,0x2,0xff,0x7,0xf,0x13,0xb,0xff,0xff,0xff,0x1,0x0,0x0,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x0,0x0,0x0,0x0},
{0x3,0x0,0x1,0x2,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x1,0x0,0x0,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x0,0x0,0x0,0x0},
{0x0,0x1,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x1,0x0,0x0,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x0,0x0,0x0,0x0},
{0x0,0x1,0xff,0xff,0x2,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x0,0x0,0x0},
#ifdef WINDOWS
{0x0,0x1,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x1,0x0,0x0,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x0,0x0,0x0},
#endif
};



kc_item kc_keyboard[NUM_KEY_CONTROLS] = {
	{  0, 15, 49, 71, 26, 55,  2, 55,  1,"Pitch forward", BT_KEY, 255 },
	{  1, 15, 49,100, 26, 50,  3,  0, 24,"Pitch forward", BT_KEY, 255 },
	{  2, 15, 57, 71, 26,  0,  4, 25,  3,"Pitch backward", BT_KEY, 255 },
	{  3, 15, 57,100, 26,  1,  5,  2, 26,"Pitch backward", BT_KEY, 255 },
	{  4, 15, 65, 71, 26,  2,  6, 27,  5,"Turn left", BT_KEY, 255 },
	{  5, 15, 65,100, 26,  3,  7,  4, 28,"Turn left", BT_KEY, 255 },
	{  6, 15, 73, 71, 26,  4,  8, 29,  7,"Turn right", BT_KEY, 255 },
	{  7, 15, 73,100, 26,  5,  9,  6, 34,"Turn right", BT_KEY, 255 },
	{  8, 15, 85, 71, 26,  6, 10, 35,  9,"Slide on", BT_KEY, 255 },
	{  9, 15, 85,100, 26,  7, 11,  8, 36,"Slide on", BT_KEY, 255 },
	{ 10, 15, 93, 71, 26,  8, 12, 37, 11,"Slide left", BT_KEY, 255 },
	{ 11, 15, 93,100, 26,  9, 13, 10, 44,"Slide left", BT_KEY, 255 },
	{ 12, 15,101, 71, 26, 10, 14, 45, 13,"Slide right", BT_KEY, 255 },
	{ 13, 15,101,100, 26, 11, 15, 12, 30,"Slide right", BT_KEY, 255 },
	{ 14, 15,109, 71, 26, 12, 16, 31, 15,"Slide up", BT_KEY, 255 },
	{ 15, 15,109,100, 26, 13, 17, 14, 32,"Slide up", BT_KEY, 255 },
	{ 16, 15,117, 71, 26, 14, 18, 33, 17,"Slide down", BT_KEY, 255 },
	{ 17, 15,117,100, 26, 15, 19, 16, 46,"Slide down", BT_KEY, 255 },
	{ 18, 15,129, 71, 26, 16, 20, 47, 19,"Bank on", BT_KEY, 255 },
	{ 19, 15,129,100, 26, 17, 21, 18, 38,"Bank on", BT_KEY, 255 },
	{ 20, 15,137, 71, 26, 18, 22, 39, 21,"Bank left", BT_KEY, 255 },
	{ 21, 15,137,100, 26, 19, 23, 20, 40,"Bank left", BT_KEY, 255 },
	{ 22, 15,145, 71, 26, 20, 48, 41, 23,"Bank right", BT_KEY, 255 },
	{ 23, 15,145,100, 26, 21, 49, 22, 42,"Bank right", BT_KEY, 255 },
	{ 24,158, 49, 83, 26, 51, 26,  1, 25,"Fire primary", BT_KEY, 255 },
	{ 25,158, 49,112, 26, 54, 27, 24,  2,"Fire primary", BT_KEY, 255 },
	{ 26,158, 57, 83, 26, 24, 28,  3, 27,"Fire secondary", BT_KEY, 255 },
	{ 27,158, 57,112, 26, 25, 29, 26,  4,"Fire secondary", BT_KEY, 255 },
	{ 28,158, 65, 83, 26, 26, 34,  5, 29,"Fire flare", BT_KEY, 255 },
	{ 29,158, 65,112, 26, 27, 35, 28,  6,"Fire flare", BT_KEY, 255 },
	{ 30,158,105, 83, 26, 44, 32, 13, 31,"Accelerate", BT_KEY, 255 },
	{ 31,158,105,112, 26, 45, 33, 30, 14,"Accelerate", BT_KEY, 255 },
	{ 32,158,113, 83, 26, 30, 46, 15, 33,"reverse", BT_KEY, 255 },
	{ 33,158,113,112, 26, 31, 47, 32, 16,"reverse", BT_KEY, 255 },
	{ 34,158, 73, 83, 26, 28, 36,  7, 35,"Drop Bomb", BT_KEY, 255 },
	{ 35,158, 73,112, 26, 29, 37, 34,  8,"Drop Bomb", BT_KEY, 255 },
	{ 36,158, 85, 83, 26, 34, 44,  9, 37,"REAR VIEW", BT_KEY, 255 },
	{ 37,158, 85,112, 26, 35, 45, 36, 10,"REAR VIEW", BT_KEY, 255 },
	{ 38,158,133, 83, 26, 46, 40, 19, 39,"Cruise Faster", BT_KEY, 255 },
	{ 39,158,133,112, 26, 47, 41, 38, 20,"Cruise Faster", BT_KEY, 255 },
	{ 40,158,141, 83, 26, 38, 42, 21, 41,"Cruise Slower", BT_KEY, 255 },
	{ 41,158,141,112, 26, 39, 43, 40, 22,"Cruise Slower", BT_KEY, 255 },
	{ 42,158,149, 83, 26, 40, 52, 23, 43,"Cruise Off", BT_KEY, 255 },
	{ 43,158,149,112, 26, 41, 53, 42, 48,"Cruise Off", BT_KEY, 255 },
	{ 44,158, 93, 83, 26, 36, 30, 11, 45,"Automap", BT_KEY, 255 },
	{ 45,158, 93,112, 26, 37, 31, 44, 12,"Automap", BT_KEY, 255 },
	{ 46,158,121, 83, 26, 32, 38, 17, 47,"Afterburner", BT_KEY, 255 },
	{ 47,158,121,112, 26, 33, 39, 46, 18,"Afterburner", BT_KEY, 255 },
	{ 48, 15,161, 71, 26, 22, 50, 43, 49,"Cycle Primary", BT_KEY, 255 },
	{ 49, 15,161,100, 26, 23, 51, 48, 52,"Cycle Primary", BT_KEY, 255 },
	{ 50, 15,169, 71, 26, 48,  1, 53, 51,"Cycle Second", BT_KEY, 255 },
	{ 51, 15,169,100, 26, 49, 24, 50, 54,"Cycle Second", BT_KEY, 255 },
	{ 52,158,163, 83, 26, 42, 54, 49, 53,"Headlight", BT_KEY, 255 },
	{ 53,158,163,112, 26, 43, 55, 52, 50,"Headlight", BT_KEY, 255 },
	{ 54,158,171, 83, 26, 52, 56, 51, 55,"Energy->Shield", BT_KEY, 255 },
	{ 55,158,171,112, 26, 53,  0, 54,  0,"Energy->Shield", BT_KEY, 255 },
   { 56,158,179,83,  26, 54,  0, 0,  0, "Toggle Bomb",  BT_KEY,255},
};
kc_item kc_joystick[NUM_OTHER_CONTROLS] = {
	{  0, 25, 46, 85, 40, 15,  1, 24,  5,"Fire primary", BT_JOY_BUTTON, 255 },
	{  1, 25, 54, 85, 40,  0,  4,  5,  6,"Fire secondary", BT_JOY_BUTTON, 255 },
	{  2, 25, 85, 85, 40, 26,  3,  9, 10,"Accelerate", BT_JOY_BUTTON, 255 },
	{  3, 25, 93, 85, 40,  2, 25, 10, 11,"reverse", BT_JOY_BUTTON, 255 },
	{  4, 25, 62, 85, 40,  1, 26,  6,  7,"Fire flare", BT_JOY_BUTTON, 255 },
	{  5,180, 46, 79, 40, 23,  6,  0,  1,"Slide on", BT_JOY_BUTTON, 255 },
	{  6,180, 54, 79, 40,  5,  7,  1,  4,"Slide left", BT_JOY_BUTTON, 255 },
	{  7,180, 62, 79, 40,  6,  8,  4, 26,"Slide right", BT_JOY_BUTTON, 255 },
	{  8,180, 70, 79, 40,  7,  9, 26,  9,"Slide up", BT_JOY_BUTTON, 255 },
	{  9,180, 78, 79, 40,  8, 10,  8,  2,"Slide down", BT_JOY_BUTTON, 255 },
	{ 10,180, 90, 79, 40,  9, 11,  2,  3,"Bank on", BT_JOY_BUTTON, 255 },
	{ 11,180, 98, 79, 40, 10, 12,  3, 12,"Bank left", BT_JOY_BUTTON, 255 },
	{ 12,180,106, 79, 40, 11, 28, 11, 25,"Bank right", BT_JOY_BUTTON, 255 },
	{ 13, 22,154, 51, 40, 24, 15, 30, 14,"Pitch U/D", BT_JOY_AXIS, 255 },
	{ 14, 22,154, 99,  8, 30, 16, 13, 17,"Pitch U/D", BT_INVERT, 255 },
	{ 15, 22,162, 51, 40, 13,  0, 18, 16,"Turn L/R", BT_JOY_AXIS, 255 },
	{ 16, 22,162, 99,  8, 14, 17, 15, 19,"Turn L/R", BT_INVERT, 255 },
	{ 17,164,154, 58, 40, 16, 19, 14, 18,"Slide L/R", BT_JOY_AXIS, 255 },
	{ 18,164,154,106,  8, 29, 20, 17, 15,"Slide L/R", BT_INVERT, 255 },
	{ 19,164,162, 58, 40, 17, 21, 16, 20,"Slide U/D", BT_JOY_AXIS, 255 },
	{ 20,164,162,106,  8, 18, 22, 19, 21,"Slide U/D", BT_INVERT, 255 },
	{ 21,164,172, 58, 40, 19, 23, 20, 22,"Bank L/R", BT_JOY_AXIS, 255 },
	{ 22,164,172,106,  8, 20, 24, 21, 23,"Bank L/R", BT_INVERT, 255 },
	{ 23,164,180, 58, 40, 21,  5, 22, 24,"throttle", BT_JOY_AXIS, 255 },
	{ 24,164,180,106,  8, 22, 13, 23,  0,"throttle", BT_INVERT, 255 },
	{ 25, 25,109, 85, 40,  3, 27, 12, 28,"REAR VIEW", BT_JOY_BUTTON, 255 },
	{ 26, 25, 70, 85, 40,  4,  2,  7,  8,"Drop Bomb", BT_JOY_BUTTON, 255 },
	{ 27, 25,117, 85, 40, 25, 30, 28, 29,"Afterburner", BT_JOY_BUTTON, 255 },
	{ 28,180,114, 79, 40, 12, 29, 25, 27,"Cycle Primary", BT_JOY_BUTTON, 255 },
	{ 29,180,122, 79, 40, 28, 18, 27, 30,"Cycle Secondary", BT_JOY_BUTTON, 255 },
	{ 30, 25,125, 85, 40, 27, 14, 29, 13,"Headlight", BT_JOY_BUTTON, 255 },
};
kc_item kc_superjoy[NUM_OTHER_CONTROLS] = {
	{  0, 25, 46, 85, 26, 15,  1, 24,  5,"Fire primary", BT_JOY_BUTTON, 255 },
	{  1, 25, 54, 85, 26,  0,  4,  5,  6,"Fire secondary", BT_JOY_BUTTON, 255 },
	{  2, 25, 85, 85, 26, 26,  3,  9, 10,"Accelerate", BT_JOY_BUTTON, 255 },
	{  3, 25, 93, 85, 26,  2, 25, 10, 11,"reverse", BT_JOY_BUTTON, 255 },
	{  4, 25, 62, 85, 26,  1, 26,  6,  7,"Fire flare", BT_JOY_BUTTON, 255 },
	{  5,180, 46, 79, 26, 23,  6,  0,  1,"Slide on", BT_JOY_BUTTON, 255 },
	{  6,180, 54, 79, 26,  5,  7,  1,  4,"Slide left", BT_JOY_BUTTON, 255 },
	{  7,180, 62, 79, 26,  6,  8,  4, 26,"Slide right", BT_JOY_BUTTON, 255 },
	{  8,180, 70, 79, 26,  7,  9, 26,  9,"Slide up", BT_JOY_BUTTON, 255 },
	{  9,180, 78, 79, 26,  8, 10,  8,  2,"Slide down", BT_JOY_BUTTON, 255 },
	{ 10,180, 90, 79, 26,  9, 11,  2,  3,"Bank on", BT_JOY_BUTTON, 255 },
	{ 11,180, 98, 79, 26, 10, 12,  3, 12,"Bank left", BT_JOY_BUTTON, 255 },
	{ 12,180,106, 79, 26, 11, 28, 11, 25,"Bank right", BT_JOY_BUTTON, 255 },
	{ 13, 22,154, 51, 26, 24, 15, 30, 14,"Pitch U/D", BT_JOY_AXIS, 255 },
	{ 14, 22,154, 99,  8, 30, 16, 13, 17,"Pitch U/D", BT_INVERT, 255 },
	{ 15, 22,162, 51, 26, 13,  0, 18, 16,"Turn L/R", BT_JOY_AXIS, 255 },
	{ 16, 22,162, 99,  8, 14, 17, 15, 19,"Turn L/R", BT_INVERT, 255 },
	{ 17,164,154, 58, 26, 16, 19, 14, 18,"Slide L/R", BT_JOY_AXIS, 255 },
	{ 18,164,154,106,  8, 29, 20, 17, 15,"Slide L/R", BT_INVERT, 255 },
	{ 19,164,162, 58, 26, 17, 21, 16, 20,"Slide U/D", BT_JOY_AXIS, 255 },
	{ 20,164,162,106,  8, 18, 22, 19, 21,"Slide U/D", BT_INVERT, 255 },
	{ 21,164,172, 58, 26, 19, 23, 20, 22,"Bank L/R", BT_JOY_AXIS, 255 },
	{ 22,164,172,106,  8, 20, 24, 21, 23,"Bank L/R", BT_INVERT, 255 },
	{ 23,164,180, 58, 26, 21,  5, 22, 24,"throttle", BT_JOY_AXIS, 255 },
	{ 24,164,180,106,  8, 22, 13, 23,  0,"throttle", BT_INVERT, 255 },
	{ 25, 25,109, 85, 26,  3, 27, 12, 28,"REAR VIEW", BT_JOY_BUTTON, 255 },
	{ 26, 25, 70, 85, 26,  4,  2,  7,  8,"Drop Bomb", BT_JOY_BUTTON, 255 },
	{ 27, 25,117, 85, 26, 25, 30, 28, 29,"Afterburner", BT_JOY_BUTTON, 255 },
	{ 28,180,114, 79, 26, 12, 29, 25, 27,"Cycle Primary", BT_JOY_BUTTON, 255 },
	{ 29,180,122, 79, 26, 28, 18, 27, 30,"Cycle Secondary", BT_JOY_BUTTON, 255 },
	{ 30, 25,125, 85, 26, 27, 14, 29, 13,"Headlight", BT_JOY_BUTTON, 255 },
};

kc_item kc_mouse[NUM_OTHER_CONTROLS] = {
	{  0, 25, 46, 85, 26, 12,  1, 24,  5,"Fire primary", BT_MOUSE_BUTTON, 255 },
	{  1, 25, 54, 85, 26,  0,  4,  5,  6,"Fire secondary", BT_MOUSE_BUTTON, 255 },
	{  2, 25, 85, 85, 26, 26,  3,  9, 10,"Accelerate", BT_MOUSE_BUTTON, 255 },
	{  3, 25, 93, 85, 26,  2, 25, 10, 11,"reverse", BT_MOUSE_BUTTON, 255 },
	{  4, 25, 62, 85, 26,  1, 26,  6,  7,"Fire flare", BT_MOUSE_BUTTON, 255 },
	{  5,180, 46, 59, 26, 24,  6,  0,  1,"Slide on", BT_MOUSE_BUTTON, 255 },
	{  6,180, 54, 59, 26,  5,  7,  1,  4,"Slide left", BT_MOUSE_BUTTON, 255 },
	{  7,180, 62, 59, 26,  6,  8,  4, 26,"Slide right", BT_MOUSE_BUTTON, 255 },
	{  8,180, 70, 59, 26,  7,  9, 26,  9,"Slide up", BT_MOUSE_BUTTON, 255 },
	{  9,180, 78, 59, 26,  8, 10,  8,  2,"Slide down", BT_MOUSE_BUTTON, 255 },
	{ 10,180, 90, 59, 26,  9, 11,  2,  3,"Bank on", BT_MOUSE_BUTTON, 255 },
	{ 11,180, 98, 59, 26, 10, 12,  3, 12,"Bank left", BT_MOUSE_BUTTON, 255 },
	{ 12,180,106, 59, 26, 11,  0, 11, 25,"Bank right", BT_MOUSE_BUTTON, 255 },
	{ 13,103,138, 58, 26, 27, 15, 25, 14,"Pitch U/D", BT_MOUSE_AXIS, 255 },
	{ 14,103,138,106,  8, 23, 16, 13, 15,"Pitch U/D", BT_INVERT, 255 },
	{ 15,103,146, 58, 26, 13, 17, 14, 16,"Turn L/R", BT_MOUSE_AXIS, 255 },
	{ 16,103,146,106,  8, 14, 18, 15, 17,"Turn L/R", BT_INVERT, 255 },
	{ 17,103,154, 58, 26, 15, 19, 16, 18,"Slide L/R", BT_MOUSE_AXIS, 255 },
	{ 18,103,154,106,  8, 16, 20, 17, 19,"Slide L/R", BT_INVERT, 255 },
	{ 19,103,162, 58, 26, 17, 21, 18, 20,"Slide U/D", BT_MOUSE_AXIS, 255 },
	{ 20,103,162,106,  8, 18, 22, 19, 21,"Slide U/D", BT_INVERT, 255 },
	{ 21,103,170, 58, 26, 19, 23, 20, 22,"Bank L/R", BT_MOUSE_AXIS, 255 },
	{ 22,103,170,106,  8, 20, 24, 21, 23,"Bank L/R", BT_INVERT, 255 },
	{ 23,103,182, 58, 26, 21, 14, 22, 24,"throttle", BT_MOUSE_AXIS, 255 },
	{ 24,103,182,106,  8, 22,  5, 23,  0,"throttle", BT_INVERT, 255 },
	{ 25, 25,109, 85, 26,  3, 27, 12, 13,"REAR VIEW", BT_MOUSE_BUTTON, 255 },
	{ 26, 25, 70, 85, 26,  4,  2,  7,  8,"Drop Bomb", BT_MOUSE_BUTTON, 255 },
	{ 27, 25,117, 85, 26, 25, 13, 25, 13,"Afterburner", BT_MOUSE_BUTTON, 255 },
};

#ifdef D2X_KEYS
//added on 2/4/99 by Victor Rachels to add d1x new keys
kc_item kc_d2x[NUM_D2X_CONTROLS] = {
//        id,x,y,w1,w2,u,d,l,r,text_num1,type,value
	{  0, 15, 49, 71, 26, 27,  2, 27,  1, "WEAPON 1", BT_KEY, 255},
	{  1, 15, 49,100, 26, 26,  3,  0,  2, "WEAPON 1", BT_JOY_BUTTON, 255},
	{  2, 15, 57, 71, 26,  0,  4,  1,  3, "WEAPON 2", BT_KEY, 255},
	{  3, 15, 57,100, 26,  1,  5,  2,  4, "WEAPON 2", BT_JOY_BUTTON, 255},
	{  4, 15, 65, 71, 26,  2,  6,  3,  5, "WEAPON 3", BT_KEY, 255},
	{  5, 15, 65,100, 26,  3,  7,  4,  6, "WEAPON 3", BT_JOY_BUTTON, 255},
	{  6, 15, 73, 71, 26,  4,  8,  5,  7, "WEAPON 4", BT_KEY, 255},
	{  7, 15, 73,100, 26,  5,  9,  6,  8, "WEAPON 4", BT_JOY_BUTTON, 255},
	{  8, 15, 81, 71, 26,  6, 10,  7,  9, "WEAPON 5", BT_KEY, 255},
	{  9, 15, 81,100, 26,  7, 11,  8, 10, "WEAPON 5", BT_JOY_BUTTON, 255},

	{ 10, 15, 89, 71, 26,  8, 12,  9, 11, "WEAPON 6", BT_KEY, 255},
	{ 11, 15, 89,100, 26,  9, 13, 10, 12, "WEAPON 6", BT_JOY_BUTTON, 255},
	{ 12, 15, 97, 71, 26, 10, 14, 11, 13, "WEAPON 7", BT_KEY, 255},
	{ 13, 15, 97,100, 26, 11, 15, 12, 14, "WEAPON 7", BT_JOY_BUTTON, 255},
	{ 14, 15,105, 71, 26, 12, 16, 13, 15, "WEAPON 8", BT_KEY, 255},
	{ 15, 15,105,100, 26, 13, 17, 14, 16, "WEAPON 8", BT_JOY_BUTTON, 255},
	{ 16, 15,113, 71, 26, 14, 18, 15, 17, "WEAPON 9", BT_KEY, 255},
	{ 17, 15,113,100, 26, 15, 19, 16, 18, "WEAPON 9", BT_JOY_BUTTON, 255},
	{ 18, 15,121, 71, 26, 16, 20, 17, 19, "WEAPON 0", BT_KEY, 255},
	{ 19, 15,121,100, 26, 17, 21, 18, 20, "WEAPON 0", BT_JOY_BUTTON, 255},

	//{ 20, 15,131, 71, 26, 18, 22, 19, 21, "CYC PRIMARY", BT_KEY, 255},
	//{ 21, 15,131,100, 26, 19, 23, 20, 22, "CYC PRIMARY", BT_JOY_BUTTON, 255},
	//{ 22, 15,139, 71, 26, 20, 24, 21, 23, "CYC SECONDARY", BT_KEY, 255},
	//{ 23, 15,139,100, 26, 21, 25, 22, 24, "CYC SECONDARY", BT_JOY_BUTTON, 255},
	//{ 24,  8,147, 78, 26, 22, 26, 23, 25, "TOGGLE_PRIM AUTO", BT_KEY, 255},
	//{ 25,  8,147,107, 26, 23, 27, 24, 26, "TOGGLE_PRIM_AUTO", BT_JOY_BUTTON, 255},
	//{ 26,  8,155, 78, 26, 24,  1, 25, 27, "TOGGLE SEC AUTO", BT_KEY, 255},
	//{ 27,  8,155,107, 26, 25,  0, 26,  0, "TOGGLE SEC AUTO", BT_JOY_BUTTON, 255},
};
//end this section addition - VR
#endif

#else		// ifndef MACINTOSH (following are macintosh controls)

ubyte default_kconfig_settings[CONTROL_MAX_TYPES][MAX_CONTROLS] = {
{0x5b,0x7e,0x54,0x7d,0x56,0x7b,0x58,0x7c,0x3a,0xff,0xff,0x53,0xff,0x55,0xff,0x4e,0xff,0x45,0xff,0xff,0xc,0x59,0xe,0x5c,0x3b,0x24,0x31,0xff,0x3,0xff,0x3f,0xff,0x6,0xff,0xb,0xff,0xf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x30,0xff,0x1,0xff,0x2b,0xff,0x2f,0xff,0x4,0xff,0x11,0xff,0xff,0xff,0x0,0x0},
{0x0,0x1,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x1,0x0,0x0,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x0,0x0,0x0},
{0x0,0x3,0xff,0xff,0xff,0xff,0xb,0xc,0x9,0xa,0xff,0x1,0x2,0x1,0x0,0x0,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x0},
{0x5,0x4,0xff,0xff,0x6,0xff,0x3,0x2,0x0,0x1,0xff,0x8,0xa,0x1,0x0,0x0,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0x3,0x1,0xb,0x7,0xd,0xe,0xf,0xc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x0,0x0,0x0,0x0},
{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x1,0x0,0x0,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x0,0x0,0x0,0x0},
{0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x1,0x0,0x0,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x0,0x0,0x0,0x0},
{0x0,0x1,0xff,0xff,0x2,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x0,0x0,0x0},
};

ubyte default_firebird_settings[MAX_CONTROLS] =
{0x0,0x1,0xff,0xff,0x2,0xff,0x4,0x6,0x5,0x7,0xff,0xb,0xc,0x1,0x0,0x0,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0x03,0x0,0xff,0x3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x0,0x0,0x0,0x0};

ubyte default_mousestick_settings[MAX_CONTROLS] =
{0x2,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x3,0x4,0x1,0x0,0x0,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x0,0x0,0x0,0x0};

kc_item kc_keyboard[NUM_KEY_CONTROLS] = {
	{  0, 15, 49, 71, 26, 55,  2, 55,  1,"Pitch forward", BT_KEY, 255 },
	{  1, 15, 49,100, 26, 50,  3,  0, 24,"Pitch forward", BT_KEY, 255 },
	{  2, 15, 57, 71, 26,  0,  4, 25,  3,"Pitch backward", BT_KEY, 255 },
	{  3, 15, 57,100, 26,  1,  5,  2, 26,"Pitch backward", BT_KEY, 255 },
	{  4, 15, 65, 71, 26,  2,  6, 27,  5,"Turn left", BT_KEY, 255 },
	{  5, 15, 65,100, 26,  3,  7,  4, 28,"Turn left", BT_KEY, 255 },
	{  6, 15, 73, 71, 26,  4,  8, 29,  7,"Turn right", BT_KEY, 255 },
	{  7, 15, 73,100, 26,  5,  9,  6, 34,"Turn right", BT_KEY, 255 },
	{  8, 15, 85, 71, 26,  6, 10, 35,  9,"Slide on", BT_KEY, 255 },
	{  9, 15, 85,100, 26,  7, 11,  8, 36,"Slide on", BT_KEY, 255 },
	{ 10, 15, 93, 71, 26,  8, 12, 37, 11,"Slide left", BT_KEY, 255 },
	{ 11, 15, 93,100, 26,  9, 13, 10, 44,"Slide left", BT_KEY, 255 },
	{ 12, 15,101, 71, 26, 10, 14, 45, 13,"Slide right", BT_KEY, 255 },
	{ 13, 15,101,100, 26, 11, 15, 12, 30,"Slide right", BT_KEY, 255 },
	{ 14, 15,109, 71, 26, 12, 16, 31, 15,"Slide up", BT_KEY, 255 },
	{ 15, 15,109,100, 26, 13, 17, 14, 32,"Slide up", BT_KEY, 255 },
	{ 16, 15,117, 71, 26, 14, 18, 33, 17,"Slide down", BT_KEY, 255 },
	{ 17, 15,117,100, 26, 15, 19, 16, 46,"Slide down", BT_KEY, 255 },
	{ 18, 15,129, 71, 26, 16, 20, 47, 19,"Bank on", BT_KEY, 255 },
	{ 19, 15,129,100, 26, 17, 21, 18, 38,"Bank on", BT_KEY, 255 },
	{ 20, 15,137, 71, 26, 18, 22, 39, 21,"Bank left", BT_KEY, 255 },
	{ 21, 15,137,100, 26, 19, 23, 20, 40,"Bank left", BT_KEY, 255 },
	{ 22, 15,145, 71, 26, 20, 48, 41, 23,"Bank right", BT_KEY, 255 },
	{ 23, 15,145,100, 26, 21, 49, 22, 42,"Bank right", BT_KEY, 255 },
	{ 24,158, 49, 83, 26, 51, 26,  1, 25,"Fire primary", BT_KEY, 255 },
	{ 25,158, 49,112, 26, 54, 27, 24,  2,"Fire primary", BT_KEY, 255 },
	{ 26,158, 57, 83, 26, 24, 28,  3, 27,"Fire secondary", BT_KEY, 255 },
	{ 27,158, 57,112, 26, 25, 29, 26,  4,"Fire secondary", BT_KEY, 255 },
	{ 28,158, 65, 83, 26, 26, 34,  5, 29,"Fire flare", BT_KEY, 255 },
	{ 29,158, 65,112, 26, 27, 35, 28,  6,"Fire flare", BT_KEY, 255 },
	{ 30,158,105, 83, 26, 44, 32, 13, 31,"Accelerate", BT_KEY, 255 },
	{ 31,158,105,112, 26, 45, 33, 30, 14,"Accelerate", BT_KEY, 255 },
	{ 32,158,113, 83, 26, 30, 46, 15, 33,"reverse", BT_KEY, 255 },
	{ 33,158,113,112, 26, 31, 47, 32, 16,"reverse", BT_KEY, 255 },
	{ 34,158, 73, 83, 26, 28, 36,  7, 35,"Drop Bomb", BT_KEY, 255 },
	{ 35,158, 73,112, 26, 29, 37, 34,  8,"Drop Bomb", BT_KEY, 255 },
	{ 36,158, 85, 83, 26, 34, 44,  9, 37,"REAR VIEW", BT_KEY, 255 },
	{ 37,158, 85,112, 26, 35, 45, 36, 10,"REAR VIEW", BT_KEY, 255 },
	{ 38,158,133, 83, 26, 46, 40, 19, 39,"Cruise Faster", BT_KEY, 255 },
	{ 39,158,133,112, 26, 47, 41, 38, 20,"Cruise Faster", BT_KEY, 255 },
	{ 40,158,141, 83, 26, 38, 42, 21, 41,"Cruise Slower", BT_KEY, 255 },
	{ 41,158,141,112, 26, 39, 43, 40, 22,"Cruise Slower", BT_KEY, 255 },
	{ 42,158,149, 83, 26, 40, 52, 23, 43,"Cruise Off", BT_KEY, 255 },
	{ 43,158,149,112, 26, 41, 53, 42, 48,"Cruise Off", BT_KEY, 255 },
	{ 44,158, 93, 83, 26, 36, 30, 11, 45,"Automap", BT_KEY, 255 },
	{ 45,158, 93,112, 26, 37, 31, 44, 12,"Automap", BT_KEY, 255 },
	{ 46,158,121, 83, 26, 32, 38, 17, 47,"Afterburner", BT_KEY, 255 },
	{ 47,158,121,112, 26, 33, 39, 46, 18,"Afterburner", BT_KEY, 255 },
	{ 48, 15,161, 71, 26, 22, 50, 43, 49,"Cycle Primary", BT_KEY, 255 },
	{ 49, 15,161,100, 26, 23, 51, 48, 52,"Cycle Primary", BT_KEY, 255 },
	{ 50, 15,169, 71, 26, 48,  1, 53, 51,"Cycle Second", BT_KEY, 255 },
	{ 51, 15,169,100, 26, 49, 24, 50, 54,"Cycle Second", BT_KEY, 255 },
	{ 52,158,163, 83, 26, 42, 54, 49, 53,"Headlight", BT_KEY, 255 },
	{ 53,158,163,112, 26, 43, 55, 52, 50,"Headlight", BT_KEY, 255 },
	{ 54,158,171, 83, 26, 52, 25, 51, 55,"Energy->Shield", BT_KEY, 255 },
	{ 55,158,171,112, 26, 53,  0, 54,  0,"Energy->Shield", BT_KEY, 255 },
};
kc_item kc_joystick[NUM_OTHER_CONTROLS] = {
	{  0, 25, 46, 85, 28, 15,  1, 24,  5,"Fire primary", BT_JOY_BUTTON, 255 },
	{  1, 25, 54, 85, 28,  0,  4,  5,  6,"Fire secondary", BT_JOY_BUTTON, 255 },
	{  2, 25, 85, 85, 28, 26,  3,  9, 10,"Accelerate", BT_JOY_BUTTON, 255 },
	{  3, 25, 93, 85, 28,  2, 25, 10, 11,"reverse", BT_JOY_BUTTON, 255 },
	{  4, 25, 62, 85, 28,  1, 26,  6,  7,"Fire flare", BT_JOY_BUTTON, 255 },
	{  5,180, 46, 79, 28, 23,  6,  0,  1,"Slide on", BT_JOY_BUTTON, 255 },
	{  6,180, 54, 79, 28,  5,  7,  1,  4,"Slide left", BT_JOY_BUTTON, 255 },
	{  7,180, 62, 79, 28,  6,  8,  4, 26,"Slide right", BT_JOY_BUTTON, 255 },
	{  8,180, 70, 79, 28,  7,  9, 26,  9,"Slide up", BT_JOY_BUTTON, 255 },
	{  9,180, 78, 79, 28,  8, 10,  8,  2,"Slide down", BT_JOY_BUTTON, 255 },
	{ 10,180, 90, 79, 28,  9, 11,  2,  3,"Bank on", BT_JOY_BUTTON, 255 },
	{ 11,180, 98, 79, 28, 10, 12,  3, 12,"Bank left", BT_JOY_BUTTON, 255 },
	{ 12,180,106, 79, 28, 11, 28, 11, 25,"Bank right", BT_JOY_BUTTON, 255 },
	{ 13, 22,154, 51, 26, 24, 15, 30, 14,"Pitch U/D", BT_JOY_AXIS, 255 },
	{ 14, 22,154, 99,  8, 30, 16, 13, 17,"Pitch U/D", BT_INVERT, 255 },
	{ 15, 22,162, 51, 26, 13,  0, 18, 16,"Turn L/R", BT_JOY_AXIS, 255 },
	{ 16, 22,162, 99,  8, 14, 17, 15, 19,"Turn L/R", BT_INVERT, 255 },
	{ 17,164,154, 58, 26, 16, 19, 14, 18,"Slide L/R", BT_JOY_AXIS, 255 },
	{ 18,164,154,106,  8, 29, 20, 17, 15,"Slide L/R", BT_INVERT, 255 },
	{ 19,164,162, 58, 26, 17, 21, 16, 20,"Slide U/D", BT_JOY_AXIS, 255 },
	{ 20,164,162,106,  8, 18, 22, 19, 21,"Slide U/D", BT_INVERT, 255 },
	{ 21,164,172, 58, 26, 19, 23, 20, 22,"Bank L/R", BT_JOY_AXIS, 255 },
	{ 22,164,172,106,  8, 20, 24, 21, 23,"Bank L/R", BT_INVERT, 255 },
	{ 23,164,180, 58, 26, 21,  5, 22, 24,"throttle", BT_JOY_AXIS, 255 },
	{ 24,164,180,106,  8, 22, 13, 23,  0,"throttle", BT_INVERT, 255 },
	{ 25, 25,109, 85, 28,  3, 27, 12, 28,"REAR VIEW", BT_JOY_BUTTON, 255 },
	{ 26, 25, 70, 85, 28,  4,  2,  7,  8,"Drop Bomb", BT_JOY_BUTTON, 255 },
	{ 27, 25,117, 85, 28, 25, 30, 28, 29,"Afterburner", BT_JOY_BUTTON, 255 },
	{ 28,180,114, 79, 28, 12, 29, 25, 27,"Cycle Primary", BT_JOY_BUTTON, 255 },
	{ 29,180,122, 79, 28, 28, 18, 27, 30,"Cycle Secondary", BT_JOY_BUTTON, 255 },
	{ 30, 25,125, 85, 28, 27, 14, 29, 13,"Headlight", BT_JOY_BUTTON, 255 },
};
kc_item kc_superjoy[NUM_OTHER_CONTROLS] = {
	{  0, 25, 46, 85, 26, 15,  1, 24,  5,"Fire primary", BT_JOY_BUTTON, 255 },
	{  1, 25, 54, 85, 26,  0,  4,  5,  6,"Fire secondary", BT_JOY_BUTTON, 255 },
	{  2, 25, 85, 85, 26, 26,  3,  9, 10,"Accelerate", BT_JOY_BUTTON, 255 },
	{  3, 25, 93, 85, 26,  2, 25, 10, 11,"reverse", BT_JOY_BUTTON, 255 },
	{  4, 25, 62, 85, 26,  1, 26,  6,  7,"Fire flare", BT_JOY_BUTTON, 255 },
	{  5,180, 46, 79, 26, 23,  6,  0,  1,"Slide on", BT_JOY_BUTTON, 255 },
	{  6,180, 54, 79, 26,  5,  7,  1,  4,"Slide left", BT_JOY_BUTTON, 255 },
	{  7,180, 62, 79, 26,  6,  8,  4, 26,"Slide right", BT_JOY_BUTTON, 255 },
	{  8,180, 70, 79, 26,  7,  9, 26,  9,"Slide up", BT_JOY_BUTTON, 255 },
	{  9,180, 78, 79, 26,  8, 10,  8,  2,"Slide down", BT_JOY_BUTTON, 255 },
	{ 10,180, 90, 79, 26,  9, 11,  2,  3,"Bank on", BT_JOY_BUTTON, 255 },
	{ 11,180, 98, 79, 26, 10, 12,  3, 12,"Bank left", BT_JOY_BUTTON, 255 },
	{ 12,180,106, 79, 26, 11, 28, 11, 25,"Bank right", BT_JOY_BUTTON, 255 },
	{ 13, 22,154, 51, 26, 24, 15, 30, 14,"Pitch U/D", BT_JOY_AXIS, 255 },
	{ 14, 22,154, 99,  8, 30, 16, 13, 17,"Pitch U/D", BT_INVERT, 255 },
	{ 15, 22,162, 51, 26, 13,  0, 18, 16,"Turn L/R", BT_JOY_AXIS, 255 },
	{ 16, 22,162, 99,  8, 14, 17, 15, 19,"Turn L/R", BT_INVERT, 255 },
	{ 17,164,154, 58, 26, 16, 19, 14, 18,"Slide L/R", BT_JOY_AXIS, 255 },
	{ 18,164,154,106,  8, 29, 20, 17, 15,"Slide L/R", BT_INVERT, 255 },
	{ 19,164,162, 58, 26, 17, 21, 16, 20,"Slide U/D", BT_JOY_AXIS, 255 },
	{ 20,164,162,106,  8, 18, 22, 19, 21,"Slide U/D", BT_INVERT, 255 },
	{ 21,164,172, 58, 26, 19, 23, 20, 22,"Bank L/R", BT_JOY_AXIS, 255 },
	{ 22,164,172,106,  8, 20, 24, 21, 23,"Bank L/R", BT_INVERT, 255 },
	{ 23,164,180, 58, 26, 21,  5, 22, 24,"throttle", BT_JOY_AXIS, 255 },
	{ 24,164,180,106,  8, 22, 13, 23,  0,"throttle", BT_INVERT, 255 },
	{ 25, 25,109, 85, 26,  3, 27, 12, 28,"REAR VIEW", BT_JOY_BUTTON, 255 },
	{ 26, 25, 70, 85, 26,  4,  2,  7,  8,"Drop Bomb", BT_JOY_BUTTON, 255 },
	{ 27, 25,117, 85, 26, 25, 30, 28, 29,"Afterburner", BT_JOY_BUTTON, 255 },
	{ 28,180,114, 79, 26, 12, 29, 25, 27,"Cycle Primary", BT_JOY_BUTTON, 255 },
	{ 29,180,122, 79, 26, 28, 18, 27, 30,"Cycle Secondary", BT_JOY_BUTTON, 255 },
	{ 30, 25,125, 85, 26, 27, 14, 29, 13,"Headlight", BT_JOY_BUTTON, 255 },
};

kc_item kc_mouse[NUM_OTHER_CONTROLS] = {
	{  0, 25, 46, 85, 26, 28,  1, 28,  5,"Fire primary", BT_MOUSE_BUTTON, 255 },
	{  1, 25, 54, 85, 26,  0,  4,  5,  6,"Fire secondary", BT_MOUSE_BUTTON, 255 },
	{  2, 25, 85, 85, 26, 26,  3,  9, 10,"Accelerate", BT_MOUSE_BUTTON, 255 },
	{  3, 25, 93, 85, 26,  2, 25, 10, 11,"reverse", BT_MOUSE_BUTTON, 255 },
	{  4, 25, 62, 85, 26,  1, 26,  6,  7,"Fire flare", BT_MOUSE_BUTTON, 255 },
	{  5,180, 46, 59, 26, 24,  6,  0,  1,"Slide on", BT_MOUSE_BUTTON, 255 },
	{  6,180, 54, 59, 26,  5,  7,  1,  4,"Slide left", BT_MOUSE_BUTTON, 255 },
	{  7,180, 62, 59, 26,  6,  8,  4, 26,"Slide right", BT_MOUSE_BUTTON, 255 },
	{  8,180, 70, 59, 26,  7,  9, 26,  9,"Slide up", BT_MOUSE_BUTTON, 255 },
	{  9,180, 78, 59, 26,  8, 10,  8,  2,"Slide down", BT_MOUSE_BUTTON, 255 },
	{ 10,180, 90, 59, 26,  9, 11,  2,  3,"Bank on", BT_MOUSE_BUTTON, 255 },
	{ 11,180, 98, 59, 26, 10, 12,  3, 12,"Bank left", BT_MOUSE_BUTTON, 255 },
	{ 12,180,106, 59, 26, 11, 28, 11, 25,"Bank right", BT_MOUSE_BUTTON, 255 },
	{ 13,103,138, 58, 26, 27, 15, 27, 14,"Pitch U/D", BT_MOUSE_AXIS, 255 },
	{ 14,103,138,106,  8, 23, 16, 13, 15,"Pitch U/D", BT_INVERT, 255 },
	{ 15,103,146, 58, 26, 13, 17, 14, 16,"Turn L/R", BT_MOUSE_AXIS, 255 },
	{ 16,103,146,106,  8, 14, 18, 15, 17,"Turn L/R", BT_INVERT, 255 },
	{ 17,103,154, 58, 26, 15, 19, 16, 18,"Slide L/R", BT_MOUSE_AXIS, 255 },
	{ 18,103,154,106,  8, 16, 20, 17, 19,"Slide L/R", BT_INVERT, 255 },
	{ 19,103,162, 58, 26, 17, 21, 18, 20,"Slide U/D", BT_MOUSE_AXIS, 255 },
	{ 20,103,162,106,  8, 18, 22, 19, 21,"Slide U/D", BT_INVERT, 255 },
	{ 21,103,170, 58, 26, 19, 23, 20, 22,"Bank L/R", BT_MOUSE_AXIS, 255 },
	{ 22,103,170,106,  8, 20, 24, 21, 23,"Bank L/R", BT_INVERT, 255 },
	{ 23,103,182, 58, 26, 21, 14, 22, 24,"throttle", BT_MOUSE_AXIS, 255 },
	{ 24,103,182,106,  8, 22,  5, 23, 28,"throttle", BT_INVERT, 255 },
	{ 25, 25,109, 85, 26,  3, 27, 12, 27,"REAR VIEW", BT_MOUSE_BUTTON, 255 },
	{ 26, 25, 70, 85, 26,  4,  2,  7,  8,"Drop Bomb", BT_MOUSE_BUTTON, 255 },
	{ 27, 25,117, 85, 26, 25, 13, 25, 13,"Afterburner", BT_MOUSE_BUTTON, 255 },
};

#endif

//added on 2/4/99 by Victor Rachels to add new keys system
ubyte default_kconfig_d2x_settings[MAX_D2X_CONTROLS] = {
 0x2 ,0xff,0x3 ,0xff,0x4 ,0xff,0x5 ,0xff,0x6 ,0xff,0x7 ,0xff,0x8 ,0xff,0x9 ,
 0xff,0xa ,0xff,0xb ,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff };
//end this section addition - VR

void kc_drawitem( kc_item *item, int is_current );
void kc_change_key( kc_item * item );
void kc_change_joybutton( kc_item * item );
void kc_change_mousebutton( kc_item * item );
void kc_next_joyaxis(kc_item *item);  //added by WraithX on 11/22/00
void kc_change_joyaxis( kc_item * item );
void kc_change_mouseaxis( kc_item * item );
void kc_change_invert( kc_item * item );
void kconfig_read_fcs( int raw_axis );
void kconfig_set_fcs_button( int btn, int button );
void kconfig_read_external_controls( void );

// the following methods added by WraithX, 4/17/00
int isJoyRotationKey(int test_key)
{
	if (test_key == kc_joystick[11].value ||
	    test_key == kc_joystick[12].value)
	{
		return 1;
	}// end if

	// else...
	return 0;
}// method isJoyRotationKey

int isMouseRotationKey(int test_key)
{
	if (test_key == kc_mouse[11].value ||
	    test_key == kc_mouse[12].value)
	{
		return 1;
	}// end if

	// else...
	return 0;
}// method isMouseRotationKey

int isKeyboardRotationKey(int test_key)
{
	if (test_key == kc_keyboard[0].value ||
	    test_key == kc_keyboard[1].value ||
	    test_key == kc_keyboard[2].value ||
	    test_key == kc_keyboard[3].value ||
	    test_key == kc_keyboard[4].value ||
	    test_key == kc_keyboard[5].value ||
	    test_key == kc_keyboard[6].value ||
	    test_key == kc_keyboard[7].value ||
	    test_key == kc_keyboard[20].value ||
	    test_key == kc_keyboard[21].value ||
	    test_key == kc_keyboard[22].value ||
	    test_key == kc_keyboard[23].value)
	{
		return 1;
	}// end if

	// else...
	return 0;
}// method isKeyboardRotationKey
// end addition - WraithX

int kconfig_is_axes_used(int axis)
{
	int i;
	for (i=0; i<NUM_OTHER_CONTROLS; i++ )	{
		if (( kc_joystick[i].type == BT_JOY_AXIS ) && (kc_joystick[i].value == axis ))
			return 1;
	}
	return 0;
}

#ifdef TABLE_CREATION
int find_item_at( kc_item * items, int nitems, int x, int y )
{
	int i;
	
	for (i=0; i<nitems; i++ )	{
		if ( ((items[i].x+items[i].w1)==x) && (items[i].y==y))
			return i;
	}
	return -1;
}

int find_next_item_up( kc_item * items, int nitems, int citem )
{
	int x, y, i;

	y = items[citem].y;
	x = items[citem].x+items[citem].w1;
	
	do {	
		y--;
		if ( y < 0 ) {
			y = grd_curcanv->cv_bitmap.bm_h-1;
			x--;
			if ( x < 0 ) {
				x = grd_curcanv->cv_bitmap.bm_w-1;
			}
		}
		i = find_item_at( items, nitems, x, y );
	} while ( i < 0 );
	
	return i;
}

int find_next_item_down( kc_item * items, int nitems, int citem )
{
	int x, y, i;

	y = items[citem].y;
	x = items[citem].x+items[citem].w1;
	
	do {	
		y++;
		if ( y > grd_curcanv->cv_bitmap.bm_h-1 ) {
			y = 0;
			x++;
			if ( x > grd_curcanv->cv_bitmap.bm_w-1 ) {
				x = 0;
			}
		}
		i = find_item_at( items, nitems, x, y );
	} while ( i < 0 );
	
	return i;
}

int find_next_item_right( kc_item * items, int nitems, int citem )
{
	int x, y, i;

	y = items[citem].y;
	x = items[citem].x+items[citem].w1;
	
	do {	
		x++;
		if ( x > grd_curcanv->cv_bitmap.bm_w-1 ) {
			x = 0;
			y++;
			if ( y > grd_curcanv->cv_bitmap.bm_h-1 ) {
				y = 0;
			}
		}
		i = find_item_at( items, nitems, x, y );
	} while ( i < 0 );
	
	return i;
}

int find_next_item_left( kc_item * items, int nitems, int citem )
{
	int x, y, i;

	y = items[citem].y;
	x = items[citem].x+items[citem].w1;
	
	do {	
		x--;
		if ( x < 0 ) {
			x = grd_curcanv->cv_bitmap.bm_w-1;
			y--;
			if ( y < 0 ) {
				y = grd_curcanv->cv_bitmap.bm_h-1;
			}
		}
		i = find_item_at( items, nitems, x, y );
	} while ( i < 0 );
	
	return i;
}
#endif

#ifdef NEWMENU_MOUSE
int get_item_height(kc_item *item)
{
	int w, h, aw;
	char btext[10];

	if (item->value==255) {
		strcpy(btext, "");
	} else {
		switch( item->type )	{
			case BT_KEY:
				strncpy( btext, key_text[item->value], 10 ); break;
			case BT_MOUSE_BUTTON:
#ifndef MACINTOSH
				strncpy( btext, Text_string[mousebutton_text[item->value]], 10); break;
#else
				strncpy( btext, mousebutton_text[item->value], 10 ); break;
#endif
			case BT_MOUSE_AXIS:
				strncpy( btext, Text_string[mouseaxis_text[item->value]], 10 ); break;
			case BT_JOY_BUTTON:
#ifdef USE_LINUX_JOY
				sprintf(btext, "J%d B%d", j_button[item->value].joydev, j_Get_joydev_button_number(item->value));
#elif defined(SDL_INPUT)
				if (joybutton_text[item->value])
					strncpy(btext, joybutton_text[item->value], 10);
				else
					sprintf(btext, "BTN%2d", item->value + 1);
#else
				if ( joybutton_text[item->value] !=-1 )
					strncpy( btext, Text_string[ joybutton_text[item->value]  ], 10 );
				else
					sprintf( btext, "BTN%d", item->value );
#endif
				break;
			case BT_JOY_AXIS:
#ifdef USE_LINUX_JOY
				sprintf( btext, "J%d A%d", j_axis[item->value].joydev, j_Get_joydev_axis_number (item->value) );
#elif defined(SDL_INPUT)
				if (joyaxis_text[item->value])
					strncpy(btext, joyaxis_text[item->value], 10);
				else
					sprintf(btext, "AXIS%2d", item->value + 1);
#else
				strncpy(btext, Text_string[joyaxis_text[item->value]], 10);
#endif
				break;
			case BT_INVERT:
				strncpy( btext, Text_string[invert_text[item->value]], 10 ); break;
		}
	}
	gr_get_string_size(btext, &w, &h, &aw  );

	return h;
}
#endif

static float scale=1.0;
void kconfig_sub(kc_item * items,int nitems, char * title)
{
	grs_font * save_font;
	int old_keyd_repeat;
#ifdef NEWMENU_MOUSE
	int mouse_state, omouse_state, mx, my, x1, x2, y1, y2;
	int close_x, close_y, close_size;
#endif

	int i,k,ocitem,citem;
	int time_stopped = 0;
	grs_canvas * save_canvas, canvas;

	All_items = items;
	Num_items = nitems;

	if (!((Game_mode & GM_MULTI) && (Function_mode == FMODE_GAME) && (!Endlevel_sequence)) )
	{
		time_stopped = 1;
		stop_time();
	}

	save_canvas = grd_curcanv;

	scale=FONTSCALE_X(GAME_FONT->ft_h/5.0);
	printf("scale=%f\n",scale);
	if (MenuHires)
		gr_init_sub_canvas(&canvas, &grd_curscreen->sc_canvas, (SWIDTH - 320*scale)/2, (SHEIGHT - 240*scale)/2, 320*scale, 240*scale);
	else
		gr_init_sub_canvas(&canvas, &grd_curscreen->sc_canvas, (SWIDTH - 320*scale)/2, (SHEIGHT - 200*scale)/2, 320*scale, 200*scale);
	gr_set_current_canvas(&canvas);

	save_font = grd_curcanv->cv_font;

#ifdef WINDOWS
KConfigPaint:
#endif
	game_flush_inputs();
	old_keyd_repeat = keyd_repeat;
	keyd_repeat = 1;

	//gr_clear_canvas( BM_XRGB(0,0,0) );

	nm_draw_background(0, 0, grd_curcanv->cv_bitmap.bm_w - 1, grd_curcanv->cv_bitmap.bm_h - 1);
	gr_palette_load (gr_palette);

	grd_curcanv->cv_font = MEDIUM3_FONT;

	{
		char * p;
		p = strchr( title, '\n' );
		if ( p ) *p = 32;
		gr_string( 0x8000, LHY(8), title );
		if ( p ) *p = '\n';
	}

#ifdef NEWMENU_MOUSE
	close_x = close_y = MenuHires?FONTSCALE_X(15):FONTSCALE_X(7);
	close_size = MenuHires?FONTSCALE_X(10):FONTSCALE_X(5);
	gr_setcolor( BM_XRGB(0, 0, 0) );
	gr_rect(close_x, close_y, close_x + close_size, close_y + close_size);
	gr_setcolor( BM_XRGB(21, 21, 21) );
	gr_rect(close_x + LHX(1), close_y + LHX(1), close_x + close_size - LHX(1), close_y + close_size - LHX(1));
#endif

	grd_curcanv->cv_font = GAME_FONT;
	gr_set_fontcolor( BM_XRGB(28,28,28), -1 );

	#ifndef MACINTOSH
	gr_string( 0x8000, LHY(20), TXT_KCONFIG_STRING_1 );
	#else
	gr_string( 0x8000, LHY(20), "Enter changes, ctrl-d deletes, ctrl-r resets defaults, ESC exits");
	#endif
	gr_set_fontcolor( BM_XRGB(28,28,28), -1 );
	if ( items == kc_keyboard )	{
		gr_set_fontcolor( BM_XRGB(31,27,6), -1 );
		gr_setcolor( BM_XRGB(31,27,6) );
		
		gr_scanline( LHX(98), LHX(106), LHY(42) );
		gr_scanline( LHX(120), LHX(128), LHY(42) );
		gr_pixel( LHX(98), LHY(43) );						
		gr_pixel( LHX(98), LHY(44) );						
		gr_pixel( LHX(128), LHY(43) );						
		gr_pixel( LHX(128), LHY(44) );						
		
		gr_string( LHX(109), LHY(40), "OR" );

		gr_scanline( LHX(253), LHX(261), LHY(42) );
		gr_scanline( LHX(274), LHX(283), LHY(42) );
		gr_pixel( LHX(253), LHY(43) );						
		gr_pixel( LHX(253), LHY(44) );						
		gr_pixel( LHX(283), LHY(43) );						
		gr_pixel( LHX(283), LHY(44) );						

		gr_string( LHX(264), LHY(40), "OR" );

	} if ( items == kc_joystick )	{
		gr_set_fontcolor( BM_XRGB(31,27,6), -1 );
		gr_setcolor( BM_XRGB(31,27,6) );
		gr_scanline( LHX(18), LHX(135), LHY(37) );
		gr_scanline( LHX(181), LHX(294), LHY(37) );
		gr_scanline( LHX(18), LHX(144), LHY(119+18) );
		gr_scanline( LHX(174), LHX(294), LHY(119+18) );
		gr_string( 0x8000, LHY(35), TXT_BUTTONS );
		gr_string( 0x8000,LHY(117+18), TXT_AXES );
		gr_set_fontcolor( BM_XRGB(28,28,28), -1 );
		gr_string( LHX(81), LHY(137+8), TXT_AXIS );
		gr_string( LHX(111), LHY(137+8), TXT_INVERT );
		gr_string( LHX(230), LHY(137+8), TXT_AXIS );
		gr_string( LHX(260), LHY(137+8), TXT_INVERT );
	} else if ( items == kc_mouse )	{
		gr_set_fontcolor( BM_XRGB(31,27,6), -1 );
		gr_setcolor( BM_XRGB(31,27,6) );
		gr_scanline( LHX(18), LHX(135), LHY(37) );
		gr_scanline( LHX(181), LHX(294), LHY(37) );
		gr_scanline( LHX(18), LHX(144), LHY(119+5) );
		gr_scanline( LHX(174), LHX(294), LHY(119+5) );
		gr_string( 0x8000, LHY(35), TXT_BUTTONS );
		gr_string( 0x8000,LHY(117+5), TXT_AXES );
		gr_set_fontcolor( BM_XRGB(28,28,28), -1 );
		gr_string( LHX(169), LHY(129), TXT_AXIS );
		gr_string( LHX(199), LHY(129), TXT_INVERT );
	}
#ifdef D2X_KEYS
	else if ( items == kc_d2x )
	{
		gr_set_fontcolor( BM_XRGB(31,27,6), -1 );
		gr_setcolor( BM_XRGB(31,27,6) );

		gr_string(LHX( 94), LHY(40), "KB");
		gr_string(LHX(121), LHY(40), "JOY");
	}
#endif

	for (i=0; i<nitems; i++ )	{
		kc_drawitem( &items[i], 0 );
	}

	citem = 0;
	kc_drawitem( &items[citem], 1 );

	newmenu_show_cursor();

#ifdef NEWMENU_MOUSE
	mouse_state = omouse_state = 0;
#endif

	while(1)		{
	//	Windows addendum to allow for kconfig input.
		gr_update();

		//see if redbook song needs to be restarted
		songs_check_redbook_repeat();

		k = key_inkey();

#ifdef NEWMENU_MOUSE
		omouse_state = mouse_state;
		mouse_state = mouse_button_state(0);
#endif

		if ( !time_stopped ) {
			#ifdef NETWORK
			if (multi_menu_poll() == -1)
				k = -2;
			#endif
		}
		ocitem = citem;
		switch( k )	{
		case KEY_BACKSP:
			Int3();
			break;
		MAC(case KEY_COMMAND+KEY_SHIFTED+KEY_3:)
		case KEY_PRINT_SCREEN:
			save_screen_shot(0);
			break;							
		case KEY_CTRLED+KEY_D:
			items[citem].value = 255;
			kc_drawitem( &items[citem], 1 );
			break;
		case KEY_CTRLED+KEY_R:	
			if ( items==kc_keyboard )	{
				for (i=0; i<NUM_KEY_CONTROLS; i++ )		{
					items[i].value=default_kconfig_settings[0][i];
					kc_drawitem( &items[i], 0 );
				}
#ifdef D2X_KEYS
			} else if ( items==kc_d2x ) {
				for(i=0;i<NUM_D2X_CONTROLS;i++)
				{
					items[i].value=default_kconfig_d2x_settings[i];
					kc_drawitem( &items[i], 0 );
				}
#endif
			} else {
				#ifdef MACINTOSH
			  // hack for firebire and mousestick default controls since I made
			  // them the same control type -- dumb dumb dumb
				if (joy_have_firebird())
					for (i=0; i<NUM_OTHER_CONTROLS; i++ )	{
						items[i].value = default_firebird_settings[i];
						kc_drawitem( &items[i], 0 );
					}
				else if (joy_have_mousestick())
					for (i=0; i<NUM_OTHER_CONTROLS; i++ )	{
						items[i].value = default_mousestick_settings[i];
						kc_drawitem( &items[i], 0 );
					}
				else
				#endif	// note link to above else
					for (i=0; i<NUM_OTHER_CONTROLS; i++ )	{
						items[i].value = default_kconfig_settings[Config_control_type][i];
						kc_drawitem( &items[i], 0 );
					}
			}
			kc_drawitem( &items[citem], 1 );
			break;
		case KEY_DELETE:
			items[citem].value=255;
			kc_drawitem( &items[citem], 1 );
			break;
		case KEY_UP: 		
		case KEY_PAD8:
#ifdef TABLE_CREATION
			if (items[citem].u==-1) items[citem].u=find_next_item_up( items,nitems, citem);
#endif
			citem = items[citem].u; 
			break;
		
		case KEY_DOWN: 	
		case KEY_PAD2:
#ifdef TABLE_CREATION
			if (items[citem].d==-1) items[citem].d=find_next_item_down( items,nitems, citem);
#endif
			citem = items[citem].d; 
			break;
		case KEY_LEFT: 	
		case KEY_PAD4:
#ifdef TABLE_CREATION
			if (items[citem].l==-1) items[citem].l=find_next_item_left( items,nitems, citem);
#endif
			citem = items[citem].l; 
			break;
		case KEY_RIGHT: 	
		case KEY_PAD6:
#ifdef TABLE_CREATION
			if (items[citem].r==-1) items[citem].r=find_next_item_right( items,nitems, citem);
#endif
			citem = items[citem].r; 
			break;
		case KEY_ENTER:	
		case KEY_PADENTER:	
			switch( items[citem].type )	{
			case BT_KEY:		kc_change_key( &items[citem] ); break;
			case BT_MOUSE_BUTTON:	kc_change_mousebutton( &items[citem] ); break;
			case BT_MOUSE_AXIS: 	kc_change_mouseaxis( &items[citem] ); break;
			case BT_JOY_BUTTON: 	kc_change_joybutton( &items[citem] ); break;
			case BT_JOY_AXIS: 	kc_change_joyaxis( &items[citem] ); break;
			case BT_INVERT: 	kc_change_invert( &items[citem] ); break;
			}
			break;
		//the following case added by WraithX on 11/22/00 to work around the weird joystick bug...
		case KEY_SPACEBAR:
			switch(items[citem].type)
			{
			case BT_JOY_AXIS:
				kc_next_joyaxis(&items[citem]);
				break;
			}
			break;
		//end addition by WraithX
		case -2:	
		case KEY_ESC:
			grd_curcanv->cv_font	= save_font;

			gr_set_current_canvas( save_canvas );
			keyd_repeat = old_keyd_repeat;
			game_flush_inputs();
			newmenu_hide_cursor();
			if (time_stopped)
				start_time();
			return;
#ifdef TABLE_CREATION
		case KEY_DEBUGGED+KEY_F12:	{
			FILE * fp;
			for (i=0; i<NUM_KEY_CONTROLS; i++ )	{
				kc_keyboard[i].u = find_next_item_up( kc_keyboard,NUM_KEY_CONTROLS, i);
				kc_keyboard[i].d = find_next_item_down( kc_keyboard,NUM_KEY_CONTROLS, i);
				kc_keyboard[i].l = find_next_item_left( kc_keyboard,NUM_KEY_CONTROLS, i);
				kc_keyboard[i].r = find_next_item_right( kc_keyboard,NUM_KEY_CONTROLS, i);
			}
			for (i=0; i<NUM_OTHER_CONTROLS; i++ )	{
				kc_joystick[i].u = find_next_item_up( kc_joystick,NUM_OTHER_CONTROLS, i);
				kc_joystick[i].d = find_next_item_down( kc_joystick,NUM_OTHER_CONTROLS, i);
				kc_joystick[i].l = find_next_item_left( kc_joystick,NUM_OTHER_CONTROLS, i);
				kc_joystick[i].r = find_next_item_right( kc_joystick,NUM_OTHER_CONTROLS, i);
			}
			for (i=0; i<NUM_OTHER_CONTROLS; i++ )	{
				kc_mouse[i].u = find_next_item_up( kc_mouse,NUM_OTHER_CONTROLS, i);
				kc_mouse[i].d = find_next_item_down( kc_mouse,NUM_OTHER_CONTROLS, i);
				kc_mouse[i].l = find_next_item_left( kc_mouse,NUM_OTHER_CONTROLS, i);
				kc_mouse[i].r = find_next_item_right( kc_mouse,NUM_OTHER_CONTROLS, i);
			}
			fp = fopen( "kconfig.cod", "wt" );

			fprintf( fp, "ubyte default_kconfig_settings[CONTROL_MAX_TYPES][MAX_CONTROLS] = {\n" );
			for (i=0; i<CONTROL_MAX_TYPES; i++ )	{
				int j;
				fprintf( fp, "{0x%x", kconfig_settings[i][0] );
				for (j=1; j<MAX_CONTROLS; j++ )
					fprintf( fp, ",0x%x", kconfig_settings[i][j] );
				fprintf( fp, "},\n" );
			}
			fprintf( fp, "};\n" );
		
			fprintf( fp, "\nkc_item kc_keyboard[NUM_KEY_CONTROLS] = {\n" );
			for (i=0; i<NUM_KEY_CONTROLS; i++ )	{
				fprintf( fp, "\t{ %2d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%c%s%c, %s, 255 },\n", 
					kc_keyboard[i].id, kc_keyboard[i].x, kc_keyboard[i].y, kc_keyboard[i].w1, kc_keyboard[i].w2,
					kc_keyboard[i].u, kc_keyboard[i].d, kc_keyboard[i].l, kc_keyboard[i].r,
                                        34, kc_keyboard[i].text, 34, btype_text[kc_keyboard[i].type] );
			}
			fprintf( fp, "};" );

			fprintf( fp, "\nkc_item kc_joystick[NUM_OTHER_CONTROLS] = {\n" );
			for (i=0; i<NUM_OTHER_CONTROLS; i++ )	{
				if (kc_joystick[i].type == BT_JOY_BUTTON)
					fprintf( fp, "\t{ %2d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%c%s%c, %s, 255 },\n", 
						kc_joystick[i].id, kc_joystick[i].x, kc_joystick[i].y, kc_joystick[i].w1, kc_joystick[i].w2,
						kc_joystick[i].u, kc_joystick[i].d, kc_joystick[i].l, kc_joystick[i].r,
	                                        34, kc_joystick[i].text, 34, btype_text[kc_joystick[i].type] );
                else
					fprintf( fp, "\t{ %2d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%c%s%c, %s, 255 },\n", 
						kc_joystick[i].id, kc_joystick[i].x, kc_joystick[i].y, kc_joystick[i].w1, kc_joystick[i].w2,
						kc_joystick[i].u, kc_joystick[i].d, kc_joystick[i].l, kc_joystick[i].r,
	                                        34, kc_joystick[i].text, 34, btype_text[kc_joystick[i].type] );
			}
			fprintf( fp, "};" );

			fprintf( fp, "\nkc_item kc_mouse[NUM_OTHER_CONTROLS] = {\n" );
			for (i=0; i<NUM_OTHER_CONTROLS; i++ )	{
				fprintf( fp, "\t{ %2d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%c%s%c, %s, 255 },\n", 
					kc_mouse[i].id, kc_mouse[i].x, kc_mouse[i].y, kc_mouse[i].w1, kc_mouse[i].w2,
					kc_mouse[i].u, kc_mouse[i].d, kc_mouse[i].l, kc_mouse[i].r,
                                        34, kc_mouse[i].text, 34, btype_text[kc_mouse[i].type] );
			}
			fprintf( fp, "};" );

			fclose(fp);

			}
			break;
#endif
		}

#ifdef NEWMENU_MOUSE
		if ( (mouse_state && !omouse_state) || (mouse_state && omouse_state) ) {
			int item_height;
			
			mouse_get_pos(&mx, &my);
			for (i=0; i<nitems; i++ )	{
				item_height = get_item_height( &items[i] );
				x1 = grd_curcanv->cv_bitmap.bm_x + LHX(items[i].x) + LHX(items[i].w1);
				x2 = x1 + LHX(items[i].w2);
				y1 = grd_curcanv->cv_bitmap.bm_y + LHY(items[i].y);
				y2 = y1 + LHX(item_height);
				if (((mx > x1) && (mx < x2)) && ((my > y1) && (my < y2))) {
					citem = i;
					break;
				}
			}
		}
		else if ( !mouse_state && omouse_state ) {
			int item_height;
			
			mouse_get_pos(&mx, &my);
			item_height = get_item_height( &items[citem] );
			x1 = grd_curcanv->cv_bitmap.bm_x + LHX(items[citem].x) + LHX(items[citem].w1);
			x2 = x1 + LHX(items[citem].w2);
			y1 = grd_curcanv->cv_bitmap.bm_y + LHY(items[citem].y);
			y2 = y1 + LHY(item_height);
			if (((mx > x1) && (mx < x2)) && ((my > y1) && (my < y2))) {
				newmenu_hide_cursor();
				switch( items[citem].type )	{
				case BT_KEY:				kc_change_key( &items[citem] ); break;
				case BT_MOUSE_BUTTON:	kc_change_mousebutton( &items[citem] ); break;
				case BT_MOUSE_AXIS: 		kc_change_mouseaxis( &items[citem] ); break;
				case BT_JOY_BUTTON: 		kc_change_joybutton( &items[citem] ); break;
				case BT_JOY_AXIS: 		kc_change_joyaxis( &items[citem] ); break;
				case BT_INVERT: 			kc_change_invert( &items[citem] ); break;
				}
				newmenu_show_cursor();
			} else {
				x1 = grd_curcanv->cv_bitmap.bm_x + close_x + LHX(1);
				x2 = x1 + close_size - LHX(1);
				y1 = grd_curcanv->cv_bitmap.bm_y + close_y + LHX(1);
				y2 = y1 + close_size - LHX(1);
				if ( ((mx > x1) && (mx < x2)) && ((my > y1) && (my < y2)) ) {
					grd_curcanv->cv_font	= save_font;
					gr_set_current_canvas( save_canvas );
					keyd_repeat = old_keyd_repeat;
					game_flush_inputs();
					newmenu_hide_cursor();
					if (time_stopped)
						start_time();
					return;
				}
			}

		}
#endif // NEWMENU_MOUSE

		if (ocitem!=citem)	{
			newmenu_hide_cursor();
			kc_drawitem( &items[ocitem], 0 );
			kc_drawitem( &items[citem], 1 );
			newmenu_show_cursor();
		}
	}
}


void kc_drawitem( kc_item *item, int is_current )
{
	int x, w, h, aw;
	char btext[16];

	if (is_current)
		gr_set_fontcolor( BM_XRGB(20,20,29), -1 );
	else
		gr_set_fontcolor( BM_XRGB(15,15,24), -1 );
   gr_string( LHX(item->x), LHY(item->y), item->text );

	if (item->value==255) {
		strcpy( btext, "" );
	} else {
		switch( item->type )	{
			case BT_KEY:
				strncpy( btext, key_text[item->value], 10 ); break;
			case BT_MOUSE_BUTTON:
				#ifndef MACINTOSH
				//strncpy( btext, Text_string[mousebutton_text[item->value]], 10 ); break;
				strncpy( btext, (item->value <= 3)?Text_string[mousebutton_text[item->value]]:mousebutton_textra[item->value-3], 10 ); break;
				#else
				strncpy( btext, mousebutton_text[item->value], 10 ); break;
				#endif
			case BT_MOUSE_AXIS:
				strncpy( btext, Text_string[mouseaxis_text[item->value]], 10 ); break;
			case BT_JOY_BUTTON:
#ifdef USE_LINUX_JOY
				sprintf(btext, "J%d B%d", j_button[item->value].joydev, j_Get_joydev_button_number(item->value));
#elif defined(SDL_INPUT)
				if (joybutton_text[item->value])
					strncpy(btext, joybutton_text[item->value], 10);
				else
					sprintf(btext, "BTN%2d", item->value + 1);
#else
# ifndef MACINTOSH
#  ifdef WINDOWS
				if (joybutton_text[item->value] != -1) 
					strncpy( btext, Text_string[ joybutton_text[item->value]  ], 10 );
				else 
					sprintf( btext, "BTN%2d", item->value+1 );
#  else
				if ( joybutton_text[item->value] !=-1 )
					strncpy( btext, Text_string[ joybutton_text[item->value]  ], 10 );
				else
					sprintf( btext, "BTN%d", item->value );
#  endif
# else
				strncpy( btext, joy_btn_name( item->value ), 10);
				if (btext == NULL)
					sprintf( btext, "BTN%d", item->value );
# endif
#endif
				break;
			case BT_JOY_AXIS:
#ifdef USE_LINUX_JOY
				sprintf(btext, "J%d A%d", j_axis[item->value].joydev, j_Get_joydev_axis_number(item->value));
#elif defined(SDL_INPUT)
				if (joyaxis_text[item->value])
					strncpy(btext, joyaxis_text[item->value], 10);
				else
					sprintf(btext, "AXIS%2d", item->value + 1);
#else
				strncpy(btext, Text_string[joyaxis_text[item->value]], 10);
#endif
				break;
			case BT_INVERT:
				strncpy( btext, Text_string[invert_text[item->value]], 10 ); break;
		}
	}
	if (item->w1) {
		gr_get_string_size(btext, &w, &h, &aw  );

		if (is_current)
			gr_setcolor( BM_XRGB(21,0,24) );
		else
			gr_setcolor( BM_XRGB(16,0,19) );
		gr_urect( LHX(item->w1+item->x), LHY(item->y-1), LHX(item->w1+item->x+item->w2), LHY(item->y)+h );
		
		gr_set_fontcolor( BM_XRGB(28,28,28), -1 );

		x = LHX(item->w1+item->x)+((LHX(item->w2)-w)/2);
	
		gr_string( x, LHY(item->y), btext );
	}
}


static int looper=0;

void kc_drawquestion( kc_item *item )
{
	int c, x, w, h, aw;

	gr_get_string_size("?", &w, &h, &aw  );

	c = BM_XRGB(21,0,24);

	//@@gr_setcolor( gr_fade_table[fades[looper]*256+c] );
	gr_setcolor(BM_XRGB(21*fades[looper]/31,0,24*fades[looper]/31));
	looper++;
	if (looper>63) looper=0;

	gr_urect( LHX(item->w1+item->x), LHY(item->y-1), LHX(item->w1+item->x+item->w2), LHY(item->y)+h );
	
	gr_set_fontcolor( BM_XRGB(28,28,28), -1 );

	x = LHX(item->w1+item->x)+((LHX(item->w2)-w)/2);
   
	gr_string( x, LHY(item->y), "?" );
gr_update();
}

void kc_change_key( kc_item * item )
{
	int i,n,f,k;
	ubyte keycode;

	gr_set_fontcolor( BM_XRGB(28,28,28), -1 );
	
	gr_string( 0x8000, LHY(INFO_Y), TXT_PRESS_NEW_KEY );

	game_flush_inputs();
	keycode=255;
	k=255;
	
	while( (k!=KEY_ESC) && (keycode==255) )	
	{				
		#ifdef NETWORK
		if ((Game_mode & GM_MULTI) && (Function_mode == FMODE_GAME) && (!Endlevel_sequence))
			multi_menu_poll();
		#endif
//		if ( Game_mode & GM_MULTI )
//			GameLoop( 0, 0 );				// Continue
		k = key_inkey();
		timer_delay(f0_1/10);
		kc_drawquestion( item );
	
		for (i=0; i<256; i++ )	{
			if (keyd_pressed[i] && (strlen(key_text[i])>0))	{
				f = 0;
				for (n=0; n<sizeof(system_keys); n++ )
					if ( system_keys[n] == i )
						f=1;
				if (!f)	
					keycode=i;
			}
		}
	}

	if (k!=KEY_ESC)	{
		for (i=0; i<Num_items; i++ )	{
			n = item - All_items;
			if ( (i!=n) && (All_items[i].type==BT_KEY) && (All_items[i].value==keycode) )		{
				All_items[i].value = 255;
				kc_drawitem( &All_items[i], 0 );
			}
		}
		item->value = keycode;
	}
	kc_drawitem( item, 1 );

	gr_set_fontcolor( BM_XRGB(28,28,28), BM_XRGB(0,0,0) );

	nm_restore_background( 0, LHY(INFO_Y), LHX(310), FONTSCALE_Y(grd_curcanv->cv_font->ft_h) );

	game_flush_inputs();

}

void kc_change_joybutton( kc_item * item )
{
	int n,i,k;
	ubyte code;

	gr_set_fontcolor( BM_XRGB(28,28,28), -1 );
	
	gr_string( 0x8000, LHY(INFO_Y), TXT_PRESS_NEW_JBUTTON );

	game_flush_inputs();
	code=255;
	k=255;
	
	while( (k!=KEY_ESC) && (code==255))	
	{				
		#ifdef NETWORK
		if ((Game_mode & GM_MULTI) && (Function_mode == FMODE_GAME) && (!Endlevel_sequence))
			multi_menu_poll();
		#endif
//		if ( Game_mode & GM_MULTI )
//			GameLoop( 0, 0 );				// Continue
		k = key_inkey();
		timer_delay(f0_1/10);

		if (k == KEY_PRINT_SCREEN)
			save_screen_shot(0);

		kc_drawquestion( item );

		WIN(code = joydefsw_do_button());
		
#ifdef MACINTOSH
		code = joy_do_buttons();
#endif
		
#if !defined(WINDOWS) && !defined(MACINTOSH)
		if (Config_control_type==CONTROL_THRUSTMASTER_FCS)	{
			int axis[4];
			joystick_read_raw_axis( JOY_ALL_AXIS, axis );
			kconfig_read_fcs( axis[3] );
			if ( joy_get_button_state(7) ) code = 7;
			if ( joy_get_button_state(11) ) code = 11;
			if ( joy_get_button_state(15) ) code = 15;
			if ( joy_get_button_state(19) ) code = 19;
			for (i=0; i<4; i++ )	{
				if ( joy_get_button_state(i) )
					code = i;
			}
		} else if (Config_control_type==CONTROL_FLIGHTSTICK_PRO) {
			for (i=4; i<20; i++ )	{
				if ( joy_get_button_state(i)  )	{
					code = i;
					mprintf(( 0, "JB: %d\n", code ));
				}
			}
		} else {
			for (i = 0; i < JOY_MAX_BUTTONS; i++)
			{
				if ( joy_get_button_state(i) )
					code = i;
			}
		}
#endif
	}
	if (code!=255)	{
		for (i=0; i<Num_items; i++ )	{
			n = item - All_items;
			if ( (i!=n) && (All_items[i].type==BT_JOY_BUTTON) && (All_items[i].value==code) ) {
				All_items[i].value = 255;
				kc_drawitem( &All_items[i], 0 );
			}
		}
		item->value = code;
	}
	kc_drawitem( item, 1 );
	nm_restore_background( 0, LHY(INFO_Y), LHX(310), FONTSCALE_Y(grd_curcanv->cv_font->ft_h) );
	game_flush_inputs();
}

void kc_change_mousebutton( kc_item * item )
{
	int n,i,b,k;
	ubyte code;

	gr_set_fontcolor( BM_XRGB(28,28,28), -1 );
	
	gr_string( 0x8000, LHY(INFO_Y), TXT_PRESS_NEW_MBUTTON );

	game_flush_inputs();
	code=255;
	k=255;
	
	while( (k!=KEY_ESC) && (code==255))	
	{				
		#ifdef NETWORK
		if ((Game_mode & GM_MULTI) && (Function_mode == FMODE_GAME) && (!Endlevel_sequence))
			multi_menu_poll();
		#endif
//		if ( Game_mode & GM_MULTI )
//			GameLoop( 0, 0 );				// Continue
		k = key_inkey();
		timer_delay(f0_1/10);

		if (k == KEY_PRINT_SCREEN)
			save_screen_shot(0);

		kc_drawquestion( item );

		b = mouse_get_btns();
		for (i = 0; i < 16; i++ ) {
			if ( b & (1<<i) )	
				code = i;
		}
	}
	if (code!=255)	{
		for (i=0; i<Num_items; i++ )	{
			n = item - All_items;
			if ( (i!=n) && (All_items[i].type==BT_MOUSE_BUTTON) && (All_items[i].value==code) )		{
				All_items[i].value = 255;
				kc_drawitem( &All_items[i], 0 );
			}
		}
		item->value = code;
	}
	kc_drawitem( item, 1 );
	nm_restore_background( 0, LHY(INFO_Y), LHX(310), FONTSCALE_Y(grd_curcanv->cv_font->ft_h) );
	game_flush_inputs();

}


// the following function added by WraithX on 11/22/00 to work around the weird joystick bug... - modified my Matt Mueller to skip already allocated axes
void kc_next_joyaxis(kc_item *item)
{
	int n, i, k, max, tries;
	ubyte code = 0;

	k = 255;
	n = 0;
	i = 0;

	// I modelled this ifdef after the code in the kc_change_joyaxis method.
	// So, if somethin's not workin here, it might not be workin there either.
	max = JOY_MAX_AXES;
	tries = 1;
	code = (item->value + 1) % max;

	if (code != 255)
	{
		for (i = 0; i < Num_items; i++)
		{
			n = item - All_items;
			if ((i != n) && (All_items[i].type == BT_JOY_AXIS) && (All_items[i].value == code))
			{
				if (tries > max)
					return; // all axes allocated already
				i = -1; // -1 so the i++ will push back to 0
				code = (item->value + ++tries) % max; // try next axis
			}//end if
		}//end for

		item->value = code;
	}//end if

	kc_drawitem(item, 1);
	nm_restore_background(0, LHY(INFO_Y), LHX(310), FONTSCALE_Y(grd_curcanv->cv_font->ft_h));
	game_flush_inputs();

}//method kc_next_joyaxis
//end addition by WraithX


void kc_change_joyaxis( kc_item * item )
{
	int axis[JOY_MAX_AXES];
	int old_axis[JOY_MAX_AXES];
	int numaxis = joy_num_axes;
	int n,i,k;
	ubyte code;

	gr_set_fontcolor( BM_XRGB(28,28,28), -1 );
	
	gr_string( 0x8000, LHY(INFO_Y), TXT_MOVE_NEW_JOY_AXIS );

	game_flush_inputs();
	code=255;
	k=255;

	WINDOS(
		joystick_read_raw_axis( JOY_ALL_AXIS+JOY_EXT_AXIS, old_axis ),
		joystick_read_raw_axis( JOY_ALL_AXIS, old_axis )
	);

	while( (k!=KEY_ESC) && (code==255))	
	{				
		#ifdef NETWORK
		if ((Game_mode & GM_MULTI) && (Function_mode == FMODE_GAME) && (!Endlevel_sequence))
			multi_menu_poll();
		#endif
//		if ( Game_mode & GM_MULTI )
//			GameLoop( 0, 0 );				// Continue
		k = key_inkey();
		timer_delay(f0_1/10);

		if (k == KEY_PRINT_SCREEN)
			save_screen_shot(0);

		kc_drawquestion( item );

		WINDOS(
			joystick_read_raw_axis( JOY_ALL_AXIS+JOY_EXT_AXIS, axis ),
			joystick_read_raw_axis( JOY_ALL_AXIS, axis )
		);
		
		for (i=0; i<numaxis; i++ )	{
#if defined (MACINTOSH)
			if ( abs(axis[i]-old_axis[i])>100 )	{
#elif defined(WINDOWS)
			if ( abs(axis[i]-old_axis[i])>1024 )	{
#else 
  			if ( abs(axis[i]-old_axis[i])>200 )	{
#endif
				code = i;
				con_printf(CON_DEBUG, "Axis Movement detected: Axis %i\n", i);
			}
			//old_axis[i] = axis[i];
		}
		for (i=0; i<Num_items; i++ )	
		 {
			n = item - All_items;
			if ( (i!=n) && (All_items[i].type==BT_JOY_AXIS) && (All_items[i].value==code) )	
				code = 255;
		 }
	
	}
	if (code!=255)	{
		for (i=0; i<Num_items; i++ )	{
			n = item - All_items;
			if ( (i!=n) && (All_items[i].type==BT_JOY_AXIS) && (All_items[i].value==code) )	{
				All_items[i].value = 255;
				kc_drawitem( &All_items[i], 0 );
			}
		}

		item->value = code;					 
	}
	kc_drawitem( item, 1 );
	nm_restore_background( 0, LHY(INFO_Y), LHX(310), FONTSCALE_Y(grd_curcanv->cv_font->ft_h) );
	game_flush_inputs();

}

void kc_change_mouseaxis( kc_item * item )
{
	int i,n,k;
	ubyte code;
	int dx,dy;
#ifdef SDL_INPUT
	int dz;
#endif

	gr_set_fontcolor( BM_XRGB(28,28,28), -1 );
	
	gr_string( 0x8000, LHY(INFO_Y), TXT_MOVE_NEW_MSE_AXIS );

	game_flush_inputs();
	code=255;
	k=255;

	mouse_get_delta( &dx, &dy );

	while( (k!=KEY_ESC) && (code==255))	
	{				
		#ifdef NETWORK
		if ((Game_mode & GM_MULTI) && (Function_mode == FMODE_GAME) && (!Endlevel_sequence))
			multi_menu_poll();
		#endif
//		if ( Game_mode & GM_MULTI )
//			GameLoop( 0, 0 );				// Continue
		k = key_inkey();
		timer_delay(f0_1/10);

		if (k == KEY_PRINT_SCREEN)
			save_screen_shot(0);

		kc_drawquestion( item );

#ifdef SDL_INPUT
		mouse_get_delta_z( &dx, &dy, &dz );
#else
		mouse_get_delta( &dx, &dy );
#endif
		if ( abs(dx)>20 ) code = 0;
		if ( abs(dy)>20 ) code = 1;
#ifdef SDL_INPUT
		if ( abs(dz)>20 ) code = 2;
#endif
	}
	if (code!=255)	{
		for (i=0; i<Num_items; i++ )	{
			n = item - All_items;
			if ( (i!=n) && (All_items[i].type==BT_MOUSE_AXIS) && (All_items[i].value==code) )		{
				All_items[i].value = 255;
				kc_drawitem( &All_items[i], 0 );
			}
		}
		item->value = code;
	}
	kc_drawitem( item, 1 );
	nm_restore_background( 0, LHY(INFO_Y), LHX(310), FONTSCALE_Y(grd_curcanv->cv_font->ft_h) );
	game_flush_inputs();

}


void kc_change_invert( kc_item * item )
{
	game_flush_inputs();

	if (item->value)
		item->value = 0;
	else 
		item->value = 1;

	kc_drawitem( item, 1 );

}

#include "screens.h"

void kconfig(int n, char * title)
{
	int i;
	grs_bitmap *save_bm;

	set_screen_mode( SCREEN_MENU );

	kc_set_controls();

	//save screen
	save_bm = gr_create_bitmap( grd_curcanv->cv_bitmap.bm_w, grd_curcanv->cv_bitmap.bm_h );
	Assert( save_bm != NULL );
	
	gr_bm_bitblt(grd_curcanv->cv_bitmap.bm_w, grd_curcanv->cv_bitmap.bm_w, 
					0, 0, 0, 0, &grd_curcanv->cv_bitmap, save_bm );

	switch(n)	{
	case 0:kconfig_sub( kc_keyboard, NUM_KEY_CONTROLS, title );break;
	case 1:kconfig_sub( kc_joystick, NUM_OTHER_CONTROLS, title );break;
	case 2:kconfig_sub( kc_mouse, NUM_OTHER_CONTROLS, title ); break;
	case 3:kconfig_sub( kc_superjoy, NUM_OTHER_CONTROLS, title); break;
#ifdef D2X_KEYS
	//added on 2/4/99 by Victor Rachels for new keys menu
	case 4:kconfig_sub( kc_d2x, NUM_D2X_CONTROLS, title ); break;
	//end this section addition - VR
#endif
 	default:
		Int3();
		return;
	}

	//restore screen
	gr_bitmap(0, 0, save_bm);
	gr_free_bitmap(save_bm);

#if 0 // set_screen_mode always calls this later... right?
	reset_cockpit();		//force cockpit redraw next time
#endif

	// Update save values...
	
	for (i=0; i<NUM_KEY_CONTROLS; i++ )	
		kconfig_settings[0][i] = kc_keyboard[i].value;

	if ( (Config_control_type>0) && (Config_control_type<5)) { 
		for (i=0; i<NUM_OTHER_CONTROLS; i++ )	
			kconfig_settings[Config_control_type][i] = kc_joystick[i].value;
	} else if (Config_control_type>4 && Config_control_type<CONTROL_WINJOYSTICK) {
		for (i=0; i<NUM_OTHER_CONTROLS; i++ )	
			kconfig_settings[Config_control_type][i] = kc_mouse[i].value;
	}
	else if (Config_control_type == CONTROL_WINJOYSTICK) {
		for (i=0; i<NUM_OTHER_CONTROLS; i++ )	
			kconfig_settings[Config_control_type][i] = kc_superjoy[i].value;
	}

#ifdef D2X_KEYS
	for (i=0; i<NUM_D2X_CONTROLS; i++)
		kconfig_d2x_settings[i] = kc_d2x[i].value;
#endif
}


void kconfig_read_fcs( int raw_axis )
{
	int raw_button, button, axis_min[4], axis_center[4], axis_max[4];

	if (Config_control_type!=CONTROL_THRUSTMASTER_FCS) return;

	joy_get_cal_vals(axis_min, axis_center, axis_max);

	if ( axis_max[3] > 1 )
		raw_button = (raw_axis*100)/axis_max[3];
	else
		raw_button = 0;

	if ( raw_button > 88 )
		button = 0;
	else if ( raw_button > 63 )
		button = 7;
	else if ( raw_button > 39 )
		button = 11;
	else if ( raw_button > 15 )
		button = 15;
	else	
		button = 19;

	kconfig_set_fcs_button( 19, button );
	kconfig_set_fcs_button( 15, button );
	kconfig_set_fcs_button( 11, button );
	kconfig_set_fcs_button( 7, button );
}
		

void kconfig_set_fcs_button( int btn, int button )
{
	int state,time_down,upcount,downcount;
	state = time_down = upcount = downcount = 0;

	if ( joy_get_button_state(btn) ) {
		if ( btn==button )	{
			state = 1;
			time_down = FrameTime;
		} else {
			upcount=1;
		}
	} else {
		if ( btn==button )	{
			state = 1;
			time_down = FrameTime;
			downcount=1;
		} else {
			upcount=1;
		}
	}				
			
	joy_set_btn_values( btn, state, time_down, downcount, upcount );
					
}



fix Last_angles_p = 0;
fix Last_angles_b = 0;
fix Last_angles_h = 0;
ubyte Last_angles_read = 0;

extern int VR_sensitivity;
int VR_sense_range[3] = { 25, 50, 75 };

#define	PH_SCALE	8

#ifndef __MSDOS__ // WINDOWS
#define	JOYSTICK_READ_TIME	(F1_0/40)		//	Read joystick at 40 Hz.
#else
#define	JOYSTICK_READ_TIME	(F1_0/10)		//	Read joystick at 10 Hz.
#endif

fix	LastReadTime = 0;

fix	joy_axis[JOY_MAX_AXES];

ubyte 			kc_use_external_control = 0;
ubyte			kc_enable_external_control = 0;
ubyte 			kc_external_intno = 0;
ext_control_info	*kc_external_control = NULL;
char			*kc_external_name = NULL;
ubyte			kc_external_version = 0;
extern int Automap_active;

void kconfig_init_external_controls(int intno, int address)
{
	int i;
	kc_external_intno = intno;
	kc_external_control	= (ext_control_info *)address;
	kc_use_external_control = 1;
	kc_enable_external_control  = 1;

	i = FindArg ( "-xname" );
	if ( i )	
		kc_external_name = Args[i+1];
	else
		kc_external_name = "External Controller";

   for (i=0;i<strlen (kc_external_name);i++)
    if (kc_external_name[i]=='_')
	  kc_external_name[i]=' '; 

	i = FindArg ( "-xver" );
	if ( i )
		kc_external_version = atoi(Args[i+1]);
	
	printf( "%s int: 0x%x, data: 0x%p, ver:%d\n", kc_external_name, kc_external_intno, kc_external_control, kc_external_version );

}

#ifdef WINDOWS
void controls_read_all_win()
{
	int i;
	int slide_on, bank_on;
	int dx, dy;
	fix ctime;
	fix mouse_axis[2];
	int raw_joy_axis[7];
	int mouse_buttons;
	fix k0, k1, k2, k3, kp;
	fix k4, k5, k6, k7, kh;
	ubyte channel_masks;
	int use_mouse, use_joystick;
	int speed_factor=1;

	if (Game_turbo_mode)
		speed_factor = 2;
	
	{
		fix temp = Controls.heading_time;
		fix temp1 = Controls.pitch_time;
		memset( &Controls, 0, sizeof(control_info) );
		Controls.heading_time = temp;
		Controls.pitch_time = temp1;
	}
	slide_on = 0;
	bank_on = 0;

	ctime = timer_get_fixed_seconds();

//	DO JOYSTICK (X,Y,Z  R,U,V)
//	----------------------------------------------------------------------------
	if ( (LastReadTime + JOYSTICK_READ_TIME) > ctime ) {
		if ((ctime < 0) && (LastReadTime > 0))
			LastReadTime = ctime;
		use_joystick=1;
	} else if ((Config_control_type==CONTROL_WINJOYSTICK)) {
		LastReadTime = ctime;
		channel_masks = joystick_read_raw_axis( JOY_ALL_AXIS+JOY_EXT_AXIS, raw_joy_axis );

		for (i=0; i<7; i++ )	{
			if (channel_masks&(1<<i))	{
				int joy_null_value = 28;

			// DO POV if joystick has a hat.
			//	--------------------------------------------------------------------
				if (i == 3) {
					joydefsw_do_winjoybutton(raw_joy_axis);
				}
				else {
				// DO AXIS
				//	--------------------------------------------------------------------	
				//	mprintf((0, "(%d)=%d,", i, raw_joy_axis[i]));
					raw_joy_axis[i] = joy_get_scaled_reading( raw_joy_axis[i], i );
				//	mprintf((0, "%d  ",raw_joy_axis[i]));
				//	if (i%2) mprintf((0,"\n"));

	
					if (kc_superjoy[23].value==i)		// If this is the throttle
						joy_null_value = 32;				// Then use a larger dead-zone
	
					if (raw_joy_axis[i] > (joy_null_value-2)) 
						raw_joy_axis[i] = ((raw_joy_axis[i]-joy_null_value)*128)/(128-joy_null_value);
					else if (raw_joy_axis[i] < -(joy_null_value+2))
						raw_joy_axis[i] = ((raw_joy_axis[i]+joy_null_value)*128)/(128-joy_null_value);
					else
						raw_joy_axis[i] = 0;
					joy_axis[i]	= (raw_joy_axis[i]*FrameTime)/128;	
				}
			} else {
				joy_axis[i] = 0;
			}
		}	
		use_joystick=1;

	} else {
		for (i=0; i<6; i++ )
			joy_axis[i] = 0;
		use_joystick=0;
	}

//	DO MOUSE 
//	----------------------------------------------------------------------------
	if (Config_control_type==5) {
		mouse_get_delta( &dx, &dy );
		mouse_axis[0] = (dx*60); //(dx*FrameTime)/35;
		mouse_axis[1] = (dy*85); //(dy*FrameTime)/25;
		mouse_buttons = mouse_get_btns();
		//mprintf(( 0, "Mouse %d,%d b:%d, 0x%x\n", mouse_axis[0], mouse_axis[1], mouse_buttons, FrameTime ));
		use_mouse=1;
   } else {
		mouse_axis[0] = 0;
		mouse_axis[1] = 0;
		mouse_buttons = 0;
		use_mouse=0;
	}

//------------- Read slide_on -------------
	
	// From keyboard...
	if ( kc_keyboard[8].value < 255 ) slide_on |= keyd_pressed[ kc_keyboard[8].value ];
	if ( kc_keyboard[9].value < 255 ) slide_on |= keyd_pressed[ kc_keyboard[9].value ];
	// From joystick...
	if ((use_joystick)&&(kc_superjoy[5].value<255)) slide_on |= joy_get_button_state( kc_superjoy[5].value );
	// From mouse...
	if ((use_mouse)&&(kc_mouse[5].value<255)) slide_on |= mouse_buttons & (1<<kc_mouse[5].value);

//------------- Read bank_on ---------------

	// From keyboard...
	if ( kc_keyboard[18].value < 255 ) bank_on |= keyd_pressed[ kc_keyboard[18].value ];
	if ( kc_keyboard[19].value < 255 ) bank_on |= keyd_pressed[ kc_keyboard[19].value ];
	// From joystick...
	if ( (use_joystick)&&(kc_superjoy[10].value < 255 )) bank_on |= joy_get_button_state( kc_superjoy[10].value );
	// From mouse...
	if ( (use_mouse)&&(kc_mouse[10].value < 255 )) bank_on |= mouse_buttons & (1<<kc_mouse[10].value);

//------------ Read pitch_time -----------
	if ( !slide_on )	{
		// mprintf((0, "pitch: %7.3f %7.3f: %7.3f\n", f2fl(k4), f2fl(k6), f2fl(Controls.heading_time)));
		kp = 0;
		k0 = speed_factor*key_down_time( kc_keyboard[0].value )/2;	// Divide by two since we want pitch to go slower
		k1 = speed_factor*key_down_time( kc_keyboard[1].value )/2;
		k2 = speed_factor*key_down_time( kc_keyboard[2].value )/2;
		k3 = speed_factor*key_down_time( kc_keyboard[3].value )/2;

		// From keyboard...
		if ( kc_keyboard[0].value < 255 ) kp += k0/PH_SCALE;
		if ( kc_keyboard[1].value < 255 ) kp += k1/PH_SCALE;
		if ( kc_keyboard[2].value < 255 ) kp -= k2/PH_SCALE;
		if ( kc_keyboard[3].value < 255 ) kp -= k3/PH_SCALE;

		// From Cyberman...
		if ((use_mouse)&&(Config_control_type==CONTROL_CYBERMAN))	{
			kp += mouse_button_down_time(MB_PITCH_FORWARD)/(PH_SCALE*2);
			kp -= mouse_button_down_time(MB_PITCH_BACKWARD)/(PH_SCALE*2);
		}
	
		if (kp == 0)
			Controls.pitch_time = 0;
		else if (kp > 0) {
			if (Controls.pitch_time < 0)
				Controls.pitch_time = 0;
		} else // kp < 0
			if (Controls.pitch_time > 0)
				Controls.pitch_time = 0;
		Controls.pitch_time += kp;
	
		// From joystick...
		if ( (use_joystick)&&(kc_superjoy[13].value < 255 ))	{
			if ( !kc_superjoy[14].value )		// If not inverted...
				Controls.pitch_time -= (joy_axis[kc_superjoy[13].value]*Config_joystick_sensitivity)/8;
			else
				Controls.pitch_time += (joy_axis[kc_superjoy[13].value]*Config_joystick_sensitivity)/8;
		}
	
		// From mouse...
		//mprintf(( 0, "UM: %d, PV: %d\n", use_mouse, kc_mouse[13].value ));
		if ( (use_mouse)&&(kc_mouse[13].value < 255) )	{
			if ( !kc_mouse[14].value )		// If not inverted...
				Controls.pitch_time -= (mouse_axis[kc_mouse[13].value]*Config_joystick_sensitivity)/8;
			else
				Controls.pitch_time += (mouse_axis[kc_mouse[13].value]*Config_joystick_sensitivity)/8;
		}
	} else {
		Controls.pitch_time = 0;
	}


//----------- Read vertical_thrust_time -----------------

	if ( slide_on )	{
		k0 = speed_factor*key_down_time( kc_keyboard[0].value );
		k1 = speed_factor*key_down_time( kc_keyboard[1].value );
		k2 = speed_factor*key_down_time( kc_keyboard[2].value );
		k3 = speed_factor*key_down_time( kc_keyboard[3].value );

		// From keyboard...
		if ( kc_keyboard[0].value < 255 ) Controls.vertical_thrust_time += k0;
		if ( kc_keyboard[1].value < 255 ) Controls.vertical_thrust_time += k1;
		if ( kc_keyboard[2].value < 255 ) Controls.vertical_thrust_time -= k2;
		if ( kc_keyboard[3].value < 255 ) Controls.vertical_thrust_time -= k3;

		// From Cyberman...
		if ((use_mouse)&&(Config_control_type==CONTROL_CYBERMAN))	{
			Controls.vertical_thrust_time -= mouse_button_down_time(MB_PITCH_FORWARD);
			Controls.vertical_thrust_time += mouse_button_down_time(MB_PITCH_BACKWARD);
		}
	
		// From joystick...
		if ((use_joystick)&&( kc_superjoy[13].value < 255 ))	{
			if ( !kc_superjoy[14].value )		// If not inverted...
				Controls.vertical_thrust_time += joy_axis[kc_superjoy[13].value];
			else
				Controls.vertical_thrust_time -= joy_axis[kc_superjoy[13].value];
		}
	
		// From mouse...
		if ( (use_mouse)&&(kc_mouse[13].value < 255 ))	{
			if ( !kc_mouse[14].value )		// If not inverted...
				Controls.vertical_thrust_time -= mouse_axis[kc_mouse[13].value];
			else
				Controls.vertical_thrust_time += mouse_axis[kc_mouse[13].value];
		}
	}

	// From keyboard...
	if ( kc_keyboard[14].value < 255 ) Controls.vertical_thrust_time += speed_factor*key_down_time( kc_keyboard[14].value );
	if ( kc_keyboard[15].value < 255 ) Controls.vertical_thrust_time += speed_factor*key_down_time( kc_keyboard[15].value );
	if ( kc_keyboard[16].value < 255 ) Controls.vertical_thrust_time -= speed_factor*key_down_time( kc_keyboard[16].value );
	if ( kc_keyboard[17].value < 255 ) Controls.vertical_thrust_time -= speed_factor*key_down_time( kc_keyboard[17].value );
	
	// From joystick...
	if ((use_joystick)&&( kc_superjoy[19].value < 255 ))	{
		if ( !kc_superjoy[20].value )		// If not inverted...
			Controls.vertical_thrust_time += joy_axis[kc_superjoy[19].value];
		else
			Controls.vertical_thrust_time -= joy_axis[kc_superjoy[19].value];
	}

	// From joystick buttons
	if ( (use_joystick)&&(kc_superjoy[8].value < 255 )) Controls.vertical_thrust_time += joy_get_button_down_time( kc_superjoy[8].value );
	if ( (use_joystick)&&(kc_superjoy[9].value < 255 )) Controls.vertical_thrust_time -= joy_get_button_down_time( kc_superjoy[9].value );

	// From mouse buttons
	if ( (use_mouse)&&(kc_mouse[8].value < 255 )) Controls.vertical_thrust_time += mouse_button_down_time( kc_mouse[8].value );
	if ( (use_mouse)&&(kc_mouse[9].value < 255 )) Controls.vertical_thrust_time -= mouse_button_down_time( kc_mouse[9].value );

	// From mouse...
	if ( (use_mouse)&&(kc_mouse[19].value < 255 ))	{
		if ( !kc_mouse[20].value )		// If not inverted...
			Controls.vertical_thrust_time += mouse_axis[kc_mouse[19].value];
		else
			Controls.vertical_thrust_time -= mouse_axis[kc_mouse[19].value];
	}

	// From Cyberman...
	if ((use_mouse)&&(Config_control_type==CONTROL_CYBERMAN))	{
		Controls.vertical_thrust_time += mouse_button_down_time(MB_Z_UP)/2;
		Controls.vertical_thrust_time -= mouse_button_down_time(MB_Z_DOWN)/2;
	}

//---------- Read heading_time -----------

	if (!slide_on && !bank_on)	{
		//mprintf((0, "heading: %7.3f %7.3f: %7.3f\n", f2fl(k4), f2fl(k6), f2fl(Controls.heading_time)));
		kh = 0;
		k4 = speed_factor*key_down_time( kc_keyboard[4].value );
		k5 = speed_factor*key_down_time( kc_keyboard[5].value );
		k6 = speed_factor*key_down_time( kc_keyboard[6].value );
		k7 = speed_factor*key_down_time( kc_keyboard[7].value );

		// From keyboard...
		if ( kc_keyboard[4].value < 255 ) kh -= k4/PH_SCALE;
		if ( kc_keyboard[5].value < 255 ) kh -= k5/PH_SCALE;
		if ( kc_keyboard[6].value < 255 ) kh += k6/PH_SCALE;
		if ( kc_keyboard[7].value < 255 ) kh += k7/PH_SCALE;

		// From Cyberman...
		if ((use_mouse)&&(Config_control_type==CONTROL_CYBERMAN))	{
			kh -= mouse_button_down_time(MB_HEAD_LEFT)/PH_SCALE;
			kh += mouse_button_down_time(MB_HEAD_RIGHT)/PH_SCALE;
		}
	
		if (kh == 0)
			Controls.heading_time = 0;
		else if (kh > 0) {
			if (Controls.heading_time < 0)
				Controls.heading_time = 0;
		} else // kh < 0
			if (Controls.heading_time > 0)
				Controls.heading_time = 0;
		Controls.heading_time += kh;

		// From joystick...
		if ( (use_joystick)&&(kc_superjoy[15].value < 255 ))	{
			if ( !kc_superjoy[16].value )		// If not inverted...
				Controls.heading_time += (joy_axis[kc_superjoy[15].value]*Config_joystick_sensitivity)/8;
			else
				Controls.heading_time -= (joy_axis[kc_superjoy[15].value]*Config_joystick_sensitivity)/8;
		}
	
		// From mouse...
		if ( (use_mouse)&&(kc_mouse[15].value < 255 ))	{
			if ( !kc_mouse[16].value )		// If not inverted...
				Controls.heading_time += (mouse_axis[kc_mouse[15].value]*Config_joystick_sensitivity)/8;
			else
				Controls.heading_time -= (mouse_axis[kc_mouse[15].value]*Config_joystick_sensitivity)/8;
		}
	} else {
		Controls.heading_time = 0;
	}

//----------- Read sideways_thrust_time -----------------

	if ( slide_on )	{
		k0 = speed_factor*key_down_time( kc_keyboard[4].value );
		k1 = speed_factor*key_down_time( kc_keyboard[5].value );
		k2 = speed_factor*key_down_time( kc_keyboard[6].value );
		k3 = speed_factor*key_down_time( kc_keyboard[7].value );

		// From keyboard...
		if ( kc_keyboard[4].value < 255 ) Controls.sideways_thrust_time -= k0;
		if ( kc_keyboard[5].value < 255 ) Controls.sideways_thrust_time -= k1;
		if ( kc_keyboard[6].value < 255 ) Controls.sideways_thrust_time += k2;
		if ( kc_keyboard[7].value < 255 ) Controls.sideways_thrust_time += k3;
	
		// From joystick...
		if ( (use_joystick)&&(kc_superjoy[15].value < 255 ))	{
			if ( !kc_superjoy[16].value )		// If not inverted...
				Controls.sideways_thrust_time += joy_axis[kc_superjoy[15].value];
			else
				Controls.sideways_thrust_time -= joy_axis[kc_superjoy[15].value];
		}
		
		// From cyberman
		if ((use_mouse)&&(Config_control_type==CONTROL_CYBERMAN))	{
			Controls.sideways_thrust_time -= mouse_button_down_time(MB_HEAD_LEFT);
			Controls.sideways_thrust_time += mouse_button_down_time(MB_HEAD_RIGHT);
		}
	
		// From mouse...
		if ( (use_mouse)&&(kc_mouse[15].value < 255 ))	{
			if ( !kc_mouse[16].value )		// If not inverted...
				Controls.sideways_thrust_time += mouse_axis[kc_mouse[15].value];
			else
				Controls.sideways_thrust_time -= mouse_axis[kc_mouse[15].value];
		}
	}

	// From keyboard...
	if ( kc_keyboard[10].value < 255 ) Controls.sideways_thrust_time -= speed_factor*key_down_time( kc_keyboard[10].value );
	if ( kc_keyboard[11].value < 255 ) Controls.sideways_thrust_time -= speed_factor*key_down_time( kc_keyboard[11].value );
	if ( kc_keyboard[12].value < 255 ) Controls.sideways_thrust_time += speed_factor*key_down_time( kc_keyboard[12].value );
	if ( kc_keyboard[13].value < 255 ) Controls.sideways_thrust_time += speed_factor*key_down_time( kc_keyboard[13].value );
	
	// From joystick...
	if ( (use_joystick)&&(kc_superjoy[17].value < 255 ))	{
		if ( !kc_superjoy[18].value )		// If not inverted...
			Controls.sideways_thrust_time -= joy_axis[kc_superjoy[17].value];
		else
			Controls.sideways_thrust_time += joy_axis[kc_superjoy[17].value];
	}

	// From joystick buttons
	if ( (use_joystick)&&(kc_superjoy[6].value < 255 )) Controls.sideways_thrust_time -= joy_get_button_down_time( kc_superjoy[6].value );
	if ( (use_joystick)&&(kc_superjoy[7].value < 255 )) Controls.sideways_thrust_time += joy_get_button_down_time( kc_superjoy[7].value );

	// From mouse buttons
	if ( (use_mouse)&&(kc_mouse[6].value < 255 )) Controls.sideways_thrust_time -= mouse_button_down_time( kc_mouse[6].value );
	if ( (use_mouse)&&(kc_mouse[7].value < 255 )) Controls.sideways_thrust_time += mouse_button_down_time( kc_mouse[7].value );

	// From mouse...
	if ( (use_mouse)&&(kc_mouse[17].value < 255 ))	{
		if ( !kc_mouse[18].value )		// If not inverted...
			Controls.sideways_thrust_time += mouse_axis[kc_mouse[17].value];
		else
			Controls.sideways_thrust_time -= mouse_axis[kc_mouse[17].value];
	}

//----------- Read bank_time -----------------

	if ( bank_on )	{
		k0 = speed_factor*key_down_time( kc_keyboard[4].value );
		k1 = speed_factor*key_down_time( kc_keyboard[5].value );
		k2 = speed_factor*key_down_time( kc_keyboard[6].value );
		k3 = speed_factor*key_down_time( kc_keyboard[7].value );

		// From keyboard...
		if ( kc_keyboard[4].value < 255 ) Controls.bank_time += k0;
		if ( kc_keyboard[5].value < 255 ) Controls.bank_time += k1;
		if ( kc_keyboard[6].value < 255 ) Controls.bank_time -= k2;
		if ( kc_keyboard[7].value < 255 ) Controls.bank_time -= k3;

		// From Cyberman...
		if ((use_mouse)&&(Config_control_type==CONTROL_CYBERMAN))	{
			Controls.bank_time -= mouse_button_down_time(MB_HEAD_LEFT);
			Controls.bank_time += mouse_button_down_time(MB_HEAD_RIGHT);
		}

		// From joystick...
		if ( (use_joystick)&&(kc_superjoy[15].value < 255) )	{
			if ( !kc_superjoy[16].value )		// If not inverted...
				Controls.bank_time -= (joy_axis[kc_superjoy[15].value]*Config_joystick_sensitivity)/8;
			else
				Controls.bank_time += (joy_axis[kc_superjoy[15].value]*Config_joystick_sensitivity)/8;
		}
	
		// From mouse...
		if ( (use_mouse)&&(kc_mouse[15].value < 255 ))	{
			if ( !kc_mouse[16].value )		// If not inverted...
				Controls.bank_time += (mouse_axis[kc_mouse[15].value]*Config_joystick_sensitivity)/8;
			else
				Controls.bank_time -= (mouse_axis[kc_mouse[15].value]*Config_joystick_sensitivity)/8;
		}
	}

	// From keyboard...
	if ( kc_keyboard[20].value < 255 ) Controls.bank_time += speed_factor*key_down_time( kc_keyboard[20].value );
	if ( kc_keyboard[21].value < 255 ) Controls.bank_time += speed_factor*key_down_time( kc_keyboard[21].value );
	if ( kc_keyboard[22].value < 255 ) Controls.bank_time -= speed_factor*key_down_time( kc_keyboard[22].value );
	if ( kc_keyboard[23].value < 255 ) Controls.bank_time -= speed_factor*key_down_time( kc_keyboard[23].value );

	// From joystick...
	if ( (use_joystick)&&(kc_superjoy[21].value < 255) )	{
		if ( !kc_superjoy[22].value )		// If not inverted...
			Controls.bank_time -= joy_axis[kc_superjoy[21].value];
		else
			Controls.bank_time += joy_axis[kc_superjoy[21].value];
	}

	// From joystick buttons
	if ( (use_joystick)&&(kc_superjoy[11].value < 255 )) Controls.bank_time += joy_get_button_down_time( kc_superjoy[11].value );
	if ( (use_joystick)&&(kc_superjoy[12].value < 255 )) Controls.bank_time -= joy_get_button_down_time( kc_superjoy[12].value );

	// From mouse buttons
	if ( (use_mouse)&&(kc_mouse[11].value < 255 )) Controls.bank_time += mouse_button_down_time( kc_mouse[11].value );
	if ( (use_mouse)&&(kc_mouse[12].value < 255 )) Controls.bank_time -= mouse_button_down_time( kc_mouse[12].value );

	// From mouse...
	if ( (use_mouse)&&(kc_mouse[21].value < 255 ))	{
		if ( !kc_mouse[22].value )		// If not inverted...
			Controls.bank_time += mouse_axis[kc_mouse[21].value];
		else
			Controls.bank_time -= mouse_axis[kc_mouse[21].value];
	}

	// From Cyberman
	if ((use_mouse)&&(Config_control_type==CONTROL_CYBERMAN))	{
		Controls.bank_time += mouse_button_down_time(MB_BANK_LEFT);
		Controls.bank_time -= mouse_button_down_time(MB_BANK_RIGHT);
	}

//----------- Read forward_thrust_time -------------

	// From keyboard...
	if ( kc_keyboard[30].value < 255 ) Controls.forward_thrust_time += speed_factor*key_down_time( kc_keyboard[30].value );
	if ( kc_keyboard[31].value < 255 ) Controls.forward_thrust_time += speed_factor*key_down_time( kc_keyboard[31].value );
	if ( kc_keyboard[32].value < 255 ) Controls.forward_thrust_time -= speed_factor*key_down_time( kc_keyboard[32].value );
	if ( kc_keyboard[33].value < 255 ) Controls.forward_thrust_time -= speed_factor*key_down_time( kc_keyboard[33].value );

	// From joystick...
	if ( (use_joystick)&&(kc_superjoy[23].value < 255 ))	{
		if ( !kc_superjoy[24].value )		// If not inverted...
			Controls.forward_thrust_time -= joy_axis[kc_superjoy[23].value];
		else
			Controls.forward_thrust_time += joy_axis[kc_superjoy[23].value];
	}

	// From joystick buttons
	if ( (use_joystick)&&(kc_superjoy[2].value < 255 )) Controls.forward_thrust_time += joy_get_button_down_time( kc_superjoy[2].value );
	if ( (use_joystick)&&(kc_superjoy[3].value < 255 )) Controls.forward_thrust_time -= joy_get_button_down_time( kc_superjoy[3].value );

	// From mouse...
	if ( (use_mouse)&&(kc_mouse[23].value < 255 ))	{
		if ( !kc_mouse[24].value )		// If not inverted...
			Controls.forward_thrust_time -= mouse_axis[kc_mouse[23].value];
		else
			Controls.forward_thrust_time += mouse_axis[kc_mouse[23].value];
	}

	// From mouse buttons
	if ( (use_mouse)&&(kc_mouse[2].value < 255 )) Controls.forward_thrust_time += mouse_button_down_time( kc_mouse[2].value );
	if ( (use_mouse)&&(kc_mouse[3].value < 255 )) Controls.forward_thrust_time -= mouse_button_down_time( kc_mouse[3].value );

//----------- Read afterburner_state -------------

	// From keyboard...
	if ( kc_keyboard[46].value < 255 ) Controls.afterburner_state |= keyd_pressed[kc_keyboard[46].value];
	if ( kc_keyboard[47].value < 255 ) Controls.afterburner_state |= keyd_pressed[kc_keyboard[47].value];

        if ( (use_mouse)&&(kc_mouse[27].value < 255 )) Controls.afterburner_state |= mouse_button_state(kc_mouse[27].value);

        if ( (use_joystick)&&(kc_superjoy[27].value < 255 )) Controls.afterburner_state |= joy_get_button_state(kc_superjoy[27].value);

//-------Read headlight key--------------------------
        if (kc_keyboard[52].value < 255 )
		   Controls.headlight_count=key_down_count(kc_keyboard[52].value);
        if (kc_keyboard[53].value < 255 )
		   Controls.headlight_count=key_down_count(kc_keyboard[53].value);
		  if ((use_joystick)&&(kc_superjoy[30].value < 255 )) 
         Controls.headlight_count=joy_get_button_down_cnt(kc_superjoy[30].value);
		 
//--------Read Cycle Primary Key------------------

		  if (kc_keyboard[48].value<255)
			 Controls.cycle_primary_count=key_down_count(kc_keyboard[48].value);
		  if (kc_keyboard[49].value<255)
			Controls.cycle_primary_count+=key_down_count(kc_keyboard[49].value);
	     if ((use_joystick)&&(kc_superjoy[28].value < 255 )) 
         Controls.cycle_primary_count+=joy_get_button_down_cnt(kc_superjoy[28].value);

//--------Read Cycle Secondary Key------------------

		  if (kc_keyboard[50].value<255)
			 Controls.cycle_secondary_count=key_down_count(kc_keyboard[50].value);
		  if (kc_keyboard[51].value<255)
			Controls.cycle_secondary_count+=key_down_count(kc_keyboard[51].value);
	     if ((use_joystick)&&(kc_superjoy[29].value < 255 )) 
			Controls.cycle_secondary_count=joy_get_button_down_cnt(kc_superjoy[29].value);

//--------Read Toggle Bomb key----------------------

		  if (kc_keyboard[56].value<255 && key_down_count(kc_keyboard[56].value))
         {
          int bomb = Secondary_last_was_super[PROXIMITY_INDEX]?PROXIMITY_INDEX:SMART_MINE_INDEX;

			 if (!Players[Player_num].secondary_ammo[PROXIMITY_INDEX] &&
				  !Players[Player_num].secondary_ammo[SMART_MINE_INDEX])
			   {
				 digi_play_sample_once( SOUND_BAD_SELECTION, F1_0 );
				 HUD_init_message ("No bombs available!");
				}
			 else
				{	
				 if (Players[Player_num].secondary_ammo[bomb]==0)
					{
					 digi_play_sample_once( SOUND_BAD_SELECTION, F1_0 );
					 HUD_init_message ("No %s available!",(bomb==SMART_MINE_INDEX)?"Smart mines":"Proximity bombs");
					}
				  else
					{
			       Secondary_last_was_super[PROXIMITY_INDEX]=!Secondary_last_was_super[PROXIMITY_INDEX];
					 digi_play_sample_once( SOUND_GOOD_SELECTION_SECONDARY, F1_0 );
					}
				}
			}
          
//---------Read Energy->Shield key----------

        if ((Players[Player_num].flags & PLAYER_FLAGS_CONVERTER) && keyd_pressed[kc_keyboard[54].value])
                transfer_energy_to_shield(key_down_time(kc_keyboard[54].value));
        if ((Players[Player_num].flags & PLAYER_FLAGS_CONVERTER) && keyd_pressed[kc_keyboard[55].value])
                transfer_energy_to_shield(key_down_time(kc_keyboard[55].value));

//----------- Read fire_primary_down_count
	if (kc_keyboard[24].value < 255 ) Controls.fire_primary_down_count += key_down_count(kc_keyboard[24].value);
	if (kc_keyboard[25].value < 255 ) Controls.fire_primary_down_count += key_down_count(kc_keyboard[25].value);
	if ((use_joystick)&&(kc_superjoy[0].value < 255 )) Controls.fire_primary_down_count += joy_get_button_down_cnt(kc_superjoy[0].value);
	if ((use_mouse)&&(kc_mouse[0].value < 255 )) Controls.fire_primary_down_count += mouse_button_down_count(kc_mouse[0].value);

//----------- Read fire_primary_state
	if (kc_keyboard[24].value < 255 ) Controls.fire_primary_state |= keyd_pressed[kc_keyboard[24].value];
	if (kc_keyboard[25].value < 255 ) Controls.fire_primary_state |= keyd_pressed[kc_keyboard[25].value];
	if ((use_joystick)&&(kc_superjoy[0].value < 255 )) Controls.fire_primary_state |= joy_get_button_state(kc_superjoy[0].value);
	if ((use_mouse)&&(kc_mouse[0].value < 255) ) Controls.fire_primary_state |= mouse_button_state(kc_mouse[0].value);

//----------- Read fire_secondary_down_count
	if (kc_keyboard[26].value < 255 ) Controls.fire_secondary_down_count += key_down_count(kc_keyboard[26].value);
	if (kc_keyboard[27].value < 255 ) Controls.fire_secondary_down_count += key_down_count(kc_keyboard[27].value);
	if ((use_joystick)&&(kc_superjoy[1].value < 255 )) Controls.fire_secondary_down_count += joy_get_button_down_cnt(kc_superjoy[1].value);
	if ((use_mouse)&&(kc_mouse[1].value < 255 )) Controls.fire_secondary_down_count += mouse_button_down_count(kc_mouse[1].value);

//----------- Read fire_secondary_state
	if (kc_keyboard[26].value < 255 ) Controls.fire_secondary_state |= keyd_pressed[kc_keyboard[26].value];
	if (kc_keyboard[27].value < 255 ) Controls.fire_secondary_state |= keyd_pressed[kc_keyboard[27].value];
	if ((use_joystick)&&(kc_superjoy[1].value < 255 )) Controls.fire_secondary_state |= joy_get_button_state(kc_superjoy[1].value);
	if ((use_mouse)&&(kc_mouse[1].value < 255) ) Controls.fire_secondary_state |= mouse_button_state(kc_mouse[1].value);

//----------- Read fire_flare_down_count
	if (kc_keyboard[28].value < 255 ) Controls.fire_flare_down_count += key_down_count(kc_keyboard[28].value);
	if (kc_keyboard[29].value < 255 ) Controls.fire_flare_down_count += key_down_count(kc_keyboard[29].value);
	if ((use_joystick)&&(kc_superjoy[4].value < 255 )) Controls.fire_flare_down_count += joy_get_button_down_cnt(kc_superjoy[4].value);
	if ((use_mouse)&&(kc_mouse[4].value < 255 )) Controls.fire_flare_down_count += mouse_button_down_count(kc_mouse[4].value);

//----------- Read drop_bomb_down_count
	if (kc_keyboard[34].value < 255 ) Controls.drop_bomb_down_count += key_down_count(kc_keyboard[34].value);
	if (kc_keyboard[35].value < 255 ) Controls.drop_bomb_down_count += key_down_count(kc_keyboard[35].value);
	if ((use_joystick)&&(kc_superjoy[26].value < 255 )) Controls.drop_bomb_down_count += joy_get_button_down_cnt(kc_superjoy[26].value);
	if ((use_mouse)&&(kc_mouse[26].value < 255 )) Controls.drop_bomb_down_count += mouse_button_down_count(kc_mouse[26].value);

//----------- Read rear_view_down_count
	if (kc_keyboard[36].value < 255 ) Controls.rear_view_down_count += key_down_count(kc_keyboard[36].value);
	if (kc_keyboard[37].value < 255 ) Controls.rear_view_down_count += key_down_count(kc_keyboard[37].value);
	if ((use_joystick)&&(kc_superjoy[25].value < 255 )) Controls.rear_view_down_count += joy_get_button_down_cnt(kc_superjoy[25].value);
	if ((use_mouse)&&(kc_mouse[25].value < 255 )) Controls.rear_view_down_count += mouse_button_down_count(kc_mouse[25].value);

//----------- Read rear_view_down_state
	if (kc_keyboard[36].value < 255 ) Controls.rear_view_down_state |= keyd_pressed[kc_keyboard[36].value];
	if (kc_keyboard[37].value < 255 ) Controls.rear_view_down_state |= keyd_pressed[kc_keyboard[37].value];
	if ((use_joystick)&&(kc_superjoy[25].value < 255 )) Controls.rear_view_down_state |= joy_get_button_state(kc_superjoy[25].value);
	if ((use_mouse)&&(kc_mouse[25].value < 255 )) Controls.rear_view_down_state |= mouse_button_state(kc_mouse[25].value);

//----------- Read automap_down_count
	if (kc_keyboard[44].value < 255 ) Controls.automap_down_count += key_down_count(kc_keyboard[44].value);
	if (kc_keyboard[45].value < 255 ) Controls.automap_down_count += key_down_count(kc_keyboard[45].value);

//----------- Read automap_state
	if (kc_keyboard[44].value < 255 ) Controls.automap_state |= keyd_pressed[kc_keyboard[44].value];
	if (kc_keyboard[45].value < 255 ) Controls.automap_state |= keyd_pressed[kc_keyboard[45].value];

//----------- Read stupid-cruise-control-type of throttle.
	{
		if ( kc_keyboard[38].value < 255 ) Cruise_speed += fixdiv(speed_factor*key_down_time(kc_keyboard[38].value)*5,FrameTime);
		if ( kc_keyboard[39].value < 255 ) Cruise_speed += fixdiv(speed_factor*key_down_time(kc_keyboard[39].value)*5,FrameTime);
		if ( kc_keyboard[40].value < 255 ) Cruise_speed -= fixdiv(speed_factor*key_down_time(kc_keyboard[40].value)*5,FrameTime);
		if ( kc_keyboard[41].value < 255 ) Cruise_speed -= fixdiv(speed_factor*key_down_time(kc_keyboard[41].value)*5,FrameTime);
		if ( (kc_keyboard[42].value < 255) && (key_down_count(kc_keyboard[42].value)) )
			Cruise_speed = 0;
		if ( (kc_keyboard[43].value < 255) && (key_down_count(kc_keyboard[43].value)) )
			Cruise_speed = 0;
	
		if (Cruise_speed > i2f(100) ) Cruise_speed = i2f(100);
		if (Cruise_speed < 0 ) Cruise_speed = 0;
	
		if (Controls.forward_thrust_time==0)
			Controls.forward_thrust_time = fixmul(Cruise_speed,FrameTime)/100;
	}

	//read_head_tracker();

	// Read external controls
	if (kc_use_external_control || CybermouseActive)
		kconfig_read_external_controls();

//----------- Clamp values between -FrameTime and FrameTime
	if (FrameTime > F1_0 )
		mprintf( (1, "Bogus frame time of %.2f seconds\n", f2fl(FrameTime) ));

	if (Controls.pitch_time > FrameTime/2 ) Controls.pitch_time = FrameTime/2;
	if (Controls.vertical_thrust_time > FrameTime ) Controls.vertical_thrust_time = FrameTime;
	if (Controls.heading_time > FrameTime ) Controls.heading_time = FrameTime;
	if (Controls.sideways_thrust_time > FrameTime ) Controls.sideways_thrust_time = FrameTime;
	if (Controls.bank_time > FrameTime ) Controls.bank_time = FrameTime;
	if (Controls.forward_thrust_time > FrameTime ) Controls.forward_thrust_time = FrameTime;
//	if (Controls.afterburner_time > FrameTime ) Controls.afterburner_time = FrameTime;

	if (Controls.pitch_time < -FrameTime/2 ) Controls.pitch_time = -FrameTime/2;
	if (Controls.vertical_thrust_time < -FrameTime ) Controls.vertical_thrust_time = -FrameTime;
	if (Controls.heading_time < -FrameTime ) Controls.heading_time = -FrameTime;
	if (Controls.sideways_thrust_time < -FrameTime ) Controls.sideways_thrust_time = -FrameTime;
	if (Controls.bank_time < -FrameTime ) Controls.bank_time = -FrameTime;
	if (Controls.forward_thrust_time < -FrameTime ) Controls.forward_thrust_time = -FrameTime;
//	if (Controls.afterburner_time < -FrameTime ) Controls.afterburner_time = -FrameTime;


//--------- Don't do anything if in debug mode
	#ifndef RELEASE
	if ( keyd_pressed[KEY_DELETE] )	{
		memset( &Controls, 0, sizeof(control_info) );
	}
	#endif
}
#else


fix Next_toggle_time[3]={0,0,0};

int allowed_to_toggle(int i)
{
  //used for keeping tabs of when its ok to toggle headlight,primary,and secondary
 
	if (Next_toggle_time[i] > GameTime)
		if (Next_toggle_time[i] < GameTime + (F1_0/8))	//	In case time is bogus, never wait > 1 second.
			return 0;

	Next_toggle_time[i] = GameTime + (F1_0/8);

	return 1;
}


#ifdef D2X_KEYS
//added on 2/7/99 by Victor Rachels for jostick state setting
int d2x_joystick_ostate[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//end this section adition - VR
#endif


void controls_read_all()
{
	int i;
	int slide_on, bank_on;
	int dx, dy;
#ifdef SDL_INPUT
	int dz;
#endif
	int idx, idy;
	fix ctime;
	fix mouse_axis[3] = {0,0,0};
	int raw_joy_axis[JOY_MAX_AXES];
	int mouse_buttons;
	fix k0, k1, k2, k3, kp;
	fix k4, k5, k6, k7, kh;
	ubyte channel_masks;
	int use_mouse, use_joystick;
	int speed_factor=1;

	mouse_buttons=0;
	use_mouse=0;

	if (Game_turbo_mode)
		speed_factor = 2;
	
	{
		fix temp = Controls.heading_time;
		fix temp1 = Controls.pitch_time;
		memset( &Controls, 0, sizeof(control_info) );
		Controls.heading_time = temp;
		Controls.pitch_time = temp1;
	}
	slide_on = 0;
	bank_on = 0;

	ctime = timer_get_fixed_seconds();

	//---------  Read Joystick -----------
#ifndef MACINTOSH
	if ( (LastReadTime + JOYSTICK_READ_TIME > ctime) && (Config_control_type!=CONTROL_THRUSTMASTER_FCS) ) {
# ifndef __MSDOS__
		if ((ctime < 0) && (LastReadTime >= 0))
# else
		if ((ctime < 0) && (LastReadTime > 0))
# endif
			LastReadTime = ctime;
		use_joystick=1;
	} else if ((Config_control_type>0) && (Config_control_type<5) ) {
		LastReadTime = ctime;
		channel_masks = joystick_read_raw_axis( JOY_ALL_AXIS, raw_joy_axis );

		for (i = 0; i < joy_num_axes; i++)
		{
#ifndef SDL_INPUT
			if (channel_masks&(1<<i))	{
#endif
				int joy_null_value = 10;

				if ( (i==3) && (Config_control_type==CONTROL_THRUSTMASTER_FCS) )	{
					kconfig_read_fcs( raw_joy_axis[i] );
				} else {
					raw_joy_axis[i] = joy_get_scaled_reading( raw_joy_axis[i], i );
	
					if (kc_joystick[23].value==i)		// If this is the throttle
						joy_null_value = 20;		// Then use a larger dead-zone
	
					if (raw_joy_axis[i] > joy_null_value) 
					  raw_joy_axis[i] = ((raw_joy_axis[i]-joy_null_value)*128)/(128-joy_null_value);
				  	else if (raw_joy_axis[i] < -joy_null_value)
					  raw_joy_axis[i] = ((raw_joy_axis[i]+joy_null_value)*128)/(128-joy_null_value);
					else
					  raw_joy_axis[i] = 0;
					joy_axis[i]	= (raw_joy_axis[i]*FrameTime)/128;	
				}
#ifndef SDL_INPUT
			} else {
				joy_axis[i] = 0;
			}
#endif
		}	
		use_joystick=1;
	} else {
		for (i = 0; i < joy_num_axes; i++)
			joy_axis[i] = 0;
		use_joystick=0;
	}
#else   // MACINTOSH
	//---------  Read Joystick -----------
	if ((Config_control_type>0) && (Config_control_type<5) ) {
		channel_masks = joystick_read_raw_axis( JOY_ALL_AXIS, raw_joy_axis );
		for (i=0; i<4; i++ )	{
			if (channel_masks&(1<<i))	{
				int joy_null_value = 10;

				raw_joy_axis[i] = joy_get_scaled_reading( raw_joy_axis[i], i );

				if (kc_joystick[23].value==i)		// If this is the throttle
					joy_null_value = 20;				// Then use a larger dead-zone

				if (raw_joy_axis[i] > joy_null_value) 
					raw_joy_axis[i] = ((raw_joy_axis[i]-joy_null_value)*128)/(128-joy_null_value);
				else if (raw_joy_axis[i] < -joy_null_value)
					raw_joy_axis[i] = ((raw_joy_axis[i]+joy_null_value)*128)/(128-joy_null_value);
				else
					raw_joy_axis[i] = 0;
				joy_axis[i]	= (raw_joy_axis[i]*FrameTime)/128;	
			} else {
				joy_axis[i] = 0;
			}
		}	
		use_joystick=1;
	} else {
		for (i=0; i<4; i++ )
			joy_axis[i] = 0;
		use_joystick=0;
	}
#endif		// ifndef MACINTOSH

	if (Config_control_type==5 && !CybermouseActive) {
		//---------  Read Mouse -----------
#ifdef SDL_INPUT
		mouse_get_delta_z( &dx, &dy, &dz );
#else
		mouse_get_delta( &dx, &dy );
#endif
		mouse_axis[0] = (dx*60); //(dx*FrameTime)/35;
		mouse_axis[1] = (dy*85); //(dy*FrameTime)/25;
#ifdef SDL_INPUT
		mouse_axis[2] = (dz*FrameTime);
#endif
		mouse_buttons = mouse_get_btns();
		//mprintf(( 0, "Mouse %d,%d b:%d, 0x%x\n", mouse_axis[0], mouse_axis[1], mouse_buttons, FrameTime ));
		use_mouse=1;
	} else if (Config_control_type==6 && !CybermouseActive) {
		//---------  Read Cyberman -----------
		mouse_get_cyberman_pos(&idx,&idy );
		mouse_axis[0] = (idx*FrameTime)/128;
		mouse_axis[1] = (idy*FrameTime)/128;
		mouse_buttons = mouse_get_btns();
		use_mouse=1;
	} else if (CybermouseActive) {
//		ReadOWL (kc_external_control);
//		CybermouseAdjust();
	} else {
		mouse_axis[0] = 0;
		mouse_axis[1] = 0;
		mouse_buttons = 0;
		use_mouse=0;
	}

#ifdef D2X_KEYS
	//added on 2/4/99 by Victor Rachels for d1x keys
	//--------- Read primary weapon select -------------
	//the following "if" added by WraithX to stop deadies from switchin weapons, 4/14/00
	if (!Player_is_dead)
	{
		{
			int d2x_joystick_state[10];

			for(i=0;i<10;i++)
				d2x_joystick_state[i] = joy_get_button_state(kc_d2x[i*2+1].value);


			//----------------Weapon 1----------------
			if(key_down_count(kc_d2x[0].value) ||
			   (joy_get_button_state(kc_d2x[1].value) &&
				(d2x_joystick_state[0]!=d2x_joystick_ostate[0]) ) )
			{
				//int i, valu=0;
				do_weapon_select(0,0);
				/*
				for(i=MAX_PRIMARY_WEAPONS;i<MAX_PRIMARY_WEAPONS+NEWPRIMS;i++)
					if(primary_order[i]>primary_order[valu]&&player_has_weapon(i,0))
						valu = i;
				LaserPowSelected = valu;
				*/
			}
			//----------------Weapon 2----------------
			if(key_down_count(kc_d2x[2].value) ||
			   (joy_get_button_state(kc_d2x[3].value) &&
				(d2x_joystick_state[1]!=d2x_joystick_ostate[1]) ) )
				do_weapon_select(1,0);
			//----------------Weapon 3----------------
			if(key_down_count(kc_d2x[4].value) ||
			   (joy_get_button_state(kc_d2x[5].value) &&
				(d2x_joystick_state[2]!=d2x_joystick_ostate[2]) ) )
				do_weapon_select(2,0);
			//----------------Weapon 4----------------
			if(key_down_count(kc_d2x[6].value) ||
			   (joy_get_button_state(kc_d2x[7].value) &&
				(d2x_joystick_state[3]!=d2x_joystick_ostate[3]) ) )
				do_weapon_select(3,0);
			//----------------Weapon 5----------------
			if(key_down_count(kc_d2x[8].value) ||
			   (joy_get_button_state(kc_d2x[9].value) &&
				(d2x_joystick_state[4]!=d2x_joystick_ostate[4]) ) )
				do_weapon_select(4,0);
 
			//--------- Read secondary weapon select ----------
			//----------------Weapon 6----------------
			if(key_down_count(kc_d2x[10].value) ||
			   (joy_get_button_state(kc_d2x[11].value) &&
				(d2x_joystick_state[5]!=d2x_joystick_ostate[5]) ) )
				do_weapon_select(0,1);
			//----------------Weapon 7----------------
			if(key_down_count(kc_d2x[12].value) ||
			   (joy_get_button_state(kc_d2x[13].value) &&
				(d2x_joystick_state[6]!=d2x_joystick_ostate[6]) ) )
				do_weapon_select(1,1);
			//----------------Weapon 8----------------
			if(key_down_count(kc_d2x[14].value) ||
			   (joy_get_button_state(kc_d2x[15].value) &&
				(d2x_joystick_state[7]!=d2x_joystick_ostate[7]) ) )
				do_weapon_select(2,1);
			//----------------Weapon 9----------------
			if(key_down_count(kc_d2x[16].value) ||
			   (joy_get_button_state(kc_d2x[17].value) &&
				(d2x_joystick_state[8]!=d2x_joystick_ostate[8]) ) )
				do_weapon_select(3,1);
			//----------------Weapon 0----------------
			if(key_down_count(kc_d2x[18].value) ||
			   (joy_get_button_state(kc_d2x[19].value) &&
				(d2x_joystick_state[9]!=d2x_joystick_ostate[9]) ) )
				do_weapon_select(4,1);
			memcpy(d2x_joystick_ostate,d2x_joystick_state,10*sizeof(int));
		}
		//end this section addition - VR


	}//end "if (!Player_is_dead)" - WraithX
#endif


//------------- Read slide_on -------------
	
	// From keyboard...
	if ( kc_keyboard[8].value < 255 ) slide_on |= keyd_pressed[ kc_keyboard[8].value ];
	if ( kc_keyboard[9].value < 255 ) slide_on |= keyd_pressed[ kc_keyboard[9].value ];
	// From joystick...
	if ((use_joystick)&&(kc_joystick[5].value<255)) slide_on |= joy_get_button_state( kc_joystick[5].value );
	// From mouse...
	if ((use_mouse)&&(kc_mouse[5].value<255)) slide_on |= mouse_buttons & (1<<kc_mouse[5].value);

//------------- Read bank_on ---------------

	// From keyboard...
	if ( kc_keyboard[18].value < 255 ) bank_on |= keyd_pressed[ kc_keyboard[18].value ];
	if ( kc_keyboard[19].value < 255 ) bank_on |= keyd_pressed[ kc_keyboard[19].value ];
	// From joystick...
	if ( (use_joystick)&&(kc_joystick[10].value < 255 )) bank_on |= joy_get_button_state( kc_joystick[10].value );
	// From mouse...
	if ( (use_mouse)&&(kc_mouse[10].value < 255 )) bank_on |= mouse_buttons & (1<<kc_mouse[10].value);

//------------ Read pitch_time -----------
	if ( !slide_on )	{
		// mprintf((0, "pitch: %7.3f %7.3f: %7.3f\n", f2fl(k4), f2fl(k6), f2fl(Controls.heading_time)));
		kp = 0;
		k0 = speed_factor*key_down_time( kc_keyboard[0].value )/2;	// Divide by two since we want pitch to go slower
		k1 = speed_factor*key_down_time( kc_keyboard[1].value )/2;
		k2 = speed_factor*key_down_time( kc_keyboard[2].value )/2;
		k3 = speed_factor*key_down_time( kc_keyboard[3].value )/2;

		// From keyboard...
		if ( kc_keyboard[0].value < 255 ) kp += k0/PH_SCALE;
		if ( kc_keyboard[1].value < 255 ) kp += k1/PH_SCALE;
		if ( kc_keyboard[2].value < 255 ) kp -= k2/PH_SCALE;
		if ( kc_keyboard[3].value < 255 ) kp -= k3/PH_SCALE;

		// From Cyberman...
		if ((use_mouse)&&(Config_control_type==CONTROL_CYBERMAN))	{
			kp += mouse_button_down_time(MB_PITCH_FORWARD)/(PH_SCALE*2);
			kp -= mouse_button_down_time(MB_PITCH_BACKWARD)/(PH_SCALE*2);
		}
	
		if (kp == 0)
			Controls.pitch_time = 0;
		else if (kp > 0) {
			if (Controls.pitch_time < 0)
				Controls.pitch_time = 0;
		} else // kp < 0
			if (Controls.pitch_time > 0)
				Controls.pitch_time = 0;
		Controls.pitch_time += kp;
	
		// From joystick...
		if ( (use_joystick)&&(kc_joystick[13].value < 255 ))	{
			if ( !kc_joystick[14].value )		// If not inverted...
				Controls.pitch_time -= (joy_axis[kc_joystick[13].value]*Config_joystick_sensitivity)/8;
			else
				Controls.pitch_time += (joy_axis[kc_joystick[13].value]*Config_joystick_sensitivity)/8;
		}
	
		// From mouse...
		//mprintf(( 0, "UM: %d, PV: %d\n", use_mouse, kc_mouse[13].value ));
		if ( (use_mouse)&&(kc_mouse[13].value < 255) )	{
			if ( !kc_mouse[14].value )		// If not inverted...
				Controls.pitch_time -= (mouse_axis[kc_mouse[13].value]*Config_joystick_sensitivity)/8;
			else
				Controls.pitch_time += (mouse_axis[kc_mouse[13].value]*Config_joystick_sensitivity)/8;
		}
	} else {
		Controls.pitch_time = 0;
	}


// the following "if" added by WraithX, 4/14/00
// done so that dead players can't move
if (!Player_is_dead)
{
//----------- Read vertical_thrust_time -----------------

	if ( slide_on )	{
		k0 = speed_factor*key_down_time( kc_keyboard[0].value );
		k1 = speed_factor*key_down_time( kc_keyboard[1].value );
		k2 = speed_factor*key_down_time( kc_keyboard[2].value );
		k3 = speed_factor*key_down_time( kc_keyboard[3].value );

		// From keyboard...
		if ( kc_keyboard[0].value < 255 ) Controls.vertical_thrust_time += k0;
		if ( kc_keyboard[1].value < 255 ) Controls.vertical_thrust_time += k1;
		if ( kc_keyboard[2].value < 255 ) Controls.vertical_thrust_time -= k2;
		if ( kc_keyboard[3].value < 255 ) Controls.vertical_thrust_time -= k3;

		// From Cyberman...
		if ((use_mouse)&&(Config_control_type==CONTROL_CYBERMAN))	{
			Controls.vertical_thrust_time -= mouse_button_down_time(MB_PITCH_FORWARD);
			Controls.vertical_thrust_time += mouse_button_down_time(MB_PITCH_BACKWARD);
		}
	
		// From joystick...
		if ((use_joystick)&&( kc_joystick[13].value < 255 ))	{
			if ( !kc_joystick[14].value )		// If not inverted...
				Controls.vertical_thrust_time += joy_axis[kc_joystick[13].value];
			else
				Controls.vertical_thrust_time -= joy_axis[kc_joystick[13].value];
		}
	
		// From mouse...
		if ( (use_mouse)&&(kc_mouse[13].value < 255 ))	{
			if ( !kc_mouse[14].value )		// If not inverted...
				Controls.vertical_thrust_time -= mouse_axis[kc_mouse[13].value];
			else
				Controls.vertical_thrust_time += mouse_axis[kc_mouse[13].value];
		}
	}

	// From keyboard...
	if ( kc_keyboard[14].value < 255 ) Controls.vertical_thrust_time += speed_factor*key_down_time( kc_keyboard[14].value );
	if ( kc_keyboard[15].value < 255 ) Controls.vertical_thrust_time += speed_factor*key_down_time( kc_keyboard[15].value );
	if ( kc_keyboard[16].value < 255 ) Controls.vertical_thrust_time -= speed_factor*key_down_time( kc_keyboard[16].value );
	if ( kc_keyboard[17].value < 255 ) Controls.vertical_thrust_time -= speed_factor*key_down_time( kc_keyboard[17].value );
	
	// From joystick...
	if ((use_joystick)&&( kc_joystick[19].value < 255 ))	{
		if ( !kc_joystick[20].value )		// If not inverted...
			Controls.vertical_thrust_time += joy_axis[kc_joystick[19].value];
		else
			Controls.vertical_thrust_time -= joy_axis[kc_joystick[19].value];
	}

	// From joystick buttons
	if ( (use_joystick)&&(kc_joystick[8].value < 255 )) Controls.vertical_thrust_time += joy_get_button_down_time( kc_joystick[8].value );
	if ( (use_joystick)&&(kc_joystick[9].value < 255 )) Controls.vertical_thrust_time -= joy_get_button_down_time( kc_joystick[9].value );

	// From mouse buttons
	if ( (use_mouse)&&(kc_mouse[8].value < 255 )) Controls.vertical_thrust_time += mouse_button_down_time( kc_mouse[8].value );
	if ( (use_mouse)&&(kc_mouse[9].value < 255 )) Controls.vertical_thrust_time -= mouse_button_down_time( kc_mouse[9].value );

	// From mouse...
	if ( (use_mouse)&&(kc_mouse[19].value < 255 ))	{
		if ( !kc_mouse[20].value )		// If not inverted...
			Controls.vertical_thrust_time += mouse_axis[kc_mouse[19].value];
		else
			Controls.vertical_thrust_time -= mouse_axis[kc_mouse[19].value];
	}

	// From Cyberman...
	if ((use_mouse)&&(Config_control_type==CONTROL_CYBERMAN))	{
		Controls.vertical_thrust_time += mouse_button_down_time(MB_Z_UP)/2;
		Controls.vertical_thrust_time -= mouse_button_down_time(MB_Z_DOWN)/2;
	}
}// end "if" added by WraithX

//---------- Read heading_time -----------

	if (!slide_on && !bank_on)	{
		//mprintf((0, "heading: %7.3f %7.3f: %7.3f\n", f2fl(k4), f2fl(k6), f2fl(Controls.heading_time)));
		kh = 0;
		k4 = speed_factor*key_down_time( kc_keyboard[4].value );
		k5 = speed_factor*key_down_time( kc_keyboard[5].value );
		k6 = speed_factor*key_down_time( kc_keyboard[6].value );
		k7 = speed_factor*key_down_time( kc_keyboard[7].value );

		// From keyboard...
		if ( kc_keyboard[4].value < 255 ) kh -= k4/PH_SCALE;
		if ( kc_keyboard[5].value < 255 ) kh -= k5/PH_SCALE;
		if ( kc_keyboard[6].value < 255 ) kh += k6/PH_SCALE;
		if ( kc_keyboard[7].value < 255 ) kh += k7/PH_SCALE;

		// From Cyberman...
		if ((use_mouse)&&(Config_control_type==CONTROL_CYBERMAN))	{
			kh -= mouse_button_down_time(MB_HEAD_LEFT)/PH_SCALE;
			kh += mouse_button_down_time(MB_HEAD_RIGHT)/PH_SCALE;
		}
	
		if (kh == 0)
			Controls.heading_time = 0;
		else if (kh > 0) {
			if (Controls.heading_time < 0)
				Controls.heading_time = 0;
		} else // kh < 0
			if (Controls.heading_time > 0)
				Controls.heading_time = 0;
		Controls.heading_time += kh;

		// From joystick...
		if ( (use_joystick)&&(kc_joystick[15].value < 255 ))	{
			if ( !kc_joystick[16].value )		// If not inverted...
				Controls.heading_time += (joy_axis[kc_joystick[15].value]*Config_joystick_sensitivity)/8;
			else
				Controls.heading_time -= (joy_axis[kc_joystick[15].value]*Config_joystick_sensitivity)/8;
		}
	
		// From mouse...
		if ( (use_mouse)&&(kc_mouse[15].value < 255 ))	{
			if ( !kc_mouse[16].value )		// If not inverted...
				Controls.heading_time += (mouse_axis[kc_mouse[15].value]*Config_joystick_sensitivity)/8;
			else
				Controls.heading_time -= (mouse_axis[kc_mouse[15].value]*Config_joystick_sensitivity)/8;
		}
	} else {
		Controls.heading_time = 0;
	}

// the following "if" added by WraithX, 4/14/00
// done so that dead players can't move
if (!Player_is_dead)
{
//----------- Read sideways_thrust_time -----------------

	if ( slide_on )	{
		k0 = speed_factor*key_down_time( kc_keyboard[4].value );
		k1 = speed_factor*key_down_time( kc_keyboard[5].value );
		k2 = speed_factor*key_down_time( kc_keyboard[6].value );
		k3 = speed_factor*key_down_time( kc_keyboard[7].value );

		// From keyboard...
		if ( kc_keyboard[4].value < 255 ) Controls.sideways_thrust_time -= k0;
		if ( kc_keyboard[5].value < 255 ) Controls.sideways_thrust_time -= k1;
		if ( kc_keyboard[6].value < 255 ) Controls.sideways_thrust_time += k2;
		if ( kc_keyboard[7].value < 255 ) Controls.sideways_thrust_time += k3;
	
		// From joystick...
		if ( (use_joystick)&&(kc_joystick[15].value < 255 ))	{
			if ( !kc_joystick[16].value )		// If not inverted...
				Controls.sideways_thrust_time += joy_axis[kc_joystick[15].value];
			else
				Controls.sideways_thrust_time -= joy_axis[kc_joystick[15].value];
		}
		
		// From cyberman
		if ((use_mouse)&&(Config_control_type==CONTROL_CYBERMAN))	{
			Controls.sideways_thrust_time -= mouse_button_down_time(MB_HEAD_LEFT);
			Controls.sideways_thrust_time += mouse_button_down_time(MB_HEAD_RIGHT);
		}
	
		// From mouse...
		if ( (use_mouse)&&(kc_mouse[15].value < 255 ))	{
			if ( !kc_mouse[16].value )		// If not inverted...
				Controls.sideways_thrust_time += mouse_axis[kc_mouse[15].value];
			else
				Controls.sideways_thrust_time -= mouse_axis[kc_mouse[15].value];
		}
	}

	// From keyboard...
	if ( kc_keyboard[10].value < 255 ) Controls.sideways_thrust_time -= speed_factor*key_down_time( kc_keyboard[10].value );
	if ( kc_keyboard[11].value < 255 ) Controls.sideways_thrust_time -= speed_factor*key_down_time( kc_keyboard[11].value );
	if ( kc_keyboard[12].value < 255 ) Controls.sideways_thrust_time += speed_factor*key_down_time( kc_keyboard[12].value );
	if ( kc_keyboard[13].value < 255 ) Controls.sideways_thrust_time += speed_factor*key_down_time( kc_keyboard[13].value );
	
	// From joystick...
	if ( (use_joystick)&&(kc_joystick[17].value < 255 ))	{
		if ( !kc_joystick[18].value )		// If not inverted...
			Controls.sideways_thrust_time -= joy_axis[kc_joystick[17].value];
		else
			Controls.sideways_thrust_time += joy_axis[kc_joystick[17].value];
	}

	// From joystick buttons
	if ( (use_joystick)&&(kc_joystick[6].value < 255 )) Controls.sideways_thrust_time -= joy_get_button_down_time( kc_joystick[6].value );
	if ( (use_joystick)&&(kc_joystick[7].value < 255 )) Controls.sideways_thrust_time += joy_get_button_down_time( kc_joystick[7].value );

	// From mouse buttons
	if ( (use_mouse)&&(kc_mouse[6].value < 255 )) Controls.sideways_thrust_time -= mouse_button_down_time( kc_mouse[6].value );
	if ( (use_mouse)&&(kc_mouse[7].value < 255 )) Controls.sideways_thrust_time += mouse_button_down_time( kc_mouse[7].value );

	// From mouse...
	if ( (use_mouse)&&(kc_mouse[17].value < 255 ))	{
		if ( !kc_mouse[18].value )		// If not inverted...
			Controls.sideways_thrust_time += mouse_axis[kc_mouse[17].value];
		else
			Controls.sideways_thrust_time -= mouse_axis[kc_mouse[17].value];
	}
}// end "if" added by WraithX

//----------- Read bank_time -----------------

	if ( bank_on )	{
		k0 = speed_factor*key_down_time( kc_keyboard[4].value );
		k1 = speed_factor*key_down_time( kc_keyboard[5].value );
		k2 = speed_factor*key_down_time( kc_keyboard[6].value );
		k3 = speed_factor*key_down_time( kc_keyboard[7].value );

		// From keyboard...
		if ( kc_keyboard[4].value < 255 ) Controls.bank_time += k0;
		if ( kc_keyboard[5].value < 255 ) Controls.bank_time += k1;
		if ( kc_keyboard[6].value < 255 ) Controls.bank_time -= k2;
		if ( kc_keyboard[7].value < 255 ) Controls.bank_time -= k3;

		// From Cyberman...
		if ((use_mouse)&&(Config_control_type==CONTROL_CYBERMAN))	{
			Controls.bank_time -= mouse_button_down_time(MB_HEAD_LEFT);
			Controls.bank_time += mouse_button_down_time(MB_HEAD_RIGHT);
		}

		// From joystick...
		if ( (use_joystick)&&(kc_joystick[15].value < 255) )	{
			if ( !kc_joystick[16].value )		// If not inverted...
				Controls.bank_time -= (joy_axis[kc_joystick[15].value]*Config_joystick_sensitivity)/8;
			else
				Controls.bank_time += (joy_axis[kc_joystick[15].value]*Config_joystick_sensitivity)/8;
		}
	
		// From mouse...
		if ( (use_mouse)&&(kc_mouse[15].value < 255 ))	{
			if ( !kc_mouse[16].value )		// If not inverted...
				Controls.bank_time += (mouse_axis[kc_mouse[15].value]*Config_joystick_sensitivity)/8;
			else
				Controls.bank_time -= (mouse_axis[kc_mouse[15].value]*Config_joystick_sensitivity)/8;
		}
	}

	// From keyboard...
	if ( kc_keyboard[20].value < 255 ) Controls.bank_time += speed_factor*key_down_time( kc_keyboard[20].value );
	if ( kc_keyboard[21].value < 255 ) Controls.bank_time += speed_factor*key_down_time( kc_keyboard[21].value );
	if ( kc_keyboard[22].value < 255 ) Controls.bank_time -= speed_factor*key_down_time( kc_keyboard[22].value );
	if ( kc_keyboard[23].value < 255 ) Controls.bank_time -= speed_factor*key_down_time( kc_keyboard[23].value );

	// From joystick...
	if ( (use_joystick)&&(kc_joystick[21].value < 255) )	{
		if ( !kc_joystick[22].value )		// If not inverted...
			Controls.bank_time -= joy_axis[kc_joystick[21].value];
		else
			Controls.bank_time += joy_axis[kc_joystick[21].value];
	}

	// From joystick buttons
	if ( (use_joystick)&&(kc_joystick[11].value < 255 )) Controls.bank_time += joy_get_button_down_time( kc_joystick[11].value );
	if ( (use_joystick)&&(kc_joystick[12].value < 255 )) Controls.bank_time -= joy_get_button_down_time( kc_joystick[12].value );

	// From mouse buttons
	if ( (use_mouse)&&(kc_mouse[11].value < 255 )) Controls.bank_time += mouse_button_down_time( kc_mouse[11].value );
	if ( (use_mouse)&&(kc_mouse[12].value < 255 )) Controls.bank_time -= mouse_button_down_time( kc_mouse[12].value );

	// From mouse...
	if ( (use_mouse)&&(kc_mouse[21].value < 255 ))	{
		if ( !kc_mouse[22].value )		// If not inverted...
			Controls.bank_time += mouse_axis[kc_mouse[21].value];
		else
			Controls.bank_time -= mouse_axis[kc_mouse[21].value];
	}

	// From Cyberman
	if ((use_mouse)&&(Config_control_type==CONTROL_CYBERMAN))	{
		Controls.bank_time += mouse_button_down_time(MB_BANK_LEFT);
		Controls.bank_time -= mouse_button_down_time(MB_BANK_RIGHT);
	}

// the following "if" added by WraithX, 4/14/00
// done so that dead players can't move
if (!Player_is_dead)
{
//----------- Read forward_thrust_time -------------

	// From keyboard...
	if ( kc_keyboard[30].value < 255 ) Controls.forward_thrust_time += speed_factor*key_down_time( kc_keyboard[30].value );
	if ( kc_keyboard[31].value < 255 ) Controls.forward_thrust_time += speed_factor*key_down_time( kc_keyboard[31].value );
	if ( kc_keyboard[32].value < 255 ) Controls.forward_thrust_time -= speed_factor*key_down_time( kc_keyboard[32].value );
	if ( kc_keyboard[33].value < 255 ) Controls.forward_thrust_time -= speed_factor*key_down_time( kc_keyboard[33].value );

	// From joystick...
	if ( (use_joystick)&&(kc_joystick[23].value < 255 ))	{
		if ( !kc_joystick[24].value )		// If not inverted...
			Controls.forward_thrust_time -= joy_axis[kc_joystick[23].value];
		else
			Controls.forward_thrust_time += joy_axis[kc_joystick[23].value];
	}

	// From joystick buttons
	if ( (use_joystick)&&(kc_joystick[2].value < 255 )) Controls.forward_thrust_time += joy_get_button_down_time( kc_joystick[2].value );
	if ( (use_joystick)&&(kc_joystick[3].value < 255 )) Controls.forward_thrust_time -= joy_get_button_down_time( kc_joystick[3].value );

	// From mouse...
	if ( (use_mouse)&&(kc_mouse[23].value < 255 ))	{
		if ( !kc_mouse[24].value )		// If not inverted...
			Controls.forward_thrust_time -= mouse_axis[kc_mouse[23].value];
		else
			Controls.forward_thrust_time += mouse_axis[kc_mouse[23].value];
	}

	// From mouse buttons
	if ( (use_mouse)&&(kc_mouse[2].value < 255 )) Controls.forward_thrust_time += mouse_button_down_time( kc_mouse[2].value );
	if ( (use_mouse)&&(kc_mouse[3].value < 255 )) Controls.forward_thrust_time -= mouse_button_down_time( kc_mouse[3].value );

//----------- Read afterburner_state -------------

	// From keyboard...
	if ( kc_keyboard[46].value < 255 ) Controls.afterburner_state |= keyd_pressed[kc_keyboard[46].value];
	if ( kc_keyboard[47].value < 255 ) Controls.afterburner_state |= keyd_pressed[kc_keyboard[47].value];

        if ( (use_mouse)&&(kc_mouse[27].value < 255 )) Controls.afterburner_state |= mouse_button_state(kc_mouse[27].value);

        if ( (use_joystick)&&(kc_joystick[27].value < 255 )) Controls.afterburner_state |= joy_get_button_state(kc_joystick[27].value);

//-------Read headlight key--------------------------

        if (kc_keyboard[52].value < 255 )
		   Controls.headlight_count=key_down_count(kc_keyboard[52].value);
        if (kc_keyboard[53].value < 255 )
		   Controls.headlight_count+=key_down_count(kc_keyboard[53].value);
		  if ((use_joystick)&&(kc_joystick[30].value < 255 )) 
	       Controls.headlight_count+=joy_get_button_down_cnt(kc_joystick[30].value);

//--------Read Cycle Primary Key------------------

		  if (kc_keyboard[48].value<255)
			 Controls.cycle_primary_count=key_down_count(kc_keyboard[48].value);
		  if (kc_keyboard[49].value<255)
			Controls.cycle_primary_count+=key_down_count(kc_keyboard[49].value);
	     if ((use_joystick)&&(kc_joystick[28].value < 255 )) 
         Controls.cycle_primary_count+=joy_get_button_down_cnt(kc_joystick[28].value);


//--------Read Cycle Secondary Key------------------

		  if (kc_keyboard[50].value<255)
			 Controls.cycle_secondary_count=key_down_count(kc_keyboard[50].value);
		  if (kc_keyboard[51].value<255)
			Controls.cycle_secondary_count+=key_down_count(kc_keyboard[51].value);
	     if ((use_joystick)&&(kc_joystick[29].value < 255 )) 
			Controls.cycle_secondary_count=joy_get_button_down_cnt(kc_joystick[29].value);

//--------Read Toggle Bomb key----------------------

		  if (kc_keyboard[56].value<255 && key_down_count(kc_keyboard[56].value))
         {
          int bomb = Secondary_last_was_super[PROXIMITY_INDEX]?PROXIMITY_INDEX:SMART_MINE_INDEX;

			 if (!Players[Player_num].secondary_ammo[PROXIMITY_INDEX] &&
				  !Players[Player_num].secondary_ammo[SMART_MINE_INDEX])
			   {
				 digi_play_sample_once( SOUND_BAD_SELECTION, F1_0 );
				 HUD_init_message ("No bombs available!");
				}
			 else
				{	
				 if (Players[Player_num].secondary_ammo[bomb]==0)
					{
					 digi_play_sample_once( SOUND_BAD_SELECTION, F1_0 );
					 HUD_init_message ("No %s available!",(bomb==SMART_MINE_INDEX)?"Smart mines":"Proximity bombs");
					}
				  else
					{
			       Secondary_last_was_super[PROXIMITY_INDEX]=!Secondary_last_was_super[PROXIMITY_INDEX];
					 digi_play_sample_once( SOUND_GOOD_SELECTION_SECONDARY, F1_0 );
					}
				}
			}
          
//---------Read Energy->Shield key----------

        if ((Players[Player_num].flags & PLAYER_FLAGS_CONVERTER) && keyd_pressed[kc_keyboard[54].value])
                transfer_energy_to_shield(key_down_time(kc_keyboard[54].value));
        if ((Players[Player_num].flags & PLAYER_FLAGS_CONVERTER) && keyd_pressed[kc_keyboard[55].value])
                transfer_energy_to_shield(key_down_time(kc_keyboard[55].value));

//----------- Read fire_primary_down_count
	if (kc_keyboard[24].value < 255 ) Controls.fire_primary_down_count += key_down_count(kc_keyboard[24].value);
	if (kc_keyboard[25].value < 255 ) Controls.fire_primary_down_count += key_down_count(kc_keyboard[25].value);
	if ((use_joystick)&&(kc_joystick[0].value < 255 )) Controls.fire_primary_down_count += joy_get_button_down_cnt(kc_joystick[0].value);
	if ((use_mouse)&&(kc_mouse[0].value < 255 )) Controls.fire_primary_down_count += mouse_button_down_count(kc_mouse[0].value);

//----------- Read fire_primary_state
	if (kc_keyboard[24].value < 255 ) Controls.fire_primary_state |= keyd_pressed[kc_keyboard[24].value];
	if (kc_keyboard[25].value < 255 ) Controls.fire_primary_state |= keyd_pressed[kc_keyboard[25].value];
	if ((use_joystick)&&(kc_joystick[0].value < 255 )) Controls.fire_primary_state |= joy_get_button_state(kc_joystick[0].value);
	if ((use_mouse)&&(kc_mouse[0].value < 255) ) Controls.fire_primary_state |= mouse_button_state(kc_mouse[0].value);

//----------- Read fire_secondary_down_count
	if (kc_keyboard[26].value < 255 ) Controls.fire_secondary_down_count += key_down_count(kc_keyboard[26].value);
	if (kc_keyboard[27].value < 255 ) Controls.fire_secondary_down_count += key_down_count(kc_keyboard[27].value);
	if ((use_joystick)&&(kc_joystick[1].value < 255 )) Controls.fire_secondary_down_count += joy_get_button_down_cnt(kc_joystick[1].value);
	if ((use_mouse)&&(kc_mouse[1].value < 255 )) Controls.fire_secondary_down_count += mouse_button_down_count(kc_mouse[1].value);

//----------- Read fire_secondary_state
	if (kc_keyboard[26].value < 255 ) Controls.fire_secondary_state |= keyd_pressed[kc_keyboard[26].value];
	if (kc_keyboard[27].value < 255 ) Controls.fire_secondary_state |= keyd_pressed[kc_keyboard[27].value];
	if ((use_joystick)&&(kc_joystick[1].value < 255 )) Controls.fire_secondary_state |= joy_get_button_state(kc_joystick[1].value);
	if ((use_mouse)&&(kc_mouse[1].value < 255) ) Controls.fire_secondary_state |= mouse_button_state(kc_mouse[1].value);

//----------- Read fire_flare_down_count
	if (kc_keyboard[28].value < 255 ) Controls.fire_flare_down_count += key_down_count(kc_keyboard[28].value);
	if (kc_keyboard[29].value < 255 ) Controls.fire_flare_down_count += key_down_count(kc_keyboard[29].value);
	if ((use_joystick)&&(kc_joystick[4].value < 255 )) Controls.fire_flare_down_count += joy_get_button_down_cnt(kc_joystick[4].value);
	if ((use_mouse)&&(kc_mouse[4].value < 255 )) Controls.fire_flare_down_count += mouse_button_down_count(kc_mouse[4].value);

//----------- Read drop_bomb_down_count
	if (kc_keyboard[34].value < 255 ) Controls.drop_bomb_down_count += key_down_count(kc_keyboard[34].value);
	if (kc_keyboard[35].value < 255 ) Controls.drop_bomb_down_count += key_down_count(kc_keyboard[35].value);
	if ((use_joystick)&&(kc_joystick[26].value < 255 )) Controls.drop_bomb_down_count += joy_get_button_down_cnt(kc_joystick[26].value);
	if ((use_mouse)&&(kc_mouse[26].value < 255 )) Controls.drop_bomb_down_count += mouse_button_down_count(kc_mouse[26].value);

//----------- Read rear_view_down_count
	if (kc_keyboard[36].value < 255 ) Controls.rear_view_down_count += key_down_count(kc_keyboard[36].value);
	if (kc_keyboard[37].value < 255 ) Controls.rear_view_down_count += key_down_count(kc_keyboard[37].value);
	if ((use_joystick)&&(kc_joystick[25].value < 255 )) Controls.rear_view_down_count += joy_get_button_down_cnt(kc_joystick[25].value);
	if ((use_mouse)&&(kc_mouse[25].value < 255 )) Controls.rear_view_down_count += mouse_button_down_count(kc_mouse[25].value);

//----------- Read rear_view_down_state
	if (kc_keyboard[36].value < 255 ) Controls.rear_view_down_state |= keyd_pressed[kc_keyboard[36].value];
	if (kc_keyboard[37].value < 255 ) Controls.rear_view_down_state |= keyd_pressed[kc_keyboard[37].value];
	if ((use_joystick)&&(kc_joystick[25].value < 255 )) Controls.rear_view_down_state |= joy_get_button_state(kc_joystick[25].value);
	if ((use_mouse)&&(kc_mouse[25].value < 255 )) Controls.rear_view_down_state |= mouse_button_state(kc_mouse[25].value);

}//end "if" added by WraithX

//----------- Read automap_down_count
	if (kc_keyboard[44].value < 255 ) Controls.automap_down_count += key_down_count(kc_keyboard[44].value);
	if (kc_keyboard[45].value < 255 ) Controls.automap_down_count += key_down_count(kc_keyboard[45].value);

//----------- Read automap_state
	if (kc_keyboard[44].value < 255 ) Controls.automap_state |= keyd_pressed[kc_keyboard[44].value];
	if (kc_keyboard[45].value < 255 ) Controls.automap_state |= keyd_pressed[kc_keyboard[45].value];

//----------- Read stupid-cruise-control-type of throttle.
	{
		if ( kc_keyboard[38].value < 255 ) Cruise_speed += fixdiv(speed_factor*key_down_time(kc_keyboard[38].value)*5,FrameTime);
		if ( kc_keyboard[39].value < 255 ) Cruise_speed += fixdiv(speed_factor*key_down_time(kc_keyboard[39].value)*5,FrameTime);
		if ( kc_keyboard[40].value < 255 ) Cruise_speed -= fixdiv(speed_factor*key_down_time(kc_keyboard[40].value)*5,FrameTime);
		if ( kc_keyboard[41].value < 255 ) Cruise_speed -= fixdiv(speed_factor*key_down_time(kc_keyboard[41].value)*5,FrameTime);
		if ( (kc_keyboard[42].value < 255) && (key_down_count(kc_keyboard[42].value)) )
			Cruise_speed = 0;
		if ( (kc_keyboard[43].value < 255) && (key_down_count(kc_keyboard[43].value)) )
			Cruise_speed = 0;
	
		if (Cruise_speed > i2f(100) ) Cruise_speed = i2f(100);
		if (Cruise_speed < 0 ) Cruise_speed = 0;
	
		if (Controls.forward_thrust_time==0)
			Controls.forward_thrust_time = fixmul(Cruise_speed,FrameTime)/100;
	}

#if !defined(MACINTOSH)
	//read_head_tracker();

	// Read external controls
	// if (kc_use_external_control || CybermouseActive)
	//	kconfig_read_external_controls();
#endif

//----------- Clamp values between -FrameTime and FrameTime
	if (FrameTime > F1_0 )
		mprintf( (1, "Bogus frame time of %.2f seconds\n", f2fl(FrameTime) ));

// ZICO - remove clamp for pitch and heading if mouselook on and no multiplayer game
if ((Config_control_type != 5) || !(i=FindArg("-mouselook")) || (Game_mode & GM_MULTI) ) {
	if (Controls.pitch_time > FrameTime/2 ) Controls.pitch_time = FrameTime/2;
	if (Controls.heading_time > FrameTime ) Controls.heading_time = FrameTime;
	if (Controls.pitch_time < -FrameTime/2 ) Controls.pitch_time = -FrameTime/2;
	if (Controls.heading_time < -FrameTime ) Controls.heading_time = -FrameTime;
}
	if (Controls.vertical_thrust_time > FrameTime ) Controls.vertical_thrust_time = FrameTime;
	if (Controls.sideways_thrust_time > FrameTime ) Controls.sideways_thrust_time = FrameTime;
	if (Controls.bank_time > FrameTime ) Controls.bank_time = FrameTime;
	if (Controls.forward_thrust_time > FrameTime ) Controls.forward_thrust_time = FrameTime;
//	if (Controls.afterburner_time > FrameTime ) Controls.afterburner_time = FrameTime;

	if (Controls.vertical_thrust_time < -FrameTime ) Controls.vertical_thrust_time = -FrameTime;
	if (Controls.sideways_thrust_time < -FrameTime ) Controls.sideways_thrust_time = -FrameTime;
	if (Controls.bank_time < -FrameTime ) Controls.bank_time = -FrameTime;
	if (Controls.forward_thrust_time < -FrameTime ) Controls.forward_thrust_time = -FrameTime;
//	if (Controls.afterburner_time < -FrameTime ) Controls.afterburner_time = -FrameTime;


//--------- Don't do anything if in debug mode
	#ifndef RELEASE
	if ( keyd_pressed[KEY_DELETE] )	{
		memset( &Controls, 0, sizeof(control_info) );
	}
	#endif
}
#endif

void reset_cruise(void)
{
	Cruise_speed=0;
}


void kc_set_controls()
{
	int i;

	for (i=0; i<NUM_KEY_CONTROLS; i++ )	
		kc_keyboard[i].value = kconfig_settings[0][i];

	if ( (Config_control_type>0) && (Config_control_type<5)) {
		for (i=0; i<NUM_OTHER_CONTROLS; i++ ) {
			kc_joystick[i].value = kconfig_settings[Config_control_type][i];
			if (kc_joystick[i].type == BT_INVERT )	{
				if (kc_joystick[i].value!=1)
					kc_joystick[i].value	= 0;
				kconfig_settings[Config_control_type][i] = kc_joystick[i].value;
			}
		}
	} else if (Config_control_type>4 && Config_control_type<CONTROL_WINJOYSTICK) {
		for (i=0; i<NUM_OTHER_CONTROLS; i++ )	{
			kc_mouse[i].value = kconfig_settings[Config_control_type][i];
			if (kc_mouse[i].type == BT_INVERT )	{
				if (kc_mouse[i].value!=1)
					kc_mouse[i].value	= 0;
				kconfig_settings[Config_control_type][i] = kc_mouse[i].value;
			}
		}
	} else if (Config_control_type == CONTROL_WINJOYSTICK) {
		for (i=0; i<NUM_OTHER_CONTROLS; i++ ) {
			kc_superjoy[i].value = kconfig_settings[Config_control_type][i];
			if (kc_superjoy[i].type == BT_INVERT )	{
				if (kc_superjoy[i].value!=1)
					kc_superjoy[i].value	= 0;
				kconfig_settings[Config_control_type][i] = kc_superjoy[i].value;
			}
		}
	}

#ifdef D2X_KEYS
	for (i=0; i<NUM_D2X_CONTROLS; i++ )
		kc_d2x[i].value = kconfig_d2x_settings[i];
#endif
}

void CybermouseAdjust ()
 {
	Controls.pitch_time += fixmul(kc_external_control->pitch_time,FrameTime);	
	Controls.vertical_thrust_time += fixmul(kc_external_control->vertical_thrust_time,FrameTime);
	Controls.heading_time += fixmul(kc_external_control->heading_time,FrameTime);
	Controls.sideways_thrust_time += fixmul(kc_external_control->sideways_thrust_time ,FrameTime);
	Controls.bank_time += fixmul(kc_external_control->bank_time ,FrameTime);
	Controls.forward_thrust_time += fixmul(kc_external_control->forward_thrust_time ,FrameTime);
//	Controls.rear_view_down_count += kc_external_control->rear_view_down_count;	
//	Controls.rear_view_down_state |= kc_external_control->rear_view_down_state;	
	Controls.fire_primary_down_count += kc_external_control->fire_primary_down_count;
	Controls.fire_primary_state |= kc_external_control->fire_primary_state;
	Controls.fire_secondary_state |= kc_external_control->fire_secondary_state;
	Controls.fire_secondary_down_count += kc_external_control->fire_secondary_down_count;
	Controls.fire_flare_down_count += kc_external_control->fire_flare_down_count;
	Controls.drop_bomb_down_count += kc_external_control->drop_bomb_down_count;	
//	Controls.automap_down_count += kc_external_control->automap_down_count;
// 	Controls.automap_state |= kc_external_control->automap_state;
  } 

char GetKeyValue (char key)
  {
	mprintf ((0,"Returning %c!\n",kc_keyboard[(int)key].value));
	return (kc_keyboard[(int)key].value);
  }

#if !defined(MACINTOSH)


extern object *obj_find_first_of_type (int);
void kconfig_read_external_controls()
{
	//union REGS r;
	int i;

	if ( !kc_enable_external_control ) return;

	if ( kc_external_version == 0 ) 
		memset( kc_external_control, 0, sizeof(ext_control_info));
	else if ( kc_external_version > 0 ) 	{

		if (kc_external_version>=4)
			memset( kc_external_control, 0, sizeof(advanced_ext_control_info));
		else if (kc_external_version>0)
			memset( kc_external_control, 0, sizeof(ext_control_info)+sizeof(vms_angvec) + 64 );
		else if (kc_external_version>2)
			memset( kc_external_control, 0, sizeof(ext_control_info)+sizeof(vms_angvec) + 64 + sizeof(vms_vector) + sizeof(vms_matrix) +4 );

		if ( kc_external_version > 1 ) {
			// Write ship pos and angles to external controls...
			ubyte *temp_ptr = (ubyte *)kc_external_control;
			vms_vector *ship_pos;
			vms_matrix *ship_orient;
			memset( kc_external_control, 0, sizeof(ext_control_info)+sizeof(vms_angvec) + 64 + sizeof(vms_vector)+sizeof(vms_matrix) );
			temp_ptr += sizeof(ext_control_info) + sizeof(vms_angvec) + 64;
			ship_pos = (vms_vector *)temp_ptr;
			temp_ptr += sizeof(vms_vector);
			ship_orient = (vms_matrix *)temp_ptr;
			// Fill in ship postion...
			*ship_pos = Objects[Players[Player_num].objnum].pos;
			// Fill in ship orientation...
			*ship_orient = Objects[Players[Player_num].objnum].orient;
		}
		if (kc_external_version>=4)
		{
			advanced_ext_control_info *temp_ptr=(advanced_ext_control_info *)kc_external_control;
		
			temp_ptr->headlight_state=(Players[Player_num].flags & PLAYER_FLAGS_HEADLIGHT_ON);
					temp_ptr->primary_weapon_flags=Players[Player_num].primary_weapon_flags;
					temp_ptr->secondary_weapon_flags=Players[Player_num].secondary_weapon_flags;
			temp_ptr->current_primary_weapon=Primary_weapon;
			temp_ptr->current_secondary_weapon=Secondary_weapon;
			
			temp_ptr->current_guidebot_command=Escort_goal_object;
			
			temp_ptr->force_vector=ExtForceVec;
			temp_ptr->force_matrix=ExtApplyForceMatrix;
			for (i=0;i<3;i++)
				temp_ptr->joltinfo[i]=ExtJoltInfo[i];  
			for (i=0;i<2;i++)
					temp_ptr->x_vibrate_info[i]=ExtXVibrateInfo[i];
					temp_ptr->x_vibrate_clear=ExtXVibrateClear;
				temp_ptr->game_status=ExtGameStatus;
			
			memset ((void *)&ExtForceVec,0,sizeof(vms_vector));
			memset ((void *)&ExtApplyForceMatrix,0,sizeof(vms_matrix));
			
			for (i=0;i<3;i++)
				ExtJoltInfo[i]=0;
			for (i=0;i<2;i++)
				ExtXVibrateInfo[i]=0;
			ExtXVibrateClear=0;
		}
	}

	if ( Automap_active )			// (If in automap...)
		kc_external_control->automap_state = 1;
	//memset(&r,0,sizeof(r));

	if ( Player_num > -1 )	{
		Objects[Players[Player_num].objnum].mtype.phys_info.flags &= (~PF_TURNROLL);	// Turn off roll when turning
		Objects[Players[Player_num].objnum].mtype.phys_info.flags &= (~PF_LEVELLING);	// Turn off leveling to nearest side.
		Auto_leveling_on = 0;

		if ( kc_external_version > 0 ) {		
			vms_matrix tempm, ViewMatrix;
			vms_angvec * Kconfig_abs_movement;
			char * oem_message;
	
			Kconfig_abs_movement = (vms_angvec *)((uint)kc_external_control + sizeof(ext_control_info));
	
			if ( Kconfig_abs_movement->p || Kconfig_abs_movement->b || Kconfig_abs_movement->h )	{
				vm_angles_2_matrix(&tempm,Kconfig_abs_movement);
				vm_matrix_x_matrix(&ViewMatrix,&Objects[Players[Player_num].objnum].orient,&tempm);
				Objects[Players[Player_num].objnum].orient = ViewMatrix;		
			}
			oem_message = (char *)((uint)Kconfig_abs_movement + sizeof(vms_angvec));
			if (oem_message[0] != '\0' )
				HUD_init_message( oem_message );
		}
	}

	Controls.pitch_time += fixmul(kc_external_control->pitch_time,FrameTime);						
	Controls.vertical_thrust_time += fixmul(kc_external_control->vertical_thrust_time,FrameTime);
	Controls.heading_time += fixmul(kc_external_control->heading_time,FrameTime);
	Controls.sideways_thrust_time += fixmul(kc_external_control->sideways_thrust_time ,FrameTime);
	Controls.bank_time += fixmul(kc_external_control->bank_time ,FrameTime);
	Controls.forward_thrust_time += fixmul(kc_external_control->forward_thrust_time ,FrameTime);
	Controls.rear_view_down_count += kc_external_control->rear_view_down_count;	
	Controls.rear_view_down_state |= kc_external_control->rear_view_down_state;	
	Controls.fire_primary_down_count += kc_external_control->fire_primary_down_count;
	Controls.fire_primary_state |= kc_external_control->fire_primary_state;
	Controls.fire_secondary_state |= kc_external_control->fire_secondary_state;
	Controls.fire_secondary_down_count += kc_external_control->fire_secondary_down_count;
	Controls.fire_flare_down_count += kc_external_control->fire_flare_down_count;
	Controls.drop_bomb_down_count += kc_external_control->drop_bomb_down_count;	
	Controls.automap_down_count += kc_external_control->automap_down_count;
	Controls.automap_state |= kc_external_control->automap_state;
	
	if (kc_external_version>=3)
	{
		ubyte *temp_ptr = (ubyte *)kc_external_control;
		temp_ptr += (sizeof(ext_control_info) + sizeof(vms_angvec) + 64 + sizeof(vms_vector) + sizeof (vms_matrix));
	
		if (*(temp_ptr))
			Controls.cycle_primary_count=(*(temp_ptr));
		if (*(temp_ptr+1))
			Controls.cycle_secondary_count=(*(temp_ptr+1));
	
		if (*(temp_ptr+2))
			Controls.afterburner_state=(*(temp_ptr+2));
		if (*(temp_ptr+3))
			Controls.headlight_count=(*(temp_ptr+3));
	}
	if (kc_external_version>=4)
	{
		int i;
		advanced_ext_control_info *temp_ptr=(advanced_ext_control_info *)kc_external_control;
	
		for (i=0;i<128;i++)
			if (temp_ptr->keyboard[i])
				key_putkey (i);
	
		if (temp_ptr->Reactor_blown)
		{
			if (Game_mode & GM_MULTI)
				net_destroy_controlcen (obj_find_first_of_type (OBJ_CNTRLCEN));
			else
				do_controlcen_destroyed_stuff(obj_find_first_of_type (OBJ_CNTRLCEN));
		}
	}
	
}
#endif

