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

void draw_details(pOpkList sel)
{
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
		source = source->next;
	}
	spFontDrawRight(LEFT_TEXT,y,0,"Filename:",font);
	//spFontDraw     (RIGHT_TEXT,y*screen->h/14,0,sel->fileName,font);
	y+=distance;
	if (block)
	{
		spFontDrawRight(LEFT_TEXT,y,0,"Description:",font);
		spFontDrawTextBlock(SP_LEFT,RIGHT_TEXT,y,0,block,screen->h*13/16-y,0,font);
	}
	y+=distance;

	spFontDrawMiddle(screen->w/2,screen->h*13/16,0,"[o] or [c] Back",font);
}

int calc_details()
{
	if (spGetInput()->button[SP_PRACTICE_CANCEL_NOWASD] || spGetInput()->button[SP_PRACTICE_OK_NOWASD])
	{
		spGetInput()->button[SP_PRACTICE_CANCEL_NOWASD] = 0;
		spGetInput()->button[SP_PRACTICE_OK_NOWASD] = 0;
		return 0;
	}
	return 1;
}