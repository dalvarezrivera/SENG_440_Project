#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// Replace with your actual definitions
#define COMPRESS true
#define DECOMPRESS false

// Stub declarations; replace with your real implementations:
int8_t codeword_compression(int16_t sample, int sign);
int8_t codeword_decompression(int16_t sample);
int signum(int16_t val);

// Convert a 16-bit binary string to int16_t two's complement
int16_t binary_to_int16(const char *bin_str) {
    uint16_t val = 0;
    for (int i = 0; i < 16; i++) {
        val <<= 1;
        if (bin_str[i] == '1') val |= 1;
        else if (bin_str[i] != '0') {
            fprintf(stderr, "Invalid character in binary string: %c\n", bin_str[i]);
            exit(EXIT_FAILURE);
        }
    }
    // Interpret as signed 16-bit
    if (val & 0x8000) {
        return (int16_t)(val | 0xFFFF0000); // sign extend to 32-bit then cast
    }
    return (int16_t)val;
}

int encode_binary_file(const char *input_txt, const char *output_bin, bool encodeType) {
    FILE *input_file = fopen(input_txt, "r");
    if (!input_file) {
        perror("Failed to open input file");
        return 1;
    }
    FILE *output_file = fopen(output_bin, "wb");
    if (!output_file) {
        perror("Failed to open output file");
        fclose(input_file);
        return 1;
    }

    // Read all samples into a dynamic array
    char line[32];
    size_t capacity = 1024;
    size_t sample_count = 0;
    int16_t *samples = malloc(capacity * sizeof(int16_t));
    if (!samples) {
        perror("Malloc failed");
        fclose(input_file);
        fclose(output_file);
        return 1;
    }

    while (fgets(line, sizeof(line), input_file)) {
        // Remove trailing newline
        size_t len = strlen(line);
        if (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[len-1] = '\0';
            len--;
        }
        if (len != 16) {
            fprintf(stderr, "Line with invalid length (expected 16): %s\n", line);
            free(samples);
            fclose(input_file);
            fclose(output_file);
            return 1;
        }

        int16_t sample = binary_to_int16(line);
        if (sample_count >= capacity) {
            capacity *= 2;
            samples = realloc(samples, capacity * sizeof(int16_t));
            if (!samples) {
                perror("Realloc failed");
                fclose(input_file);
                fclose(output_file);
                return 1;
            }
        }
        samples[sample_count++] = sample;
    }
    fclose(input_file);

    // Check that sample_count is multiple of 4 for block processing
    if (sample_count % 4 != 0) {
        fprintf(stderr, "Sample count (%zu) not multiple of 4\n", sample_count);
        free(samples);
        fclose(output_file);
        return 1;
    }

    // Allocate output buffer size depends on encode or decode
    size_t output_size = encodeType == COMPRESS ? sample_count / 2 : sample_count * 2;
    uint8_t *output_buffer = malloc(output_size);
    if (!output_buffer) {
        perror("Malloc failed");
        free(samples);
        fclose(output_file);
        return 1;
    }

    // Process samples in blocks of 4 (matching your original 8-byte blocks)
    for (size_t i = 0, o = 0; i < sample_count; i += 4) {
        int16_t s1 = samples[i];
        int16_t s2 = samples[i+1];
        int16_t s3 = samples[i+2];
        int16_t s4 = samples[i+3];

        int8_t c1, c2, c3, c4;
        if (encodeType == COMPRESS) {
            c1 = codeword_compression(s1, signum(s1));
            c2 = codeword_compression(s2, signum(s2));
            c3 = codeword_compression(s3, signum(s3));
            c4 = codeword_compression(s4, signum(s4));

            output_buffer[o++] = c1;
            output_buffer[o++] = c2;
            output_buffer[o++] = c3;
            output_buffer[o++] = c4;
        } else {
            // Note: your original code uses int16_t as input to decompression, 
            // but samples here are 16-bit PCM — you might need to adjust this 
            // depending on what codeword_decompression expects.
            c1 = codeword_decompression(s1);
            c2 = codeword_decompression(s2);
            c3 = codeword_decompression(s3);
            c4 = codeword_decompression(s4);

            // Store decompressed samples as 16-bit pairs (little endian)
            output_buffer[o++] = c1 & 0xFF;
            output_buffer[o++] = (c1 >> 8) & 0xFF;
            output_buffer[o++] = c2 & 0xFF;
            output_buffer[o++] = (c2 >> 8) & 0xFF;
            output_buffer[o++] = c3 & 0xFF;
            output_buffer[o++] = (c3 >> 8) & 0xFF;
            output_buffer[o++] = c4 & 0xFF;
            output_buffer[o++] = (c4 >> 8) & 0xFF;
        }
    }

    fwrite(output_buffer, 1, output_size, output_file);

    free(samples);
    free(output_buffer);
    fclose(output_file);

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <input_binary_txt> <output_bin> <compress|decompress>\n", argv[0]);
        return 1;
    }

    const char *input_txt = argv[1];
    const char *output_bin = argv[2];
    bool encodeType;

    if (strcmp(argv[3], "compress") == 0) {
        encodeType = COMPRESS;
    } else if (strcmp(argv[3], "decompress") == 0) {
        encodeType = DECOMPRESS;
    } else {
        fprintf(stderr, "Third argument must be 'compress' or 'decompress'\n");
        return 1;
    }

    return encode_binary_file(input_txt, output_bin, encodeType);
}
