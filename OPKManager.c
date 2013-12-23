 /* This file is part of OPKManager.
  * Sparrow3d is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 2 of the License, or
  * (at your option) any later version.
  * 
  * Sparrow3d is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  * 
  * You should have received a copy of the GNU General Public License
  * along with Foobar.  If not, see <http://www.gnu.org/licenses/>
  * 
  * For feedback and questions about my Files and Projects please mail me,
  * Alexander Matthes (Ziz) , zizsdl_at_googlemail.com */

#ifdef X86CPU
	#define TESTING
	#define GCW
	#undef X86CPU
#endif
#include <sparrow3d.h>
#ifdef TESTING
	#define X86CPU
	#undef GCW
#endif
#include <opk.h>

#ifndef X86CPU
	#define ROOT "/media"
#else
	#define ROOT "./test"
#endif
#define VERSION "0.5.0"
#define FONT_LOCATION "./font/CabinCondensed-Regular.ttf"
#define FONT_SIZE 11
#define FONT_SIZE_SMALL 9
#define FONT_COLOR spGetRGB(48,48,48)
#define BACKGROUND_COLOR spGetRGB(225,225,160)
#define LIST_BACKGROUND_COLOR spGetRGB(255,255,180)
#define SELECTED_BACKGROUND_COLOR spGetRGB(185,185,100)

SDL_Surface* screen;
SDL_Surface* listSurface = NULL;
SDL_Surface* sdcard_surface;
SDL_Surface* internal_surface;
SDL_Surface* web_surface;
SDL_Surface* usb_surface;
SDL_Surface* update_surface;
spFontPointer font = NULL;
spFontPointer font_small = NULL;
#define ONE_HOUR (60*60)

typedef struct sLocation *pLocation;
typedef struct sLocation {
	char name[256];
	int kind; //0 internal, 1 sdcard, 2 internet, 3 usb
	char* url; //according to kind something like "/media/data/apps/" or an url
	char* update_call;
	pLocation next;
} tLocation;

typedef struct sSourceList *pSourceList;
typedef struct sSourceList {
	Sint64 version;
	char* fileName;
	char* description;
	spTextBlockPointer block;
	pLocation location;
	char* url_addition;
	pSourceList next;
} tSourcelist;

typedef struct sOpkList *pOpkList;
typedef struct sOpkList {
	char longName[1024];
	char smallLongName[1024];
	pSourceList sources;
	pOpkList next;
} tOpkList;

pLocation locationList = NULL;
pOpkList opkList = NULL;
int selected = 0;
int next_in_a_row = 0;
int time_until_next = 0;
int opk_count = 0;

int show_details = 0;
int show_copy = 0;
int copy_is_install;
int show_move = 0;
int show_delete = 0;
int show_error = 0;
pLocation from_sel,to_sel;
pSourceList from_sel_source,to_sel_source;

#include "list.c"
#include "details.c"
#include "selection.c"
#include "systemcalls.c"

void info(char* buffer,int dimm)
{
	if (dimm)
		spInterpolateTargetToColor(0,SP_ONE/2);
	else
		spClearTarget( BACKGROUND_COLOR );
	spFontDrawMiddle(screen->w/2,screen->h/2,0,buffer,font);
	spFlip();
}

int offset = 0;

void draw( void )
{
	//filling the list
	spSelectRenderTarget(listSurface);
	spClearTarget( LIST_BACKGROUND_COLOR );
	int i = 0;
	pOpkList opk = opkList;
	pOpkList sel = NULL;
	while (opk)
	{
		spSetVerticalOrigin(SP_TOP);
		spSetHorizontalOrigin(SP_LEFT);			
		if (i == selected)
		{
			spRectangle(0,1+(offset+i)*font->maxheight,0,screen->w-2,font->maxheight,SELECTED_BACKGROUND_COLOR);
			sel = opk;
		}
		int sdcard = 0, internal = 0, web = 0, usb = 0;;
		int sdcard_u = 0, internal_u = 0, web_u = 0, usb_u = 0;
		pSourceList source = opk->sources;
		Sint64 oldest = 0;
		while (source)
		{
			if (source->version < oldest || oldest == 0)
				oldest = source->version;
			source = source->next;
		}
		source = opk->sources;
		while (source)
		{
			switch (source->location->kind)
			{
				case 0: internal = 1; break;
				case 1: sdcard = 1; break;
				case 2: web = 1; break;
				case 3: usb = 1; break;
			}
			if (oldest+ONE_HOUR < source->version)
				switch (source->location->kind)
				{
					case 0: internal_u = 1; break;
					case 1: sdcard_u = 1; break;
					case 2: web_u = 1; break;
					case 3: usb_u = 1; break;
				}
			source = source->next;
		}
		if (internal)
			spBlitSurface(2+0*16,1+(offset+i)*font->maxheight+font->maxheight-font_small->maxheight,0,internal_surface);
		if (internal_u)
			spBlitSurface(2+0*16,1+(offset+i)*font->maxheight+font->maxheight-font_small->maxheight,0,update_surface);
		if (sdcard)
			spBlitSurface(2+1*16,1+(offset+i)*font->maxheight+font->maxheight-font_small->maxheight,0,sdcard_surface);
		if (sdcard_u)
			spBlitSurface(2+1*16,1+(offset+i)*font->maxheight+font->maxheight-font_small->maxheight,0,update_surface);
		if (usb)
			spBlitSurface(2+2*16,1+(offset+i)*font->maxheight+font->maxheight-font_small->maxheight,0,usb_surface);
		if (usb_u)
			spBlitSurface(2+2*16,1+(offset+i)*font->maxheight+font->maxheight-font_small->maxheight,0,update_surface);
		if (web)
			spBlitSurface(2+3*16,1+(offset+i)*font->maxheight+font->maxheight-font_small->maxheight,0,web_surface);
		if (web_u)
			spBlitSurface(2+3*16,1+(offset+i)*font->maxheight+font->maxheight-font_small->maxheight,0,update_surface);
		spSetVerticalOrigin(SP_CENTER);
		spSetHorizontalOrigin(SP_CENTER);
		spFontDraw(2+4*16,1+(offset+i)*font->maxheight,0,opk->longName,font);
		i++;
		opk = opk->next;
	}
	if (offset != listSurface->h/font->maxheight-opk_count)
	{
		spTriangle(listSurface->w- 1,listSurface->h-11,0,
		           listSurface->w-11,listSurface->h-11,0,
		           listSurface->w- 6,listSurface->h- 6,0,FONT_COLOR);
	}
	if (offset != 0)
	{
		spTriangle(listSurface->w- 6,  6,0,
		           listSurface->w-11, 11,0,
		           listSurface->w- 1, 11,0,FONT_COLOR);
	}
	//drawing all
	
	spSelectRenderTarget(spGetWindowSurface());
	spClearTarget( BACKGROUND_COLOR );
	spFontDrawMiddle(screen->w/2,0,0,"OPKManager",font);
	int way = 7;
	spBlitSurface(way,3*font->maxheight/2,0,internal_surface);
	way+=7;
	spFontDraw(way,font->maxheight,0,": internal ",font);
	way+=spFontWidth(": internal ",font);
	way+=7;
	spBlitSurface(way,3*font->maxheight/2,0,sdcard_surface);
	way+=7;
	spFontDraw(way,font->maxheight,0,": sd-card  ",font);
	way+=spFontWidth(": sdcard  ",font);
	way+=7;
	spBlitSurface(way,3*font->maxheight/2,0,usb_surface);
	way+=7;
	spFontDraw(way,font->maxheight,0,": usb ",font);
	way+=spFontWidth(": usb  ",font);
	way+=7;
	spBlitSurface(way,3*font->maxheight/2,0,web_surface);
	way+=7;
	spFontDraw(way,font->maxheight,0,": repository ",font);
	way+=spFontWidth(": repository ",font);
	way+=7;
	spBlitSurface(way,3*font->maxheight/2,0,update_surface);
	way+=7;
	spFontDraw(way,font->maxheight,0,": newer version",font);	spSetVerticalOrigin(SP_TOP);
	spSetHorizontalOrigin(SP_LEFT);
	spBlitSurface( 1, font->maxheight*2,0,listSurface);
	spRectangleBorder( 0,font->maxheight*2,0,listSurface->w+2,listSurface->h+2,1,1,FONT_COLOR);
	spSetVerticalOrigin(SP_CENTER);
	spSetHorizontalOrigin(SP_CENTER);

	spFontDraw(0*screen->w/8,screen->h-2*font->maxheight,0,"[a]: Copy/Install",font_small);
	spFontDraw(2*screen->w/8,screen->h-2*font->maxheight,0,"[d]: Move",font_small);
	spFontDraw(4*screen->w/8,screen->h-2*font->maxheight,0,"[w]: Delete",font_small);
	spFontDraw(6*screen->w/8,screen->h-2*font->maxheight,0,"[S]: Details",font_small);
	spFontDraw(0*screen->w/8,screen->h-1*font->maxheight,0,"[q]: Update Repos, needs internet!",font_small);
	spFontDraw(4*screen->w/8,screen->h-1*font->maxheight,0,"[e]: Run",font_small);
	spFontDraw(6*screen->w/8,screen->h-1*font->maxheight,0,"[E]: Exit",font_small);
	spFontDraw(0,0,0,"made by Ziz",font_small);
	spFontDrawRight(screen->w,0,0,"Version "VERSION,font_small);
	if (show_details)
		draw_details(sel);
	char buffer[256],buffer2[256];
	switch (show_copy)
	{
		case 1:
			if (copy_is_install)
				sprintf(buffer,"Copy/Install %s from which location?",sel->longName);
			else
				sprintf(buffer,"Copy %s from which location?",sel->longName);
			draw_selection(buffer,sel,1,NULL,1);
			break;
		case 2:
			if (copy_is_install)
				sprintf(buffer,"Install %s to which location?",sel->longName);
			else
				sprintf(buffer,"Copy %s to which location?",sel->longName);
			draw_selection(buffer,sel,0,from_sel,0);
			break;
		case 3:
			sprintf(buffer,"%s%s",from_sel->url,from_sel_source->fileName);
			sprintf(buffer2,"%s%s ?",to_sel->url,to_sel_source->fileName);
			draw_sure("Are you sure you want to overwrite",buffer2,"with",buffer,0);
			break;
	}
	switch (show_move)
	{
		case 1:
			sprintf(buffer,"Move %s from which location?",sel->longName);
			draw_selection(buffer,sel,1,NULL,0);
			break;
		case 2:
			sprintf(buffer,"Move %s to which location?",sel->longName);
			draw_selection(buffer,sel,0,from_sel,0);
			break;
		case 3:
			sprintf(buffer,"%s%s",from_sel->url,from_sel_source->fileName);
			sprintf(buffer2,"%s%s ?",to_sel->url,to_sel_source->fileName);
			draw_sure("Are you sure you want to overwrite",buffer2,"with",buffer,0);
			break;
	}
	switch (show_delete)
	{
		case 1:
			sprintf(buffer,"Delete %s from which location?",sel->longName);
			draw_selection(buffer,sel,1,NULL,0);
			break;
		case 3:
			sprintf(buffer,"%s%s",from_sel->url,from_sel_source->fileName);
			draw_sure("Are you sure you want to delete",buffer,"","",0);
			break;
	}
	switch (show_error)
	{
		case 1:
			draw_sure("Connection error!","Coudln't download'",sel->longName,"Maybe no network connection?",1);
			break;
	}
	spFlip();
}

int calc(Uint32 steps)
{
	if (show_details)
	{
		show_details = calc_details();
		return 0;
	}
	pOpkList opk = opkList;
	pOpkList sel = NULL;
	int i = 0;
	while (opk)
	{
		if (i == selected)
			sel = opk;
		i++;
		opk = opk->next;
	}
	int result;
	//COPYING
	switch (show_copy)
	{
		case 1:
			result = calc_selection(steps,sel,1,NULL,1);
			if (result == 2)
			{
				show_copy = 2;
				from_sel = locationList;
				int i = 0;
				pSourceList source;
				while (from_sel)
				{
					source = sel->sources;
					while (source)
					{
						if (source->location == from_sel)
							break;
						source = source->next;
					}
					if (source)
					{
						if (selection_selection == i)
							break;
						i++;
					}
					from_sel = from_sel->next;
				}
				from_sel_source = source;
				if (from_sel->kind == 2)
					copy_is_install = 1;
				else
					copy_is_install = 0;
				selection_selection = 0;
			}
			else
			if (result == 0)
				show_copy = 0;
			return 0;
		case 2:
			result = calc_selection(steps,sel,0,from_sel,0);
			if (result == 0)
				show_copy = 0;
			if (result == 2)
			{
				//Copying!
				to_sel = locationList;
				int i = 0;
				while (to_sel)
				{
					if (to_sel == from_sel || to_sel->kind == 2)
					{
						to_sel = to_sel->next;
						continue;
					}
					if (selection_selection == i)
						break;
					i++;
					to_sel = to_sel->next;
				}
				//location allready there?
				to_sel_source = sel->sources;
				while (to_sel_source)
				{
					if (to_sel_source->location == to_sel)
						break;
					to_sel_source = to_sel_source->next;
				}
				if (to_sel_source)
					show_copy = 3;
				else
				{
					printf("Copying from %s%s to %s%s\n",from_sel->url,from_sel_source->fileName,to_sel->url,from_sel_source->fileName);
					if (copy_is_install)
						info("Downloading...",1);
					system_copy_new(sel,from_sel_source,to_sel);
					show_copy = 0;
				}
			}
			return 0;
		case 3:
			result = calc_sure();
			if (result != 1)
			{
				if (result == 2)
				{
					printf("Overwriting from %s%s to %s%s\n",from_sel->url,from_sel_source->fileName,to_sel->url,to_sel_source->fileName);					
					if (copy_is_install)
						info("Downloading...",1);
					system_copy_overwrite(from_sel_source,to_sel_source);
				}
				show_copy = 0;
			}
			return 0;
	}
	//MOVING
	switch (show_move)
	{
		case 1:
			result = calc_selection(steps,sel,1,NULL,0);
			if (result == 2)
			{
				show_move = 2;
				from_sel = locationList;
				int i = 0;
				pSourceList source;
				while (from_sel)
				{
					if (from_sel->kind == 2)
					{
						from_sel = from_sel->next;
						continue;
					}
					source = sel->sources;
					while (source)
					{
						if (source->location == from_sel)
							break;
						source = source->next;
					}
					if (source)
					{
						if (selection_selection == i)
							break;
						i++;
					}
					from_sel = from_sel->next;
				}
				from_sel_source = source;
				selection_selection = 0;
			}
			else
			if (result == 0)
				show_move = 0;
			return 0;
		case 2:
			result = calc_selection(steps,sel,0,from_sel,0);
			if (result == 0)
				show_move = 0;
			if (result == 2)
			{
				//Moving!
				to_sel = locationList;
				int i = 0;
				while (to_sel)
				{
					if (to_sel == from_sel || to_sel->kind == 2)
					{
						to_sel = to_sel->next;
						continue;
					}
					if (selection_selection == i)
						break;
					i++;
					to_sel = to_sel->next;
				}
				//location allready there?
				to_sel_source = sel->sources;
				while (to_sel_source)
				{
					if (to_sel_source->location == to_sel)
						break;
					to_sel_source = to_sel_source->next;
				}
				if (to_sel_source)
					show_move = 3;
				else
				{
					printf("Moving from %s%s to %s%s\n",from_sel->url,from_sel_source->fileName,to_sel->url,from_sel_source->fileName);
					system_move_new(sel,from_sel_source,to_sel);
					show_move = 0;
				}
			}
			return 0;
		case 3:
			result = calc_sure();
			if (result != 1)
			{
				if (result == 2)
				{
					printf("Moving overwriting from %s%s to %s%s\n",from_sel->url,from_sel_source->fileName,to_sel->url,to_sel_source->fileName);					
					system_move_overwrite(sel,from_sel_source,to_sel_source);
				}
				show_move = 0;
			}
			return 0;
	}
	//DELETING
	switch (show_delete)
	{
		case 1:
			result = calc_selection(steps,sel,1,NULL,0);
			if (result == 2)
			{
				show_delete = 3;
				from_sel = locationList;
				int i = 0;
				pSourceList source;
				while (from_sel)
				{
					if (from_sel->kind == 2)
					{
						from_sel = from_sel->next;
						continue;
					}
					source = sel->sources;
					while (source)
					{
						if (source->location == from_sel)
							break;
						source = source->next;
					}
					if (source)
					{
						if (selection_selection == i)
							break;
						i++;
					}
					from_sel = from_sel->next;
				}
				from_sel_source = source;
				selection_selection = 0;
			}
			else
			if (result == 0)
				show_delete = 0;
			return 0;
		case 3:
			result = calc_sure();
			if (result != 1)
			{
				if (result == 2)
				{
					printf("Deleting %s%s\n",from_sel->url,from_sel_source->fileName);					
					system_delete(sel,from_sel_source);
				}
				show_delete = 0;
			}
			return 0;
	}
	//ERROR
	if (show_error)
	{
		if (calc_sure() != 1)
			show_error = 0;
		return 0;
	}
	if (spGetInput()->button[SP_BUTTON_L_NOWASD])
	{
		spGetInput()->button[SP_BUTTON_L_NOWASD] = 0;
		info("Updating repository packages...",1);
		update_repositories();
	}
	if (spGetInput()->button[SP_BUTTON_START_NOWASD] && opk_count>0) //DETAILS
	{
		spGetInput()->button[SP_BUTTON_START_NOWASD] = 0;
		show_details = 1;
		return 0;
	}
	if (spGetInput()->button[SP_BUTTON_LEFT_NOWASD] && opk_count>0) //COPY
	{
		spGetInput()->button[SP_BUTTON_LEFT_NOWASD] = 0;
		pSourceList source = sel->sources;
		int source_count = 0;
		copy_is_install = 0;
		while (source)
		{
			source_count++;
			if (source->location->kind == 2)
				copy_is_install = 1;
			source = source->next;
		}
		if (source_count > 1)
		{
			selection_selection = 0;
			show_copy = 1;
			return 0;
		}
		else
		if (source_count == 1)
		{
			selection_selection = 0;
			from_sel_source = sel->sources;
			from_sel = sel->sources->location;
			show_copy = 2;
			return 0;
		}
	}
	if (spGetInput()->button[SP_BUTTON_UP_NOWASD] && opk_count>0) //DELETE
	{
		spGetInput()->button[SP_BUTTON_UP_NOWASD] = 0;
		pSourceList source = sel->sources;
		int source_count = 0;
		while (source)
		{
			if (source->location->kind != 2)
				source_count++;
			source = source->next;
		}
		if (source_count > 1)
		{
			selection_selection = 0;
			show_delete = 1;
			return 0;
		}
		else
		if (source_count == 1)
		{
			selection_selection = 0;
			from_sel_source = sel->sources;
			from_sel = sel->sources->location;
			show_delete = 3;
			return 0;
		}
	}
	if (spGetInput()->button[SP_BUTTON_RIGHT_NOWASD] && opk_count>0) //MOVE
	{
		spGetInput()->button[SP_BUTTON_RIGHT_NOWASD] = 0;
		pSourceList source = sel->sources;
		int source_count = 0;
		while (source)
		{
			if (source->location->kind != 2)
				source_count++;
			source = source->next;
		}
		if (source_count > 1)
		{
			selection_selection = 0;
			show_move = 1;
			return 0;
		}
		else
		if (source_count == 1)
		{
			selection_selection = 0;
			from_sel_source = sel->sources;
			from_sel = sel->sources->location;
			show_move = 2;
			return 0;
		}
	}
	if (time_until_next > 0)
		time_until_next -= steps;
	if (spGetInput()->axis[1] < 0 && selected > 0)
	{
		if (time_until_next <= 0)
		{
			selected--;
			if ((offset+selected)*font->maxheight < 0)
				offset++;
			next_in_a_row++;
			time_until_next = 300/next_in_a_row;
		}
	}
	else
	if (spGetInput()->axis[1] > 0 && selected < opk_count-1)
	{
		if (time_until_next <= 0)
		{
			selected++;
			if ((offset+selected+1)*font->maxheight >= listSurface->h)
				offset--;
			next_in_a_row++;
			time_until_next = 300/next_in_a_row;
		}
	}
	else
	{
		time_until_next = 0;
		next_in_a_row = 0;
	}
	if (spGetInput()->button[SP_BUTTON_SELECT_NOWASD])
		return 1;
	return 0;
}

void resize(Uint16 w,Uint16 h)
{
  //Setup of the new/resized window
  spSelectRenderTarget(spGetWindowSurface());
  
	spFontShadeButtons(1);
	//Font Loading
	spFontSetShadeColor(BACKGROUND_COLOR);
	if (font)
		spFontDelete(font);
	font = spFontLoad(FONT_LOCATION,FONT_SIZE*spGetSizeFactor()>>SP_ACCURACY);
	spFontAdd(font,SP_FONT_GROUP_ASCII,FONT_COLOR);//whole ASCII
	spFontAddBorder(font,BACKGROUND_COLOR);
	spFontMulWidth(font,15<<SP_ACCURACY-4);
	spFontAddButton( font, 'a', SP_BUTTON_LEFT_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'd', SP_BUTTON_RIGHT_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'w', SP_BUTTON_UP_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 's', SP_BUTTON_DOWN_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'q', SP_BUTTON_L_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'e', SP_BUTTON_R_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'o', SP_PRACTICE_OK_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'c', SP_PRACTICE_CANCEL_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'S', SP_BUTTON_START_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font, 'E', SP_BUTTON_SELECT_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));

	spFontSetShadeColor(BACKGROUND_COLOR);
	if (font_small)
		spFontDelete(font_small);
	font_small = spFontLoad(FONT_LOCATION,FONT_SIZE_SMALL*spGetSizeFactor()>>SP_ACCURACY);
	spFontAdd(font_small,SP_FONT_GROUP_ASCII,FONT_COLOR);//whole ASCII
	spFontAddBorder(font_small,BACKGROUND_COLOR);
	spFontMulWidth(font_small,15<<SP_ACCURACY-4);
	spFontAddButton( font_small, 'a', SP_BUTTON_LEFT_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font_small, 'd', SP_BUTTON_RIGHT_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font_small, 'w', SP_BUTTON_UP_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font_small, 's', SP_BUTTON_DOWN_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font_small, 'q', SP_BUTTON_L_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font_small, 'e', SP_BUTTON_R_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font_small, 'o', SP_PRACTICE_OK_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font_small, 'c', SP_PRACTICE_CANCEL_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font_small, 'S', SP_BUTTON_START_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font_small, 'E', SP_BUTTON_SELECT_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));

  if (listSurface)
		spDeleteSurface(listSurface);
	listSurface = spCreateSurface(w-2,
	                              h-1*font->maxheight-4*font_small->maxheight);

}

int main(int argc, char **argv)
{
	srand(time(NULL));
	spInitCore();
	spSetAffineTextureHack(0); //We don't need it :)
	spInitMath();
	screen = spCreateDefaultWindow();
	resize(screen->w,screen->h);
	sdcard_surface = spLoadSurface("./data/sdcard.png");
	internal_surface = spLoadSurface("./data/internal.png");
	web_surface = spLoadSurface("./data/web.png");
	usb_surface = spLoadSurface("./data/usb.png");
	update_surface = spLoadSurface("./data/update.png");
	spSetZSet(0);
	spSetZTest(0);
	read_locations();
	info("Searching local packages...",0);
	add_all_locations();

	spLoop( draw, calc, 10, resize, NULL );
	spFontDelete(font);
	spFontDelete(font_small);
	spDeleteSurface(sdcard_surface);
	spDeleteSurface(internal_surface);
	spDeleteSurface(web_surface);
	spDeleteSurface(usb_surface);
	spDeleteSurface(update_surface);
	spQuitCore();
	return 0;
}
