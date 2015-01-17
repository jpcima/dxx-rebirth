/*
 * Portions of this file are copyright Rebirth contributors and licensed as
 * described in COPYING.txt.
 * Portions of this file are copyright Parallax Software and licensed
 * according to the Parallax license below.
 * See COPYING.txt for license details.

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
 * Routines to display the credits.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "dxxerror.h"
#include "pstypes.h"
#include "gr.h"
#include "window.h"
#include "key.h"
#include "mouse.h"
#include "palette.h"
#include "game.h"
#include "timer.h"
#include "gamefont.h"
#include "pcx.h"
#include "credits.h"
#include "u_mem.h"
#include "screens.h"
#include "digi.h"
#include "rbaudio.h"
#include "text.h"
#include "songs.h"
#include "menu.h"
#include "config.h"
#include "physfsx.h"
#if defined(DXX_BUILD_DESCENT_II)
#include "mission.h"
#include "gamepal.h"
#include "args.h"
#endif

#include "compiler-make_unique.h"

#define ROW_SPACING			(SHEIGHT / 17)
#define NUM_LINES			20 //14
#if defined(DXX_BUILD_DESCENT_I)
#define CREDITS_FILE 			"credits.tex"
#elif defined(DXX_BUILD_DESCENT_II)
#define CREDITS_FILE    		(PHYSFSX_exists("mcredits.tex",1)?"mcredits.tex":PHYSFSX_exists("ocredits.tex",1)?"ocredits.tex":"credits.tex")
#define ALLOWED_CHAR			(!Current_mission ? 'R' : (is_SHAREWARE ? 'S' : 'R'))
#endif

struct credits : ignore_window_pointer_t
{
	PHYSFS_file * file;
	int have_bin_file;
	array<PHYSFSX_gets_line_t<80>, NUM_LINES> buffer;
	int buffer_line;
	int first_line_offset;
	int extra_inc;
	int done;
	int row;
	grs_bitmap backdrop;
};

static window_event_result credits_handler(window *wind,const d_event &event, credits *cr)
{
	int l, y;
	switch (event.type)
	{
		case EVENT_KEY_COMMAND:
			if (!call_default_handler(event))	// if not print screen, debug etc
			{
				return window_event_result::close;
			}
			return window_event_result::handled;

		case EVENT_MOUSE_BUTTON_DOWN:
		case EVENT_MOUSE_BUTTON_UP:
			if (event_mouse_get_button(event) == MBTN_LEFT || event_mouse_get_button(event) == MBTN_RIGHT)
			{
				return window_event_result::close;
			}
			break;

		case EVENT_IDLE:
			if (cr->done>NUM_LINES)
			{
				return window_event_result::close;
			}
			break;
			
		case EVENT_WINDOW_DRAW:
#if defined(DXX_BUILD_DESCENT_I)
			timer_delay(F1_0/17);
#elif defined(DXX_BUILD_DESCENT_II)
			timer_delay(F1_0/28);
#endif
			
			if (cr->row == 0)
			{
				do {
					cr->buffer_line = (cr->buffer_line+1) % NUM_LINES;
#if defined(DXX_BUILD_DESCENT_II)
				get_line:;
#endif
					if (PHYSFSX_fgets( cr->buffer[cr->buffer_line], cr->file ))	{
						char *p;
						if (cr->have_bin_file) // is this a binary tbl file
							decode_text_line (cr->buffer[cr->buffer_line]);
#if defined(DXX_BUILD_DESCENT_I)
						p = strchr(&cr->buffer[cr->buffer_line][0],'\n');
						if (p) *p = '\0';
#elif defined(DXX_BUILD_DESCENT_II)
						p = cr->buffer[cr->buffer_line];
						if (p[0] == ';')
							goto get_line;
						
						if (p[0] == '%')
						{
							if (p[1] == ALLOWED_CHAR)
							{
								for (int i = 0; p[i]; i++)
									p[i] = p[i+2];
							}
							else
								goto get_line;
						}
#endif	
					} else	{
						//fseek( file, 0, SEEK_SET);
						cr->buffer[cr->buffer_line][0] = 0;
						cr->done++;
					}
				} while (cr->extra_inc--);
				cr->extra_inc = 0;
			}

			// cheap but effective: towards end of credits sequence, fade out the music volume
			if (cr->done >= NUM_LINES-16)
			{
				static int curvol = -10; 
				if (curvol == -10) 
					curvol = GameCfg.MusicVolume;
				if (curvol > (NUM_LINES-cr->done)/2)
				{
					curvol = (NUM_LINES-cr->done)/2;
					songs_set_volume(curvol);
				}
			}
			
			y = cr->first_line_offset - cr->row;
			show_fullscr(cr->backdrop);
			for (uint_fast32_t j=0; j != NUM_LINES; ++j, y += ROW_SPACING)
			{
				l = (cr->buffer_line + j + 1 ) %  NUM_LINES;
				const char *s = cr->buffer[l];
				if (!s)
					continue;
				
				if ( s[0] == '!' ) {
					s++;
				} else if ( s[0] == '$' )	{
					gr_set_curfont( HUGE_FONT );
					s++;
				} else if ( s[0] == '*' )	{
					gr_set_curfont( MEDIUM3_FONT );
					s++;
				} else
					gr_set_curfont( MEDIUM2_FONT );
				
				const auto tempp = strchr( s, '\t' );
				if ( !tempp )	{
					// Wacky Fast Credits thing
					int w, h, aw;
					
					gr_get_string_size( s, &w, &h, &aw);
					gr_string( 0x8000, y, s );
				}
			}
			
			cr->row += SHEIGHT/200;
			if (cr->row >= ROW_SPACING)
				cr->row = 0;
			break;

		case EVENT_WINDOW_CLOSE:
			gr_free_bitmap_data(cr->backdrop);
			PHYSFS_close(cr->file);
			songs_set_volume(GameCfg.MusicVolume);
			songs_play_song( SONG_TITLE, 1 );
			delete cr;
			break;
			
		default:
			break;
	}
	return window_event_result::ignored;
}

//if filename passed is NULL, show normal credits
void credits_show(const char *credits_filename)
{
	window *wind;
	int pcx_error;
	const char *filename = CREDITS_FILE;
	palette_array_t backdrop_palette;
	
	auto cr = make_unique<credits, credits>({});
	if (credits_filename) {
		filename = credits_filename;
		cr->have_bin_file = 1;
	}
	cr->file = PHYSFSX_openReadBuffered( filename );
	if (!cr->file)
	{
		char nfile[32];
		
		if (credits_filename)
		{
			return;		//ok to not find special filename
		}

		auto tempp = strchr(filename, '.');
		snprintf(nfile, sizeof(nfile), "%.*stxb", static_cast<int>(tempp - filename + 1), filename);
		cr->file = PHYSFSX_openReadBuffered(nfile);
		if (!cr->file)
			Error("Missing CREDITS.TEX and CREDITS.TXB file\n");
		cr->have_bin_file = 1;
	}

	set_screen_mode(SCREEN_MENU);
#if defined(DXX_BUILD_DESCENT_II)
	gr_use_palette_table( "credits.256" );
#endif
	cr->backdrop.bm_data=NULL;

	pcx_error = pcx_read_bitmap(STARS_BACKGROUND, cr->backdrop, BM_LINEAR,backdrop_palette);
	if (pcx_error != PCX_ERROR_NONE)		{
		PHYSFS_close(cr->file);
		return;
	}

	songs_play_song( SONG_CREDITS, 1 );

	gr_remap_bitmap_good( &cr->backdrop,backdrop_palette, -1, -1 );

	gr_set_current_canvas(NULL);
	show_fullscr(cr->backdrop);
	gr_palette_load( gr_palette );

	key_flush();

	credits *pcr = cr.get();
	wind = window_create(&grd_curscreen->sc_canvas, 0, 0, SWIDTH, SHEIGHT, credits_handler, cr.release());
	if (!wind)
	{
		d_event event = { EVENT_WINDOW_CLOSE };
		credits_handler(NULL, event, pcr);
		return;
	}

	while (window_exists(wind))
		event_process();
}
