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

#define PROGRAM_NAME "OPKManager"

#include <sys/stat.h>

#define GCW_FEELING

#if defined GCW_FEELING && defined X86CPU
	#define TESTING
	#define GCW
	#undef X86CPU
#endif
#include <sparrow3d.h>
#if defined GCW_FEELING && defined TESTING
	#define X86CPU
	#undef GCW
#endif
#include <opk.h>

#ifndef X86CPU
	#define ROOT "/media"
#else
	#define ROOT "./test"
#endif
#define VERSION "0.9.8"
#define FONT_LOCATION "./font/CabinCondensed-Regular.ttf"
#define FONT_SIZE 11
#define FONT_SIZE_SMALL 9
#define FONT_COLOR spGetRGB(48,48,48)
#define BACKGROUND_COLOR spGetRGB(225,225,160)
#define LIST_BACKGROUND_COLOR spGetRGB(255,255,180)
#define SELECTED_BACKGROUND_COLOR spGetRGB(185,185,100)

#define WGET "wget --timeout=5"
#define TIMEOUT 5

SDL_Surface* screen;
SDL_Surface* listSurface = NULL;
SDL_Surface* sdcard_surface;
SDL_Surface* internal_surface;
SDL_Surface* web_surface;
SDL_Surface* usb_surface;
SDL_Surface* update_surface;
#define MAX_HELP 65536
char help[MAX_HELP];
spTextBlockPointer helpBlock = NULL;
spFontPointer font = NULL;
spFontPointer font_small = NULL;
#define ONE_HOUR (60*60)

typedef struct sRepository *pRepository;
typedef struct sRepository {
	char url[1024];
	pRepository next;
} tRepository;

typedef struct sAlias *pAlias;
typedef struct sAlias {
	char name[256];
	char alias_name[256];
	pAlias next;
} tAlias;

typedef struct sLocation *pLocation;
typedef struct sLocation {
	char name[256];
	int kind; //0 internal, 1 sdcard, 2 internet, 3 usb
	char* url; //according to kind something like "/media/data/apps/" or an url
	char* update_call;
	char letter;
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
	char* image_url;
	char* long_description;
	spTextBlockPointer long_block;
	int size;
	pSourceList next;
} tSourcelist;

typedef struct sOpkList *pOpkList;
typedef struct sOpkList {
	char longName[1024];
	char smallLongName[1024];
	int newest_version;
	int biggest_size;
	pSourceList sources;
	pOpkList next;
	pOpkList sortedNext;
} tOpkList;

pLocation locationList = NULL;
pOpkList opkList = NULL;
pOpkList sortedOpkList = NULL;
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
int show_run = 0;
int show_help = 0;
pLocation from_sel,to_sel;
pSourceList from_sel_source,to_sel_source;
pRepository repositoryList = NULL;
pRepository aliasRepositoryList = NULL;
pAlias aliasList = NULL;
int sorting = 0;

void info(char* buffer,int dimm)
{
	if (dimm)
		spInterpolateTargetToColor(0,SP_ONE/2);
	else
		spClearTarget( BACKGROUND_COLOR );
	spFontDrawMiddle(screen->w/2,screen->h/2,0,buffer,font);
	spFlip();
}

#include "list.c"
#include "details.c"
#include "selection.c"
#include "systemcalls.c"

int offset = 0;

void draw( void )
{
	int location_count = 0;
	pLocation loc = locationList;
	while (loc)
	{
		if (loc->kind == 2)
			location_count++;
		loc = loc->next;
	}
	pLocation location[location_count+1];
	location_count = 0;
	loc = locationList;
	while (loc)
	{
		if (loc->kind == 2)
		{
			location_count++;
			location[location_count] = loc;
		}
		loc = loc->next;
	}
	//filling the list
	spSelectRenderTarget(listSurface);
	spClearTarget( LIST_BACKGROUND_COLOR );
	int i = 0;
	pOpkList opk = sortedOpkList;
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
		int sdcard = 0, internal = 0, web[location_count+1], usb = 0;;
		int sdcard_u = 0, internal_u = 0, web_u[location_count+1], usb_u = 0;
		memset(web,0,sizeof(int)*(location_count+1));
		memset(web_u,0,sizeof(int)*(location_count+1));
		pSourceList source = opk->sources;
		Sint64 oldest = 0;
		while (source)
		{
			if (source->location->kind != 2 && (source->version < oldest || oldest == 0))
				oldest = source->version;
			source = source->next;
		}
		source = opk->sources;
		while (source)
		{
			int j;
			switch (source->location->kind)
			{
				case 0: internal = 1; break;
				case 1: sdcard = 1; break;
				case 2: 
					for (j = 1; j <= location_count; j++)
						if (location[j] == source->location)
							web[j] = 1;
					break;
				case 3: usb = 1; break;
			}
			if (oldest!= 0 && oldest+ONE_HOUR < source->version)
				switch (source->location->kind)
				{
					case 0: internal_u = 1; break;
					case 1: sdcard_u = 1; break;
					case 2: 
						for (j = 1; j <= location_count; j++)
							if (location[j] == source->location)
								web_u[j] = 1;
						break;
					case 3: usb_u = 1; break;
				}
			source = source->next;
		}
		int x = 0;
		if (internal)
			spBlitSurface(2+x*16,1+(offset+i)*font->maxheight+font->maxheight-font_small->maxheight,0,internal_surface);
		if (internal_u)
			spBlitSurface(2+x*16,1+(offset+i)*font->maxheight+font->maxheight-font_small->maxheight,0,update_surface);
		x++;
		if (sdcard)
			spBlitSurface(2+x*16,1+(offset+i)*font->maxheight+font->maxheight-font_small->maxheight,0,sdcard_surface);
		if (sdcard_u)
			spBlitSurface(2+x*16,1+(offset+i)*font->maxheight+font->maxheight-font_small->maxheight,0,update_surface);
		x++;
		if (usb)
			spBlitSurface(2+x*16,1+(offset+i)*font->maxheight+font->maxheight-font_small->maxheight,0,usb_surface);
		if (usb_u)
			spBlitSurface(2+x*16,1+(offset+i)*font->maxheight+font->maxheight-font_small->maxheight,0,update_surface);
		x++;
		int j;
		for (j = 1; j <= location_count; j++)
		{
			if (web[j])
				spBlitSurface(2+x*16,1+(offset+i)*font->maxheight+font->maxheight-font_small->maxheight,0,web_surface);
			if (web[j])
			{
				char buffer[2];
				sprintf(buffer,"%c",location[j]->letter);
				spFontDrawMiddle(2+x*16+2,1+(offset+i)*font->maxheight+font->maxheight-font_small->maxheight-8,0,buffer,font_small);
			}
			if (web_u[j])
				spBlitSurface(2+x*16,1+(offset+i)*font->maxheight+font->maxheight-font_small->maxheight,0,update_surface);
			x++;
		}
		spSetVerticalOrigin(SP_CENTER);
		spSetHorizontalOrigin(SP_CENTER);
		spFontDraw(2+x*16,1+(offset+i)*font->maxheight,0,opk->longName,font);
		if (strcmp(opk->longName,PROGRAM_NAME) == 0)
			spFontDraw(2+x*16+spFontWidth(opk->longName,font)+2,1+(offset+i)*font->maxheight+font->maxheight-font_small->maxheight,0," (uneditable for now)",font_small);
		if (opk->biggest_size > 0)
		{
			char buffer[256];
			if (opk->biggest_size < 1024)
				sprintf(buffer,"max. size: %iB",opk->biggest_size);
			else
			if (opk->biggest_size < 1024*1024)
				sprintf(buffer,"max. size: %ikB",opk->biggest_size/1024);
			else
				sprintf(buffer,"max. size: %.1fMB",(float)opk->biggest_size/(1024.0f*1024.0f));
			spFontDraw(listSurface->w*3/4,1+(offset+i)*font->maxheight+font->maxheight-font_small->maxheight,0,buffer,font_small);
		}
		i++;
		opk = opk->sortedNext;
	}
	if (listSurface->h/font->maxheight-opk_count < 0 && offset != listSurface->h/font->maxheight-opk_count)
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
	char buffer[256];
	switch (sorting)
	{
		case 0:
			sprintf(buffer,"Sorting by: Name");
			break;
		case 1:
			sprintf(buffer,"Sorting by: Version / Date");
			break;
		case 2:
			sprintf(buffer,"Sorting by: Size");
			break;
	}
	spFontDraw(2,2*font->maxheight-2,0,buffer,font);
	spFontDraw(2+spFontWidth(buffer,font),2*font->maxheight,0," (Use left and right to change the sorting)",font_small);
	int way = 7;
	spBlitSurface(way,3*font->maxheight/2,0,internal_surface);
	way+=7;
	spFontDraw(way,font->maxheight,0,": internal",font);
	way+=spFontWidth(": internal ",font);
	way+=7;
	spBlitSurface(way,3*font->maxheight/2,0,sdcard_surface);
	way+=7;
	spFontDraw(way,font->maxheight,0,": sd-card  ",font);
	way+=spFontWidth(": sdcard  ",font);
	way+=7;
	spBlitSurface(way,3*font->maxheight/2,0,usb_surface);
	way+=7;
	spFontDraw(way,font->maxheight,0,": usb",font);
	way+=spFontWidth(": usb ",font);
	way+=7;
	spBlitSurface(way,3*font->maxheight/2,0,web_surface);
	way+=7;
	spFontDraw(way,font->maxheight,0,": repository",font);
	way+=spFontWidth(": repository",font);
	way+=7;
	spBlitSurface(way,3*font->maxheight/2,0,update_surface);
	way+=7;
	spFontDraw(way,font->maxheight,0,": newer version",font);	spSetVerticalOrigin(SP_TOP);
	spSetHorizontalOrigin(SP_LEFT);
	spBlitSurface( 1, font->maxheight*3-2,0,listSurface);
	spRectangleBorder( 0,font->maxheight*3-2,0,listSurface->w+2,listSurface->h+2,1,1,FONT_COLOR);
	spSetVerticalOrigin(SP_CENTER);
	spSetHorizontalOrigin(SP_CENTER);

	spFontDraw(0*screen->w/30+2,screen->h-2*font->maxheight-1,0,"[a]: Copy/Install",font_small);
	spFontDraw(8*screen->w/30,screen->h-2*font->maxheight-1,0,"[d]: Move",font_small);
	spFontDraw(13*screen->w/30,screen->h-2*font->maxheight-1,0,"[w]: Delete",font_small);
	spFontDraw(6*screen->w/10,screen->h-2*font->maxheight-1,0,"[s]: Details",font_small);
	spFontDraw(8*screen->w/10,screen->h-2*font->maxheight-1,0,"[S]: Run",font_small);
	
	spFontDraw(0*screen->w/10+2,screen->h-1*font->maxheight,0,"[q]: Update Repositories, needs internet!",font_small);
	spFontDraw(6*screen->w/10,screen->h-1*font->maxheight,0,"[e]: Help",font_small);
	spFontDraw(8*screen->w/10,screen->h-1*font->maxheight,0,"[E]: Exit",font_small);
	spFontDraw(0,0,0,"made by Ziz",font_small);
	spFontDrawRight(screen->w,0,0,"Version "VERSION,font_small);
	if (show_details)
		draw_details(sel);
	if (show_help)
		draw_help(helpBlock);
	char buffer2[256];
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
	if (show_run)
	{
			sprintf(buffer,"Run %s from which location?",sel->longName);
			draw_selection(buffer,sel,1,NULL,0);
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
	//Reset Sorting
	pOpkList opk = opkList;
	while (opk)
	{
		opk->sortedNext = NULL;
		opk = opk->next;
	}
	//Setting sorted next
	pOpkList temp = NULL;
	pOpkList before = NULL;
	switch (sorting)
	{
		case 0: //easist
			opk = opkList;
			while (opk)
			{
				opk->sortedNext = opk->next;
				opk = opk->next;
			}
			sortedOpkList = opkList;
			break;
		case 1: //version
			sortedOpkList = NULL;
			opk = opkList;
			while (opk)
			{
				//sorted insert
				temp = sortedOpkList;
				before = NULL;
				while (temp)
				{
					if (temp->newest_version < opk->newest_version)
						break;
					before = temp;
					temp = temp->sortedNext;
				}
				if (before)
					before->sortedNext = opk;
				else
					sortedOpkList = opk;
				opk->sortedNext = temp;
				opk = opk->next;
			}
			break;
		case 2: //size
			sortedOpkList = NULL;
			opk = opkList;
			while (opk)
			{
				//sorted insert
				temp = sortedOpkList;
				before = NULL;
				while (temp)
				{
					if (temp->biggest_size < opk->biggest_size)
						break;
					before = temp;
					temp = temp->sortedNext;
				}
				if (before)
					before->sortedNext = opk;
				else
					sortedOpkList = opk;
				opk->sortedNext = temp;
				opk = opk->next;
			}
			break;	}
		
	
	
	
	opk = sortedOpkList;
	pOpkList sel = NULL;
	int i = 0;
	while (opk)
	{
		if (i == selected)
			sel = opk;
		i++;
		opk = opk->sortedNext;
	}
	if (show_details)
	{
		show_details = calc_details(sel,steps);
		return 0;
	}
	if (show_help)
	{
		show_help = calc_help(helpBlock,steps);
		return 0;
	}
	int result;
	//COPYING
	switch (show_copy)
	{
		case 1:
			result = calc_selection(steps,sel,1,NULL,1);
			if (result == 2)
			{
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
				int location_count = 0;
				pLocation location = locationList;
				while (location)
				{
					if (location->kind != 2 && location != from_sel)
					{
						to_sel = location;
						location_count++;
					}
					location = location->next;
				}
				if (location_count == 1)
				{
					to_sel_source = NULL;
					source = sel->sources;
					while (source)
					{
						if (source->location == to_sel)
						{
							to_sel_source = source;
							break;
						}
						source = source->next;
					}
					show_copy = 3;
				}
				else
					show_copy = 2;
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
					system_copy_overwrite(sel,from_sel_source,to_sel_source);
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
				int location_count = 0;
				pLocation location = locationList;
				while (location)
				{
					if (location->kind != 2 && location != from_sel)
					{
						to_sel = location;
						location_count++;
					}
					location = location->next;
				}
				if (location_count == 1)
				{
					to_sel_source = NULL;
					source = sel->sources;
					while (source)
					{
						if (source->location == to_sel)
						{
							to_sel_source = source;
							break;
						}
						source = source->next;
					}
					show_move = 3;
				}
				else
					show_move = 2;
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
	//RUNNING
	switch (show_run)
	{
		case 1:
			result = calc_selection(steps,sel,1,NULL,0);
			if (result == 2)
			{
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
				printf("Running %s%s\n",from_sel_source->location->url,from_sel_source->fileName);
				system_run(sel,from_sel_source);
				show_run = 0;
			}
			else
			if (result == 0)
				show_run = 0;
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
		download_new_alias();
		update_alias();
		update_repositories();
	}
	if (spGetInput()->button[SP_BUTTON_DOWN_NOWASD] && opk_count>0) //DETAILS
	{
		spGetInput()->button[SP_BUTTON_DOWN_NOWASD] = 0;
		show_details = 1;
		return 0;
	}
	if (spGetInput()->button[SP_BUTTON_R_NOWASD] && opk_count>0) //HELP
	{
		spGetInput()->button[SP_BUTTON_R_NOWASD] = 0;
		show_help = 1;
		return 0;
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
	if (spGetInput()->axis[0] < 0)
	{
		spGetInput()->axis[0] = 0;
		sorting = (sorting+2)%3;
	}
	if (spGetInput()->axis[0] > 0)
	{
		spGetInput()->axis[0] = 0;
		sorting = (sorting+1)%3;
	}
	
	if (spGetInput()->button[SP_BUTTON_SELECT_NOWASD])
		return 1;
	if (strcmp(sel->longName,PROGRAM_NAME) == 0) //No copy and co of OPKManager itself
		return 0;
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
			int location_count = 0;
			pLocation location = locationList;
			while (location)
			{
				if (location->kind != 2 && location != from_sel)
				{
					to_sel = location;
					location_count++;
				}
				location = location->next;
			}
			if (location_count == 1)
			{
				to_sel_source = NULL;
				source = sel->sources;
				while (source)
				{
					if (source->location == to_sel)
					{
						to_sel_source = source;
						break;
					}
					source = source->next;
				}
				printf("Copying from %s%s to %s%s\n",from_sel->url,from_sel_source->fileName,to_sel->url,from_sel_source->fileName);
				system_copy_new(sel,from_sel_source,to_sel);
				show_copy = 0;
			}
			else
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
			{
				from_sel_source = source;
				source_count++;
			}
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
			from_sel = from_sel_source->location;
			show_delete = 3;
			return 0;
		}
	}
	if (spGetInput()->button[SP_BUTTON_START_NOWASD] && opk_count>0) //RUN
	{
		spGetInput()->button[SP_BUTTON_START_NOWASD] = 0;
		pSourceList source = sel->sources;
		int source_count = 0;
		while (source)
		{
			if (source->location->kind != 2)
			{
				from_sel_source = source;
				source_count++;
			}
			source = source->next;
		}
		if (source_count > 1)
		{
			selection_selection = 0;
			show_run = 1;
			return 0;
		}
		else
		if (source_count == 1)
		{
			selection_selection = 0;
			from_sel = from_sel_source->location;
			printf("Running %s%s\n",from_sel_source->location->url,from_sel_source->fileName);
			system_run(sel,from_sel_source);
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
			{
				from_sel_source = source;
				source_count++;
			}
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
			from_sel = from_sel_source->location;
			int location_count = 0;
			pLocation location = locationList;
			while (location)
			{
				if (location->kind != 2 && location != from_sel)
				{
					to_sel = location;
					location_count++;
				}
				location = location->next;
			}
			if (location_count == 1)
			{
				to_sel_source = NULL;
				source = sel->sources;
				while (source)
				{
					if (source->location == to_sel)
					{
						to_sel_source = source;
						break;
					}
					source = source->next;
				}
				printf("Moving from %s%s to %s%s\n",from_sel->url,from_sel_source->fileName,to_sel->url,from_sel_source->fileName);
				system_move_new(sel,from_sel_source,to_sel);
				show_move = 0;
			}
			else
			if (location_count != 0)
				show_move = 2;
			return 0;
		}
	}
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
	                              h-1*font->maxheight-5*font_small->maxheight);

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
	info("Searching local packages...",0);
	read_locations();
	add_all_locations();
	//Reading scripts download list
	char buffer[256];
	spCreateDirectoryChain(get_path(buffer,"scripts"));
	if (spFileExists(get_path(buffer,"repositories.txt")) == 0)
	{
		FILE *fp = fopen(get_path(buffer,"repositories.txt"), "w");
		fprintf(fp,"http://ziz.gp2x.de/gcw-repos/ziz.py\n");
		fprintf(fp,"http://ziz.gp2x.de/gcw-repos/official.py\n");
		fclose(fp);
	}
	load_repository_list();
	
	spCreateDirectoryChain(get_path(buffer,"alias"));
	if (spFileExists(get_path(buffer,"alias.txt")) == 0)
	{
		FILE *fp = fopen(get_path(buffer,"alias.txt"), "w");
		fprintf(fp,"http://ziz.gp2x.de/gcw-repos/predefined.txt\n");
		fclose(fp);
	}
	load_alias_list();
	update_alias();
	
	//Reading help
	SDL_RWops *file=SDL_RWFromFile("./README.md","r");
	if (file)
	{
		int count = SDL_RWread(file,help,1,MAX_HELP-1);
		SDL_RWclose(file);
		help[count-1] = 0;
		helpBlock = spCreateTextBlock(help,screen->w*9/12,font);
	}
	

	spLoop( draw, calc, 10, resize, NULL );
	spFontDelete(font);
	spFontDelete(font_small);
	spDeleteSurface(sdcard_surface);
	spDeleteSurface(internal_surface);
	spDeleteSurface(web_surface);
	spDeleteSurface(usb_surface);
	spDeleteSurface(update_surface);
	if (helpBlock)
		spDeleteTextBlock(helpBlock);
	spQuitCore();
	return 0;
}
