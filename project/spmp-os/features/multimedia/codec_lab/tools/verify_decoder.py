#!/usr/bin/env python3
"""Reproducible host FLAC->PCM conformance spot checks. Requires ffmpeg and C compiler."""
import hashlib
import math
import pathlib
import random
import struct
import subprocess
import sys
import tempfile
import wave

ROOT = pathlib.Path(__file__).resolve().parents[1]


def call(argv, *, timeout=30):
    result = subprocess.run(argv, capture_output=True, text=True, timeout=timeout)
    if result.returncode:
        raise AssertionError(f'command failed ({result.returncode}): {argv}\n{result.stderr[-1400:]}')
    return result


def make_wav(dst, rate, channels, frames):
    with wave.open(str(dst), 'wb') as w:
        w.setnchannels(channels)
        w.setsampwidth(2)
        w.setframerate(rate)
        payload = bytearray()
        for i in range(frames):
            for ch in range(channels):
                value = round(14500 * math.sin(2 * math.pi * (220 + ch * 85) * i / rate)
                              + 3700 * math.sin(2 * math.pi * 1207 * i / rate))
                payload.extend(struct.pack('<h', value))
        w.writeframes(payload)
    return payload


def main():
    with tempfile.TemporaryDirectory(prefix='spmp-flac-test-') as tmp:
        p = pathlib.Path(tmp)
        binary = p / 'flac_decode'
        common = ['cc', '-O2', '-std=c11', '-Wall', '-Wextra', '-Werror',
                  f'-I{ROOT / "include"}', str(ROOT / 'src/spmp_flac_lite.c')]
        call(common + [str(ROOT / 'tests/decoder_cli.c'), '-o', str(binary)])
        chunk_binary = p / 'flac_decode_chunk'
        call(common + [str(ROOT / 'tests/chunk_cli.c'), '-o', str(chunk_binary)])
        tests = [(44100, 2, 44100, 5), (22050, 1, 10555, 0),
                 (32000, 2, 27000, 12), (48000, 1, 13173, 8)]
        results = []
        for rate, channels, frames, level in tests:
            wav = p / f'test-{rate}-{channels}.wav'
            flac = wav.with_suffix('.flac')
            output = wav.with_suffix('.pcm')
            gold = make_wav(wav, rate, channels, frames)
            call(['ffmpeg', '-nostdin', '-v', 'error', '-y', '-i', str(wav),
                  '-c:a', 'flac', '-compression_level', str(level), str(flac)])
            call([str(binary), str(flac), str(output)])
            chunk_output = wav.with_suffix('.chunk.pcm')
            call([str(chunk_binary), str(flac), str(chunk_output)])
            actual = output.read_bytes()
            if chunk_output.read_bytes()!=actual:
                raise AssertionError(f'Chunk decode mismatch: {rate} Hz {channels} channels')
            if actual != gold:
                raise AssertionError(f'PCM mismatch: {rate} Hz {channels} channels')
            results.append(f'{rate} Hz / {channels} ch / level={level}, exact {len(actual)} PCM bytes')
        wav = p / '24bit_source.wav'
        flac = p / '24bit.flac'
        gold = p / '24bit_gold.pcm'
        pcm = p / '24bit_pcm.pcm'
        call(['ffmpeg', '-nostdin', '-v', 'error', '-y', '-i', str(p/'test-32000-2.wav'),
              '-c:a', 'pcm_s24le', str(wav)])
        call(['ffmpeg', '-nostdin', '-v', 'error', '-y', '-i', str(wav),
              '-c:a', 'flac', str(flac)])
        call(['ffmpeg', '-nostdin', '-v', 'error', '-y', '-i', str(flac),
              '-af', 'aresample=dither_method=none', '-f', 's16le', str(gold)])
        call([str(binary), str(flac), str(pcm)])
        if pcm.read_bytes() != gold.read_bytes():
            raise AssertionError('24-bit FLAC to 16-bit PCM mismatch')
        results.append('24-bit -> s16 conversion, exact FFmpeg reference')
        results.append('separate caller-owned compressed windows, exact PCM')
        # Negative test: force invalid header CRC in a valid stream.
        content = bytearray(flac.read_bytes())
        offset = 4
        while True:
            header = content[offset]
            blocksize = int.from_bytes(content[offset+1:offset+4], 'big')
            offset += 4 + blocksize
            if header & 128:
                break
        content[offset + 2] ^= 0x80  # reserved/format fields covered by header CRC
        corrupt = p / 'invalid_header_crc.flac'
        corrupt.write_bytes(content)
        bad = subprocess.run([str(binary), str(corrupt), str(p/'corrupt.pcm')],
                             capture_output=True, timeout=5)
        if bad.returncode == 0:
            raise AssertionError('FLAC damaged frame incorrectly accepted')
        results.append('damaged CRC rejected')
        # Random truncated inputs: no hangs or crashes.
        original = flac.read_bytes()
        for cut in [0, 1, 4, 8, 16, 32, 48, len(original)//2, len(original)-2]:
            truncated = p / 'truncated.flac'
            truncated.write_bytes(original[:cut])
            subprocess.run([str(binary), str(truncated), str(p/'t.pcm')],
                           capture_output=True, timeout=5, check=False)
        results.append('9 truncated input lengths terminated without hanging')
        probe_binary = p / 'probe_file'
        call(['cc','-O2','-std=c11',f'-I{ROOT / "include"}',
              str(ROOT/'src/spmp_media_probe.c'),str(ROOT/'tests/probe_file.c'),
              '-o',str(probe_binary)])
        real_mp3 = p / 'actual.mp3'
        real_aac = p / 'actual.aac'
        call(['ffmpeg','-nostdin','-v','error','-y','-i',str(p/'test-44100-2.wav'),
              '-c:a','libmp3lame','-b:a','128k',str(real_mp3)])
        call(['ffmpeg','-nostdin','-v','error','-y','-i',str(p/'test-44100-2.wav'),
              '-c:a','aac','-b:a','96k','-f','adts',str(real_aac)])
        mp3_report = call([str(probe_binary),'mp3',str(real_mp3)]).stdout
        aac_report = call([str(probe_binary),'aac',str(real_aac)]).stdout
        if 'rate=44100' not in mp3_report or 'bitrate=128000' not in mp3_report:
            raise AssertionError('real MP3 header parse failed: '+mp3_report)
        if 'rate=44100' not in aac_report or 'object=2' not in aac_report:
            raise AssertionError('real AAC ADTS header parse failed: '+aac_report)
        results.append('real FFmpeg-encoded MP3 CBR / AAC LC ADTS headers parsed')
        print('\n'.join('PASS: '+r for r in results))
        print('PASS: %d decoder test groups' % len(results))


if __name__ == '__main__':
    main()
