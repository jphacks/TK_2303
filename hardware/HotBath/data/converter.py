import wave
import array

def wav_to_uint16_array(file_path):
    with wave.open(file_path, 'rb') as wave_file:
        # WAVファイルのパラメータを取得
        sample_width = wave_file.getsampwidth()
        frames = wave_file.getnframes()

        # WAVデータを読み込む
        wave_data = wave_file.readframes(frames)

        # WAVデータをuint16_tの配列に変換
        if sample_width == 2:
            # 16-bitサンプルの場合
            uint16_array = array.array('H', wave_data)
        else:
            # 他のサンプルフォーマットにも対応する場合はここに追加
            raise ValueError("Unsupported sample width")

    return uint16_array

def write_to_hpp(uint16_array, output_file_path):
    with open(output_file_path, 'w') as hpp_file:
        hpp_file.write("#pragma once\n\n")
        hpp_file.write("#include <cstdint>\n\n")
        hpp_file.write("const std::uint16_t audio_data[] = {\n")

        for value in uint16_array:
            hpp_file.write(f"    {value},\n")

        hpp_file.write("};\n")

# 使用例
file_path = 'chime.wav'
output_file_path = 'chime.hpp'

uint16_array = wav_to_uint16_array(file_path)
write_to_hpp(uint16_array, output_file_path)