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
#include "./datasource.h"
#include <string.h>

size_t modi_read(void* ptr, size_t size, size_t nmemb, struct DataSource* datasource) {
    return datasource->read(ptr, size, nmemb, &datasource->data);
}

int modi_seek(struct DataSource* datasource, long offset, int whence) {
    return datasource->seek(&datasource->data, offset, whence);
}

size_t modi_file_read(void* ptr, size_t size, size_t nmemb, union RawData* raw_data) {
    return fread(ptr, size, nmemb, raw_data->file);
}

int modi_file_seek(union RawData* raw_data, long offset, int whence) {
    return fseek(raw_data->file, offset, whence);
}

#ifndef MODI_16_BIT_SUPPORT
size_t modi_buffer_read(void* ptr, size_t size, size_t nmemb, union RawData* raw_data) {
        struct Buffer* buff = raw_data->buffer;

        size_t bytes_read = 0;

        for (size_t i = 0; i < nmemb; i++) {
            if (buff->cursor + size > buff->length) {
                size = buff->length - buff->cursor;
            }

            memcpy(ptr + (size * i), buff->buffer + buff->cursor, size);
            buff->cursor += size;
            bytes_read += size;
        }

        return bytes_read;
}

int modi_buffer_seek(union RawData* raw_data, long offset, int whence) {
    struct Buffer* buf = raw_data->buffer;

     switch (whence) {
        case SEEK_SET: buf->cursor = offset; break;
        case SEEK_CUR: buf->cursor += offset; break;
        case SEEK_END: buf->cursor = buf->length + offset; break;
        default: return -1; // Invalid 'whence'
    }

    if (buf->cursor > buf->length) {
        buf->cursor = buf->length; 
    }

    return 0;
}
#endif