//////////////////////////////////////////////////////////////////////////
// crocpack.cpp
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

MSGERRORTYPE CrocPack(char *dirname, char *fsname, char *outputpath, char *list, ENDIAN endian, bool align, bool verbose)
{
	FILE *fDir, *fArc, *fList;
	if (!OpenFile(dirname,	&fDir, OPENFILECREATE)) return FILENOTCREATED;
	if (!OpenFile(fsname,	&fArc, OPENFILECREATE)) return FILENOTCREATED;
	if (!OpenFile(list,		&fList, OPENFILEREAD)) return FILENOTFOUND;

	unsigned int entries = 0;
	fseek(fDir, 4, SEEK_SET);
	unsigned int pos = 0;
	char name[0x0D];
	char path[MAX_PATH];
	CROCFILE crocfile;
	while(fscanf(fList, "%s", name) != EOF)
	{
		sprintf(path, "%s\\%s", outputpath, name);
		if (verbose) printf("Writing %s... ", name);
		FILE *f = fopen(path, "rb");
		if (!f)
		{
			if (verbose) printf("ERROR!");
			else printf("Unable to %s %s", "open", path);
			return GENERALERROR;
		}
		int i;
		for(i=0; i<0xC; i++)
			if (name[i])
				crocfile.name[i] = name[i];
			else
				break;
		for(; i<0xC; i++)
			crocfile.name[i] = 0;
		crocfile.size = GetFileSize(f);
		crocfile.pos = pos;
		crocfile.dummy = 0;

		void *data = malloc(crocfile.size);
		fread(data, 1, crocfile.size, f);
		fclose(f);
		fwrite(data, 1, crocfile.size, fArc);
		free(data);
		if (verbose) printf("OK!\n");

		if (align)
		{
			pos += (crocfile.size & 0x7FF) != 0 ? crocfile.size + (0x800 - (crocfile.size%0x800)) : crocfile.size;
			fseek(fArc, pos, SEEK_SET);
		}
		else pos += crocfile.size;

		if (endian == EBIG)
		{
			EndianSwap(&crocfile.size);
			EndianSwap(&crocfile.pos);
		}
		fwrite(&crocfile, 1, sizeof(CROCFILE), fDir);
		entries++;
	}
	unsigned int unknowvalue = 0;
	fclose(fList);
	fclose(fArc);
	if (endian == EBIG) EndianSwap(&entries);
	fwrite(&unknowvalue, 1, sizeof(unknowvalue), fDir);
	fseek(fDir, 0, SEEK_SET);
	fwrite(&entries, 1, sizeof(entries), fDir);
	fclose(fDir);
	return NOERRORS;
}