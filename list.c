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
  
void read_locations()
{
	spFileListPointer directoryList = NULL;
	spFileGetDirectory(&directoryList,ROOT,0,1);
	int rlen = strlen(ROOT);
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
		if (strcmp(directory->name,ROOT"/sdcard") == 0)
		{
			sprintf(loc->name,"SD Card");
			loc->kind = 1;
		}
		else
		{
			sprintf(loc->name,"USB Device");			
			loc->kind = 3;
		}
		loc->url = (char*)malloc(strlen(directory->name)+7);
		sprintf(loc->url,"%s/apps/",directory->name);
		printf("Found %s\n",loc->url);
		loc->next = locationList;
		locationList = loc;
		directory = directory->next;
	}
	spFileDeleteList(directoryList);
}

void add_new_source(pOpkList file,pLocation location,char* filename,Sint64 version,char* description)
{
	pSourceList newSource = (pSourceList)malloc(sizeof(tSourcelist));
	newSource->version = version;
	newSource->location = location;
	if (filename)
	{
		int l = strlen(filename)+1;
		newSource->fileName = (char*)malloc(l);
		memcpy(newSource->fileName,filename,l);
	}
	else
		newSource->fileName = NULL;
	if (description)
	{
		int l = strlen(description)+1;
		newSource->description = (char*)malloc(l);
		memcpy(newSource->description,description,l);
		newSource->block = spCreateTextBlock(description,screen->w/2,font);
	}
	else
	{
		newSource->description = NULL;
		newSource->block = NULL;
	}
	newSource->next = file->sources;
	file->sources = newSource;
}

void add_new_file(char* longname,char* filename,pLocation location,Sint64 version,char* description)
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
	add_new_source(newOpk,location,filename,version,description);
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
}

void add_file_to_opkList(char* longname,char* filename,pLocation location,Sint64 version,char* description)
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
		add_new_source(file,location,filename,version,description);
	else
		add_new_file(longname,filename,location,version,description);
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
		spFileListPointer fileList = NULL;
		spFileGetDirectory(&fileList,loc->url,0,1);
		merge_fileList_to_opkList(fileList,loc);
		spFileDeleteList(fileList);
		loc = loc->next;
	}	
}
