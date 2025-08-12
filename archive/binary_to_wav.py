import numpy as np
from pydub import AudioSegment
import soundfile as sf

# Inverse of your twosComplement function
# to convert two's complement binary string to int
def twos_complement_to_int(bstr):
    # if the sign bit is set (negative number)
    if bstr[0] == '1':
        # convert from two's complement to negative int
        return int(bstr, 2) - (1 << len(bstr))
    else:
        return int(bstr, 2)

def main():
    input_file = "data_test.txt"
    output_wav = "recovered_audio_data_2.wav"

    float_values = []

    with open(input_file, "r") as f:
        for line in f:
            bstr = line.strip()

            # Keep only first 32 bits (float32) --> for data.txt check
            bstr_32 = bstr[:32]

            int_val = int(bstr_32, 2)
            byte_data = int_val.to_bytes(4, byteorder='big', signed=False)
            float_val = np.frombuffer(byte_data, dtype=np.float32)[0]

            # Convert from two's complement binary string to int
            # int_val = twos_complement_to_int(bstr)

            # Interpret the int bits as a float32
            # float_val = np.frombuffer(int_val.to_bytes(4, byteorder='big', signed=True), dtype=np.float32)[0]

            float_values.append(float_val)

    # Convert list to numpy array
    audio_array = np.array(float_values, dtype=np.float32)

    # Save to WAV file - assuming 22050 Hz sampling rate (default from librosa)
    sr = 22050
    sf.write(output_wav, audio_array, sr)

    print(f"Recovered WAV audio saved as {output_wav}")

if __name__ == "__main__":
    main()
