/*
 * Copyright 2013, Jeffery Qiu. All rights reserved.
 *
 * Licensed under the GNU LESSER GENERAL PUBLIC LICENSE(the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.gnu.org/licenses/lgpl.html
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//// Author: Jeffery Qiu (dungeonsnd at gmail dot com)
////

#ifndef _HEADER_FILE_CFD_CF_TYPES_DEFINATION_H_
#define _HEADER_FILE_CFD_CF_TYPES_DEFINATION_H_


#ifndef cf_const
#define cf_const const
#endif // cf_const


typedef void cf_void;

typedef cf_void * cf_pvoid;
typedef cf_void ** cf_ppvoid;
typedef cf_const cf_void * cf_cpvoid;
typedef cf_void * cf_const cf_pcvoid;
typedef cf_void * cf_const * cf_pcpvoid;


typedef char cf_char;
typedef unsigned char cf_byte;
typedef unsigned char cf_ubyte;
typedef signed char cf_sbyte;
typedef signed char cf_int8;
typedef unsigned char cf_uint8;

typedef cf_const cf_char * cf_cpstr;
typedef cf_char * cf_const cf_pcstr;


typedef short int cf_short;
typedef unsigned short int cf_ushort;
typedef short int cf_int16;
typedef unsigned short int cf_uint16;


typedef int cf_int;
typedef unsigned int cf_uint;
typedef int cf_int32;
typedef unsigned int cf_uint32;

typedef long int cf_long;
typedef unsigned long int cf_ulong;

typedef long long cf_llong;
typedef unsigned long long cf_ullong;
typedef long long cf_int64;
typedef unsigned long long cf_uint64;

#if __WORDSIZE==64  // 64Bit platform.
typedef cf_int64 cf_intptr;
typedef cf_uint64 cf_uintptr;
#elif __WORDSIZE==32 // 32Bit
typedef cf_int32 cf_intptr;
typedef cf_uint32 cf_uintptr;
#else // 16Bit
typedef cf_int16 cf_intptr;
typedef cf_uint16 cf_uintptr;
#endif

#ifndef NULL
#ifdef __cplusplus
#if __WORDSIZE==64
#define NULL                0x0000000000000000
#else
#define NULL                0x00000000
#endif
#else
#if __WORDSIZE==64
#define NULL                #define NULL ((void *)0x0000000000000000)
#else
#define NULL                ((void *)0x00000000)
#endif
#endif // __cplusplus
#endif // NULL

#ifndef ISNULL
#define ISNULL(p) (NULL==(void *)p)
#endif // ISNULL

#ifndef TRUE
#define TRUE 1
#endif // TRUE
#ifndef FALSE
#define FALSE 0
#endif // FALSE

#ifndef LOWORD
#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#endif // LOWORD
#ifndef LOBYTE
#define LOBYTE(w)           ((cf_byte)(w))
#define HIBYTE(w)           ((cf_byte)(((WORD)(w) >> 8) & 0xFF))
#endif // LOBYTE


typedef cf_int cf_fd;
typedef cf_uint32 cf_ev;

#endif // _HEADER_FILE_CFD_CF_TYPES_DEFINATION_H_
