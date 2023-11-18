import wave
import array

def wav_to_uint16_array(file_paths):
    uint16_arrays = []
    for file_path in file_paths:
        with wave.open(file_path, 'rb') as wave_file:
            # WAVファイルのパラメータを取得
            sample_width = wave_file.getsampwidth()
            frames = wave_file.getnframes()

            # WAVデータを読み込む
            wave_data = wave_file.readframes(frames)

            # WAVデータをuint16_tの配列に変換
            if sample_width == 2:
                # 16-bitサンプルの場合
                uint16_arrays.append(array.array('H', wave_data))
            else:
                # 他のサンプルフォーマットにも対応する場合はここに追加
                raise ValueError("Unsupported sample width")

    return uint16_arrays

def write_to_hpp(uint16_arrays, names, output_file_path):
    with open(output_file_path, 'w') as hpp_file:
        hpp_file.write("#pragma once\n\n")
        hpp_file.write("#include <cstdint>\n\n")

        for i, uint16_array in enumerate(uint16_arrays):
            hpp_file.write("const uint16_t " + names[i] + "_sound[] = {\n")

            for value in uint16_array:
                hpp_file.write(f"    {value},\n")

            hpp_file.write("};\n")

# 使用例
file_paths = ['big_sound.wav','bathin.wav', 'bathout.wav', 'chime.wav', 'heatshock_attention.wav', 'long_time.wav', 'danger.wav']
names = ['big_sound', 'bathin', 'bathout', 'chime', 'heatshock_attention', 'long_time', 'danger']
output_file_path = '../include/sound_data.hpp'

uint16_arrays = wav_to_uint16_array(file_paths)
write_to_hpp(uint16_arrays, names, output_file_path)