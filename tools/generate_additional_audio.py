#!/usr/bin/env python3
"""
Generate additional audio for Space Invaders
Creates game music and damage sound
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
    data_size = len(samples_interleaved) * 2
    
    with open(filename, 'wb') as f:
        f.write(b'RIFF')
        f.write(struct.pack('<I', 36 + data_size))
        f.write(b'WAVE')
        f.write(b'fmt ')
        f.write(struct.pack('<I', 16))
        f.write(struct.pack('<H', 1))
        f.write(struct.pack('<H', num_channels))
        f.write(struct.pack('<I', sample_rate))
        f.write(struct.pack('<I', byte_rate))
        f.write(struct.pack('<H', block_align))
        f.write(struct.pack('<H', bits_per_sample))
        f.write(b'data')
        f.write(struct.pack('<I', data_size))
        f.write(samples_interleaved.tobytes())

def generate_game_music():
    """Generate energetic action music for levels 1-3"""
    sample_rate = 44100
    duration = 25  # 25 second loop
    bpm = 130
    beat_duration = 60.0 / bpm
    
    t = np.linspace(0, duration, int(sample_rate * duration))
    
    # A minor scale frequencies
    a_minor = {
        'A2': 110.00, 'B2': 123.47, 'C3': 130.81, 'D3': 146.83,
        'E3': 164.81, 'F3': 174.61, 'G3': 196.00,
        'A3': 220.00, 'B3': 246.94, 'C4': 261.63, 'D4': 293.66,
        'E4': 329.63, 'F4': 349.23, 'G4': 392.00, 'A4': 440.00,
        'B4': 493.88, 'C5': 523.25, 'D5': 587.33, 'E5': 659.25
    }
    
    # Driving bass line (eighth notes)
    bass = np.zeros_like(t)
    bass_pattern = ['A2', 'A2', 'E3', 'E3', 'D3', 'D3', 'E3', 'E3']
    for i, note in enumerate(bass_pattern * (int(duration / (len(bass_pattern) * beat_duration)) + 1)):
        if i * beat_duration >= duration:
            break
        start = i * beat_duration
        end = start + beat_duration * 0.4
        mask = (t >= start) & (t < end)
        freq = a_minor.get(note, a_minor['A2'])
        bass[mask] += np.sin(2 * np.pi * freq * t[mask])
    
    # Energetic melody
    melody = np.zeros_like(t)
    melody_pattern = [
        'A4', 'E4', 'A4', 'B4', 'C5', 'B4', 'A4', 'E4',  # Rising phrase
        'G4', 'D4', 'G4', 'A4', 'G4', 'F4', 'E4', 'D4',  # Answer phrase
        'A4', 'A4', 'C5', 'C5', 'D5', 'E5', 'D5', 'C5',  # Climax
        'B4', 'A4', 'G4', 'E4', 'A4', 'A4', 'A4', 'A4'   # Resolution
    ]
    
    for i, note in enumerate(melody_pattern * (int(duration / (len(melody_pattern) * beat_duration)) + 1)):
        if i * beat_duration >= duration:
            break
        start = i * beat_duration
        end = start + beat_duration * 0.7
        mask = (t >= start) & (t < end)
        freq = a_minor.get(note, a_minor['A4'])
        melody[mask] += np.sin(2 * np.pi * freq * t[mask]) * np.exp(-2 * (t[mask] - start))
    
    # Percussion-like rhythm (short bursts)
    percussion = np.zeros_like(t)
    for i in range(int(duration / (beat_duration / 2))):
        start = i * beat_duration / 2
        end = start + 0.02  # Very short burst
        mask = (t >= start) & (t < end)
        # Noise-like percussive sound
        percussion[mask] += np.sin(2 * np.pi * 80 * t[mask]) * 0.3
    
    # Mix
    music = bass * 0.5 + melody * 0.6 + percussion * 0.2
    
    # Normalize
    music = music / (np.max(np.abs(music)) + 0.001) * 0.7
    
    # Create stereo
    stereo = np.column_stack([music, music])
    
    return stereo, sample_rate

def generate_damage_sound():
    """Generate player damage/hit sound"""
    sample_rate = 44100
    duration = 0.2  # 200ms
    t = np.linspace(0, duration, int(sample_rate * duration))
    
    # Descending frequency sweep (impact sound)
    f0 = 600  # Start high
    f1 = 150  # End low
    frequency = f0 - (f0 - f1) * (t / duration)
    
    # Generate impact sound with noise component
    tone = np.sin(2 * np.pi * frequency * t)
    
    # Add noise for "thud" quality
    noise = np.random.normal(0, 0.3, len(t))
    
    # Mix tone and noise
    sound = tone * 0.7 + noise * 0.3
    
    # Envelope - sharp attack, sustain, decay
    attack_time = 0.01
    decay_time = 0.05
    attack_samples = int(attack_time * sample_rate)
    decay_start_samples = int((duration - decay_time) * sample_rate)
    
    envelope = np.ones_like(t)
    envelope[:attack_samples] = np.linspace(0, 1, attack_samples)
    envelope[decay_start_samples:] = np.linspace(1, 0, len(t) - decay_start_samples)
    
    sound = sound * envelope * 0.8
    
    return sound, sample_rate

def main():
    output_dir = "src/assets"
    
    os.makedirs(output_dir, exist_ok=True)
    
    print("Generating game music...")
    music, sr = generate_game_music()
    write_wav(f"{output_dir}/music_game.wav", music, sr)
    print(f"  ✓ Created {output_dir}/music_game.wav")
    
    print("Generating damage sound...")
    sound, sr = generate_damage_sound()
    write_wav(f"{output_dir}/damage.wav", sound, sr)
    print(f"  ✓ Created {output_dir}/damage.wav")
    
    print("\n✓ All audio files generated successfully!")
    print("\nAudio files created:")
    print("  - music_game.wav: Energetic A minor action music (25s loop)")
    print("  - damage.wav: Player damage impact sound (200ms)")

if __name__ == "__main__":
    main()
