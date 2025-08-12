import numpy as np
import soundfile as sf

def twos_complement_to_int(bstr):
    bits = len(bstr)
    val = int(bstr, 2)
    # If sign bit is set, compute negative value
    if bstr[0] == '1':
        val -= (1 << bits)
    return val

def main():
    input_file = "daniel_text_decompressed.txt"   # Text file with binary strings
    output_wav = "reconstructed_check.wav"
    sample_rate = 48000       # Use the original WAV sample rate
    bit_depth = 16            # Number of bits per sample

    int_samples = []
    with open(input_file, "r") as f:
        for line in f:
            bstr = line.strip()
            if not bstr:
                continue
            val = twos_complement_to_int(bstr)
            int_samples.append(val)

    # Convert list to numpy array of int16
    audio_data = np.array(int_samples, dtype=np.int16)

    # Convert integers back to float32 range [-1, 1] for saving as wav
    float_audio = audio_data.astype(np.float32) / 32767.0

    # Write audio to wav file
    sf.write(output_wav, float_audio, sample_rate)
    print(f"WAV file saved as {output_wav}")

if __name__ == "__main__":
    main()
