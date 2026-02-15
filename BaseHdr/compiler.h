/**
* BSD 2-Clause License
*
* Copyright (c) 2022-2024, Manas Kamal Choudhury
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
*    list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
**/

#ifndef _COMPILER_H
#define _COMPILER_H

// Compiler detection
#if defined(_MSC_VER)
    #define COMPILER_MSVC 1
#elif defined(__GNUC__)
    #define COMPILER_GCC 1
#elif defined(__clang__)
    #define COMPILER_CLANG 1
#endif

// Alignment macros
#ifdef COMPILER_MSVC
    #define ALIGN(n) __declspec(align(n))
#else
    #define ALIGN(n) __attribute__((aligned(n)))
#endif

// Packed structure macros
#ifdef COMPILER_MSVC
    #define PACKED_START __pragma(pack(push, 1))
    #define PACKED_END __pragma(pack(pop))
    #define PACKED_STRUCT(name) PACKED_START struct name
    #define PACKED_STRUCT_T(name) PACKED_START typedef struct name
#else
    #define PACKED_START
    #define PACKED_END
    #define PACKED_STRUCT(name) struct __attribute__((packed)) name
    #define PACKED_STRUCT_T(name) typedef struct __attribute__((packed)) name
#endif

// Calling convention (compatible on x86_64)
#ifdef COMPILER_MSVC
    #define CDECL __cdecl
#else
    #define CDECL
#endif

// Inline assembly
#ifdef COMPILER_MSVC
    #define ASM __asm
#else
    #define ASM __asm__ __volatile__
#endif

// Function attributes
#ifdef COMPILER_MSVC
    #define NORETURN __declspec(noreturn)
    #define FORCEINLINE __forceinline
    #define NAKED __declspec(naked)
#else
    #define NORETURN __attribute__((noreturn))
    #define FORCEINLINE __attribute__((always_inline)) inline
    #define NAKED __attribute__((naked)) 
#endif

#endif // _COMPILER_H
