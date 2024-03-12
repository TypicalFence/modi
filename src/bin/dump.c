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
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define BITS_PER_SAMPLE 8
// C-3
#define SAMPLE_RATE 16574

// ---------------------------------------------------------------------------------------
// Wav conversion
// ---------------------------------------------------------------------------------------
struct WAVHeader {
    char chunkID[4];        // "RIFF"
    unsigned int chunkSize; 
    char format[4];         // "WAVE"

    char subChunk1ID[4];    // "fmt "
    unsigned int subChunk1Size; // 16 for PCM
    unsigned short audioFormat; // PCM = 1
    unsigned short numChannels;
    unsigned int sampleRate;
    unsigned int byteRate;  
    unsigned short blockAlign; 
    unsigned short bitsPerSample;

    char subChunk2ID[4];    // "data"
    unsigned int subChunk2Size; 
};

void buildWAVHeader(struct WAVHeader* header, int dataSize) {
    // All samples are effectivly mono, 
    // including it to make the calculations easier to follow
    int channels = 1;

    strncpy(header->chunkID, "RIFF", 4);
    header->chunkSize = 36 + dataSize;
    strncpy(header->format, "WAVE", 4);

    strncpy(header->subChunk1ID, "fmt ", 4);
    header->subChunk1Size = 16; // PCM header size
    header->audioFormat = 1; // PCM
    header->numChannels = channels;
    header->sampleRate = SAMPLE_RATE;
    header->byteRate = SAMPLE_RATE;
    header->blockAlign = 1;
    header->bitsPerSample = 8;

    strncpy(header->subChunk2ID, "data", 4);
    header->subChunk2Size = dataSize;
}

void convertSignedToUnsigned8bit(int8_t* inputData, uint8_t* outputData, int dataSize) {
    for(int i = 0; i < dataSize; i++) {
        outputData[i] = (uint8_t)(inputData[i] + 128);
    }
}

// ---------------------------------------------------------------------------------------
// Main Program 
// ---------------------------------------------------------------------------------------
char* moduleFilePath = NULL;
int sampleNumberToDump = -1;
char* outputPath = NULL;


void printHelp(); 
void printUsage(); 

void handleArgs(int argc, char* argv[]) {
    if (argc == 1) {
        printUsage();
        exit(1);
    }
    
    int c;
    while ((c = getopt (argc, argv, "hm:s:o:")) != -1) {
        switch (c) {
            case 'h':
                printHelp();
                exit(0);
                break;
            case 'm':
                moduleFilePath = optarg;
                break;
            case 's': {
                char *errstr;
                sampleNumberToDump = strtonum(optarg, 0, 30, &errstr);
                if (errstr != NULL) {
                    printf("Sample number is out of bounds", errstr);
                    exit(1);
                }
            }
            case 'o' :
                outputPath = optarg;
                break;
        } 

    }

    if (moduleFilePath == NULL) {
        moduleFilePath = argv[1];
    }

    if (outputPath != NULL && sampleNumberToDump == -1) {
        printf("Error must specify a sample to dump when specifying a outputpath");
        exit(1);
    }
}

void dumpSample(struct AmigaModule* module, char* moduleFilePath, int8_t sampleNumber) {
    if (sampleNumber < 0 || sampleNumber > 30) {
        printf("Sample number out of bounds");
        return;
    }

    int8_t *sample;
    uint16_t sampleLength = module->samples[sampleNumber].sampleLength;
    struct WAVHeader header;
    uint8_t unsignedPCM[sampleLength];
    // TODO fix overflow once we add custom output name
    char filename[100];
    FILE *file;

    printf("Dumping Sample %d: %s\n", sampleNumber, module->samples[sampleNumber].sampleName);
    sample = load_amiga_sample_from_disk(moduleFilePath, module, sampleNumber);

    snprintf(filename, 100, "sample-%d.wav", sampleNumber);
    convertSignedToUnsigned8bit(sample, unsignedPCM, sampleLength);
    buildWAVHeader(&header, sampleLength);

    file = fopen(filename, "wb");
    fwrite(&header, sizeof(struct WAVHeader), 1, file);
    fwrite(&unsignedPCM, 1, sampleLength, file);
    fclose(file);
}

void dumpAllSamples(struct AmigaModule* module, char* moduleFilePath) {
    for (size_t i = 0; i < 31; i++) {
            if (module->samples[i].sampleLength == 0) {
                continue;
            }
            dumpSample(module, moduleFilePath, i);
    }
}

int main(int argc, char* argv[]) {
    handleArgs(argc, argv);
    struct AmigaModule* module; 

    module = malloc(sizeof(struct AmigaModule));

    // TODO handle status
    uint8_t status = parse_amiga_module_from_disk(moduleFilePath, module);
    printf("%s\n", module->magicChars);
    printf("Song: %s\n", module->songName);

    if (sampleNumberToDump == -1) {
        dumpAllSamples(module, moduleFilePath);
    } else {
        dumpSample(module, moduleFilePath, sampleNumberToDump);
    }

    return 0;
}

void printHelp() {
    printUsage();
    printf("\nOptions:\n");
    printf("    -m [file]\tmodule file (positional arg should be omited when set).\n");
    printf("    -s\t\tsample number to dump. modi-dump will only dump the specified sample.\n");
    printf("    -o\t\toutput file to write the sample to. Requires -s to be set.\n");
    printf("    -h\t\tdisplays this message\n");
}

void printUsage() {
    printf("modi-dump\n\nExtracts samples from ProTracker mod files\n\n");
    printf("Usage: modi-dump [options] modfile\n");
}