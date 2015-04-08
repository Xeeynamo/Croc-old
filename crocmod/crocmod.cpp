//////////////////////////////////////////////////////////////////////////
// crocmod/crocmod.cpp
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

#include <cstdio>
#include <cassert>
#include <cstdint>
#include <cstring>
#include "mod.h"

bool Extract(const char *filein, const char* objname, const char *fileoutObj, const char *fileoutMtl)
{
	if (filein == NULL || fileoutObj == NULL || fileoutMtl == NULL)
		return false;

	Croc::Mod mod;
	if (!mod.Open(filein))
		return false;

	FILE *file = fopen(fileoutObj, "w");
	FILE *fileMtl = fopen(fileoutMtl, "w");
	if (file == NULL || fileMtl == NULL)
		return false;

	int vbase = 0, ibase = 0;
	fprintf(file, "mtllib ./fileoutMtl\n");
	for (size_t j = 0; j < mod.GetMeshCount(); j++)
	{
		fprintf(file, "o %s_%i\n", objname, j);
		mod.SetCurrentMesh(j);

		for (size_t i = 0; i < mod.GetVerticesCount(); i++)
		{
			Croc::Vertexf3 v;
			mod.GetVertex(v, i);
			fprintf(file, "v %f %f %f\n", v.x, v.y, v.z);
		}
		for (size_t i = 0; i < mod.GetVerticesCount(); i++)
		{
			Croc::Vertexf3 v;
			mod.GetNormal(v, i);
			fprintf(file, "vn %f %f %f\n", v.x, v.y, v.z);
		}
		for (size_t i = 0; i < mod.GetFacesCount(); i++)
		{
			Croc::Vertexf3 v;
			mod.GetColor(v, i);
			fprintf(fileMtl, "newmtl mtl%i\n", i + ibase);
			fprintf(fileMtl, "Kd %f %f %f\n\n", v.x, v.y, v.z);
		}
		for (size_t i = 0; i < mod.GetFacesCount(); i++)
		{
			int v[4];
			v[0] = mod.GetIndex(i, 0) + 1 + vbase;
			v[1] = mod.GetIndex(i, 1) + 1 + vbase;
			v[2] = mod.GetIndex(i, 2) + 1 + vbase;
			v[3] = mod.GetIndex(i, 3) + 1 + vbase;
			fprintf(file, "usemtl mtl%i\n", i + ibase);
			fprintf(file, "f %i %i %i\n", v[0], v[1], v[2]);
			fprintf(file, "f %i %i %i\n", v[1], v[2], v[3]);
		}

		vbase += mod.GetVerticesCount();
		ibase += mod.GetFacesCount();
	}
	fclose(fileMtl);
	fclose(file);
	return true;
}

const int MAX_PATH = 260;
bool Extract(const char *filein, const char *fileout)
{
	if (filein == NULL || fileout == NULL)
		return false;

	char outObj[MAX_PATH];
	char outMtl[MAX_PATH];
	int len = strlen(fileout);
	if (len > MAX_PATH + 6)
		len = MAX_PATH - 6;
	memcpy(outObj, fileout, len);
	memcpy(outObj + len, ".obj", 5);
	memcpy(outMtl, fileout, len);
	memcpy(outMtl + len, ".mtl", 5);

	return Extract(filein, fileout, outObj, outMtl);
}
bool Extract(const char *filein)
{
	if (filein == NULL)
		return false;
	char outName[MAX_PATH];
	int len = strlen(filein);
	if (len >= MAX_PATH)
		len = MAX_PATH - 1;
	strcpy(outName, filein);
	while (--len >= 0)
	{
		if (outName[len] == '.')
		{
			outName[len] = '\0';
			return Extract(filein, outName);
		}
	}
	return false;
}


void PrintUsage()
{
	const char *APP_NAME = "crocmod";

	printf("%s - MOD (3d models) tool for Croc: The leggend of Gobbos\n\n", APP_NAME);
	printf("Usage: %s model.mod [-e [fileout]]\n", APP_NAME);
	printf("-e\textract informations in Wavefront OBJ file and MTL material\n");
}

int crocmod(int argc, char *argv[])
{
	if (argc < 3)
		return -1;
	int err = 0;
	int mode = 0;
	const char *strOutFilename = NULL;
	for (int i = 2; i < argc; i++)
	{
		if (*argv[i] != '-')
			return -2;
		switch (argv[i][1])
		{
		case 'e':
			if (mode != 0)
				return -2;
			if (i + 1 < argc && argv[i + 1][0] != '-')
				strOutFilename = argv[i + 1];
			mode = 1;
			break;
		}
	}
	switch (mode)
	{
	case 0:
		return -2;
	case 1:
		if (strOutFilename)
			err = Extract(argv[1], strOutFilename);
		else
			err = Extract(argv[1]);
		if (err == false)
			fprintf(stderr, "Generic error.\n");
		err = 0;
		break;
	}
	return err;
}

int main(int argc, char *argv[])
{
	int res;
	if (res = crocmod(argc, argv) < 0)
		PrintUsage();
	return res;
}