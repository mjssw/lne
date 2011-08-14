/*
 *  Copyright (C) 2011  Vietor Liu <vietor.liu@gmail.com>
 *
 *  This file is part of LNE.
 *  LNE is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Lesser Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  LNE is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with LNE.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LNE_CONFIG_WIN32_H
#define LNE_CONFIG_WIN32_H

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <time.h>
#include <sys/timeb.h>

#if (WINVER < 0x0600)
#error WINVER >= 0x0600 required.
#endif

#pragma warning(disable:4103)
#pragma warning(disable:4200)
#pragma warning(disable:4210)
#pragma warning(disable:4251)
#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32.lib")

#if LNE_EXPORTS
# define LNE_Export __declspec (dllexport)
#else
# define LNE_Export __declspec (dllimport)
#endif
#define LNE_INLINE __forceinline

typedef HANDLE POLLER;
const int INVALID_POLLER = NULL;
#define closepoller(f) CloseHandle(f)

typedef long suseconds_t;
typedef int ssize_t;
typedef int socklen_t;

struct timespec {
	time_t      tv_sec;        /* seconds */
	suseconds_t tv_nsec;       /* nanoseconds */
};

#endif

