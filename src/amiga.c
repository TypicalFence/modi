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

void parse_note_bytes(uint8_t *noteBytes, struct AmigaNote *note) {
    uint16_t frequency = (noteBytes[0] & 0x0f) << 8 | noteBytes[1];
    uint8_t sampleNumber = (noteBytes[0] & 0xf0) | (noteBytes[2] >> 4);
    enum AmigaEffect effect = (noteBytes[2] & 0xf0) >> 4;
    uint8_t effectParameter = noteBytes[3];

    note->frequency = frequency;
    note->sampleNumber = sampleNumber;
    note->effect = effect;
    note->effectParameter = effectParameter;
}

uint8_t parse_amiga_module(const char *filename, struct AmigaModule *module) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Unable to open file: %s\n", filename);
        return 1;
    }


    fseek(file, 0, SEEK_SET);
    fread(module->songName, 20, 1, file);

    // parse samples
    for (int i = 0; i < 31; i++) {
        fread(module->samples[i].sampleName, 22, 1, file);
        u_int8_t lengthBytes[2];
        fread(&lengthBytes, 1, 2, file);
        uint16_t rawSampleLength = (lengthBytes[0] << 8) | lengthBytes[1];
        uint16_t sampleLength = rawSampleLength * 2;
        module->samples[i].sampleLength = sampleLength;
        fread(&module->samples[i].finetune, 1, 1, file);
        fread(&module->samples[i].volume, 1, 1, file);
        fread(&module->samples[i].repeatOffset, 2, 1, file);
        fread(&module->samples[i].repeatLength, 2, 1, file);
    }

    fread(&module->songLength, 1, 1, file);
    int magicPatternCount = 0;
    fread(&magicPatternCount, 1, 1, file);
    printf("Magic pattern count: %d\n", magicPatternCount);

    for (int i = 0; i < 128; i++) {
        fread(&module->patternMap[i], 1, 1, file);
    }

    fread(&module->magicChars, 4, 1, file);

    uint8_t highestPattern = 0;
    for (int i = 0; i < 128; i++) {
        if (module->patternMap[i] > highestPattern) {
            highestPattern = module->patternMap[i];
        }
    }

    for (int i = 0; i < highestPattern + 5; i++) {
        for (int j = 0; j < 64; j++) {
            for (int k = 0; k < 4; k++) {
                uint8_t noteBytes[4];
                fread(&noteBytes, 4, 1, file);
                parse_note_bytes(noteBytes, &module->patterns[i].rows[j][k]);
            }
        }
    }

    module->sampleOffset = ftell(file);

    fclose(file);

    return 0;
}


int8_t* load_amiga_sample(struct AmigaModule *module, int instrumentIndex, const char *filename) {
    struct AmigaSample sampleMetadata = module->samples[instrumentIndex];
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Unable to open file: %s\n", filename);
        return NULL;
    }

    uint64_t previousSamplesOffset= 0;

    if (instrumentIndex > 0) {
        for (size_t i = 0; i < instrumentIndex; i++) {
            previousSamplesOffset += module->samples[i].sampleLength;
        }
    }

    

    fseek(file, module->sampleOffset + previousSamplesOffset - 4400 , SEEK_SET);
    int8_t* sample = malloc(sampleMetadata.sampleLength * sizeof(int8_t));
    fread(sample, sizeof(int8_t), sampleMetadata.sampleLength, file);
    fclose(file);

    return sample;
}
