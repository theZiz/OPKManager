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

void add_new_source(pOpkList file,int kind,char* url,Sint64 version)
{
	pSourceList newSource = (pSourceList)malloc(sizeof(tSourcelist));
	newSource->version = version;
	newSource->location = kind;
	if (url)
	{
		int l = strlen(url)+1;
		newSource->url = (char*)malloc(l);
		memcpy(newSource->url,url,l);
	}
	else
		url = NULL;
	newSource->next = file->sources;
	file->sources = newSource;
}

void add_new_file(char* filename,int kind,char* url,Sint64 version)
{
	pOpkList newOpk = (pOpkList)malloc(sizeof(tOpkList));
	sprintf(newOpk->fileName,"%s",filename);
	sprintf(newOpk->longName,"%s",filename);
	int l = strlen(newOpk->longName);
	if (l > 4)
		newOpk->longName[l-4] = 0;
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
	add_new_source(newOpk,kind,url,version);
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

void add_file_to_opkList(char* filename,int kind,Sint64 version)
{
	//Searching in the opkList
	pOpkList file = opkList;
	while (file)
	{
		if (strcmp(file->fileName,filename) == 0)
			break;
		file = file->next;
	}
	if (file) //found
		add_new_source(file,kind,NULL,version);
	else
		add_new_file(filename,kind,NULL,version);
}

void merge_fileList_to_opkList(spFileListPointer fileList,int kind)
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
		add_file_to_opkList(filename,kind,file->last_mod);
		file = file->next;
	}
}
