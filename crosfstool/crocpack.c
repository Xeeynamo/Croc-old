//////////////////////////////////////////////////////////////////////////
// crocpack.c
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

enum Result CrocPack(const char *dirname, const char *fsname, const char *outputpath, const char *list, enum ENDIAN endian, bool align)
{
	FILE *fDir, *fArc, *fList;
	if (!OpenFile(&fDir, dirname, OPENFILECREATE))
		return FILENOTCREATED;
	if (!OpenFile(&fArc, fsname, OPENFILECREATE))
		return FILENOTCREATED;
	if (!OpenFile(&fList, list, OPENFILEREAD))
		return FILENOTFOUND;

	if (endian == ENOTSET)
	{
		msgPrint(MsgLv_Warning, "Endian not specified.\n");
		msgPrint(MsgLv_Info, "Endian set to %s.\n", "LITTLE (PS1 ver.)");
		endian = ELITTLE;
	}


	unsigned int entries = 0;
	fseek(fDir, 4, SEEK_SET);
	unsigned int pos = 0;
	char name[0x0D];
	char path[MAX_PATH];
	struct CrocFsEntry CrocFsEntry;
	while(fscanf(fList, "%s", name) != EOF)
	{
		int i;
		FILE *f;

		sprintf(path, "%s\\%s", outputpath, name);
		msgPrint(MsgLv_Message, "Packing %s\n", name);
		if (!OpenFile(&f, name, OPENFILECREATE))
			return FILENOTCREATED;

		for (i = 0; i < 0xC && name[i] != '\0'; i++)
			CrocFsEntry.name[i] = name[i];
		for(; i < 0xC; i++)
			CrocFsEntry.name[i] = 0;
		CrocFsEntry.size = GetFileSize(f);
		CrocFsEntry.pos = pos;
		CrocFsEntry.dummy = 0;

		void *data = malloc(CrocFsEntry.size);
		fread(data, 1, CrocFsEntry.size, f);
		fclose(f);
		fwrite(data, 1, CrocFsEntry.size, fArc);
		free(data);

		if (align)
		{
			pos += (CrocFsEntry.size & 0x7FF) != 0 ? CrocFsEntry.size + (0x800 - (CrocFsEntry.size%0x800)) : CrocFsEntry.size;
			fseek(fArc, pos, SEEK_SET);
		}
		else
			pos += CrocFsEntry.size;

		if (endian == EBIG)
		{
			CrocFsEntry.size = EndianSwap(CrocFsEntry.size);
			CrocFsEntry.size = EndianSwap(CrocFsEntry.pos);
		}
		fwrite(&CrocFsEntry, 1, sizeof(struct CrocFsEntry), fDir);
		entries++;
	}
	unsigned int unknowvalue = 0;
	fclose(fList);
	fclose(fArc);
	if (endian == EBIG)
		entries = EndianSwap(entries);
	fwrite(&unknowvalue, 1, sizeof(unknowvalue), fDir);
	fseek(fDir, 0, SEEK_SET);
	fwrite(&entries, 1, sizeof(entries), fDir);
	fclose(fDir);
	return NOERRORS;
}