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

void system_copy_overwrite(pSourceList from_source,pSourceList to_source)
{
	char buffer[2048];
	sprintf(buffer,"cp --preserve=all %s%s %s%s",from_source->location->url,from_source->fileName,to_source->location->url,to_source->fileName);
	system(buffer);
	to_source->version = from_source->version;
}

void system_copy_new(pOpkList opkFile,pSourceList from_source,pLocation new_location)
{
	char buffer[2048];
	sprintf(buffer,"cp --preserve=all %s%s %s",from_source->location->url,from_source->fileName,new_location->url);
	system(buffer);
	add_new_source(opkFile,new_location,from_source->fileName,from_source->version,from_source->description);
}

void system_move_overwrite(pSourceList from_source,pSourceList to_source)
{
	char buffer[2048];
	sprintf(buffer,"mv %s%s %s%s",from_source->location->url,from_source->fileName,to_source->location->url,to_source->fileName);
	system(buffer);
	from_source->location = to_source->location;
	free(from_source->fileName);
	from_source->fileName = to_source->fileName;	
	from_source->next = to_source->next;
	free(to_source->description);
	spDeleteTextBlock(to_source->block);
	free(to_source);
}

void system_move_new(pOpkList opkFile,pSourceList from_source,pLocation new_location)
{
	char buffer[2048];
	sprintf(buffer,"mv %s%s %s%s",from_source->location->url,from_source->fileName,new_location->url,from_source->fileName);
	system(buffer);
	from_source->location = new_location;
}
