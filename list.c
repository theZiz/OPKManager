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

char* get_path(char* buffer,char* file)
{
	sprintf(buffer,"%s/.config/OPKManager/%s",getenv("HOME"),file);
	return buffer;
}

void load_repository_list()
{
	char buffer[1024];
	FILE *fp = fopen(get_path(buffer,"repositories.txt"), "r");
	while (fgets(buffer, 1024, fp) != NULL)
	{
		pRepository repository = (pRepository)malloc(sizeof(tRepository));
		sprintf(repository->url,"%s",buffer);
		repository->next = repositoryList;
		repositoryList = repository;
	}
	fclose(fp);
}

void download_new_repositories()
{
	char buffer[1024];
	pRepository repository = repositoryList;
	while (repository)
	{
		sprintf(buffer,"Download %s...",repository->url);
		info(buffer,1);
		char random_filename[64];
		sprintf(random_filename,"/tmp/OPKManager_tmp_%i%i%i%i%i%i%i%i%i%i.script",
			rand()%10,rand()%10,rand()%10,rand()%10,rand()%10,
			rand()%10,rand()%10,rand()%10,rand()%10,rand()%10);
		char filename[256];
		int i;
		for (i = strlen(repository->url)-1; i >= 0; i--)
			if (repository->url[i] == '/')
				break;
		sprintf(filename,"sripts/%s",&repository->url[i+1]);
		sprintf(buffer,WGET" -O %s %s",random_filename,repository->url);
		if (system(buffer)) //Err0r
		{
			sprintf(buffer,"rm %s",random_filename);
			system(buffer);
		}
		else
		{
			sprintf(buffer,"mv %s %s",random_filename,get_path(buffer,filename));
			system(buffer);
		}
		repository = repository->next;
	}
}

void read_locations()
{
	spFileListPointer directoryList = NULL;
	spFileGetDirectory(&directoryList,ROOT,0,1);
	spFileListPointer directory = directoryList;
	while (directory)
	{
		pLocation loc = (pLocation)malloc(sizeof(tLocation));
		if (strcmp(directory->name,ROOT"/data") == 0)
		{
			sprintf(loc->name,"Internal");
			loc->kind = 0;
		}
		else
		{
			//Looking, whether this is an SD Card or not.
#ifdef X86CPU
			if (strcmp(directory->name,ROOT"/sdcard") == 0)
#else
			FILE *fp = fopen("/proc/1/mounts", "r");
			int yes = 0;
			if (fp == NULL)
				printf("Failed to load mounts file\n" );
			else
			{
				char buffer[1024];
				while (fgets(buffer, 1024, fp) != NULL)
				{
					if (strstr(buffer,"/dev/mmc") == buffer) //starts with it
					{
						//search for the mount point
						char* space_place = strchr(buffer,' ');
						if (space_place)
						{
							space_place++;
							if (strstr(space_place,directory->name) == space_place) //starts with
								yes = 1;
						}
					}
				}
				fclose(fp);		
			}
			if (yes)
#endif
			{
				sprintf(loc->name,"SD Card");
				loc->kind = 1;
			}
			else
			{
				sprintf(loc->name,"USB Device");
				loc->kind = 3;
			}
		}
		loc->url = (char*)malloc(strlen(directory->name)+7);
		sprintf(loc->url,"%s/apps/",directory->name);
		loc->update_call = NULL;
		loc->next = locationList;
		locationList = loc;
		printf("Found %s (%i): %s\n",loc->name,loc->kind,loc->url);
		directory = directory->next;
	}
	spFileDeleteList(directoryList);
	//scripts!
	directoryList = NULL;
	spFileGetDirectory(&directoryList,"./repositories",0,1);
	directory = directoryList;
	while (directory)
	{
		//Calling the script:
		char buffer[256];
		sprintf(buffer,"%s --register",directory->name);
		FILE *fp = popen(buffer, "r");
		if (fp == NULL)
		{
			printf("Failed to run repository script\n" );
			directory = directory->next;
			continue;
		}
		int i = 0;
		pLocation loc = (pLocation)malloc(sizeof(tLocation));
		fgets(loc->name, sizeof(loc->name), fp);
		loc->name[strlen(loc->name)-1] = 0;
		fgets(buffer, 256, fp);
		if (strcmp(buffer,"web\n") == 0)
			loc->kind = 2;
		else
			loc->kind = 3;
		fgets(buffer, 256, fp);
		int l = strlen(buffer)+1;
		loc->url = (char*)malloc(l);
		memcpy(loc->url,buffer,l);
		loc->url[l-2] = 0;

		fgets(buffer, 256, fp);
		l = strlen(buffer)+1;
		loc->update_call = (char*)malloc(l);
		memcpy(loc->update_call,buffer,l);
		loc->update_call[l-2] = 0;

		loc->next = locationList;
		locationList = loc;
		pclose(fp);		
		printf("Found %s (%i): %s\n",loc->name,loc->kind,loc->url);
		directory = directory->next;
	}
	spFileDeleteList(directoryList);
}

pSourceList add_new_source(pOpkList file,pLocation location,char* filename,Sint64 version,char* description)
{
	//Source already in file?
	pSourceList newSource = file->sources;
	while (newSource)
	{
		if (newSource->location == location)
			break;
		newSource = newSource->next;
	}
	if (newSource == NULL)
	{
		newSource = (pSourceList)malloc(sizeof(tSourcelist));
		newSource->location = location;
		newSource->url_addition = NULL;	
		newSource->image_url = NULL;
		newSource->fileName = NULL;
		newSource->description = NULL;
		newSource->next = file->sources;
		file->sources = newSource;
	}
	if (filename)
	{
		if (newSource->fileName)
			free(newSource->fileName);
		int l = strlen(filename)+1;
		newSource->fileName = (char*)malloc(l);
		memcpy(newSource->fileName,filename,l);
	}
	if (description)
	{
		if (newSource->description)
		{
			free(newSource->description);
			spDeleteTextBlock(newSource->block);
		}
		int l = strlen(description)+1;
		newSource->description = (char*)malloc(l);
		memcpy(newSource->description,description,l);
		newSource->block = spCreateTextBlock(description,screen->w/2,font);
	}
	newSource->version = version;
	return newSource;
}

pSourceList add_new_file(char* longname,char* filename,pLocation location,Sint64 version,char* description)
{
	pOpkList newOpk = (pOpkList)malloc(sizeof(tOpkList));
	sprintf(newOpk->longName,"%s",longname);
	int i;
	for (i = 0; newOpk->longName[i] != 0; i++)
	{
		if (newOpk->longName[i] >= 'A' && newOpk->longName[i] <= 'Z')
			newOpk->smallLongName[i] = newOpk->longName[i]-'A'+'a';
		else
			newOpk->smallLongName[i] = newOpk->longName[i];
	}
	newOpk->smallLongName[i] = 0;
	newOpk->sources = NULL;
	pSourceList result = add_new_source(newOpk,location,filename,version,description);
	//Searching the first, which is bigger
	pOpkList bigger = opkList;
	pOpkList smaller = NULL;
	while (bigger)
	{
		if (strcmp(newOpk->smallLongName,bigger->smallLongName) < 0)
			break;
		smaller = bigger;
		bigger = bigger->next;
	}
	if (smaller)
		smaller->next = newOpk;
	else
		opkList = newOpk;
	newOpk->next = bigger;
	opk_count++;
	return result;
}

pSourceList add_file_to_opkList(char* longname,char* filename,pLocation location,Sint64 version,char* description)
{
	//Searching in the opkList
	pOpkList file = opkList;
	while (file)
	{
		if (strcmp(file->longName,longname) == 0)
			break;
		file = file->next;
	}
	if (file) //found
		return add_new_source(file,location,filename,version,description);
	return add_new_file(longname,filename,location,version,description);
}

void merge_fileList_to_opkList(spFileListPointer fileList,pLocation location)
{
	spFileListPointer file = fileList;
	while (file)
	{
		//Searching the last /
		int i;
		for (i = strlen(file->name)-1; i >= 0; i--)
			if (file->name[i] == '/')
				break;
		char* filename = &(file->name[i+1]);
		struct OPK* opkFile = opk_open(file->name);
		char* longname = NULL;
		char* description = NULL;
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
				longname = (char*)malloc(sval+1);
				memcpy(longname,val,sval);
				longname[sval] = 0;
				printf("Loading %s\n",longname);
			}
			if (strcmp(key_string,"Comment") == 0)
			{
				description = (char*)malloc(sval+1);
				memcpy(description,val,sval);
				description[sval] = 0;
			}
		}
		opk_close(opkFile);
		if (longname == NULL)
		{
			longname = (char*)malloc(strlen("Error while reading applications name!")+1);
			sprintf(longname,"Error while reading applications name!");
		}
		add_file_to_opkList(longname,filename,location,file->last_mod,description);
		file = file->next;
	}
}

void add_all_locations()
{
	pLocation loc = locationList;
	while (loc)
	{
		if (loc->kind == 2)
		{
			loc = loc->next;
			continue;
		}
		spFileListPointer fileList = NULL;
		spFileGetDirectory(&fileList,loc->url,0,1);
		merge_fileList_to_opkList(fileList,loc);
		spFileDeleteList(fileList);
		loc = loc->next;
	}	
}

void add_new_web(char* name,char* filename,char* url_addition,char* description,Sint64 version,pLocation loc,char* image_url)
{
	pSourceList source = add_file_to_opkList(name,filename,loc,version,description[0]?description:NULL);
	if (url_addition[0])
	{
		if (source->url_addition)
			free(source->url_addition);
		int k = strlen(url_addition)+1;
		source->url_addition = (char*)malloc(k);
		memcpy(source->url_addition,url_addition,k);
	}
	if (image_url[0])
	{
		if (source->image_url)
			free(source->image_url);
		int k = strlen(image_url)+1;
		source->image_url = (char*)malloc(k);
		memcpy(source->image_url,image_url,k);
	}
	printf("Added: %s, Version %s       %s%s%s\n       %s\n       %s\n",name,ctime((time_t*)&(version)),loc->url,url_addition,filename,description,image_url);
	description[0] = 0;
	url_addition[0] = 0;
	image_url[0] = 0;
}

void update_repositories()
{
	char buffer[1024];
	download_new_repositories();
	pLocation location = locationList;
	while (location)
	{
		if (location->kind != 2)
		{
			location = location->next;
			continue;
		}
		sprintf(buffer,"Update %s...",location->name);
		info(buffer,1);
		FILE *fp = popen(location->update_call, "r");
		if (fp == NULL)
		{
			printf("Failed to run update for %s\n",location->name);
			location = location->next;
			continue;
		}
		/* Read the output a line at a time - output it. */
		char name[256] = "";
		char filename[256] = "";
		char url_addition[256] = "";
		char description[1024] = "";
		char image_url[1024] = "";
		Sint64 version = 0;
		while (fgets(buffer, 1024, fp) != NULL)
		{
			if (buffer[0] == '\n') //line break, skip
				continue;
			int l = strlen(buffer);
			buffer[l-1] = 0;
			l--;
			if (buffer[0] == '[') //new entry
			{
				if (name[0]) //Adding old entry
					add_new_web(name,filename,url_addition,description,version,location,image_url);
				char* end_character = strchr(buffer,']');
				if (end_character)
					end_character[0] = 0;
				memcpy(name,&(buffer[1]),l-1);
			}
			else
			{
				char* middle_character = strchr(buffer,':');
				if (middle_character == NULL)
					continue;
				middle_character[0] = 0;
				char key[256];
				sprintf(key,"%s",buffer);
				middle_character++;
				while (middle_character[0] == ' ')
					middle_character++;
				char value[256];
				sprintf(value,"%s",middle_character);
				if (strcmp(key,"version") == 0)
					version = atoi(value);
				else
				if (strcmp(key,"description") == 0)
					sprintf(description,"%s",value);
				else
				if (strcmp(key,"url_addition") == 0)
					sprintf(url_addition,"%s",value);
				else
				if (strcmp(key,"filename") == 0)
					sprintf(filename,"%s",value);
				else
				if (strcmp(key,"image_url") == 0)
					sprintf(image_url,"%s",value);
			}
		}
		if (name[0]) //Adding last entry
			add_new_web(name,filename,url_addition,description,version,location,image_url);
		pclose(fp);		
		location = location->next;
	}
}
