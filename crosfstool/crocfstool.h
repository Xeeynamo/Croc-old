//////////////////////////////////////////////////////////////////////////
// crocfstool.h
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

#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <direct.h>
#define MAX_PATH 260

enum MSGERRORTYPE
{
	NOERRORS		=  0,
	FILENOTFOUND	= -1,
	FILECORRUPTED	= -2,
	FILENOTCREATED	= -3,
	MEMORYPROTECTION= -254,
	GENERALERROR	= -255
};
enum ENDIAN
{
	ENOTSET = 0,
	ELITTLE = 1,
	EBIG	= 2
};
enum OPENFILETYPE
{
	OPENFILEREAD		= 1,
	OPENFILEREADWRITE	= 2,
	OPENFILECREATE		= 3,
	OPENFILECREATEREAD	= 4
};
struct CROCFILE
{
	char name[0x0C];
	unsigned int size;
	unsigned int pos;
	unsigned int dummy;
};

bool OpenFile(char *filename, FILE **f, OPENFILETYPE type);
int GetFileSize(FILE *f);
void EndianSwap(unsigned int *v);
MSGERRORTYPE CrocUnpack(char *dir, char *fs, char *outputpath, char *list, ENDIAN endian, bool verbose);
MSGERRORTYPE CrocPack(char *dir, char *fs, char *outputpath, char *list, ENDIAN endian, bool align, bool verbose);