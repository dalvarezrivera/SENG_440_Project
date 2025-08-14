# Audio Compression and Decompression Project 
## Instructions:
### Pre-requisites:
 - You have gcc
 - Python is installed (We checked and this is ok - as it is far more difficult to process wav files in C)

 1.  Listen to .wav audio file labelled "input_audio". This is an un-altered audio recording.
 2.  Execute the wav_to_binary.py file to convert the audio file into binary: "python wav_to_binary.py" 
 3.  Compile: gcc compress.c -o compress (Other flags are optional)
 4.  Run: "./compress" - this runs the compression and decompression algorithms and produceds a text file "output_decompressed.txt" with the decompressed binaries in it,
 5.  Run: "python binary_to_wav.py" - This converts the binaries back to a wav file.
 6.  Listen to .wav file labelled "reconstructed_audio.wav". This is the compressed and decompressed audio recording. Compare to original for quality.

Optional: record your own .wav audio file to test and name it "input_audio" as well and change the name of the current "inpubt_audio" file so there isn't any confusion. 
   
