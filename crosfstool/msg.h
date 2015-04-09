//////////////////////////////////////////////////////////////////////////
// msg.h
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

#ifndef MSG_H
#define MSG_H_

enum MsgLevel
{
	MsgLv_None = 0, // stdout
	MsgLv_Error = 1, // stderr
	MsgLv_Warning = 2, // stderr
	MsgLv_Info = 3, // stderr
	MsgLv_Message = 4, // stdout
};

enum MsgLevel msgGetLevel();
void msgSetLevel(enum MsgLevel level);
void msgPrint(enum MsgLevel, const char *format, ...);

#endif