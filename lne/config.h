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

#ifndef LNE_CONFIG_H
#define LNE_CONFIG_H

#if !defined(__cplusplus)
#error C++ compiler required.
#endif

#define LNE_NAMESPACE_USING using namespace lne;
#define LNE_NAMESPACE_BEGIN namespace lne {
#define LNE_NAMESPACE_END   };

typedef int                LNE_INT;
typedef unsigned int       LNE_UINT;
typedef char               LNE_INT8;
typedef unsigned char      LNE_UINT8;
typedef short              LNE_INT16;
typedef unsigned short     LNE_UINT16;
typedef int                LNE_INT32;
typedef unsigned int       LNE_UINT32;
typedef long long          LNE_INT64;
typedef unsigned long long LNE_UINT64;

#define LNERR_OK                 0x00000000
#define LNERR_UNKNOW             0x80000001
#define LNERR_PARAMETER          0x80000002
#define LNERR_NOINIT             0x80000003
#define LNERR_TIMEOUT            0x80000004
#define LNERR_REENTRY            0x80000005
#define LNERR_NOMEMORY           0x80000006
#define LNERR_NOSUPPORT          0x80000007
#define LNERR_IOCLOSED           0x80000008
#define LNERR_NODATA             0x80000009

#define LNE_SUCCESSED(f) \
	((f) < 0x80000000)
#define LNE_FAILED(f) \
	((f) > 0x80000000)

#include "config-os.h"

#if defined(LNE_WIN32)
#include "config-win32.h"
#elif defined(LNE_LINUX)
#include "config-linux.h"
#elif defined(LNE_FREEBSD)
#include "config-freebsd.h"
#else
#error not support this platform
#endif

#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(NDEBUG)
#define LNE_ASSERT_IF(f) \
	if(!(f))
#define LNE_ASSERT_RETURN(f,t) \
	if(!(f)) return t
#define LNE_ASSERT_RETURN_VOID(f) \
	if(!(f)) return
#else
#include <assert.h>
#define LNE_ASSERT_IF(f) \
	assert(f);
#define LNE_ASSERT_RETURN(f,t) \
	assert(f)
#define LNE_ASSERT_RETURN_VOID(f) \
	assert(f)
#endif

#endif
