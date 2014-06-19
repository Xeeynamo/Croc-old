//////////////////////////////////////////////////////////////////////////
// crocfstool.cpp
// Part of https://github.com/Xeeynamo/Croc
// Copyright(C) 2014  Luciano Ciccariello (Xeeynamo)
// 
// This program is free software; you can redistribute it and / or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or(at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110 - 1301, USA.

#include "crocfstool.h"

bool OpenFile(char *filename, FILE **f, OPENFILETYPE type)
{
	char *mode;
	switch(type)
	{
	case OPENFILEREAD:
		mode = "rb";
		break;
	case OPENFILEREADWRITE:
		mode = "r+b";
	case OPENFILECREATE:
		mode = "wb";
		break;
	case OPENFILECREATEREAD:
		mode = "w+b";
		break;
	default:
		return false;
	}
	*f = fopen(filename, mode);
	if (!f)
	{
		if (type == OPENFILEREAD || type == OPENFILEREADWRITE)
			printf("Unable to %s %s", "open", filename);
		else if (type == OPENFILECREATE || type == OPENFILECREATEREAD)
			printf("Unable to %s %s", "create", filename);
		return false;
	}
	return true;
}
int GetFileSize(FILE *f)
{
    if (!f) return 0;
	int prev = ftell(f);
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, prev, SEEK_SET);
    return size;
}
void EndianSwap(unsigned int *v)
{
	*v = (*v>>24) | ((*v<<8) & 0x00FF0000) | ((*v>>8) & 0x0000FF00) | (*v<<24);
}

bool isBigEndian = false;
const char *CORRUPTEDLBA = "Corrupted DIR\n";

int main(int argc, char *argv[])
{
    if (sizeof(CROCFILE) != 0x18)
    {
        printf("Initialization fail: %X", 0x18 - sizeof(CROCFILE));
    }

	if (argc < 4)
	{
LOC_USAGE:
		printf("%s - Pack or unpack the filesystem of Croc: The leggend of Gobbos\n\n", "crocfstool");
		printf("Usage: crocfstool crocfile.dir crocfile.1 outputdir [-c list.txt] [-d list.txt] [-e b/l] [-a]\n");
		printf("-c\tcreate a filesystem using a file list\n");
		printf("-d\tunpack a filesystem and make a file list\n");
		printf("-e\tforce the use b(ig endian) or l(ittle endian) [default -e l]\n");
		printf("-a\talign the files to speedup the game (compatible with -c)\n");
		goto LOC_END;
	}
	int mode = 0;
	ENDIAN endian = ENOTSET;
	bool align = false;
	bool verbose = false;
	char *listname = NULL;
	for(int i=4; i<argc; i++)
	{
		if (*argv[i] != '-')
			goto LOC_USAGE;
		switch(argv[i][1])
		{
		case 'c':
			if (mode != 0)
			{
				printf("The option -%c is incompatible with -%c\n\n", 'c', 'd');
				goto LOC_USAGE;
			}
			listname = &argv[i++][3];
			mode = 1;
			break;
		case 'd':
			if (mode != 0)
			{
				printf("The option -%c is incompatible with -%c\n\n", 'd', 'c');
				goto LOC_USAGE;
			}
			listname = &argv[i++][3];
			mode = 2;
			break;
		case 'e':
			if (argv[i][3] == 'l')
				endian = ELITTLE;
			else if (argv[i][3] == 'b')
				endian = EBIG;
			else
			{
				printf("-e %c is an incompatible option\n\n", argv[3][3]);
				goto LOC_USAGE;
			}
			i++;
			break;
		case 'a':
			align = true;
			break;
		case 'v':
			verbose = true;
			break;
		}
	}
	MSGERRORTYPE err;
	switch(mode)
	{
	case 0:
		printf("No mode selected. Please select -c or -d\n");
		goto LOC_USAGE;
	case 1:
		err = CrocPack(argv[1], argv[2], argv[3], listname, endian, align, verbose);
		break;
	case 2:
		err = CrocUnpack(argv[1], argv[2], argv[3], listname, endian, verbose);
		break;
	}


LOC_END:
	printf("\nPress CTRL+C to exit");
    while(true) _sleep(1);
}