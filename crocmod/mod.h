//////////////////////////////////////////////////////////////////////////
// crocmod/mod.h
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

#pragma once
#include <cstdio>
#include <cstdint>
#include <cstring>

namespace Croc
{
	struct Vertexf3
	{
		float x, y, z;
	};

	//! \brief manage Croc's MOD files
	/** \details contains 3D models. Every MOD can contains multiple meshes.
	 * For every mesh, there are two sections: vertices and faces. Vertices
	 * contains vertex position and normals, faces describes the indices
	 * of vertices to draw, colors and textures.
	 */
	class Mod
	{
	public:
		Mod();
		~Mod();

		bool Open(const char *filename);
		void Close();

		size_t GetMeshCount() const;
		size_t GetCurrentMesh() const;
		void SetCurrentMesh(size_t meshIndex);

		//! \brief get how vertices are present
		//! \return vertices count
		size_t GetVerticesCount() const;
		//! \brief get vertex position
		//! \param[out] v vertex that will contains position
		//! \param[in] index between 0 and GetVerticesCount() - 1
		void GetVertex(Vertexf3& v, size_t index) const;
		//! \brief get normals
		//! \param[out] v vertex that will contains normals
		//! \param[in] index between 0 and GetVerticesCount() - 1
		void GetNormal(Vertexf3& v, size_t index) const;

		//! \brief get how faces are present
		//! \return faces count
		size_t GetFacesCount() const;
		//! \brief get color used from a single face
		//! \param[out] v read normalized colors
		//! \param[in] index between ' and GetFacesCount() - 1
		//! \details when a texture is defined, v will be an identity.
		void GetColor(Vertexf3& v, size_t index) const;
		short GetIndex(size_t index, size_t pos) const;

	private:
		struct Point
		{
			short x, y, z;
		};
		struct BoundBox
		{
			Point v[12];
		};
		struct Vertex
		{
			short x, y, z, padding;
		};
		struct Face
		{
			short unk[4];
			short vindex[4];
			union
			{
				short texture;
				struct
				{
					unsigned char r, g, b;
				};
			};
			// 8 = 4v, use colors
			// 9 = 4v, use texture
			unsigned char format : 4;
			unsigned char flip : 1;
			unsigned char light : 2;
		};
		struct Mesh
		{
			uint32_t radius;
			uint32_t verticesCount;
			uint32_t facesCount;
			BoundBox boundbox;
			Vertex *vertices;
			Vertex *normals;
			Face *faces;

			Mesh() : radius(0), verticesCount(0), facesCount(0),
				vertices(NULL), normals(NULL), faces(NULL)
			{
				memset(&boundbox, 0, sizeof(BoundBox));
			}
			~Mesh()
			{
				if (vertices)
					delete[] vertices;
				if (normals)
					delete[] normals;
				if (faces)
					delete[] faces;
			}
		};

		uint32_t m_count;
		Mesh *m_mesh;
		uint32_t m_curmesh;

		bool SubOpen(FILE *file);

	};
}