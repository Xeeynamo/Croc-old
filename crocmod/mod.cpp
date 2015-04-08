//////////////////////////////////////////////////////////////////////////
// crocmod/mod.cpp
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

#include "mod.h"
#include <cassert>

#define FLOAT_FROM_SHORT(i) ((float)(i) / 32767.0f)
#define FLOAT_FROM_BYTE(i) ((float)(i) / 255.0f)

Croc::Mod::Mod()
{
	m_mesh = NULL;
}
Croc::Mod::~Mod()
{
	Close();
}
bool Croc::Mod::Open(const char *filename)
{
	bool ret = false;
	Close();
	FILE *file = fopen(filename, "rb");
	if (file != NULL)
	{
		ret = SubOpen(file);
		fclose(file);
	}
	return ret;
}
void Croc::Mod::Close()
{
	if (m_mesh)
	{
		delete[] m_mesh;
		m_mesh = NULL;
	}
}
bool Croc::Mod::SubOpen(FILE *file)
{
	assert(file != NULL);
	fread(&m_count, 1, 4, file);
	m_mesh = new Mesh[m_count];

	for (size_t i = 0; i < m_count; i++)
	{
		Mesh& mesh = m_mesh[i];
		fread(&mesh.radius, 4, 1, file);
		fread(&mesh.boundbox, sizeof(BoundBox), 1, file);

		fread(&mesh.verticesCount, 4, 1, file);
		mesh.vertices = new Vertex[mesh.verticesCount];
		mesh.normals = new Vertex[mesh.verticesCount];
		fread(mesh.vertices, sizeof(Vertex), mesh.verticesCount, file);
		fread(mesh.normals, sizeof(Vertex), mesh.verticesCount, file);

		fread(&mesh.facesCount, 4, 1, file);
		mesh.faces = new Face[mesh.facesCount];
		fread(mesh.faces, sizeof(Face), mesh.facesCount, file);
	}
	return true;
}

size_t Croc::Mod::GetMeshCount() const
{
	return m_count;
}
size_t Croc::Mod::GetCurrentMesh() const
{
	return m_curmesh;
}
void Croc::Mod::SetCurrentMesh(size_t meshIndex)
{
	m_curmesh = meshIndex;
}
size_t Croc::Mod::GetVerticesCount() const
{
	return m_mesh[m_curmesh].verticesCount;
}
void Croc::Mod::GetVertex(Vertexf3& v, size_t index) const
{
	Vertex& vv = m_mesh[m_curmesh].vertices[index];
	v.x = FLOAT_FROM_SHORT(vv.x);
	v.y = FLOAT_FROM_SHORT(vv.y);
	v.z = FLOAT_FROM_SHORT(vv.z);
}

void Croc::Mod::GetNormal(Vertexf3& v, size_t index) const
{
	Vertex& vv = m_mesh[m_curmesh].normals[index];
	v.x = FLOAT_FROM_SHORT(vv.x);
	v.y = FLOAT_FROM_SHORT(vv.y);
	v.z = FLOAT_FROM_SHORT(vv.z);
}

size_t Croc::Mod::GetFacesCount() const
{
	return m_mesh[m_curmesh].facesCount;
}
void Croc::Mod::GetColor(Vertexf3& v, size_t index) const
{
	const Face& face = m_mesh[m_curmesh].faces[index];
	if (face.format & 1)
	{
		// USE TEXTURE
		v.x = 1.0f;
		v.y = 1.0f;
		v.z = 1.0f;
	}
	else
	{
		// USE DIFFUSE
		v.x = FLOAT_FROM_BYTE(face.r);
		v.y = FLOAT_FROM_BYTE(face.g);
		v.z = FLOAT_FROM_BYTE(face.b);
	}
}
short Croc::Mod::GetIndex(size_t index, size_t pos) const
{
	return m_mesh[m_curmesh].faces[index].vindex[pos];
}
