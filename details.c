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

#define LEFT_TEXT screen->w*9/32
#define RIGHT_TEXT screen->w*10/32

SDL_Surface* show_image = NULL;

void draw_details(pOpkList sel)
{
	if (show_image)
	{
		spClearTarget(0);
		Sint32 zoom = spMin(screen->w*SP_ONE/show_image->w,screen->h*SP_ONE/show_image->h);
		spRotozoomSurface(screen->w/2,screen->h/2,0,show_image,zoom,zoom,0);
		spFontDrawMiddle(screen->w/2,screen->h-font_small->maxheight-2,0,"[o] or [c] Back",font_small);
		return;
	}
	char buffer[256];
	spInterpolateTargetToColor(0,SP_ONE/2);
	spRectangle(screen->w/2,screen->h/2,0,screen->w*5/6,screen->h*5/6,LIST_BACKGROUND_COLOR);
	spRectangleBorder(screen->w/2,screen->h/2,0,screen->w*5/6+2,screen->h*5/6+2,1,1,FONT_COLOR);
	int distance = screen->h/14;
	int y = 2*distance;
	spFontDrawRight(LEFT_TEXT,y,0,"Package:",font);
	spFontDraw     (RIGHT_TEXT,y,0,sel->longName,font);
	y+=distance;
	spFontDrawRight(LEFT_TEXT,y,0,"Version:",font);
	pSourceList source = sel->sources;
	spTextBlockPointer block = NULL;
	char* image_url = NULL;
	while (source)
	{
		switch (source->location->kind)
		{
			case 0: spBlitSurface(RIGHT_TEXT+7,y+7,0,internal_surface); break;
			case 1: spBlitSurface(RIGHT_TEXT+7,y+7,0,sdcard_surface); break;
			case 2: spBlitSurface(RIGHT_TEXT+7,y+7,0,web_surface); break;
			case 3: spBlitSurface(RIGHT_TEXT+7,y+7,0,usb_surface); break;
		}
		//struct tm* myTime = gmtime ((time_t*)&(source->version));
		//sprintf(buffer,"%i.%i.%i %i:%i",myTime->tm_mday,myTime->tm_mon+1,myTime->tm_year+1900,myTime->tm_hour,myTime->tm_min);
		//spFontDraw     (RIGHT_TEXT+14,y,0,buffer,font);
		spFontDraw     (RIGHT_TEXT+14,y,0,ctime((time_t*)&(source->version)),font);
		y+=distance/2;
		sprintf(buffer,"Filename: \"%s\"",source->fileName);
		spFontDraw     (RIGHT_TEXT,y+5,0,buffer,font_small);
		y+=distance;
		if (source->block)
			block = source->block;
		if (source->image_url)
			image_url = source->image_url;
		source = source->next;
	}
	if (block)
	{
		spFontDrawRight(LEFT_TEXT,y,0,"Description:",font);
		spFontDrawTextBlock(left,RIGHT_TEXT,y,0,block,screen->h*13/16-y,0,font);
	}
	y+=distance;

	if (image_url)
		spFontDrawMiddle(screen->w/2,screen->h*13/16,0,"[o] Show screenshot     [c] Back",font);
	else
		spFontDrawMiddle(screen->w/2,screen->h*13/16,0,"[o] or [c] Back",font);
}

int calc_details(pOpkList sel)
{
	if (show_image)
	{
		if (spGetInput()->button[SP_PRACTICE_CANCEL_NOWASD] || spGetInput()->button[SP_PRACTICE_OK_NOWASD])
		{
			spGetInput()->button[SP_PRACTICE_CANCEL_NOWASD] = 0;
			spGetInput()->button[SP_PRACTICE_OK_NOWASD] = 0;
			spDeleteSurface(show_image);
			show_image = NULL;
		}
		return 1;
	}
	pSourceList source = sel->sources;
	char* image_url = NULL;
	while (source)
	{
		if (source->image_url)
			image_url = source->image_url;
		source = source->next;
	}
	if (image_url && spGetInput()->button[SP_PRACTICE_OK_NOWASD])
	{
		info("Downloading image...",1);
		spGetInput()->button[SP_PRACTICE_OK_NOWASD] = 0;
		char random_filename[64];
		sprintf(random_filename,"/tmp/OPKManager_tmp_%i%i%i%i%i%i%i%i%i%i.png",
			rand()%10,rand()%10,rand()%10,rand()%10,rand()%10,
			rand()%10,rand()%10,rand()%10,rand()%10,rand()%10);
		char buffer[1024];
		sprintf(buffer,"wget -O %s %s",random_filename,image_url);
		//sprintf(buffer,"cp /home/alexander/develop/handhelds/puzzletube/build/pandora/puzzletube/previews/screenshot1.png %s",random_filename);
		if (system(buffer) == 0)
			show_image = spLoadSurface(random_filename);
		sprintf(buffer,"rm %s",random_filename);
		system(buffer);
	}
	if (spGetInput()->button[SP_PRACTICE_CANCEL_NOWASD] || (image_url == NULL && spGetInput()->button[SP_PRACTICE_OK_NOWASD]))
	{
		spGetInput()->button[SP_PRACTICE_CANCEL_NOWASD] = 0;
		spGetInput()->button[SP_PRACTICE_OK_NOWASD] = 0;
		return 0;
	}
	return 1;
}

Sint32 help_pos = 0;

void draw_help()
{
	char buffer[256];
	spInterpolateTargetToColor(0,SP_ONE/2);
	spRectangle(screen->w/2,screen->h/2,0,screen->w*5/6,screen->h*5/6,LIST_BACKGROUND_COLOR);
	spRectangleBorder(screen->w/2,screen->h/2,0,screen->w*5/6+2,screen->h*5/6+2,1,1,FONT_COLOR);
	spFontDrawTextBlock(left,screen->w/8,screen->h/8,0,helpBlock,screen->h*3/4,help_pos,font);
	spFontDrawMiddle(screen->w/2,screen->h*11/12-font->maxheight,0,"[o] or [c] Back",font_small);
	if (help_pos != SP_ONE)
	{
		spTriangle(screen->w*11/12 - 1,screen->h*11/12 -11,0,
		           screen->w*11/12 -11,screen->h*11/12 -11,0,
		           screen->w*11/12 - 6,screen->h*11/12 - 6,0,FONT_COLOR);
	}
	if (help_pos != 0)
	{
		spTriangle(screen->w*11/12 - 6,screen->h/12+ 6,0,
		           screen->w*11/12 -11,screen->h/12+11,0,
		           screen->w*11/12 - 1,screen->h/12+11,0,FONT_COLOR);
	}
}

int calc_help(int steps)
{
	if (time_until_next > 0)
		time_until_next -= steps;
	if (spGetInput()->axis[1] < 0 && help_pos > 0)
	{
		if (time_until_next <= 0)
		{
			help_pos-=SP_ONE/16;
			next_in_a_row++;
			time_until_next = 300/next_in_a_row;
		}
	}
	else
	if (spGetInput()->axis[1] > 0 && help_pos < SP_ONE)
	{
		if (time_until_next <= 0)
		{
			help_pos+=SP_ONE/16;
			next_in_a_row++;
			time_until_next = 300/next_in_a_row;
		}
	}
	else
	{
		time_until_next = 0;
		next_in_a_row = 0;
	}
	if (spGetInput()->button[SP_PRACTICE_CANCEL_NOWASD] || spGetInput()->button[SP_PRACTICE_OK_NOWASD])
	{
		spGetInput()->button[SP_PRACTICE_CANCEL_NOWASD] = 0;
		spGetInput()->button[SP_PRACTICE_OK_NOWASD] = 0;
		return 0;
	}
	return 1;
}
