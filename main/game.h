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
 * Constants & prototypes which pertain to the game only
 *
 */

#ifndef _GAME_H
#define _GAME_H

#include "pstypes.h"
#include "window.h"
#include "cfile.h"
#include "vecmat.h"

#ifdef NDEBUG
#define MAXIMUM_FPS 200
#else
#define MAXIMUM_FPS 1000
#endif

struct object;

extern struct window *Game_wind;

// from mglobal.c
extern fix FrameTime;           // time in seconds since last frame
extern fix GameTime;            // time in game (sum of FrameTime)
extern int FrameCount;          // how many frames rendered
extern int FixedStep;		//fixed time bytes stored here
extern fix Next_laser_fire_time;    // Time at which player can next fire his selected laser.
extern fix Last_laser_fired_time;
extern fix Next_missile_fire_time;  // Time at which player can next fire his selected missile.
extern fix Next_flare_fire_time;
extern fix Laser_delay_time;        // Delay between laser fires.
extern fix Fusion_next_sound_time;
extern int Cheats_enabled;

// bits for FixedStep
#define EPS4	1
#define EPS20	2
#define EPS30	4

extern struct object *Missile_viewer;
extern int Missile_viewer_sig;

#define CV_NONE     0
#define CV_ESCORT   1
#define CV_REAR     2
#define CV_COOP     3
#define CV_MARKER   4

extern int Coop_view_player[2];     // left & right
extern int Marker_viewer_num[2];    // left & right

// constants for ft_preference
#define FP_RIGHT        0
#define FP_UP           1
#define FP_FORWARD      2       // this is the default
#define FP_LEFT         3
#define FP_DOWN         4
#define FP_FIRST_TIME   5

extern int ft_preference;

// The following bits define the game modes.
#define GM_EDITOR       1       // You came into the game from the editor
// #define GM_SERIAL       2       // You are in serial mode // OBSOLETE
#define GM_NETWORK      4       // You are in network mode
#define GM_MULTI_ROBOTS 8       // You are in a multiplayer mode with robots.
#define GM_MULTI_COOP   16      // You are in a multiplayer mode and can't hurt other players.
// #define GM_MODEM        32      // You are in a modem (serial) game // OBSOLETE
#define GM_UNKNOWN      64      // You are not in any mode, kind of dangerous...
#define GM_GAME_OVER    128     // Game has been finished
#define GM_TEAM         256     // Team mode for network play
#define GM_CAPTURE      512     // Capture the flag mode for D2
#define GM_HOARD        1024    // New hoard mode for D2 Christmas
#define GM_NORMAL       0       // You are in normal play mode, no multiplayer stuff
#define GM_MULTI        38      // You are in some type of multiplayer game


#define NDL 5       // Number of difficulty levels.

extern int Game_mode;
extern u_int32_t Game_screen_mode;

extern int gauge_message_on;

#ifndef NDEBUG      // if debugging, these are variables

extern int Slew_on;                 // in slew or sim mode?

#else               // if not debugging, these are constants

#define Slew_on             0       // no slewing in real game
#define Game_double_buffer  1       // always double buffer in real game

#endif

// Suspend flags

#define SUSP_NONE       0           // Everything moving normally
#define SUSP_ROBOTS     1           // Robot AI doesn't move
#define SUSP_WEAPONS    2           // Lasers, etc. don't move

extern int Game_suspended;          // if non-zero, nothing moves but player

#define	SHOW_EXIT_PATH	1


// from game.c
void init_game(void);
void game(void);
void close_game(void);
void init_cockpit(void);
void calc_frame_time(void);
void FixedStepCalc();
int do_flythrough(struct object *obj,int first_time);

extern int Difficulty_level;    // Difficulty level in 0..NDL-1, 0 = easiest, NDL-1 = hardest
extern int Global_laser_firing_count;
extern int Global_missile_firing_count;
extern int Render_depth;
extern fix Auto_fire_fusion_cannon_time, Fusion_charge;

extern int PaletteRedAdd, PaletteGreenAdd, PaletteBlueAdd;

#define MAX_PALETTE_ADD 30

extern void PALETTE_FLASH_ADD(int dr, int dg, int db);

//sets the rgb values for palette flash
#define PALETTE_FLASH_SET(_r,_g,_b) PaletteRedAdd=(_r), PaletteGreenAdd=(_g), PaletteBlueAdd=(_b)

extern int draw_gauges_on;

extern void init_game_screen(void);

extern void game_flush_inputs();    // clear all inputs

extern int Playing_game;    // True if playing game
extern int Auto_flythrough; // if set, start flythough automatically
extern int Mark_count;      // number of debugging marks set
extern char faded_in;
extern int last_drawn_cockpit;

extern void stop_time(void);
extern void start_time(void);
extern void reset_time(void);       // called when starting level

// If automap_flag == 1, then call automap routine to write message.
extern void save_screen_shot(int automap_flag);

//valid modes for cockpit
#define CM_FULL_COCKPIT     0   // normal screen with cockput
#define CM_REAR_VIEW        1   // looking back with bitmap
#define CM_STATUS_BAR       2   // small status bar, w/ reticle
#define CM_FULL_SCREEN      3   // full screen, no cockpit (w/ reticle)
#define CM_LETTERBOX        4   // half-height window (for cutscenes)

extern int Game_window_w,       // width and height of player's game window
           Game_window_h;

extern int Rear_view;           // if true, looking back.

// initalize flying
void fly_init(struct object *obj);

// selects a given cockpit (or lack of one).
void select_cockpit(int mode);

// force cockpit redraw next time. call this if you've trashed the screen
void reset_cockpit(void);       // called if you've trashed the screen

// functions to save, clear, and resture palette flash effects
void palette_save(void);
void reset_palette_add(void);
void palette_restore(void);
void full_palette_save(void);	// all of the above plus gr_palette_load(gr_palette)

// put up the help message
void show_help();
void show_netgame_help();
void show_newdemo_help();

// show a message in a nice little box
void show_boxed_message(char *msg, int RenderFlag);

// turns off rear view & rear view cockpit
void reset_rear_view(void);

extern int Game_turbo_mode;

// returns ptr to escort robot, or NULL
struct object *find_escort();

extern void apply_modified_palette(void);

//Flickering light system
typedef struct  {
	short segnum, sidenum;
	unsigned long mask;     // determines flicker pattern
	fix timer;              // time until next change
	fix delay;              // time between changes
} flickering_light;

#define MAX_FLICKERING_LIGHTS 100

extern flickering_light Flickering_lights[MAX_FLICKERING_LIGHTS];
extern int Num_flickering_lights;

// returns ptr to flickering light structure, or NULL if can't find
flickering_light *find_flicker(int segnum, int sidenum);

// turn flickering off (because light has been turned off)
void disable_flicker(int segnum, int sidenum);

// turn flickering off (because light has been turned on)
void enable_flicker(int segnum, int sidenum);

// returns 1 if ok, 0 if error
int add_flicker(int segnum, int sidenum, fix delay, unsigned long mask);

int gr_toggle_fullscreen(void);

/*
 * reads a flickering_light structure from a CFILE
 */
void flickering_light_read(flickering_light *fl, CFILE *fp);

void flickering_light_write(flickering_light *fl, PHYSFS_file *fp);


void game_render_frame_mono(int flip);
void game_leave_menus(void);
#endif /* _GAME_H */
