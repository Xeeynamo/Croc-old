//////////////////////////////////////////////////////////////////////////
// crocunpack.c
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

enum Result CrocUnpack(const char *dirname, const char *fsname, const char *outputpath, const char *listname, enum ENDIAN endian)
{
	FILE *fDir, *fArc, *fList, *fOut;
	if (!OpenFile(&fDir, dirname, OPENFILEREAD))
		return FILENOTFOUND;
	if (!OpenFile(&fArc, fsname, OPENFILEREAD))
		return FILENOTFOUND;

	size_t fSize = GetFileSize(fDir);
	if (fSize < 4 || EndianSwap(fSize) < 4)
		return FILECORRUPTED;
	unsigned int entries;
	fread(&entries, 1, sizeof(entries), fDir);

	if (endian == ENOTSET)
	{
		msgPrint(MsgLv_Warning, "Endian not specified.\n");
		if (entries > 0x7FFFF)
		{
			msgPrint(MsgLv_Info, "Endian set to %s.\n", "BIG (Saturn ver.)");
			endian = EBIG;
		}
		else
		{
			msgPrint(MsgLv_Info, "Endian set to %s.\n", "LITTLE (PS1 ver.)");
			endian = ELITTLE;
		}
	}

	if (endian == EBIG)
		entries = EndianSwap(entries);
	size_t expectedSize = sizeof(struct CrocFsEntry) * entries + 8;
	if (fSize != expectedSize)
	{
		if (fSize != entries + 8)
		{
			msgPrint(MsgLv_Error, "%s seems to be corrupted.");
			return FILECORRUPTED;
		}
		else
		{
			msgPrint(MsgLv_Info, "Found Croc v0.12 format.\n");
			entries /= sizeof(struct CrocFsEntry);
			msgPrint(MsgLv_Warning, "Format not yet supported. Extraction aborted.\n");
			return FILECORRUPTED;
		}
	}

	if (!OpenFile(&fList, listname, OPENFILECREATE))
		return FILENOTCREATED;

	char outpath[MAX_PATH];
	char tmp[MAX_PATH];
	tmp[0xC] = '\0';
	_mkdir(outputpath);

	size_t fsLen = GetFileSize(fArc);
	for(unsigned int i = 0; i < entries; i++)
	{
		struct CrocFsEntry entry;
		fread(&entry, sizeof(struct CrocFsEntry), 1, fDir);
		memcpy(tmp, entry.name, 0xC);
		msgPrint(MsgLv_Message, "Unpacking %s\n", tmp);
		sprintf(outpath, "%s\\%s", outputpath, tmp);

		if (endian == EBIG)
		{
			entry.pos = EndianSwap(entry.pos);
			entry.size = EndianSwap(entry.size);
		}
		if (entry.pos + entry.size > fsLen)
		{
			msgPrint(MsgLv_Warning, "%s position exceed file system's length; it will not be extracted.\n", tmp);
			continue;
		}

		if (!OpenFile(&fOut, outpath, OPENFILECREATE))
			return FILENOTCREATED;
		if (entry.size > 0x7FFFFF)
			msgPrint(MsgLv_Warning, "%s file size %iKB?\n", entry.size);
		fseek(fArc, entry.pos, SEEK_SET);
		FileCopy(fOut, fArc, entry.size);
		fclose(fOut);
		fprintf(fList, "%s\n", tmp);
	}
	fclose(fList);
	return NOERRORS;
}