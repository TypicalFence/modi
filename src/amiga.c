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
#include <modi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./datasource.h"

static inline void parse_note_bytes(uint8_t *noteBytes, struct AmigaNote *note) {
    uint16_t period= (noteBytes[0] & 0x0f) << 8 | noteBytes[1];
    uint8_t sampleNumber = (noteBytes[0] & 0xf0) | (noteBytes[2] >> 4);
    enum AmigaEffect effect = (noteBytes[2] & 0xf0) >> 4;
    uint8_t effectParameter = noteBytes[3];

    note->period = period;
    note->sampleNumber = sampleNumber;
    note->effect = effect;
    note->effectParameter = effectParameter;
}

uint8_t parse_amiga_module(struct DataSource* data, struct AmigaModule *module) {
    modi_seek(data, 0, SEEK_SET);
    modi_read(module->songName, 20, 1, data);

    // parse samples
    for (int i = 0; i < 31; i++) {
        modi_read(module->samples[i].sampleName, 22, 1, data);
        u_int8_t lengthBytes[2];
        modi_read(&lengthBytes, 1, 2, data);
        uint16_t rawSampleLength = (lengthBytes[0] << 8) | lengthBytes[1];
        uint16_t sampleLength = rawSampleLength * 2;
        module->samples[i].sampleLength = sampleLength;

        modi_read(&module->samples[i].finetune, 1, 1, data);
        modi_read(&module->samples[i].volume, 1, 1, data);
        modi_read(&module->samples[i].repeatOffset, 2, 1, data);
        modi_read(&module->samples[i].repeatLength, 2, 1, data);
    }

    modi_read(&module->songLength, 1, 1, data);
    int magicPatternCount = 0;
    modi_read(&magicPatternCount, 1, 1, data);

    for (int i = 0; i < 128; i++) {
        modi_read(&module->patternMap[i], 1, 1, data);
    }

    modi_read(&module->magicChars, 4, 1, data);

    uint8_t highestPattern = 0;
    for (int i = 0; i < 128; i++) {
        if (module->patternMap[i] > highestPattern) {
            highestPattern = module->patternMap[i] + 1;
        }
    }

    for (int i = 0; i < highestPattern; i++) {
        for (int j = 0; j < 64; j++) {
            for (int k = 0; k < 4; k++) {
                uint8_t noteBytes[4];
                modi_read(&noteBytes, 4, 1, data);
                parse_note_bytes(noteBytes, &module->patterns[i].rows[j][k]);
            }
        }
    }

    int patternDataSize = (highestPattern + 1) * 1024;
    module->sampleOffset = 1084 + patternDataSize;

    return 0;
}

inline static void seek_to_sample(struct DataSource *ds, struct AmigaModule *module, int instrumentIndex) {
    #ifndef MODI_16_BIT_SUPPORT
    uint32_t previousSamplesOffset = 0;

    if (instrumentIndex > 0) {
        for (size_t i = 0; i < instrumentIndex; i++) {
            previousSamplesOffset += module->samples[i].sampleLength;
        }
    }

    modi_seek(ds, module->sampleOffset + previousSamplesOffset, SEEK_SET);
    #else
    modi_seek(ds, module->sampleOffset, SEEK_SET);

    if (instrumentIndex > 0) {
        for (size_t i = 0; i < instrumentIndex; i++) {
            modi_seek(ds, module->samples[i].sampleLength, SEEK_CUR);
        }
    }
    #endif
};

int8_t* load_amiga_sample(struct DataSource *ds, struct AmigaModule *module, int instrumentIndex) {
    struct AmigaSample sampleMetadata = module->samples[instrumentIndex];
    seek_to_sample(ds, module, instrumentIndex);
    
    int8_t* sample = malloc(sampleMetadata.sampleLength * sizeof(int8_t));
    modi_read(sample, sizeof(int8_t), sampleMetadata.sampleLength, ds);

    return sample;
}

uint8_t parse_amiga_module_from_disk(const char *filename, struct AmigaModule *module) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Unable to open file: %s\n", filename);
        return 1;
    }

    struct DataSource ds = {
        .data = {
            .file = file
        },
        .read = modi_file_read,
        .seek = modi_file_seek,
    };

    uint8_t status = parse_amiga_module(&ds, module);

    fclose(file);

    return status;
}

uint8_t parse_amiga_module_from_memory(const uint8_t* bytes, size_t length, struct AmigaModule *module) {
    struct Buffer buffer = {
        .buffer = bytes,
        .cursor = 0,
        .length = length,
    };
    struct DataSource ds = {
        .data = {
            .buffer = &buffer,
        },
        .read = modi_buffer_read,
        .seek = modi_buffer_seek,
    };

    return parse_amiga_module(&ds, module);
}

int8_t* load_amiga_sample_from_disk(const char *filename, struct AmigaModule *module, int instrumentIndex) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Unable to open file: %s\n", filename);
        return NULL;
    }

    struct DataSource ds = {
        .data = {
            .file = file
        },
        .read = modi_file_read,
        .seek = modi_file_seek,
    };

    int8_t* sample = load_amiga_sample(&ds, module, instrumentIndex);

    fclose(file);

    return sample;
}

int8_t* load_amiga_sample_from_memory(const uint8_t* bytes, size_t length, struct AmigaModule *module, int instrumentIndex) {
    struct Buffer buffer = {
        .buffer = bytes,
        .cursor = 0,
        .length = sizeof(bytes),
    };

    struct DataSource ds = {
        .data = {
            .buffer = &buffer ,
        },
        .read = modi_buffer_read,
        .seek = modi_buffer_seek,
    };

    return load_amiga_sample(&ds, module, instrumentIndex);
}