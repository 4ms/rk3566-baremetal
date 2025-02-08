import numpy as np
from scipy.io.wavfile import write

def read_integers_from_file(file_path):
    with open(file_path, 'r') as file:
        integers = [int(line.strip()) for line in file]
    return integers


def write_integers_to_wav(file_path, sample_rate, data):
    write(file_path, sample_rate, np.array(data, dtype=np.int16))


def text_to_wav(text_file_path, wav_file_path, sample_rate):
    """
    Converts a text file of integers to a WAV file.

    Args:
        text_file_path: Path to the input text file.
        wav_file_path: Path to the output WAV file.
        sample_rate: Sample rate of the audio (e.g., 44100 Hz).
    """
    data = read_integers_from_file(text_file_path)
    write_integers_to_wav(wav_file_path, sample_rate, data)

# Example usage:
text_file = 'dj.txt'
wav_file = 'output.wav'
sample_rate = 48000  # You can adjust the sample rate as needed

text_to_wav(text_file, wav_file, sample_rate)

