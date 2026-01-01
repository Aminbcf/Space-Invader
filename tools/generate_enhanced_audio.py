#!/usr/bin/env python3
"""
Generate enhanced audio for Space Invaders
Creates improved shooting sound, boss music, and victory music
"""
import numpy as np
import struct
import os

def write_wav(filename, samples, sample_rate=44100):
    """Write samples to a WAV file"""
    # Handle stereo (2 channels)
    if len(samples.shape) == 2:
        num_channels = samples.shape[1]
        # Interleave channels
        samples_interleaved = samples.flatten('F')
    else:
        num_channels = 1
        samples_interleaved = samples
    
    # Normalize samples to 16-bit range
    samples_interleaved = np.clip(samples_interleaved, -1.0, 1.0)
    samples_interleaved = (samples_interleaved * 32767).astype(np.int16)
    
    # WAV file header
    num_samples = len(samples_interleaved) // num_channels
    bits_per_sample = 16
    byte_rate = sample_rate * num_channels * bits_per_sample // 8
    block_align = num_channels * bits_per_sample // 8
    data_size = len(samples_interleaved) * 2  # 2 bytes per sample
    
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
        f.write(samples_interleaved.tobytes())

def generate_improved_shooting_sound():
    """Generate improved player shooting sound - rising laser pew"""
    sample_rate = 44100
    duration = 0.1  # 100ms
    t = np.linspace(0, duration, int(sample_rate * duration))
    
    # Rising frequency sweep (low to high for "pew" effect)
    f0 = 300  # Start frequency
    f1 = 1200  # End frequency
    frequency = f0 + (f1 - f0) * (t / duration)
    
    # Generate sound with sharp attack, quick decay
    sound = np.sin(2 * np.pi * frequency * t)
    
    # Sharp envelope - quick attack, exponential decay
    attack_time = 0.005
    attack_samples = int(attack_time * sample_rate)
    envelope = np.ones_like(t)
    envelope[:attack_samples] = np.linspace(0, 1, attack_samples)
    envelope[attack_samples:] = np.exp(-15 * (t[attack_samples:] - attack_time) / duration)
    
    sound = sound * envelope * 0.6
    
    return sound, sample_rate

def generate_boss_music():
    """Generate intense boss fight music"""
    sample_rate = 44100
    duration = 30  # 30 second loop
    bpm = 140
    beat_duration = 60.0 / bpm
    
    t = np.linspace(0, duration, int(sample_rate * duration))
    
    # D minor scale frequencies (D E F G A Bb C)
    d_minor = {
        'D3': 146.83, 'E3': 164.81, 'F3': 174.61, 'G3': 196.00,
        'A3': 220.00, 'Bb3': 233.08, 'C4': 261.63,
        'D4': 293.66, 'E4': 329.63, 'F4': 349.23, 'G4': 392.00,
        'A4': 440.00, 'Bb4': 466.16, 'C5': 523.25, 'D5': 587.33
    }
    
    # Bass line (driving eighth notes on D)
    bass = np.zeros_like(t)
    bass_freq = d_minor['D3']
    for i in range(int(duration / (beat_duration / 2))):
        start = i * beat_duration / 2
        end = start + beat_duration / 4
        mask = (t >= start) & (t < end)
        bass[mask] += np.sin(2 * np.pi * bass_freq * t[mask])
    
    # Melody - repeating dramatic pattern
    melody = np.zeros_like(t)
    pattern = ['D4', 'D4', 'E4', 'F4', 'G4', 'F4', 'E4', 'D4',
               'A4', 'A4', 'Bb4', 'C5', 'Bb4', 'A4', 'G4', 'F4']
    
    for i, note in enumerate(pattern * (int(duration / (len(pattern) * beat_duration)) + 1)):
        if i * beat_duration >= duration:
            break
        start = i * beat_duration
        end = start + beat_duration * 0.8
        mask = (t >= start) & (t < end)
        freq = d_minor.get(note, d_minor['D4'])
        melody[mask] += np.sin(2 * np.pi * freq * t[mask]) * np.exp(-3 * (t[mask] - start))
    
    # Mix and envelope
    music = bass * 0.4 + melody * 0.6
    
    # Normalize
    music = music / (np.max(np.abs(music)) + 0.001) * 0.7
    
    # Create stereo
    stereo = np.column_stack([music, music])
    
    return stereo, sample_rate

def generate_victory_music():
    """Generate triumphant victory music"""
    sample_rate = 44100
    duration = 8  # 8 seconds
    bpm = 120
    beat_duration = 60.0 / bpm
    
    t = np.linspace(0, duration, int(sample_rate * duration))
    
    # C major scale
    c_major = {
        'C4': 261.63, 'D4': 293.66, 'E4': 329.63, 'F4': 349.23,
        'G4': 392.00, 'A4': 440.00, 'B4': 493.88, 'C5': 523.25,
        'D5': 587.33, 'E5': 659.25, 'F5': 698.46, 'G5': 783.99
    }
    
    # Triumphant fanfare melody
    melody_notes = [
        'C4', 'E4', 'G4', 'C5',  # Rising arpeggio
        'G4', 'E4', 'C5', 'C5',  # Emphatic
        'D5', 'D5', 'C5', 'B4',  # Descending
        'C5', 'C5', 'C5', 'C5'   # Resolution
    ]
    
    # Harmony (thirds below)
    harmony_notes = [
        'E4', 'C4', 'E4', 'G4',
        'E4', 'C4', 'A4', 'A4',
        'B4', 'B4', 'A4', 'G4',
        'E4', 'E4', 'E4', 'E4'
    ]
    
    melody = np.zeros_like(t)
    harmony = np.zeros_like(t)
    
    for i, (m_note, h_note) in enumerate(zip(melody_notes, harmony_notes)):
        if i * beat_duration >= duration:
            break
        start = i * beat_duration
        end = start + beat_duration * 0.9
        mask = (t >= start) & (t < end)
        
        # Melody
        m_freq = c_major.get(m_note, c_major['C4'])
        melody[mask] += np.sin(2 * np.pi * m_freq * t[mask]) * np.exp(-2 * (t[mask] - start))
        
        # Harmony
        h_freq = c_major.get(h_note, c_major['C4'])
        harmony[mask] += np.sin(2 * np.pi * h_freq * t[mask]) * np.exp(-2 * (t[mask] - start))
    
    # Mix
    music = melody * 0.6 + harmony * 0.4
    
    # Add some bass
    bass = np.zeros_like(t)
    bass_pattern = ['C4', 'C4', 'G4', 'G4', 'A4', 'A4', 'C4', 'C4']
    for i, note in enumerate(bass_pattern * 2):
        if i * beat_duration >= duration:
            break
        start = i * beat_duration
        end = start + beat_duration / 2
        mask = (t >= start) & (t < end)
        freq = c_major.get(note, c_major['C4']) / 2  # One octave down
        bass[mask] += np.sin(2 * np.pi * freq * t[mask])
    
    music = music + bass * 0.3
    
    # Normalize
    music = music / (np.max(np.abs(music)) + 0.001) * 0.8
    
    # Fade out at the end
    fade_duration = 1.0
    fade_start = duration - fade_duration
    fade_mask = t >= fade_start
    fade_env = np.ones_like(t)
    fade_env[fade_mask] = np.linspace(1, 0, np.sum(fade_mask))
    music = music * fade_env
    
    # Create stereo
    stereo = np.column_stack([music, music])
    
    return stereo, sample_rate

def main():
    output_dir = "src/assets"
    
    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    
    print("Generating improved shooting sound...")
    sound, sr = generate_improved_shooting_sound()
    write_wav(f"{output_dir}/shooting_improved.wav", sound, sr)
    print(f"  ✓ Created {output_dir}/shooting_improved.wav")
    
    print("Generating boss fight music...")
    music, sr = generate_boss_music()
    write_wav(f"{output_dir}/music_boss.wav", music, sr)
    print(f"  ✓ Created {output_dir}/music_boss.wav")
    
    print("Generating victory music...")
    music, sr = generate_victory_music()
    write_wav(f"{output_dir}/music_victory.wav", music, sr)
    print(f"  ✓ Created {output_dir}/music_victory.wav")
    
    print("\n✓ All enhanced audio files generated successfully!")
    print("\nAudio files created:")
    print("  - shooting_improved.wav: Sharp laser effect (100ms)")
    print("  - music_boss.wav: Intense D minor boss music (30s loop)")
    print("  - music_victory.wav: Triumphant C major fanfare (8s)")

if __name__ == "__main__":
    main()
