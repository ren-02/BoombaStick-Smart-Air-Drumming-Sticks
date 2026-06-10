"""
BoombaStick - Host Sound Player (Left Stick)

Receives drum hit events over serial from the left BoombaStick
and plays the corresponding drum sample using pygame.

Usage:
    1. Update COMPORT to match your Arduino's serial port.
    2. Run: python playsound_left.py
"""

import serial
import time
from pygame import mixer

# ======================== CONFIGURATION ========================
# Change this to match your Arduino's COM port
COMPORT = 'COM8'
BAUD_RATE = 230400
AUDIO_BUFFER_SIZE = 256  # Lower = less latency (try 512 if you hear clicks)
# ===============================================================

# === AUDIO SETUP ===
mixer.init(frequency=44100, buffer=AUDIO_BUFFER_SIZE)
mixer.set_num_channels(16)

print("Loading samples...")
try:
    snare = mixer.Sound(r'SAMPLES\SNARES\07_Snare_16_SP.wav')
    hihat = mixer.Sound(r'SAMPLES\HIHATS\Boom-Bap Hat CL 53.wav')
    tom = mixer.Sound(r'SAMPLES\TOMS\TOM.wav')
    crash = mixer.Sound(r'SAMPLES\CYMBALS\07_Perc_05_SP.wav')
    ride = mixer.Sound(r'SAMPLES\CYMBALS\MachineRide.wav')
    kick = mixer.Sound(r'SAMPLES\KICKS\Boom-Bap Kick 53.wav')
except FileNotFoundError as e:
    print(f"\n--- ERROR ---")
    print(f"Could not load sample file: {e.filename}")
    print("Please check your 'SAMPLES' folder path and file names.")
    print("Exiting.")
    exit()


drums = [snare, hihat, tom, crash, ride, kick]
DRUM_NAMES = ['Snare', 'Hi-Hat', 'Tom', 'Crash', 'Ride', 'Kick']

for d in drums:
    d.set_volume(1.0)

# === SERIAL SETUP ===
ser = serial.Serial(COMPORT, BAUD_RATE, timeout=0)

time.sleep(1)  # Give serial connection time to establish
print(f"Listening on {COMPORT}...")

# Pre-calculate the ASCII offset for '0'
CMD_OFFSET = ord('0')

while True:
    try:
        # Read all waiting bytes from the buffer at once
        if ser.in_waiting > 0:
            data_bytes = ser.read(ser.in_waiting)

            for byte in data_bytes:
                idx = byte - CMD_OFFSET

                if 0 <= idx <= 5:
                    drums[idx].play()
                    # NOTE: This print adds a small amount of latency.
                    # Comment it out for maximum performance.
                    print(f"Played: {DRUM_NAMES[idx]}")

        # Tiny sleep to prevent 100% CPU usage
        time.sleep(0.001)

    except KeyboardInterrupt:
        print("\nExiting...")
        break
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
        time.sleep(0.1)

ser.close()
mixer.quit()
print("Serial port closed. Goodbye.")
