#!/usr/bin/env python3
"""
Generate action-packed gameplay music
Creates high-energy, fast-tempo music for the main game loop
"""
import numpy as np
import struct
import os

def write_wav(filename, samples, sample_rate=44100):
    """Write samples to a WAV file"""
    if len(samples.shape) == 2:
        num_channels = samples.shape[1]
        samples_interleaved = samples.flatten('F')
    else:
        num_channels = 1
        samples_interleaved = samples
    
    samples_interleaved = np.clip(samples_interleaved, -1.0, 1.0)
    samples_interleaved = (samples_interleaved * 32767).astype(np.int16)
    
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

def generate_action_music():
    """Generate high-energy action music"""
    sample_rate = 44100
    duration = 64  # Roughly 64 seconds loop
    bpm = 150      # Fast tempo!
    beat_duration = 60.0 / bpm
    
    t = np.linspace(0, duration, int(sample_rate * duration))
    
    # E Minor Pentatonic / Blues Scale frequences
    # E2, G2, A2, Bb2, B2, D3, E3...
    
    notes = {
        'E2': 82.41, 'G2': 98.00, 'A2': 110.00, 'Bb2': 116.54, 'B2': 123.47,
        'D3': 146.83, 'E3': 164.81, 'G3': 196.00, 'A3': 220.00, 'Bb3': 233.08, 'B3': 246.94,
        'D4': 293.66, 'E4': 329.63, 'G4': 392.00, 'A4': 440.00, 'B4': 493.88
    }
    
    # 1. Driving Bass Line (E2 pulsating)
    bass = np.zeros_like(t)
    # 16th note pulse
    sixteenth = beat_duration / 4
    num_sixteenths = int(duration / sixteenth)
    
    for i in range(num_sixteenths):
        start = i * sixteenth
        end = start + sixteenth * 0.8 # Staccato
        mask = (t >= start) & (t < end)
        
        # Vary the note slightly every 4th bar
        bar_idx = (i // 16) % 4
        note_key = 'E2'
        if i % 8 == 6: note_key = 'G2'
        if i % 8 == 7: note_key = 'E2'
        
        freq = notes[note_key]
        
        # Square waveish for retro feel
        wave = np.sign(np.sin(2 * np.pi * freq * t[mask])) * 0.5 + \
               np.sin(2 * np.pi * freq * t[mask]) * 0.5
        
        bass[mask] += wave * 0.6
        
    # 2. Arpeggiated Mid-range (Synth)
    mid = np.zeros_like(t)
    arp_pattern = ['E3', 'G3', 'B3', 'E4', 'B3', 'G3', 'E3', 'B2']
    
    for i in range(num_sixteenths):
        start = i * sixteenth
        end = start + sixteenth * 0.9
        if start >= duration: break
        mask = (t >= start) & (t < end)
        
        note = arp_pattern[i % 8]
        freq = notes[note]
        
        # Sawtooth-like
        wave = ((t[mask] * freq) % 1.0) * 2.0 - 1.0
        
        # Filter sweep effect (LFO)
        lfo = 0.5 + 0.5 * np.sin(2 * np.pi * 0.1 * t[mask]) # Slow sweep
        mid[mask] += wave * 0.3 * lfo

    # 3. High Melody (comes in later)
    melody = np.zeros_like(t)
    # Simple dramatic stabs
    stab_pattern = [
        ('E4', 4.0), ('G4', 4.0), ('A4', 4.0), ('B4', 4.0)
    ]
    
    for i in range(int(duration / beat_duration)):
        if i % 16 < 8: continue # Rest first half of phrase
        
        start = i * beat_duration
        end = start + beat_duration * 0.5
        mask = (t >= start) & (t < end)
        
        note_idx = (i // 4) % 4
        note = stab_pattern[note_idx][0]
        freq = notes[note]
        
        wave = np.sin(2 * np.pi * freq * t[mask]) + \
               0.5 * np.sin(2 * np.pi * freq * 2 * t[mask])
        
        melody[mask] += wave * 0.4

    # 4. Percussion (White Noise Hi-hats)
    drums = np.zeros_like(t)
    for i in range(num_sixteenths):
        if i % 2 == 0: continue # 8th notes
        start = i * sixteenth
        end = start + sixteenth * 0.2 # Short tick
        if start >= duration: break
        mask = (t >= start) & (t < end)
        
        noise = np.random.uniform(-1, 1, np.sum(mask))
        drums[mask] += noise * 0.15

    # Kick drum on beats
    kick = np.zeros_like(t)
    for i in range(int(duration / beat_duration)):
        start = i * beat_duration
        end = start + 0.1
        if start >= duration: break
        mask = (t >= start) & (t < end)
        
        # Frequency sweep drop
        f_sweep = np.linspace(150, 50, np.sum(mask))
        kick[mask] += np.sin(2 * np.pi * f_sweep * (t[mask]-start)) * 0.8

    # Mix Layers
    mix = bass + mid + melody + drums + kick
    
    # Normalize
    mix = mix / (np.max(np.abs(mix)) + 0.001) * 0.85
    
    return np.column_stack([mix, mix]), sample_rate

def main():
    # Write directly to assets_dir and bin/assets/ to ensure it's picked up
    output_dirs = ["src/assets", "bin/assets"]
    
    print("Generating High-Octane Action Music...")
    music, sr = generate_action_music()
    
    for d in output_dirs:
        if os.path.exists(os.path.dirname(d)) or os.path.exists(d):
            os.makedirs(d, exist_ok=True)
            fname = f"{d}/music_game.wav"
            write_wav(fname, music, sr)
            print(f"  ✓ Created {fname}")

if __name__ == "__main__":
    main()
