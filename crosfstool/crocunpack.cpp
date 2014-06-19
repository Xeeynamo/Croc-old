//////////////////////////////////////////////////////////////////////////
// crocunpack.cpp
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

MSGERRORTYPE CrocUnpack(char *dirname, char *fsname, char *outputpath, char *listname, ENDIAN endian, bool verbose)
{
	FILE *fDir, *fArc, *fList, *fOut;
	if (!OpenFile(dirname, &fDir, OPENFILEREAD)) return FILENOTFOUND;
	if (!OpenFile(fsname , &fArc, OPENFILEREAD)) return FILENOTFOUND;

	int fSize = GetFileSize(fDir);
	if (fSize < 4) return FILECORRUPTED;
	unsigned int entries;
	fread(&entries, 1, sizeof(entries), fDir);

	if (endian == ENOTSET)
	{
		if (entries > 0xFFFFFF) endian = EBIG;
	}

	if (endian == EBIG) EndianSwap(&entries);
	if (fSize != (sizeof(CROCFILE)*entries + 8))
	{
		printf("%s seems to be corrupted. Be sure to select the right endian.\n"
			"Remember that the PSX uses Little Endian and Saturn uses Big Endian.\n", dirname);
		return FILECORRUPTED;
	}

	CROCFILE *crocfs = new CROCFILE[entries];
	fread(crocfs, entries, sizeof(CROCFILE), fDir);

	if (!OpenFile(listname, &fList, OPENFILECREATE)) return FILENOTCREATED;

	void *dtmp;
	char outpath[0x260];
	char tmp[0xD];
	tmp[0xC] = '\0';
	_mkdir(outputpath);
	if (endian == EBIG)
	{
		for(unsigned int i=0; i<entries; i++)
		{
			EndianSwap(&crocfs[i].pos);
			EndianSwap(&crocfs[i].size);
		}
	}
	for(unsigned int i=0; i<entries; i++)
	{
		memcpy(tmp, crocfs[i].name, 0xC);
		if (verbose) printf("Unpacking %s... ", tmp);
		sprintf(outpath, "%s\\%s", outputpath, tmp);
		fOut = fopen(outpath, "wb");
		if (!fOut)
		{
			if (verbose) printf("ERROR!");
			else printf("Unable to %s %s", "create", outpath);
			return GENERALERROR;
		}
		if (crocfs[i].size > 0x7FFFFF)
		{
			printf("!!! MEMORY PROTECTION WARNING !!!\n");
			printf("%s process froze because the program started to alloc %i MB of RAM.\n"
				"This can happen if the program use a wrong endian or if the input file is damaged.\n"
				"Check if there is something' wrong and run %s again.", "crocfstool", crocfs[i].size, "crocfstool");
			return MEMORYPROTECTION;
		}
		dtmp = malloc(crocfs[i].size);
		fseek(fArc, crocfs[i].pos, SEEK_SET);
		fread(dtmp, 1, crocfs[i].size, fArc);
		fwrite(dtmp, 1, crocfs[i].size, fOut);
		free(dtmp);
		fclose(fOut);
		if (verbose) printf("OK!\n");
		fprintf(fList, "%s\n", tmp);
	}
	fclose(fList);
	delete crocfs;
	return NOERRORS;
}