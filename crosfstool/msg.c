//////////////////////////////////////////////////////////////////////////
// msg.c
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

#include "msg.h"
#include <stdio.h>
#include <stdarg.h>

enum MsgLevel _msgLevel = MsgLv_Warning;

enum MsgLevel msgGetLevel()
{
	return _msgLevel;
}
void msgSetLevel(enum MsgLevel level)
{
	_msgLevel = level;
}
void msgPrint(enum MsgLevel level, const char *format, ...)
{
	if (level <= _msgLevel)
	{
		va_list args;
		va_start(args, format);
		vfprintf(level == MsgLv_Message ? stdout : stderr, format, args);
		va_end(args);
	}
}