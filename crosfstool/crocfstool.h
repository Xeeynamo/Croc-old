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

#ifndef CROCFSTOOL_H
#define CROCFSTOOL_H_

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <direct.h>
#include "msg.h"
#define MAX_PATH 260

enum Result
{
	NOERRORS		=  0,
	FILENOTFOUND	= -1,
	FILECORRUPTED	= -2,
	FILENOTCREATED	= -3,
	MEMORYPROTECTION= -254,
	GENERALERROR	= -255
};
enum Endian
{
	ENOTSET = 0,
	ELITTLE = 1,
	EBIG	= 2
};
enum OpenType
{
	OPENFILEREAD		= 1,
	OPENFILEREADWRITE	= 2,
	OPENFILECREATE		= 3,
	OPENFILECREATEREAD	= 4
};
struct CrocFsEntry
{
	char name[0x0C];
	unsigned int size;
	unsigned int pos;
	unsigned int dummy;
};

#define EndianSwap(x) ((x >> 24) | ((x << 8) & 0x00FF0000) | ((x >> 8) & 0x0000FF00) | (x << 24))

bool OpenFile(FILE **f, const char *filename, enum OpenType type);
size_t GetFileSize(FILE *f);
void FileCopy(FILE *dst, FILE *src, size_t length);
enum Result CrocUnpack(const char *dir, const char *fs, const char *outputpath, const char *list, enum Endian Endian);
enum Result CrocPack(const char *dir, const char *fs, const char *outputpath, const char *list, enum Endian Endian, bool align);

#endif