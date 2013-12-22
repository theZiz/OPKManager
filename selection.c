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

int selection_selection = 0;

//show: 0 all, 1 only available, 2 only unavailable
void draw_selection(char* caption,pOpkList sel, int show, pLocation except_location,int with_web)
{
	char buffer[256];
	spInterpolateTargetToColor(0,SP_ONE/2);
	//counting locations
	pLocation location = locationList;
	int location_count = 0;
	pSourceList source;
	while (location)
	{
		if (location == except_location || (with_web == 0 && location->kind == 2))
		{
			location = location->next;
			continue;
		}
		switch (show)
		{
			case 0:
				location_count++;
				break;
			case 1:
				source = sel->sources;
				while (source)
				{
					if (source->location == location)
						break;
					source = source->next;
				}
				if (source)
					location_count++;
				break;
			case 2:
				source = sel->sources;
				while (source)
				{
					if (source->location == location)
						break;
					source = source->next;
				}
				if (source == NULL)
					location_count++;
				break;
		}
		location = location->next;
	}
	spRectangle(screen->w/2,screen->h/2,0,screen->w*5/6,font->maxheight*(location_count+5),LIST_BACKGROUND_COLOR);
	spRectangleBorder(screen->w/2,screen->h/2,0,screen->w*5/6+2,font->maxheight*(location_count+5)+2,1,1,FONT_COLOR);
	int y = screen->h/2-font->maxheight*(location_count+5)/2+font->maxheight/2;
	spFontDrawMiddle(screen->w/2,y,0,caption,font);
	y+=font->maxheight;
	y+=font->maxheight/2;
	source = sel->sources;
	location = locationList;
	int i = 0;
	while (location)
	{
		if (location == except_location || (with_web == 0 && location->kind == 2))
		{
			location = location->next;
			continue;
		}
		switch (show)
		{
			case 1:
				source = sel->sources;
				while (source)
				{
					if (source->location == location)
						break;
					source = source->next;
				}
				if (source == NULL)
				{
					location = location->next;
					continue;
				}
				break;
			case 2:
				source = sel->sources;
				while (source)
				{
					if (source->location == location)
						break;
					source = source->next;
				}
				if (source)
				{
					location = location->next;
					continue;
				}
				break;
		}
		if (i == selection_selection)
			spRectangle(screen->w/2+1,y+font->maxheight/2,0,screen->w*5/6-4,font->maxheight,SELECTED_BACKGROUND_COLOR);
		sprintf(buffer," %s (%s)",location->name,location->url);
		switch (location->kind)
		{
			case 0: spBlitSurface(screen->w/2-spFontWidth(buffer,font)/2,y+7,0,internal_surface); break;
			case 1: spBlitSurface(screen->w/2-spFontWidth(buffer,font)/2,y+7,0,sdcard_surface); break;
			case 2: spBlitSurface(screen->w/2-spFontWidth(buffer,font)/2,y+7,0,web_surface); break;
			case 3: spBlitSurface(screen->w/2-spFontWidth(buffer,font)/2,y+7,0,usb_surface); break;
		}
		spFontDrawMiddle(screen->w/2+7,y,0,buffer,font);
		y+=font->maxheight;
		i++;
		location = location->next;
	}
	y+=font->maxheight*3/2;
	spFontDrawMiddle(screen->w/2,y,0,"[o] Select      [c] Cancel",font);
}

int calc_selection(int steps,pOpkList sel, int show, pLocation except_location,int with_web)
{
	//counting locations
	pLocation location = locationList;
	int location_count = 0;
	pSourceList source;
	while (location)
	{
		if (location == except_location || (with_web == 0 && location->kind == 2))
		{
			location = location->next;
			continue;
		}
		switch (show)
		{
			case 0:
				location_count++;
				break;
			case 1:
				source = sel->sources;
				while (source)
				{
					if (source->location == location)
						break;
					source = source->next;
				}
				if (source)
					location_count++;
				break;
			case 2:
				source = sel->sources;
				while (source)
				{
					if (source->location == location)
						break;
					source = source->next;
				}
				if (source == NULL)
					location_count++;
				break;
		}
		location = location->next;
	}
	if (time_until_next > 0)
		time_until_next -= steps;
	if (spGetInput()->axis[1] < 0 && selection_selection > 0)
	{
		if (time_until_next <= 0)
		{
			selection_selection--;
			next_in_a_row++;
			time_until_next = 300/next_in_a_row;
		}
	}
	else
	if (spGetInput()->axis[1] > 0 && selection_selection < location_count-1)
	{
		if (time_until_next <= 0)
		{
			selection_selection++;
			next_in_a_row++;
			time_until_next = 300/next_in_a_row;
		}
	}
	else
	{
		time_until_next = 0;
		next_in_a_row = 0;
	}
	if (spGetInput()->button[SP_PRACTICE_CANCEL_NOWASD])
	{
		spGetInput()->button[SP_PRACTICE_CANCEL_NOWASD] = 0;
		return 0;
	}
	if (spGetInput()->button[SP_PRACTICE_OK_NOWASD])
	{
		spGetInput()->button[SP_PRACTICE_OK_NOWASD] = 0;
		return 2;
	}
	return 1;
}

void draw_sure(char* caption1,char* caption2,char* caption3,char* caption4)
{
	char buffer[256];
	spInterpolateTargetToColor(0,SP_ONE/2);
	spRectangle(screen->w/2,screen->h/2,0,screen->w*5/6,font->maxheight*7,LIST_BACKGROUND_COLOR);
	spRectangleBorder(screen->w/2,screen->h/2,0,screen->w*5/6+2,font->maxheight*7+2,1,1,FONT_COLOR);
	int y = screen->h/2-font->maxheight*7/2+font->maxheight/2;
	spFontDrawMiddle(screen->w/2,y,0,caption1,font);
	y+=font->maxheight;
	spFontDrawMiddle(screen->w/2,y,0,caption2,font);
	y+=font->maxheight;
	spFontDrawMiddle(screen->w/2,y,0,caption3,font);
	y+=font->maxheight;
	spFontDrawMiddle(screen->w/2,y,0,caption4,font);
	y+=font->maxheight*2;
	spFontDrawMiddle(screen->w/2,y,0,"[o] Okay      [c] No",font);
}

int calc_sure()
{
	if (spGetInput()->button[SP_PRACTICE_CANCEL_NOWASD])
	{
		spGetInput()->button[SP_PRACTICE_CANCEL_NOWASD] = 0;
		return 0;
	}
	if (spGetInput()->button[SP_PRACTICE_OK_NOWASD])
	{
		spGetInput()->button[SP_PRACTICE_OK_NOWASD] = 0;
		return 2;
	}
	return 1;
}
