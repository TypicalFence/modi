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
#include <tau/tau.h>
#include <modi.h>
#include <stdio.h>

TAU_MAIN() // sets up Tau (+ main function)


TEST(from_disk, parse_1) {
    struct AmigaModule module;
    parse_amiga_module_from_disk("./data/loituma.mod", &module);
    
    CHECK_STREQ(module.songName, "Ievan Polkka");
}

TEST(from_disk, parse_2) {
    struct AmigaModule module;
    parse_amiga_module_from_disk("./data/drozerix_-_may_is_4_her.mod", &module);
    
    CHECK_STREQ(module.songName, "May is 4 Her ");
}


uint8_t* readFile(const char* path, size_t* size) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return NULL;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for the buffer
    uint8_t* buffer = malloc(*size);
    if (buffer == NULL) {
        fclose(file);
        return NULL;
    }

    // Read the file into the buffer
    size_t bytesRead = fread(buffer, 1, *size, file);
    if (bytesRead < *size) {
        free(buffer);
        buffer = NULL;
    }

    fclose(file);
    return buffer;
}

TEST(from_memory, parse_1) {
    struct AmigaModule module2;
    size_t size = 0;
    uint8_t *buffer = readFile("./data/loituma.mod", &size);
    parse_amiga_module_from_memory(buffer, size, &module2);
    CHECK_STREQ(module2.songName, "Ievan Polkka");
}

TEST(from_memory, parse_2) {
    struct AmigaModule module;
    size_t size = 0;
    uint8_t *buffer = readFile("./data/drozerix_-_may_is_4_her.mod", &size);
    parse_amiga_module_from_memory(buffer, size, &module);
    
    CHECK_STREQ(module.songName, "May is 4 Her ");
}

