#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

time_t start, stop, start2, stop2;
double compression_time, decompression_time;

#define MAX_LINE_LEN 20

// Get sign bit (1 for positive or zero, 0 for negative)
int get_sign(int16_t sample) {
    return (sample >= 0) ? 1 : 0;
}

// Get magnitude (absolute value)
unsigned int get_magnitude(int16_t sample) {
    return (sample < 0) ? (unsigned int)(-sample) : (unsigned int)sample;
}

// μ-law compression: return 8-bit compressed codeword
// Format: 1 sign bit (inverted), 3 chord bits, 4 step bits
// Similar style to your codeword_compression
unsigned char mu_law_compress(unsigned int mag, int sign) {
    int chord = 0;
    int step = 0;

    // Find chord by checking bits top-down (like original code)
    if (mag & (1 << 12)) {
        chord = 7;
        step = (mag >> 8) & 0xF;
    } else if (mag & (1 << 11)) {
        chord = 6;
        step = (mag >> 7) & 0xF;
    } else if (mag & (1 << 10)) {
        chord = 5;
        step = (mag >> 6) & 0xF;
    } else if (mag & (1 << 9)) {
        chord = 4;
        step = (mag >> 5) & 0xF;
    } else if (mag & (1 << 8)) {
        chord = 3;
        step = (mag >> 4) & 0xF;
    } else if (mag & (1 << 7)) {
        chord = 2;
        step = (mag >> 3) & 0xF;
    } else if (mag & (1 << 6)) {
        chord = 1;
        step = (mag >> 2) & 0xF;
    } else if (mag & (1 << 5)) {
        chord = 0;
        step = (mag >> 1) & 0xF;
    } else {
        chord = 0;
        step = 0;
    }

    // Compose compressed byte
    // Invert sign bit like your original code (0 for negative, 1 for positive)
    unsigned char compressed = ((abs(sign - 1)) << 7) | (chord << 4) | step;
    return compressed;
}

// μ-law decompression: reverse compressed byte to 16-bit magnitude with sign
// Similar to your codeword_decompression style
int16_t mu_law_decompress(unsigned char compressed) {
    int step = compressed & 0x0F;
    int chord = (compressed & 0x70) >> 4;
    int sign_bit = (compressed & 0x80) >> 7;

    unsigned int magnitude = 0;

    switch(chord) {
        case 7: magnitude = (step << 8) | 0xFF; break;
        case 6: magnitude = (step << 7) | 0x7F; break;
        case 5: magnitude = (step << 6) | 0x3F; break;
        case 4: magnitude = (step << 5) | 0x1F; break;
        case 3: magnitude = (step << 4) | 0x0F; break;
        case 2: magnitude = (step << 3) | 0x07; break;
        case 1: magnitude = (step << 2) | 0x03; break;
        case 0: magnitude = (step << 1) | 0x01; break;
    }

    // Restore sign: in original code 0 means negative, 1 means positive
    int16_t sample = (sign_bit == 0) ? -(int16_t)magnitude : (int16_t)magnitude;

    return sample;
}

// Convert 16-bit signed int to binary string MSB first
void int16_to_binary_str(int16_t val, char *out) {
    for (int i = 15; i >= 0; i--) {
        out[15 - i] = (val & (1 << i)) ? '1' : '0';
    }
    out[16] = '\0';
}

// Convert binary string MSB first to int16_t
int16_t binary_str_to_int16(const char *in) {
    int16_t val = 0;
    for (int i = 0; i < 16; i++) {
        if (in[i] == '1') {
            val |= (1 << (15 - i));
        }
    }
    return val;
}

int main() {
    FILE *fin = fopen("data_test.txt", "r");
    if (!fin) {
        perror("Error opening data_test.txt");
        return 1;
    }
    FILE *fout = fopen("output_decompressed_gpt2.txt", "w");
    if (!fout) {
        perror("Error opening output_decompressed.txt");
        fclose(fin);
        return 1;
    }

    char line[20];
    char out_binary[17];

    start = clock();
    while (fgets(line, sizeof(line), fin)) {
        // Remove newline
        for (int i = 0; i < 16; i++) {
            if (line[i] == '\n' || line[i] == '\r') {
                line[i] = '\0';
                break;
            }
        }
        if (strlen(line) < 16) continue;

        int16_t sample = binary_str_to_int16(line);
        int sign = get_sign(sample);
        unsigned int mag = get_magnitude(sample);

        // start = clock();

        unsigned char compressed = mu_law_compress(mag, sign);

        // stop = clock();
        // compression_time = (double)((double)stop - (double)start) / CLOCKS_PER_SEC;

        // start2 = clock(); 

        int16_t decompressed = mu_law_decompress(compressed);

        // stop2 = clock();
        // decompression_time = (double)((double)stop2 - (double)start2) / CLOCKS_PER_SEC;

        int16_to_binary_str(decompressed, out_binary);
        fprintf(fout, "%s\n", out_binary);
    }
    stop = clock();
    compression_time = (double)((double)stop - (double)start) / CLOCKS_PER_SEC;
    printf("Complete\n");
    printf("Audio Compression and decompression Time:%lf  Seconds\n", compression_time);
    //printf("Audio Decompression Time: %lf  Seconds \n", decompression_time);

    fclose(fin);
    fclose(fout);

    printf("μ-law compression & decompression done.\n");
    return 0;
}
