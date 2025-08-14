#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

time_t start, stop, start2, stop2;
double compression_time, decompression_time;

#define MAX_LINE_LEN 20

int signum (int sample){
    if(sample < 0){
        return(0) ; /* sign is ’0’ for negative samples */
    }else {
        return(1) ; /* sign is ’1’ for positive samples */
    }
}

int get_magnitude (int sample){
    if(sample < 0) {
        sample = -sample;
    }
    return(sample);
}


// Convert 16-bit signed int to binary string MSB first
void int_to_binary(int16_t n, char *output) {
    for (int i = 15; i >= 0; i--) {
        output[15 - i] = (n & (1 << i)) ? '1' : '0';
    }
    output[16] = '\0';
}

// Convert binary string MSB first to int
int binary_to_int(const char *in) {
    int val = 0;
    for (int i = 0; i < 16; i++) {
        if (in[i] == '1') {
            val |= (1 << (15 - i));
        }
    }
    return val;
}

// μ-law compression - returns an 8-bit compressed codeword
// format: 1 sign bit (inverted), 3 chord bits, 4 step bits

// unsigned char codeword_compression(unsigned int sample_magnitude, int sign) {
//     int chord = 0; 
//     int step = 0;
//     unsigned char codeword_tmp;

//     //find the chord by checking bits top-down
//     if (sample_magnitude & (1 << 12)) {
//         chord = 0x7;
//         step = (sample_magnitude >> 8) & 0xF;
//     } else if (sample_magnitude & (1 << 11)) {
//         chord = 0x6;
//         step = (sample_magnitude >> 7) & 0xF;
//     } else if (sample_magnitude & (1 << 10)) {
//         chord = 0x5;
//         step = (sample_magnitude >> 6) & 0xF;
//     } else if (sample_magnitude & (1 << 9)) {
//         chord = 0x4;
//         step = (sample_magnitude >> 5) & 0xF;
//     } else if (sample_magnitude & (1 << 8)) {
//         chord = 0x3;
//         step = (sample_magnitude >> 4) & 0xF;
//     } else if (sample_magnitude & (1 << 7)) {
//         chord = 0x2;
//         step = (sample_magnitude >> 3) & 0xF;
//     } else if (sample_magnitude & (1 << 6)) {
//         chord = 0x1;
//         step = (sample_magnitude >> 2) & 0xF;
//     } else if (sample_magnitude & (1 << 5)) {
//         chord = 0x0;
//         step = (sample_magnitude >> 1) & 0xF;
//     } else {
//         chord = 0x0;
//         step = 0x0;
//     }

//     codeword_tmp = ((abs(sign - 1)) << 7) | (chord << 4) | step;
//     return codeword_tmp;
// }

// // μ-law compression with CLZ
// int codeword_compression_clz(unsigned int sample_magnitude, int sign) {
//     int chord, step;
//     int codeword_tmp;
//     int zeros_at_left_side = __builtin_clz(sample_magnitude << 19);
//     // printf("%d\n", zeros_at_left_side);
//     if(zeros_at_left_side == 0){
//         chord = 0x7;
//         step = (sample_magnitude >> 8) & 0xF;
//         codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
//         return (codeword_tmp);
//     }
//     if(zeros_at_left_side == 1){
//         chord = 0x6;
//         step = (sample_magnitude >> 7) & 0xF;
//         codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
//         return (codeword_tmp);
//     }
//     if(zeros_at_left_side == 2){
//         chord = 0x5;
//         step = (sample_magnitude >> 6) & 0xF;
//         codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
//         return (codeword_tmp);
//     }
//     if(zeros_at_left_side == 3){
//         chord = 0x4;
//         step = (sample_magnitude >> 5) & 0xF;
//         codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
//         return (codeword_tmp);
//     }
//     if(zeros_at_left_side == 4){
//         chord = 0x3;
//         step = (sample_magnitude >> 4) & 0xF;
//         codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
//         return (codeword_tmp);
//     }
//     if (zeros_at_left_side == 5){
//         chord = 0x2;
//         step = (sample_magnitude >> 3) & 0xF;
//         codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
//         return (codeword_tmp);
//     }
//     if(zeros_at_left_side == 6){
//         chord = 0x1;
//         step = (sample_magnitude >> 2) & 0xF;
//         codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
//         return (codeword_tmp);
//     }
//     if (zeros_at_left_side == 7){
//         chord = 0x0;
//         step = (sample_magnitude >> 1) & 0xF;
//         codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
//         return (codeword_tmp);
//     }
//     return (int) sample_magnitude;
// }

// μ-law compression with CLZ and switch 
int codeword_compression_clz_switch(unsigned int sample_magnitude, int sign) {
    int chord, step;
    int codeword_tmp;
    int zeros_at_left_side = __builtin_clz(sample_magnitude << 19);

    switch (zeros_at_left_side){
        case 0:
            chord = 0x7;
            step = (sample_magnitude >> 8) & 0xF;
            codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
            return (codeword_tmp);
        case 1:
            chord = 0x6;
            step = (sample_magnitude >> 7) & 0xF;
            codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
            return (codeword_tmp);
        case 2:
            chord = 0x5;
            step = (sample_magnitude >> 6) & 0xF;
            codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
            return (codeword_tmp);
        case 3:
            chord = 0x4;
            step = (sample_magnitude >> 5) & 0xF;
            codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
            return (codeword_tmp);
        case 4:
            chord = 0x3;
            step = (sample_magnitude >> 4) & 0xF;
            codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
        return (codeword_tmp);
        case 5:
            chord = 0x2;
            step = (sample_magnitude >> 3) & 0xF;
            codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
            return (codeword_tmp);
        case 6:
            chord = 0x1;
            step = (sample_magnitude >> 2) & 0xF;
            codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
            return (codeword_tmp);
        case 7:
            chord = 0x0;
            step = (sample_magnitude >> 1) & 0xF;
            codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
            return (codeword_tmp);
        default:
            return (int) sample_magnitude;
    }
}

// // μ-law decompression
// int16_t codeword_decompression(unsigned char compressed) {
//     int step = compressed & 0x0F;
//     int codeword_tmp = 1;
//     int chord = (compressed & 0x70) >> 4;
//     int sign = (compressed & 0x80) >> 7;
//     int zeros_ones;

//     if (chord == 7){
//         zeros_ones = 0x80;
//         codeword_tmp = (1 << 12) | (step << 8) | zeros_ones;
//     } else if (chord == 6){
//         zeros_ones = 0x840; // Rightmost zeros
//         codeword_tmp = (step << 7) | zeros_ones;
//     } else if (chord == 5){
//         zeros_ones = 0x420; // Rightmost zeros
//         codeword_tmp = (step << 6) | zeros_ones;
//     } else if (chord == 4){
//         zeros_ones = 0x210; // Rightmost zeros
//         codeword_tmp = (step << 5) | zeros_ones;
//     } else if (chord == 3){
//         zeros_ones = 0x108; // Rightmost zeros
//         codeword_tmp = (step << 4) | zeros_ones;
//     } else if (chord == 2){
//         zeros_ones = 0x84; // Rightmost zeros
//         codeword_tmp = (step << 3) | zeros_ones;
//     } else if (chord == 1){
//         zeros_ones = 0x102; // Rightmost zeros
//         codeword_tmp = (step << 2) | zeros_ones;
//     } else if (chord == 0){
//         zeros_ones = 0x21; // Rightmost zeros
//         codeword_tmp = (step << 1) | zeros_ones;
//     }

//     int16_t sample;
//     if (sign == 0) {
//         sample = -(int16_t)codeword_tmp;
//     } else {
//         sample = (int16_t)codeword_tmp;
//     }

//     return sample;
// }

// μ-law decompression with switches
int16_t codeword_decompression_switch(unsigned char compressed) {
    int step = compressed & 0x0F;
    int codeword_tmp = 1;
    int chord = (compressed & 0x70) >> 4;
    int sign = (compressed & 0x80) >> 7;
    int zeros_ones;

    switch (chord){
        case 7:
            zeros_ones = 0x80;
            codeword_tmp = (1 << 12) | (step << 8) | zeros_ones; break;
        case 6:
            zeros_ones = 0x840; // Rightmost zeros
            codeword_tmp = (step << 7) | zeros_ones; break;
        case 5:
            zeros_ones = 0x420; // Rightmost zeros
            codeword_tmp = (step << 6) | zeros_ones; break;
        case 4:
            zeros_ones = 0x210; // Rightmost zeros
            codeword_tmp = (step << 5) | zeros_ones; break;
        case 3:
            zeros_ones = 0x108; // Rightmost zeros
            codeword_tmp = (step << 4) | zeros_ones; break;
        case 2:
            zeros_ones = 0x84; // Rightmost zeros
            codeword_tmp = (step << 3) | zeros_ones; break;
        case 1:
            zeros_ones = 0x102; // Rightmost zeros
            codeword_tmp = (step << 2) | zeros_ones; break;
        case 0:
            zeros_ones = 0x21; // Rightmost zeros
            codeword_tmp = (step << 1) | zeros_ones; break;
    }
    int16_t sample;
    if (sign == 0) {
        sample = -(int16_t)codeword_tmp;
    } else {
        sample = (int16_t)codeword_tmp;
    }
    return sample;
}

int main() {
    FILE *input_file = fopen("input_binary.txt", "r");
    if (!input_file) {
        perror("Error opening input_binary.txt");
        return 1;
    }else{
        printf("Opened input file\n");
    }
    FILE *output_file = fopen("output_decompressed.txt", "w");
    if (!output_file) {
        perror("Error opening output_decompressed.txt");
        fclose(input_file);
        return 1;
    }else {
        printf("Opened output file\n");
    }

    char line[20];
    char out_binary[17];

    start = clock();

    while (fgets(line, sizeof(line), input_file)) {
        // Remove newline if present 
        for (int i = 0; i < 16; i++) {
            if (line[i] == '\n' || line[i] == '\r') {
                line[i] = '\0';
                break;
            }
        }

        int16_t sample = binary_to_int(line);
        int sign = signum(sample);
        unsigned int mag = get_magnitude(sample);


        unsigned char compressed = codeword_compression_clz_switch(mag, sign);

        int16_t binaries_decompressed = codeword_decompression_switch(compressed);

        int_to_binary(binaries_decompressed, out_binary);
        fprintf(output_file, "%s\n", out_binary);
    }
    stop = clock();
    compression_time = (double)((double)stop - (double)start) / CLOCKS_PER_SEC;
    printf("Complete\n");
    printf("Audio Compression and decompression Time:%lf  Seconds\n", compression_time);

    fclose(input_file);
    fclose(output_file);

    printf("μ-law compression & decompression done.\n");
    return 0;
}
