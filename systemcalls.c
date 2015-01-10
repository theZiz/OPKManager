 /* This file is part of OPKManager.
  * OPKManager is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 2 of the License, or
  * (at your option) any later version.
  * 
  * OPKManager is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  * 
  * You should have received a copy of the GNU General Public License
  * along with OPKManager.  If not, see <http://www.gnu.org/licenses/>
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

void check_name(pOpkList opk,pSourceList opk_source)
{
	//Getting the name from the opk:
	char buffer[512];
	sprintf(buffer,"%s%s",opk_source->location->url,opk_source->fileName);
	struct OPK* opkFile = opk_open(buffer);
	if (opkFile == NULL)
	{
		printf("That shouldn't happen\n");
		return;
	}
	const char* metaname;
	opk_open_metadata(opkFile, &metaname);
	const char *key, *val;
	size_t skey, sval;
	while(opk_read_pair(opkFile, &key, &skey, &val, &sval) && key)
	{
		char key_string[256];
		sprintf(key_string,"%.*s",(int)skey,key);
		if (strcmp(key_string,"Name") == 0)
		{
			memcpy(buffer,val,sval);
			buffer[sval] = 0;
		}
	}
	opk_close(opkFile);
	if (strcmp(buffer,opk->longName))
	{
		printf("Repo and opk name differ, adding a new line to automatic_alias.txt\n");
		char buffer2[512];
		FILE *fp = fopen(get_path(buffer2,"automatic_alias.txt"), "a");
		fprintf(fp,"[%s]\n",opk->longName);
		fprintf(fp,"%s\n",buffer);
		fprintf(fp,"\n");
		fclose(fp);		
	}
}

void system_copy_overwrite(pOpkList opkFile,pSourceList from_source,pSourceList to_source)
{
	if (from_source->location->kind != 2)
		info("Copying...",1);
	else
	{
		char buffer2[2048];
		sprintf(buffer2,"Downloading %s",opkFile->longName);
		info(buffer2,0);
	}
	char buffer[2048];
	sprintf(buffer,"%s",to_source->location->url);
	spCreateDirectoryChain(buffer);
	if (from_source->location->kind == 2)
	{
		char random_filename[64];
		sprintf(random_filename,"%sOPKManager_tmp_%i%i%i%i%i%i%i%i%i%i.opk.part",to_source->location->url,
			rand()%10,rand()%10,rand()%10,rand()%10,rand()%10,
			rand()%10,rand()%10,rand()%10,rand()%10,rand()%10);
		char* download_filename = from_source->download_fileName;
		if (download_filename == NULL)
			download_filename = from_source->fileName;
		if (from_source->url_addition)
			sprintf(buffer,WGET" -O %s %s%s%s"PROGRESS_MAGIC,random_filename,from_source->location->url,from_source->url_addition,download_filename);
		else
			sprintf(buffer,WGET" -O %s %s%s"PROGRESS_MAGIC,random_filename,from_source->location->url,download_filename);
		FILE *fp = popen(buffer, "r");
		while (fgets(buffer, 2048, fp) != NULL)
		{
			printf("%s",buffer);
			char* percent = strchr(buffer,'%');
			if (percent)
			{
				percent-=3;
				percent[3] = 0;
				char buffer2[2048];
				sprintf(buffer2,"Downloading %s\n%s%%",opkFile->longName,percent);
				info(buffer2,0);
			}
		}
		if (pclose(fp)) //Err0r
		{
			sprintf(buffer,"rm %s",random_filename);
			system(buffer);
			show_error = 1;
			return;
		}
		else
			show_error = 2;
		sprintf(buffer,"mv %s %s%s",random_filename,to_source->location->url,to_source->fileName);
		system(buffer);
		struct tm* myTime = localtime ((time_t*)&(from_source->version));
		sprintf(buffer,"touch -t %04i%02i%02i%02i%02i.%02i %s%s",myTime->tm_year+1900,myTime->tm_mon+1,myTime->tm_mday,myTime->tm_hour,myTime->tm_min,myTime->tm_sec,to_source->location->url,to_source->fileName);
		system(buffer);
	}
	else
	{
		sprintf(buffer,"cp %s%s %s%s",from_source->location->url,from_source->fileName,to_source->location->url,to_source->fileName);
		system(buffer);
		sprintf(buffer,"touch -r %s%s %s%s",from_source->location->url,from_source->fileName,to_source->location->url,to_source->fileName);
		system(buffer);
	}
	to_source->version = from_source->version;
	update_newest_version(opkFile);
	update_biggest_size(opkFile);
}

void system_copy_new(pOpkList opkFile,pSourceList from_source,pLocation new_location)
{
	if (from_source->location->kind != 2)
		info("Copying...",1);
	else
	{
		char buffer2[2048];
		sprintf(buffer2,"Downloading %s",opkFile->longName);
		info(buffer2,0);
	}
	char buffer[2048];
	sprintf(buffer,"%s",new_location->url);
	spCreateDirectoryChain(buffer);
	if (from_source->location->kind == 2)
	{
		char random_filename[64];
		sprintf(random_filename,"%sOPKManager_tmp_%i%i%i%i%i%i%i%i%i%i.opk.part",new_location->url,
			rand()%10,rand()%10,rand()%10,rand()%10,rand()%10,
			rand()%10,rand()%10,rand()%10,rand()%10,rand()%10);
		char* download_filename = from_source->download_fileName;
		if (download_filename == NULL)
			download_filename = from_source->fileName;
		if (from_source->url_addition)
			sprintf(buffer,WGET" -O %s %s%s%s"PROGRESS_MAGIC,random_filename,from_source->location->url,from_source->url_addition,download_filename);
		else
			sprintf(buffer,WGET" -O %s %s%s"PROGRESS_MAGIC,random_filename,from_source->location->url,download_filename);
		FILE *fp = popen(buffer, "r");
		while (fgets(buffer, 2048, fp) != NULL)
		{
			printf("%s",buffer);
			char* percent = strchr(buffer,'%');
			if (percent)
			{
				percent-=3;
				percent[3] = 0;
				char buffer2[2048];
				sprintf(buffer2,"Downloading %s\n%s%%",opkFile->longName,percent);
				info(buffer2,0);
			}
		}
		if (pclose(fp)) //Err0r
		{
			sprintf(buffer,"rm %s",random_filename);
			system(buffer);
			show_error = 1;
			return;
		}
		else
			show_error = 2;
		sprintf(buffer,"mv %s %s%s",random_filename,new_location->url,from_source->fileName);
		system(buffer);
		struct tm* myTime = localtime ((time_t*)&(from_source->version));
		sprintf(buffer,"touch -t %04i%02i%02i%02i%02i.%02i %s%s",myTime->tm_year+1900,myTime->tm_mon+1,myTime->tm_mday,myTime->tm_hour,myTime->tm_min,myTime->tm_sec,new_location->url,from_source->fileName);
		system(buffer);
	}
	else
	{
		sprintf(buffer,"cp %s%s %s",from_source->location->url,from_source->fileName,new_location->url);
		system(buffer);
		sprintf(buffer,"touch -r %s%s %s%s",from_source->location->url,from_source->fileName,new_location->url,from_source->fileName);
		system(buffer);
	}
	pSourceList to_source = add_new_source(opkFile,new_location,from_source->fileName,from_source->version,from_source->description,from_source->long_description);
	check_name(opkFile,to_source);
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
		opk = sortedOpkList;
		before = NULL;
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
	quit_OPKManager(0);
	sprintf(buffer,"opkrun %s%s",from_source->location->url,from_source->fileName);
	if (system(buffer) == 0)
	{}
	else
	{
		sprintf(buffer,"./opkrun %s%s",from_source->location->url,from_source->fileName);
		if (system(buffer) == 0)
		{}
		else
			show_error = 4;
	}
	init_OPKManager(0);
}
