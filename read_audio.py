import librosa
from pydub import AudioSegment
import numpy as np

# Source for 2's complement code: https://www.geeksforgeeks.org/python-program-to-convert-floating-to-binary/

# Python program to find 1's and 2's 
# complement of a binary number

# Function to find 1's complement
def onesComplement(s):
    
    # Traverse each bit and flip it
    result = ""
    for c in s:
        if c == '0':
            result += '1'
        else:
            result += '0'
    
    return result

# Function to find 2's complement
def twosComplement(s):
    
    # Get 1's complement of the binary number
    s = onesComplement(s)
    n = len(s)

    # Add 1 to the 1's complement
    result = list(s)
    for i in range(n - 1, -1, -1):
        
        # If we find '0', change it 
        # to '1' and stop
        if s[i] == '0':
            result[i] = '1'
            break
        
        # If we find '1', change it 
        # to '0' and continue
        else:
            result[i] = '0'

    # If all bits were flipped, we need
    # to add an extra '1'
    # at the beginning to maintain 
    # correct two's complement
    if result[0] == '0':
        result.insert(0, '1')

    return "".join(result)

    

def main():
    # Convert m4a to wav (librosa doesn't load m4a directly)
    audio = AudioSegment.from_file("New_Recording.m4a", format="m4a")
    audio.export("temp.wav", format="wav")

    # Load with librosa
    y, sr = librosa.load("temp.wav")
    i = 0
    result = []
    for value in y:
        res = np.binary_repr(np.float32(value).view(np.int32), width=32)
        result.append(twosComplement(res))

    with open("data.txt", "w") as f:
        for number in result:
            f.write(str(number) + "\n")
    
if __name__ == '__main__':
    main()