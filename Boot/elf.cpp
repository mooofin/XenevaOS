/**
* BSD 2-Clause License
*
* Copyright (c) 2023-2025, Manas Kamal Choudhury
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

#include "elf.h"
#include "clib.h"
#include "paging.h"
#include "xnout.h"
#include "physm.h"

// Helper: memcpy
static void copy_mem(void* dst, void* src, size_t length) {
	uint8_t* dstp = (uint8_t*)dst;
	uint8_t* srcp = (uint8_t*)src;
	while (length--)
		*dstp++ = *srcp++;
}

// Helper: memset
static void zero_mem(void* dst, size_t length) {
	uint8_t* dstp = (uint8_t*)dst;
	while (length--)
		*dstp++ = 0;
}

/*
 * XELFLoadImage -- loads an ELF64 executable image to memory
 * @param filebuf -- file buffer
 */
void* XELFLoadImage(void* filebuf) {
    Elf64_Ehdr* hdr = (Elf64_Ehdr*)filebuf;

    // Verify ELF Magic
    if (hdr->e_ident[0] != 0x7F || hdr->e_ident[1] != 'E' ||
        hdr->e_ident[2] != 'L' || hdr->e_ident[3] != 'F') {
        XEGuiPrint("Invalid ELF Magic\n");
        return nullptr;
    }

    // Verify 64-bit class (1 = 32-bit, 2 = 64-bit)
    if (hdr->e_ident[4] != 2) {
        XEGuiPrint("Not a 64-bit ELF file\n");
        return nullptr;
    }

    Elf64_Phdr* phdr = (Elf64_Phdr*)((uint8_t*)filebuf + hdr->e_phoff);

    for (int i = 0; i < hdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            uint64_t vaddr = phdr[i].p_vaddr;
            uint64_t memsz = phdr[i].p_memsz;
            uint64_t filesz = phdr[i].p_filesz;
            uint64_t offset = phdr[i].p_offset;

            // Align memory allocation to page size (4KB)
            uint64_t num_pages = (memsz + 0xFFF) / 0x1000;
            
            // Map pages
            // Note: We use PADDR_T_MAX to let the allocator choose physical address, 
            // but we map it to the specific 'vaddr' requested by ELF.
            if (!XEPagingMap((void*)vaddr, PADDR_T_MAX, memsz, PAGE_ATTRIBUTE_WRITABLE)) {
                XEGuiPrint("ELF: Failed to map segment at %x\n", vaddr);
                return nullptr;
            }

            // Copy data from file
            if (filesz > 0) {
                copy_mem((void*)vaddr, (uint8_t*)filebuf + offset, filesz);
            }

            // Zero out BSS (remaining memory size)
            if (memsz > filesz) {
                zero_mem((void*)(vaddr + filesz), memsz - filesz);
            }
        }
    }

    return (void*)hdr->e_entry;
}

/*
 * XELFGetEntryPoint -- returns the entry point 
 * of loaded ELF image
 * @param image -- Pointer to file buffer
 */
XEImageEntry XELFGetEntryPoint(void* image) {
    Elf64_Ehdr* hdr = (Elf64_Ehdr*)image;
    return (XEImageEntry)hdr->e_entry;
}
