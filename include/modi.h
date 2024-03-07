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
#ifndef MODI_H 
#define MODI_H 

#include <stdint.h>

struct AmigaSample {
    char sampleName[22];
    u_int16_t sampleLength;
    u_int8_t finetune;
    u_int8_t volume;
    u_int16_t repeatOffset;
    u_int16_t repeatLength;
};

enum AmigaEffect {
    NORMAL = 0,
    SLIDE_UP = 1,
    SLIDE_DOWN = 2,
    PORTAMENTO = 3,
    VIBRATO = 4,
    PORTAMENTO_AND_VOLUME_SLIDE = 5,
    VIBRATO_AND_VOLUME_SLIDE = 6,
    TREMOLO = 7,
    NOT_USED = 8,
    SET_SAMPLE_OFFSET = 9,
    VOLUME_SLIDE = 10,
    POSITION_JUMP = 11,
    SET_VOLUME = 12,
    PATTERN_BREAK = 13,
    EXTENDED_EFFECT = 14,
    SET_SPEED = 15
};

struct AmigaNote {
    u_int16_t frequency;
    u_int8_t sampleNumber;
    enum AmigaEffect effect;
    u_int8_t effectParameter;
};

struct AmigaPattern {
    struct AmigaNote rows[64][4];
};

struct AmigaModule {
    char songName[20];
    struct AmigaSample samples[31];
    u_int8_t songLength;
    // there should be not parsed magic number here 127
    u_int8_t patternMap[128];
    char magicChars[4];
    struct AmigaPattern patterns[128];
    // where the sample data starts
    long sampleOffset;
};

enum ParseError {
    NO_ERROR = 0,
    UNKNOWN_ERROR = 1,
};

uint8_t parse_amiga_module(const char *filename, struct AmigaModule *module);
int8_t* load_amiga_sample(struct AmigaModule *module, int instrumentIndex, const char *filename);

#endif