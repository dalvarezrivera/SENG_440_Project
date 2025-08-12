import wave
import struct

def wav_to_binary_txt(input_wav, output_txt):
    with wave.open(input_wav, 'rb') as wav:
        num_channels = wav.getnchannels()
        sample_width = wav.getsampwidth()
        sample_rate = wav.getframerate()
        num_frames = wav.getnframes()

        if sample_width != 2:
            raise ValueError("This script only supports 16-bit PCM WAV files.")

        print(f"Channels: {num_channels}, Sample Rate: {sample_rate}, Frames: {num_frames}")

        # Read all frames as bytes
        raw_data = wav.readframes(num_frames)

    # Convert each sample to binary string (little endian)
    with open(output_txt, 'w') as out:
        for i in range(0, len(raw_data), 2):
            sample = struct.unpack('<h', raw_data[i:i+2])[0]  # 16-bit signed
            # Two's complement binary representation
            binary_str = format(sample & 0xFFFF, '016b')
            out.write(binary_str + '\n')

if __name__ == "__main__":
    wav_to_binary_txt("temp.wav", "input_binary.txt")
