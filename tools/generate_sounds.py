#!/usr/bin/env python3
"""
Generate placeholder sound effects for Space Invaders
Creates simple audio files using sine waves
"""
import numpy as np
import struct
import os

def write_wav(filename, samples, sample_rate=44100):
    """Write samples to a WAV file"""
    # Normalize samples to 16-bit range
    samples = np.clip(samples, -1.0, 1.0)
    samples = (samples * 32767).astype(np.int16)
    
    # WAV file header
    num_samples = len(samples)
    num_channels = 1
    bits_per_sample = 16
    byte_rate = sample_rate * num_channels * bits_per_sample // 8
    block_align = num_channels * bits_per_sample // 8
    data_size = num_samples * num_channels * bits_per_sample // 8
    
    with open(filename, 'wb') as f:
        # RIFF header
        f.write(b'RIFF')
        f.write(struct.pack('<I', 36 + data_size))
        f.write(b'WAVE')
        
        # fmt chunk
        f.write(b'fmt ')
        f.write(struct.pack('<I', 16))  # Chunk size
        f.write(struct.pack('<H', 1))   # Audio format (PCM)
        f.write(struct.pack('<H', num_channels))
        f.write(struct.pack('<I', sample_rate))
        f.write(struct.pack('<I', byte_rate))
        f.write(struct.pack('<H', block_align))
        f.write(struct.pack('<H', bits_per_sample))
        
        # data chunk
        f.write(b'data')
        f.write(struct.pack('<I', data_size))
        f.write(samples.tobytes())

def generate_enemy_bullet_sound():
    """Generate enemy bullet sound - lower pitched pew"""
    sample_rate = 44100
    duration = 0.15  # seconds
    t = np.linspace(0, duration, int(sample_rate * duration))
    
    # Descending frequency sweep (higher to lower)
    f0 = 800  # Start frequency
    f1 = 200  # End frequency
    frequency = f0 - (f0 - f1) * (t / duration)
    
    # Generate sound with envelope
    sound = np.sin(2 * np.pi * frequency * t)
    envelope = np.exp(-8 * t / duration)  # Exponential decay
    sound = sound * envelope * 0.5
    
    return sound, sample_rate

def generate_gameover_sound():
    """Generate game over sound - sad descending tones"""
    sample_rate = 44100
    duration = 1.5  # seconds
    t = np.linspace(0, duration, int(sample_rate * duration))
    
    # Three descending tones
    tone1 = np.sin(2 * np.pi * 440 * t)  # A4
    tone2 = np.sin(2 * np.pi * 392 * t)  # G4
    tone3 = np.sin(2 * np.pi * 349 * t)  # F4
    
    # Create segments
    segment_len = len(t) // 3
    sound = np.zeros_like(t)
    sound[:segment_len] = tone1[:segment_len]
    sound[segment_len:2*segment_len] = tone2[segment_len:2*segment_len]
    sound[2*segment_len:] = tone3[2*segment_len:]
    
    # Apply envelope
    envelope = np.concatenate([
        np.ones(segment_len),
        np.ones(segment_len) * 0.8,
        np.linspace(0.8, 0, len(t) - 2*segment_len)
    ])
    sound = sound * envelope * 0.3
    
    return sound, sample_rate

def main():
    output_dir = "src/assets"
    
    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    
    print("Generating enemy bullet sound...")
    sound, sr = generate_enemy_bullet_sound()
    write_wav(f"{output_dir}/enemy_bullet.wav", sound, sr)
    print(f"  ✓ Created {output_dir}/enemy_bullet.wav")
    
    print("Generating game over sound...")
    sound, sr = generate_gameover_sound()
    write_wav(f"{output_dir}/gameover.wav", sound, sr)
    print(f"  ✓ Created {output_dir}/gameover.wav")
    
    print("\n✓ All sound effects generated successfully!")
    print("Note: These are WAV files. You may want to convert them to MP3 for smaller size:")
    print("  ffmpeg -i enemy_bullet.wav -b:a 128k enemy_bullet.mp3")
    print("  ffmpeg -i gameover.wav -b:a 128k gameover.mp3")

if __name__ == "__main__":
    main()
