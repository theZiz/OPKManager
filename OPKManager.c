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

#include <sparrow3d.h>

#ifdef X86
	#define INTERNAL "/media/data/apps"
	#define SDCARD "/media/sdcard/apps"
#else
	#define INTERNAL "./test/data"
	#define SDCARD "./test/sdcard"
#endif
#define VERSION "1.0.0"
#define FONT_LOCATION "./font/CabinCondensed-Regular.ttf"
#define FONT_SIZE 10
#define FONT_SIZE_SMALL 9
#define FONT_SIZE_VERY_SMALL 8
#define FONT_COLOR spGetRGB(48,48,48)
#define BACKGROUND_COLOR spGetRGB(225,225,160)
#define LIST_BACKGROUND_COLOR spGetRGB(255,255,180)
#define SELECTED_BACKGROUND_COLOR spGetRGB(185,185,100)

SDL_Surface* screen;
SDL_Surface* listSurface = NULL;
spFontPointer font = NULL;
spFontPointer font_small = NULL;
spFontPointer font_very_small = NULL;

typedef struct sSourceList *pSourceList;
typedef struct sSourceList {
	Sint64 version;
	int location; //0 internal, 1 sdcard, 2 internet
	char* url; //with maybe another filename
	pSourceList next;
} tSourcelist;

typedef struct sOpkList *pOpkList;
typedef struct sOpkList {
	char fileName[1024];
	char longName[1024];
	char smallLongName[1024];
	pSourceList sources;
	pOpkList next;
} tOpkList;

pOpkList opkList = NULL;
int selected = 0;
int next_in_a_row = 0;
int time_until_next = 0;
int opk_count = 0;

#include "list.c"

void info(char* buffer)
{
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
	while (opk)
	{
		if (i == selected)
		{
			spSetVerticalOrigin(SP_TOP);
			spSetHorizontalOrigin(SP_LEFT);			
			spRectangle(0,1+(offset+i)*font->maxheight,0,screen->w-2,font->maxheight,SELECTED_BACKGROUND_COLOR);
			spSetVerticalOrigin(SP_CENTER);
			spSetHorizontalOrigin(SP_CENTER);
		}
		spFontDraw(2,1+(offset+i)*font->maxheight,0,opk->longName,font);
		i++;
		opk = opk->next;
	}	
	//drawing all
	spSelectRenderTarget(spGetWindowSurface());
	spClearTarget( BACKGROUND_COLOR );
	spFontDrawMiddle(screen->w/2,0,0,"OPKManager",font);
	spSetVerticalOrigin(SP_TOP);
	spSetHorizontalOrigin(SP_LEFT);
	spBlitSurface( 1, font->maxheight,0,listSurface);
	spSetVerticalOrigin(SP_CENTER);
	spSetHorizontalOrigin(SP_CENTER);
	spFontDraw(0,screen->h-font_very_small->maxheight,0,"made by Ziz",font_very_small);
	spFontDrawRight(screen->w,screen->h-font_very_small->maxheight,0,"Version "VERSION,font_very_small);
	spFlip();
}

int calc(Uint32 steps)
{
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
	if (spGetInput()->button[SP_BUTTON_SELECT])
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
	spFontAddButton( font_small, 'S', SP_BUTTON_START_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font_small, 'E', SP_BUTTON_SELECT_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));

	if (font_very_small)
		spFontDelete(font_very_small);
	font_very_small = spFontLoad(FONT_LOCATION,FONT_SIZE_VERY_SMALL*spGetSizeFactor()>>SP_ACCURACY);
	spFontAdd(font_very_small,SP_FONT_GROUP_ASCII,FONT_COLOR);//whole ASCII
	spFontAddBorder(font_very_small,BACKGROUND_COLOR);
	spFontMulWidth(font_very_small,15<<SP_ACCURACY-4);
	spFontAddButton( font_very_small, 'a', SP_BUTTON_LEFT_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font_very_small, 'd', SP_BUTTON_RIGHT_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font_very_small, 'w', SP_BUTTON_UP_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font_very_small, 's', SP_BUTTON_DOWN_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font_very_small, 'q', SP_BUTTON_L_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font_very_small, 'e', SP_BUTTON_R_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font_very_small, 'S', SP_BUTTON_START_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));
	spFontAddButton( font_very_small, 'E', SP_BUTTON_SELECT_NOWASD_NAME, spGetRGB(230,230,230), spGetRGB(64,64,64));

  if (listSurface)
		spDeleteSurface(listSurface);
	listSurface = spCreateSurface(w-2,
	                              h-font->maxheight-font_very_small->maxheight);

}

int main(int argc, char **argv)
{
	spInitCore();
	spSetAffineTextureHack(0); //We don't need it :)
	spInitMath();
	screen = spCreateDefaultWindow();
	resize(screen->w,screen->h);
	spSetZSet(0);
	spSetZTest(0);
	info("Searching internal packages...");
	spFileListPointer fileList = NULL;
	spFileGetDirectory(&fileList,INTERNAL,0,1);
	merge_fileList_to_opkList(fileList,0);
	spFileDeleteList(fileList);
	info("Searching sdcard packages...");
	fileList = NULL;
	spFileGetDirectory(&fileList,SDCARD,0,1);
	merge_fileList_to_opkList(fileList,0);
	spFileDeleteList(fileList);
	spLoop( draw, calc, 10, resize, NULL );
	spFontDelete(font);
	spFontDelete(font_small);
	spFontDelete(font_very_small);
	spQuitCore();
	return 0;
}
