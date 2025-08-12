import librosa
import numpy as np

def int_to_twos_complement_bin(value, bits=16):
    # Make sure value is Python int, not numpy int16
    value = int(value)
    if value < 0:
        value = (1 << bits) + value
    return format(value, f'0{bits}b')


def main():
    # Load wav file (mono)
    y, sr = librosa.load("temp.wav", sr=None)  # sr=None to keep original sampling rate

    # Convert float samples to 16-bit signed integers
    int_samples = np.int16(y * 32767)

    # Convert each sample to 16-bit two's complement binary string
    result = [int_to_twos_complement_bin(sample, bits=16) for sample in int_samples]

    # Write binary strings to text file
    with open("data_test.txt", "w") as f:
        for binary_str in result:
            f.write(binary_str + "\n")

if __name__ == "__main__":
    main()
