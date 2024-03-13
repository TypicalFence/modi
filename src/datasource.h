/*
 * modi
 * Copyright (c) 2024 fence 
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef MODI_DS_H
#define MODI_DS_H 

/**
 * Abstraction allowing to parse modules either from disk or from memory.
 */

#include <stdio.h>
#include <stdint.h>


struct Buffer {
    const uint8_t* buffer;
    size_t cursor;
    size_t length;
};

union RawData {
    #if defined(MODI_FS_SUPPORT)
    FILE* file;
    #endif
    struct Buffer* buffer;
};

struct DataSource {
    size_t (*read)(void* ptr, size_t size, size_t nmemb, union RawData* raw_data);
    int (*seek)(union RawData* raw_data, long offset, int whence);
    union RawData data;
};

// Functions for defining a data source

#if defined(MODI_FS_SUPPORT)
size_t modi_file_read(void* ptr, size_t size, size_t nmemb, union RawData* raw_data);
int modi_file_seek(union RawData* raw_data, long offset, int whence);
#endif

#ifndef MODI_16_BIT_SUPPORT
    size_t modi_buffer_read(void* ptr, size_t size, size_t nmemb, union RawData* raw_data);
    int modi_buffer_seek(union RawData* raw_data, long offset, int whence);
#endif 


size_t modi_read(void* ptr, size_t size, size_t nmemb, struct DataSource* datasource);
int modi_seek(struct DataSource* datasource, long offset, int whence);
#endif // MODI_DS_H