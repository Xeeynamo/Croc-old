//////////////////////////////////////////////////////////////////////////
// crocfstool.c
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

bool OpenFile(FILE **f, const char *filename, enum OpenType type)
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
			msgPrint(MsgLv_Error, "Unable to %s %s", "open", filename);
		else if (type == OPENFILECREATE || type == OPENFILECREATEREAD)
			msgPrint(MsgLv_Error, "Unable to %s %s", "create", filename);
		return false;
	}
	return true;
}
size_t GetFileSize(FILE *f)
{
    if (!f) return 0;
	size_t prev = ftell(f);
    fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
    fseek(f, prev, SEEK_SET);
    return size;
}

char fileCopyBuffer[65536];
void FileCopy(FILE *dst, FILE *src, size_t length)
{
	while (length > 0)
	{
		size_t bytes = length > sizeof(fileCopyBuffer) ? sizeof(fileCopyBuffer) : length;
		length -= bytes;
		bytes = fread(fileCopyBuffer, 1, bytes, src);
		fwrite(fileCopyBuffer, 1, bytes, dst);
	}
}

bool isBigEndian = false;
const char *CORRUPTEDLBA = "Corrupted DIR\n";

void PrintUsage()
{
	const char *APP_NAME = "crocfstool";

	msgPrint(MsgLv_None, "%s - Pack or unpack the filesystem of Croc: The leggend of Gobbos\n\n", APP_NAME);
	msgPrint(MsgLv_None, "Usage: %s crocfile.dir crocfile.1 outputdir [-c list.txt] [-d list.txt] [-e b/l] [-a] [-q]\n", APP_NAME);
	msgPrint(MsgLv_None, "-c\tcreate a filesystem using a file list\n");
	msgPrint(MsgLv_None, "-d\tunpack a filesystem and make a file list\n");
	msgPrint(MsgLv_None, "-e\tforce the use b(ig endian) or l(ittle endian) [default -e l]\n");
	msgPrint(MsgLv_None, "-a\talign files to speedup loading times (compatible with -c)\n");
	msgPrint(MsgLv_None, "-q\tquiet mode\n");
}

int crocfstool(int argc, char *argv[])
{
	int mode = 0;
	bool align = false;
	bool verbose = false;
	char *listname = NULL;
	enum ENDIAN endian = ENOTSET;
	enum Result err;

	if (argc < 4)
		return -1;

	msgSetLevel(MsgLv_Info);
	for (int i = 4; i < argc; i++)
	{
		if (*argv[i] != '-')
			return -2;
		switch (argv[i][1])
		{
		case 'c':
			if (mode != 0)
			{
				msgPrint(MsgLv_Error, "Option -%c is incompatible with -%c\n\n", 'c', 'd');
				return -2;
			}
			listname = &argv[i++][3];
			mode = 1;
			break;
		case 'd':
			if (mode != 0)
			{
				msgPrint(MsgLv_Error, "Option -%c is incompatible with -%c\n\n", 'd', 'c');
				return -2;
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
				msgPrint(MsgLv_Error, "-e %c is an incompatible option\n\n", argv[3][3]);
				return -2;
			}
			i++;
			break;
		case 'a':
			align = true;
			break;
		case 'q':
			msgSetLevel(MsgLv_Error);
			break;
		case 'v':
			msgSetLevel(MsgLv_Message);
			break;
		}
	}
	switch (mode)
	{
	case 0:
		msgPrint(MsgLv_Error, "No mode selected. Please select -c or -d\n");
		return -2;
	case 1:
		err = CrocPack(argv[1], argv[2], argv[3], listname, endian, align);
		break;
	case 2:
		err = CrocUnpack(argv[1], argv[2], argv[3], listname, endian);
		break;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	assert(sizeof(struct CrocFsEntry) == 0x18);
	if (crocfstool(argc, argv) < 0)
		PrintUsage();
}