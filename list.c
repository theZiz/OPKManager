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

char* get_path(char* buffer,char* file)
{
	sprintf(buffer,"%s/.config/OPKManager/%s",getenv("HOME"),file);
	return buffer;
}

void update_size(pSourceList source)
{
	if (source->location->kind == 2)
	{
		source->size = 0;
		return;
	}
	char filename[512];
	sprintf(filename,"%s%s",source->location->url,source->fileName);
	struct stat st;
	stat(filename, &st);
	source->size = st.st_size;	
}

void load_repository_list()
{
	char buffer[1024];
	FILE *fp = fopen(get_path(buffer,"repositories.txt"), "r");
	while (fgets(buffer, 1024, fp) != NULL)
	{
		if (buffer[0] == '#')
			continue;
		pRepository repository = (pRepository)malloc(sizeof(tRepository));
		sprintf(repository->url,"%s",buffer);
		repository->next = repositoryList;
		repositoryList = repository;
	}
	fclose(fp);
}

void load_alias_list()
{
	char buffer[1024];
	FILE *fp = fopen(get_path(buffer,"alias.txt"), "r");
	while (fgets(buffer, 1024, fp) != NULL)
	{
		if (buffer[0] == '#')
			continue;
		pRepository alias = (pRepository)malloc(sizeof(tRepository));
		sprintf(alias->url,"%s",buffer);
		alias->next = aliasRepositoryList;
		aliasRepositoryList = alias;
	}
	fclose(fp);
}

void download_new_repositories()
{
	char buffer[1024];
	pRepository repository = repositoryList;
	while (repository)
	{
		sprintf(buffer,"Downloading %s...",repository->url);
		info(buffer,1);
		char random_filename[64];
		sprintf(random_filename,"/tmp/OPKManager_tmp_%i%i%i%i%i%i%i%i%i%i.py",
			rand()%10,rand()%10,rand()%10,rand()%10,rand()%10,
			rand()%10,rand()%10,rand()%10,rand()%10,rand()%10);
		char filename[256];
		int i;
		for (i = strlen(repository->url)-1; i >= 0; i--)
			if (repository->url[i] == '/')
				break;
		sprintf(filename,"scripts/%s",&repository->url[i+1]);
		sprintf(buffer,WGET" -O %s %s",random_filename,repository->url);
		if (system(buffer)) //Err0r
		{
			sprintf(buffer,"rm %s",random_filename);
			system(buffer);
		}
		else
		{
			char buffer2[1024];
			sprintf(buffer,"mv %s %s",random_filename,get_path(buffer2,filename));
			system(buffer);
			sprintf(buffer,"chmod +x %s",get_path(buffer2,filename));
			system(buffer);
		}
		repository = repository->next;
	}
	//scripts!
	spFileListPointer directoryList = NULL;
	spFileGetDirectory(&directoryList,get_path(buffer,"scripts"),0,1);
	spFileListPointer directory = directoryList;
	while (directory)
	{
		//Calling the script:
		sprintf(buffer,"%s --register",directory->name);
		FILE *fp = popen(buffer, "r");
		if (fp == NULL)
		{
			printf("Failed to run repository script\n" );
			directory = directory->next;
			continue;
		}
		int i = 0;
		fgets(buffer, sizeof(buffer), fp);
		buffer[strlen(buffer)-1] = 0;
		pLocation loc = locationList;
		while (loc)
		{
			if (strcmp(loc->name,buffer) == 0)
				break;
			loc = loc->next;
		}
		if (loc == NULL)
		{
			loc = (pLocation)malloc(sizeof(tLocation));
			loc->next = locationList;
			locationList = loc;
		}
		sprintf(loc->name,"%s",buffer);
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

		fgets(buffer, 256, fp);
		loc->letter = buffer[0];

		pclose(fp);		
		printf("Found %s (%i): %s\n",loc->name,loc->kind,loc->url);
		directory = directory->next;
	}
	spFileDeleteList(directoryList);
}

void download_new_alias()
{
	char buffer[1024];
	pRepository alias = aliasRepositoryList;
	while (alias)
	{
		sprintf(buffer,"Downloading %s...",alias->url);
		info(buffer,1);
		char random_filename[64];
		sprintf(random_filename,"/tmp/OPKManager_tmp_%i%i%i%i%i%i%i%i%i%i.txt",
			rand()%10,rand()%10,rand()%10,rand()%10,rand()%10,
			rand()%10,rand()%10,rand()%10,rand()%10,rand()%10);
		char filename[256];
		int i;
		for (i = strlen(alias->url)-1; i >= 0; i--)
			if (alias->url[i] == '/')
				break;
		sprintf(filename,"alias/%s",&alias->url[i+1]);
		sprintf(buffer,WGET" -O %s %s",random_filename,alias->url);
		if (system(buffer)) //Err0r
		{
			sprintf(buffer,"rm %s",random_filename);
			system(buffer);
		}
		else
		{
			char buffer2[1024];
			sprintf(buffer,"mv %s %s",random_filename,get_path(buffer2,filename));
			system(buffer);
		}
		alias = alias->next;
	}
}

void add_alias(char* name,char* alias_name)
{
	//Searching alias_name
	pAlias alias = aliasList;
	while (alias)
	{
		if (strcmp(alias->alias_name,alias_name) == 0)
			return;
		alias = alias->next;
	}
	alias = (pAlias)malloc(sizeof(tAlias));
	sprintf(alias->name,"%s",name);
	sprintf(alias->alias_name,"%s",alias_name);
	alias->next = aliasList;
	aliasList = alias;
}

void remove_all_alias()
{
	while (aliasList)
	{
		pAlias next = aliasList->next;
		free(aliasList);
		aliasList = next;
	}
}

void add_own_alias(spFileListPointer *directory)
{
	char buffer[256];
	if (spFileExists(get_path(buffer,"automatic_alias.txt")) == 0)
		return;
	spFileListPointer own = (spFileListPointer)malloc(sizeof(spFileList));
	own->next = *directory;
	if (*directory)
		(*directory)->prev = own;
	own->prev = NULL;
	sprintf(own->name,"%s",get_path(buffer,"automatic_alias.txt"));
	own->type = SP_FILE_FILE;
	*directory = own;
}

void update_alias()
{
	remove_all_alias();
	spFileListPointer directoryList = NULL;
	char buffer[1024];
	spFileGetDirectory(&directoryList,get_path(buffer,"alias"),0,1);
	add_own_alias(&directoryList);
	spFileListPointer directory = directoryList;
	while (directory)
	{
		//Opening and reading the alias
		sprintf(buffer,"Reading alias %s...",directory->name);
		info(buffer,1);
		char buffer2[1024];
		FILE *fp = fopen(directory->name, "r");
		if (fp == NULL)
		{
			directory = directory->next;
			continue;
		}
		char name[256] = "";
		while (fgets(buffer, 1024, fp) != NULL)
		{
			if (buffer[0] == '\n') //line break, skip
				continue;
			int l = strlen(buffer);
			buffer[l-1] = 0;
			l--;
			if (buffer[0] == '[') //new entry
			{
				char* end_character = strchr(buffer,']');
				if (end_character)
					end_character[0] = 0;
				else
					return; //This file is corrupted!
				memcpy(name,&(buffer[1]),l-1);
				add_alias(name,name); //self adding
			}
			else
			{
				if (name[0] == 0) //No header? Corrupted!
					return;
				add_alias(name,buffer);
			}
		}
		fclose(fp);
		directory = directory->next;
	}
	spFileDeleteList(directoryList);
	//Using the alias list to change the names in the list, if already there
	pOpkList opk = opkList;
	while (opk)
	{
		pAlias alias = aliasList;
		while (alias)
		{
			if (strcmp(alias->alias_name,opk->longName) == 0)
			{
				sprintf(opk->longName,"%s",alias->name);
				break;
			}
			alias = alias->next;
		}
		opk = opk->next;
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
			loc->kind = 0;
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
				int error = 0;
				while (fgets(buffer, 1024, fp) != NULL)
				{
					if (strstr(buffer,"/dev/mmcblk0p1") == buffer) //root partition in /media?
					{
						//search for the mount point
						char* space_place = strchr(buffer,' ');
						if (space_place)
						{
							space_place++;
							if (strstr(space_place,directory->name) == space_place)
							{
								error = 1;
								break;
							}
						}
					}
					if (strstr(buffer,"/dev/mmc") == buffer) //starts with mmc
					{
						//search for the mount point
						char* space_place = strchr(buffer,' ');
						if (space_place)
						{
							space_place++;
							if (strstr(space_place,directory->name) == space_place)
								yes = 1;
						}
					}
				}
				fclose(fp);
				if (error)
				{
					free(loc);
					directory = directory->next;
					continue;
				}
			}
			if (yes)
#endif
				loc->kind = 1;
			else
				loc->kind = 3;
		}
		char* ending = directory->name;
		ending+=strlen(ROOT)+1;
		sprintf(loc->name,"%s",	&ending[0]);
		loc->url = (char*)malloc(strlen(directory->name)+7);
		sprintf(loc->url,"%s/apps/",directory->name);
		loc->update_call = NULL;
		loc->next = locationList;
		locationList = loc;
		printf("Found %s (%i): %s\n",loc->name,loc->kind,loc->url);
		directory = directory->next;
	}
	spFileDeleteList(directoryList);
}

pSourceList add_new_source(pOpkList file,pLocation location,char* filename,Sint64 version,char* description,char* long_description)
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
		newSource->download_fileName = NULL;
		newSource->description = NULL;
		newSource->long_description = NULL;
		newSource->block = NULL;
		newSource->long_block = NULL;
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
	if (long_description)
	{
		if (newSource->long_description)
		{
			free(newSource->long_description);
			spDeleteTextBlock(newSource->long_block);
		}
		int l = strlen(long_description)+1;
		newSource->long_description = (char*)malloc(l);
		memcpy(newSource->long_description,long_description,l);
		newSource->long_block = spCreateTextBlock(long_description,screen->w*9/12,font);
	}
	newSource->version = version;
	if (file->newest_version < version)
		file->newest_version = version;
	update_size(newSource);
	if (file->biggest_size < newSource->size)
		file->biggest_size = newSource->size;
	return newSource;
}

pSourceList add_new_file(char* longname,char* filename,pLocation location,Sint64 version,char* description,char* long_description)
{
	pOpkList newOpk = (pOpkList)malloc(sizeof(tOpkList));
	pAlias alias = aliasList;
	while (alias)
	{
		if (strcmp(alias->alias_name,longname) == 0)
			break;
		alias = alias->next;
	}
	if (alias)
		sprintf(newOpk->longName,"%s",alias->name);
	else
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
	newOpk->newest_version = 0;
	newOpk->biggest_size = 0;
	pSourceList result = add_new_source(newOpk,location,filename,version,description,long_description);
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

pSourceList add_file_to_opkList(char* longname,char* filename,pLocation location,Sint64 version,char* description,char* long_description)
{
	pAlias alias = aliasList;
	while (alias)
	{
		if (strcmp(alias->alias_name,longname) == 0)
			break;
		alias = alias->next;
	}
	char* test;
	if (alias)
		test = alias->name;
	else
		test = longname;
	//Searching in the opkList
	pOpkList file = opkList;
	while (file)
	{
		if (strcmp(file->longName,test) == 0)
			break;
		file = file->next;
	}
	if (file) //found
		return add_new_source(file,location,filename,version,description,long_description);
	return add_new_file(test,filename,location,version,description,long_description);
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
		if (opkFile == NULL)
		{
			file = file->next;
			continue;
		}
		char* longname = NULL;
		char* description = NULL;
		char* long_description = NULL;
		const char* metaname = NULL;
		if (opk_open_metadata(opkFile, &metaname) != 1)
		{
			file = file->next;
			continue;
		}
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
		add_file_to_opkList(longname,filename,location,file->last_mod,description,NULL);
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

void add_new_web(char* name,char* filename,char* url_addition,char* description,char* long_description,Sint64 version,pLocation loc,char* image_url,char* download_filename)
{
	pSourceList source = add_file_to_opkList(name,filename,loc,version,description[0]?description:NULL,long_description[0]?long_description:NULL);
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
	if (download_filename[0])
	{
		if (source->download_fileName)
			free(source->download_fileName);
		int k = strlen(download_filename)+1;
		source->download_fileName = (char*)malloc(k);
		memcpy(source->download_fileName,download_filename,k);
	}
	printf("Added: %s, Version %s       %s%s%s\n       %s\n       %s\n",name,ctime((time_t*)&(version)),loc->url,url_addition,filename,description,image_url);
	description[0] = 0;
	long_description[0] = 0;
	url_addition[0] = 0;
	image_url[0] = 0;
	download_filename[0] = 0;
}

void update_repositories()
{
	char buffer[65536];
	download_new_repositories();
	pLocation location = locationList;
	while (location)
	{
		if (location->kind != 2)
		{
			location = location->next;
			continue;
		}
		sprintf(buffer,"Updating %s...",location->name);
		info(buffer,1);
		char buffer2[1024];
		sprintf(buffer,"%s/%s %i",get_path(buffer2,"scripts"),location->update_call,TIMEOUT);
		FILE *fp = popen(buffer, "r");
		if (fp == NULL)
		{
			printf("Failed to run update for %s\n",location->name);
			location = location->next;
			continue;
		}
		/* Read the output a line at a time - output it. */
		char name[256] = "";
		char filename[256] = "";
		char download_filename[256] = "";
		char url_addition[256] = "";
		char description[1024] = "";
		char long_description[65536] = "";
		char image_url[1024] = "";
		Sint64 version = 0;
		while (fgets(buffer, 65536, fp) != NULL)
		{
			if (buffer[0] == '\n') //line break, skip
				continue;
			int l = strlen(buffer);
			buffer[l-1] = 0;
			l--;
			if (buffer[0] == '[') //new entry
			{
				if (name[0]) //Adding old entry
					add_new_web(name,filename,url_addition,description,long_description,version,location,image_url,download_filename);
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
				char value[65536];
				sprintf(value,"%s",middle_character);
				if (strcmp(key,"version") == 0)
					version = atoi(value);
				else
				if (strcmp(key,"description") == 0)
					sprintf(description,"%s",value);
				else
				if (strcmp(key,"long_description") == 0)
				{
					int i;
					int j = 0;
					for (i = 0; value[i]!=0; i++)
					{
						if (value[i] == '\\' && value[i+1] == 'n')
						{
							j++;
							i++;
							long_description[i-j] = '\n';
						}
						else
							long_description[i-j] = value[i];
					}
					long_description[i-j] = 0;
				}
				else
				if (strcmp(key,"url_addition") == 0)
					sprintf(url_addition,"%s",value);
				else
				if (strcmp(key,"filename") == 0)
					sprintf(filename,"%s",value);
				else
				if (strcmp(key,"download_filename") == 0)
					sprintf(download_filename,"%s",value);
				else
				if (strcmp(key,"image_url") == 0)
					sprintf(image_url,"%s",value);
			}
		}
		if (name[0]) //Adding last entry
			add_new_web(name,filename,url_addition,description,long_description,version,location,image_url,download_filename);
		pclose(fp);		
		location = location->next;
	}
}
