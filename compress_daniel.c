#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

//#define MAX_LINES 2535547
#define MAX_LINES 551937
#define MAX_LINES_ADJUSTED 760638 // 33 bits per line, we want only 13 per line
#define LINE_LENGTH_READ 35 // 32 bits + '\0' + safety
#define LINE_LENGTH_ADJUSTED 13

//LSB to MSB order 
void int_to_binary(int n, char *output, int length) {
    for (int i = 0; i < length; i++){
        output[i] = ((1 << i) & n) ? '1' : '0';
    }
    output[length] = '\0';
}


//MSB to LSB order
// void int_to_binary(int n, char *output, int length) {
//     for (int i = 0; i < length; i++){
//         // Print MSB first:
//         output[i] = (n & (1 << (length - 1 - i))) ? '1' : '0';
//     }
//     output[length] = '\0';
// }

//good should work 
int signum (int sample){
    if(sample < 0){
        return(0) ; /* sign is ’0’ for negative samples */
    }else {
        return(1) ; /* sign is ’1’ for positive samples */
    }
}


// int magnitude (int sample){
//     if(sample < 0) {
//         sample = -sample;
//     }
//     return(sample);
// }

// Get magnitude (absolute value)
unsigned int get_magnitude(int16_t sample) {
    return (sample < 0) ? (unsigned int)(-sample) : (unsigned int)sample;
}

int codeword_compression (unsigned int sample_magnitude, int sign) {
    int chord, step;
    int codeword_tmp;
    if(sample_magnitude & (1 << 12)){
        chord = 0x7;
        step = (sample_magnitude >> 8) & 0xF;
        codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
        return (codeword_tmp);
    }
    if(sample_magnitude & (1 << 11)){
        chord = 0x6;
        step = (sample_magnitude >> 7) & 0xF;
        codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
        return (codeword_tmp);
    }
    if(sample_magnitude & (1 << 10)){
        chord = 0x5;
        step = (sample_magnitude >> 6) & 0xF;
        codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
        return (codeword_tmp);
    }
    if(sample_magnitude & (1 << 9)){
        chord = 0x4;
        step = (sample_magnitude >> 5) & 0xF;
        codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
        return (codeword_tmp);
    }
    if(sample_magnitude & (1 << 8)){
        chord = 0x3;
        step = (sample_magnitude >> 4) & 0xF;
        codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
        return (codeword_tmp);
    }
    if (sample_magnitude & (1 << 7)){
        chord = 0x2;
        step = (sample_magnitude >> 3) & 0xF;
        codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
        return (codeword_tmp);
    }
    if(sample_magnitude & (1 << 6)){
        chord = 0x1;
        step = (sample_magnitude >> 2) & 0xF;
        codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
        return (codeword_tmp);
    }
    if (sample_magnitude & (1 << 5)){
        chord = 0x0;
        step = (sample_magnitude >> 1) & 0xF;
        codeword_tmp = ((abs(sign-1)) << 7) | (chord << 4) | step;
        return (codeword_tmp);
    }
    return (int) sample_magnitude;
}

int codeword_decompression (int compressed) {
    int step = compressed & 0x0F;
    int codeword_tmp = 1;
    int chord = (compressed & 0x70) >> 4;
    int sign = (compressed & 0x80) >> 7;
    //printf("Chord: %d\n", chord);
    if (chord == 7){
        int zeros = 0x80;
        if (abs(sign-1) == 1){
            sign = 3; // For the Sign and first value of the string
        } else{
            sign = 1;
        }
        //printf("Sign: %d\n", sign);
        codeword_tmp = (sign << 12) | (step << 8) | zeros;
        return (codeword_tmp);
    }
    if (chord == 6){
        int zeros_ones = 0x840; // Rightmost zeros
        int step_zeros_ones = (step << 7) | zeros_ones;
        if (abs(sign-1) == 1){
            sign = 2; // For the Sign and first value of the string
        } else{
            sign = 0;
        }
        //printf("Sign: %d\n", sign);
        codeword_tmp = (sign << 12) | step_zeros_ones;
        return (codeword_tmp);
    }
    if (chord == 5){
        int zeros_ones = 0x420; // Rightmost zeros
        int step_zeros_ones = (step << 6) | zeros_ones;
        if (abs(sign-1) == 1){
            sign = 2; // For the Sign and first value of the string
        } else{
            sign = 0;
        }
        //printf("Sign: %d\n", sign);
        codeword_tmp = (sign << 12) | step_zeros_ones;
        return (codeword_tmp);
    }
    if (chord == 4){
        int zeros_ones = 0x210; // Rightmost zeros
        int step_zeros_ones = (step << 5) | zeros_ones;
        if (abs(sign-1) == 1){
            sign = 2; // For the Sign and first value of the string
        } else{
            sign = 0;
        }
        //printf("Sign: %d\n", sign);
        codeword_tmp = (sign << 12) | step_zeros_ones;
        return (codeword_tmp);
    }
    if (chord == 3){
        int zeros_ones = 0x108; // Rightmost zeros
        int step_zeros_ones = (step << 4) | zeros_ones;
        if (abs(sign-1) == 1){
            sign = 2; // For the Sign and first value of the string
        } else{
            sign = 0;
        }
        //printf("Sign: %d\n", sign);
        codeword_tmp = (sign << 12) | step_zeros_ones;
        return (codeword_tmp);
    }
    if (chord == 2){
        int zeros_ones = 0x084; // Rightmost zeros
        int step_zeros_ones = (step << 3) | zeros_ones;
        if (abs(sign-1) == 1){
            sign = 2; // For the Sign and first value of the string
        } else{
            sign = 0;
        }
        //printf("Sign: %d\n", sign);
        codeword_tmp = (sign << 12) | step_zeros_ones;
        return (codeword_tmp);
    }
    if (chord == 1){
        int zeros_ones = 0x102; // Rightmost zeros
        int step_zeros_ones = (step << 2) | zeros_ones;
        if (abs(sign-1) == 1){
            sign = 2; // For the Sign and first value of the string
        } else{
            sign = 0;
        }
        //printf("Sign: %d\n", sign);
        codeword_tmp = (sign << 12) | step_zeros_ones;
        return (codeword_tmp);
    }
    if (chord == 0){
        int zeros_ones = 0x021; // Rightmost zeros
        int step_zeros_ones = (step << 1) | zeros_ones;
        if (abs(sign-1) == 1){
            sign = 2; // For the Sign and first value of the string
        } else{
            sign = 0;
        }
        //printf("Sign: %d\n", sign);
        codeword_tmp = (sign << 12) | step_zeros_ones;
        return (codeword_tmp);
    }
    return 0;
}

int main() {
    FILE *file = fopen("data_test.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    } else {
        printf("Opened file\n");
    }

    char **binaries = malloc(MAX_LINES * sizeof(char *));
    char **binaries_adjusted = malloc(MAX_LINES_ADJUSTED * sizeof(char *));
    int *binaries_compressed = malloc(MAX_LINES_ADJUSTED * sizeof(int));
    int *binaries_decompressed = malloc(MAX_LINES_ADJUSTED * sizeof(int));
    printf("After Mallocs.\n");

    if (binaries == NULL) {
        perror("malloc failed for binaries");
        return 1;
    }
    if (binaries_adjusted == NULL) {
        perror("malloc failed for binaries_adjusted");
        return 1;
    }
    if (binaries_compressed == NULL) {
        perror("malloc failed for binaries_compressed");
        return 1;
    }
    if (binaries_decompressed == NULL) {
        perror("malloc failed for binaries_compressed");
        return 1;
    }
    printf("After Nulls.\n");

    for (int i = 0; i < MAX_LINES; i++) {
        binaries[i] = malloc(LINE_LENGTH_READ * sizeof(char));
        binaries_adjusted[i] = malloc(LINE_LENGTH_ADJUSTED * sizeof(char));
        // binaries_compressed[i] = malloc(LINE_LENGTH_ADJUSTED * sizeof(char));

        if (binaries[i] == NULL) {
            perror("malloc failed for binaries[i]");
            return 1;
        }
        if (binaries_adjusted[i] == NULL) {
            perror("malloc failed for binaries_adjusted[i]");
            return 1;
        }
    }
    int line_count = 0;

    printf("Before while loop.\n");
    while (fgets(binaries[line_count], LINE_LENGTH_READ, file) != NULL) {
        // Remove newline if present
        // printf("%s\n", binaries[line_count]);
        binaries[line_count][strcspn(binaries[line_count], "\n")] = '\0';
        line_count++;

        if (line_count >= MAX_LINES) {
            printf("Reached max lines limit.\n");
            break;
        }
    }
    printf("After while loop.\n");

    fclose(file);
    
    int adjusted_rows = 0;
    int adjusted_cols = 0;
    int sign; // For compression
    int sign_check = 1; // To know if it's the sign bit
    unsigned int magnitude;

    for (int i = 0; i < line_count; i++) {
        adjusted_cols = 0;
        sign_check = 1;
        for(int j = 0; j < LINE_LENGTH_READ; j++){ // Get 13 bits per row
            if(adjusted_cols >= LINE_LENGTH_ADJUSTED){
                adjusted_cols = 0;
                sign_check = 1;
                // Converting char * to unsigned int
                magnitude = (unsigned int)strtoul(binaries_adjusted[adjusted_rows], NULL, 2);
                //printf("Magnitude: %lld\n", magnitude);
                binaries_compressed[adjusted_rows] = codeword_compression(magnitude, sign);
                //printf("Line %d: Not compressed: %s, Compressed: %d\n", adjusted_rows, binaries_adjusted[adjusted_rows], binaries_compressed[adjusted_rows]);
                adjusted_rows++;
            }
            if(sign_check == 1){ // Sign of the expression
                sign =  -48 + binaries[i][j]; // Converting from char to int
                sign_check = 0;
            } else{
                binaries_adjusted[adjusted_rows][adjusted_cols] = binaries[i][j];
                adjusted_cols++;
            }
        }
        binaries_compressed[adjusted_rows] = strtoul(binaries_adjusted[adjusted_rows], NULL, 2);
    // printf("Line %d: Not compressed: %s, Compressed: %d\n", adjusted_rows, binaries_adjusted[adjusted_rows], binaries_compressed[adjusted_rows]);
        adjusted_rows++;
    }
    printf("Before open file.\n");

    // Write to file the decompressed data
    FILE *file_2 = fopen("decompressed.txt", "w");

    // Check if the file was opened successfully
    if (file_2 == NULL) {
        perror("Error opening decompressed.txt");
        return 1;
    }

    // Decompress
    char bin_str[LINE_LENGTH_ADJUSTED + 1]; // 13 bits + null terminator
    char bin_str_2[LINE_LENGTH_ADJUSTED + 1]; // 13 bits + null terminator
    char bin_str_3[7 + 1]; // 7 bits + null terminator


    for (int i = 0; i < adjusted_rows; i++) {
        if(i%3 == 0){
            binaries_decompressed[i] = binaries_compressed[i];
        } else{
            binaries_decompressed[i] = codeword_decompression(binaries_compressed[i]);
        }
        //printf("Line: %d, compressed: %d, decompressed: %d.\n", i, binaries_compressed[i], binaries_decompressed[i]);

        // int_to_binary(binaries_decompressed[i], bin_str, LINE_LENGTH_ADJUSTED);
        // fprintf(file_2, "%s\n", bin_str);

        //fprintf(file_2, "%d\n", binaries_decompressed[i]);
    }

    for (int i = 0; i < adjusted_rows; i+=3) {

        //printf("Line: %d, compressed: %d, decompressed: %d.\n", i, binaries_compressed[i], binaries_decompressed[i]);

        int_to_binary(binaries_decompressed[i], bin_str, LINE_LENGTH_ADJUSTED);
        int_to_binary(binaries_decompressed[i+1], bin_str_2, LINE_LENGTH_ADJUSTED);
        int_to_binary(binaries_decompressed[i+2], bin_str_3, LINE_LENGTH_ADJUSTED);

        fprintf(file_2, "%s%s%s\n", bin_str, bin_str_2, bin_str_3);

        //fprintf(file_2, "%d\n", binaries_decompressed[i]);
    }

    fclose(file);
    fclose(file_2);

    for (int i = 0; i < MAX_LINES; i++) {
        free(binaries[i]);
    }

    for (int i = 0; i < MAX_LINES_ADJUSTED; i++) {
        free(binaries_adjusted[i]);
    }

    free(binaries);
    free(binaries_adjusted);
    free(binaries_compressed);
    free(binaries_decompressed);

    return 0;
}