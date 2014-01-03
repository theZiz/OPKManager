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

void update_newest_version(pOpkList file)
{
	file->newest_version = 0;
	pSourceList source = file->sources;
	while (source)
	{
		if (file->newest_version < source->version)
			file->newest_version = source->version;
		source = source->next;
	}
}

void update_biggest_size(pOpkList file)
{
	file->biggest_size = 0;
	pSourceList source = file->sources;
	while (source)
	{
		if (file->biggest_size < source->size)
			file->biggest_size = source->size;
		source = source->next;
	}
}

void system_copy_overwrite(pOpkList opkFile,pSourceList from_source,pSourceList to_source)
{
	if (from_source->location->kind == 2)
		info("Downloading...",1);
	else
		info("Copying...",1);
	char buffer[2048];
	sprintf(buffer,"%s",to_source->location->url);
	spCreateDirectoryChain(buffer);
	if (from_source->location->kind == 2)
	{
		char random_filename[64];
		sprintf(random_filename,"/tmp/OPKManager_tmp_%i%i%i%i%i%i%i%i%i%i.opk",
			rand()%10,rand()%10,rand()%10,rand()%10,rand()%10,
			rand()%10,rand()%10,rand()%10,rand()%10,rand()%10);
		if (from_source->url_addition)
			sprintf(buffer,WGET" -O %s %s%s%s",random_filename,from_source->location->url,from_source->url_addition,from_source->fileName);
		else
			sprintf(buffer,WGET" -O %s %s%s",random_filename,from_source->location->url,from_source->fileName);
		if (system(buffer)) //Err0r
		{
			sprintf(buffer,"rm %s",random_filename);
			system(buffer);
			show_error = 1;
			return;
		}
		sprintf(buffer,"mv %s %s%s",random_filename,to_source->location->url,to_source->fileName);
		system(buffer);
		struct tm* myTime = localtime ((time_t*)&(from_source->version));
		sprintf(buffer,"touch -t %04i%02i%02i%02i%02i.%02i %s%s",myTime->tm_year+1900,myTime->tm_mon+1,myTime->tm_mday,myTime->tm_hour,myTime->tm_min,myTime->tm_sec,to_source->location->url,to_source->fileName);
		system(buffer);
	}
	else
	{
		sprintf(buffer,"cp --preserve=all %s%s %s%s",from_source->location->url,from_source->fileName,to_source->location->url,to_source->fileName);
		system(buffer);
	}
	to_source->version = from_source->version;
	to_source->size = from_source->size;
	update_newest_version(opkFile);
	update_biggest_size(opkFile);
}

void system_copy_new(pOpkList opkFile,pSourceList from_source,pLocation new_location)
{
	if (from_source->location->kind == 2)
		info("Downloading...",1);
	else
		info("Copying...",1);
	char buffer[2048];
	sprintf(buffer,"%s",new_location->url);
	spCreateDirectoryChain(buffer);
	if (from_source->location->kind == 2)
	{
		char random_filename[64];
		sprintf(random_filename,"/tmp/OPKManager_tmp_%i%i%i%i%i%i%i%i%i%i.opk",
			rand()%10,rand()%10,rand()%10,rand()%10,rand()%10,
			rand()%10,rand()%10,rand()%10,rand()%10,rand()%10);
		if (from_source->url_addition)
			sprintf(buffer,WGET" -O %s %s%s%s",random_filename,from_source->location->url,from_source->url_addition,from_source->fileName);
		else
			sprintf(buffer,WGET" -O %s %s%s",random_filename,from_source->location->url,from_source->fileName);
		if (system(buffer))
		{
			sprintf(buffer,"rm %s",random_filename);
			system(buffer);
			show_error = 1;
			return;
		}
		sprintf(buffer,"mv %s %s%s",random_filename,new_location->url,from_source->fileName);
		system(buffer);
		struct tm* myTime = localtime ((time_t*)&(from_source->version));
		sprintf(buffer,"touch -t %04i%02i%02i%02i%02i.%02i %s%s",myTime->tm_year+1900,myTime->tm_mon+1,myTime->tm_mday,myTime->tm_hour,myTime->tm_min,myTime->tm_sec,new_location->url,from_source->fileName);
		system(buffer);
	}
	else
	{
		sprintf(buffer,"cp --preserve=all %s%s %s",from_source->location->url,from_source->fileName,new_location->url);
		system(buffer);
	}
	add_new_source(opkFile,new_location,from_source->fileName,from_source->version,from_source->description,from_source->long_description);
}

void system_move_overwrite(pOpkList sel,pSourceList from_source,pSourceList to_source)
{
	info("Moving...",1);
	char buffer[2048];
	sprintf(buffer,"%s",to_source->location->url);
	spCreateDirectoryChain(buffer);
	sprintf(buffer,"mv %s%s %s%s",from_source->location->url,from_source->fileName,to_source->location->url,to_source->fileName);
	system(buffer);
	
	from_source->location = to_source->location;
	free(from_source->fileName);
	from_source->fileName = to_source->fileName;
	
	pSourceList before = NULL;
	pSourceList mom = sel->sources;
	while (mom)
	{
		if (mom == to_source)
			break;
		before = mom;
		mom = mom->next;
	}
	if (before)
		before->next = to_source->next;
	else
		sel->sources = to_source->next;
	free(to_source->description);
	free(to_source->long_description);
	spDeleteTextBlock(to_source->block);
	spDeleteTextBlock(to_source->long_block);
	free(to_source);
	update_newest_version(sel);
	update_biggest_size(sel);
}

void system_move_new(pOpkList opkFile,pSourceList from_source,pLocation new_location)
{
	info("Moving...",1);
	char buffer[2048];
	sprintf(buffer,"%s",new_location->url);
	spCreateDirectoryChain(buffer);
	sprintf(buffer,"mv %s%s %s%s",from_source->location->url,from_source->fileName,new_location->url,from_source->fileName);
	system(buffer);
	from_source->location = new_location;
}

void system_delete(pOpkList opkFile,pSourceList from_source)
{
	info("Deleting...",1);
	char buffer[2048];
	sprintf(buffer,"rm %s%s",from_source->location->url,from_source->fileName);
	system(buffer);
	//Removing the source
	pSourceList source = opkFile->sources;
	pSourceList before = NULL;
	while (source)
	{
		if (source == from_source)
			break;
		before = source;
		source = source->next;
	}
	if (before)
		before->next = from_source->next;
	else
		opkFile->sources = from_source->next;
	free(from_source->fileName);
	free(from_source->description);
	free(from_source->long_description);
	spDeleteTextBlock(from_source->block);
	spDeleteTextBlock(from_source->long_block);
	free(from_source);
	//Deleting opkFile, when sources are empty
	if (opkFile->sources == NULL)
	{
		pOpkList opk = opkList;
		pOpkList before = NULL;
		while (opk)
		{
			if (opk == opkFile)
				break;
			before = opk;
			opk = opk->next;
		}
		if (before)
			before->next = opkFile->next;
		else
			opkList = opkFile->next;
		if (opkFile->next == NULL) //was last
			selected--;
		//same for sorting list
		while (opk)
		{
			if (opk == opkFile)
				break;
			before = opk;
			opk = opk->sortedNext;
		}
		if (before)
			before->sortedNext = opkFile->sortedNext;
		else
			sortedOpkList = opkFile->sortedNext;
		opk_count--;
		free(opkFile);
	}
	else
	{
		update_newest_version(opkFile);
		update_biggest_size(opkFile);
	}
}

void system_run(pOpkList sel,pSourceList from_source)
{
	char buffer[2048];
	sprintf(buffer,"Running %s from %s...",sel->longName,from_source->location->name);
	info(buffer,1);
	sprintf(buffer,"opkrun %s%s",from_source->location->url,from_source->fileName);
	system(buffer);
}
