# 🥁 BoombaStick — Smart Air Drumming Sticks

<div align="center">

**Play drums in the air — no physical percussion instruments needed.**

A pair of handheld smart sticks that detect motion and orientation to trigger drum sounds in real time.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Arduino](https://img.shields.io/badge/Platform-Arduino-00979D?logo=arduino)](https://www.arduino.cc/)
[![Python](https://img.shields.io/badge/Python-3.7+-3776AB?logo=python&logoColor=white)](https://www.python.org/)

</div>

---

## 📖 Overview

BoombaStick uses a **BNO055 9-axis IMU** mounted on each stick to track orientation (yaw/pitch). When you swing the stick and hit a virtual drum zone, the Arduino sends a signal over serial to a Python host script, which plays the corresponding drum sample with ultra-low latency.

### How It Works

```
┌──────────────┐    Serial (230400 baud)    ┌──────────────────┐
│  Arduino +   │ ──────────────────────────► │  Python Host     │
│  BNO055 IMU  │    Zone ID ('0'-'5')       │  (pygame mixer)  │
│  + Buttons   │                            │  ► Plays .wav    │
└──────────────┘                            └──────────────────┘
```

**5 Drum Zones** are mapped to spatial orientations:

| Zone ID | Instrument | Trigger |
|---------|-----------|---------|
| `0`     | Snare     | Swing + orientation match |
| `1`     | Hi-Hat    | Swing + orientation match |
| `2`     | Tom       | Swing + orientation match |
| `3`     | Crash     | Swing + orientation match |
| `4`     | Ride      | Swing + orientation match |
| `5`     | Kick      | Button press (pin 4)      |

---

## 📁 Project Structure

```
BoombaStick/
├── firmware/                          # Arduino sketches
│   ├── BoombaStick_Main/              # Main drumstick firmware
│   │   └── BoombaStick_Main.ino
│   └── BoombaStick_Calibration/       # Calibration tool
│       └── BoombaStick_Calibration.ino
├── host/                              # Python host scripts
│   ├── playsound_left.py              # Sound player for left stick
│   ├── playsound_right.py             # Sound player for right stick
│   └── SAMPLES/                       # Drum sample library
│       ├── SNARES/
│       ├── HIHATS/
│       ├── TOMS/
│       ├── CYMBALS/
│       ├── KICKS/
│       └── BASS/
├── requirements.txt                   # Python dependencies
├── CONTRIBUTING.md                    # Contribution guidelines
├── LICENSE                            # MIT License
└── README.md                         # This file
```

---

## 🔧 Hardware Requirements

| Component | Quantity | Notes |
|-----------|----------|-------|
| Arduino board (Nano/Uno) | 2 | One per stick |
| Adafruit BNO055 IMU | 2 | 9-axis absolute orientation sensor |
| Tactile push button | 2–4 | Swing trigger (pin 6) + Kick (pin 4) |
| USB cable | 2 | For serial communication to PC |
| 10kΩ resistors | — | Optional (internal pull-ups are used) |

### Wiring

| BNO055 Pin | Arduino Pin |
|-----------|-------------|
| VIN       | 5V          |
| GND       | GND         |
| SDA       | A4          |
| SCL       | A5          |

| Button        | Arduino Pin | Function          |
|---------------|-------------|-------------------|
| Swing trigger | 6           | Detect intentional hits |
| Kick button   | 4           | Trigger kick drum       |

---

## 🚀 Getting Started

### 1. Install Arduino Libraries

Open the Arduino IDE and install via **Library Manager** (`Sketch → Include Library → Manage Libraries`):

- `Adafruit BNO055`
- `Adafruit Unified Sensor`

### 2. Calibrate Your Sticks

1. Upload `firmware/BoombaStick_Calibration/BoombaStick_Calibration.ino` to your Arduino.
2. Open Serial Monitor at **230400 baud**.
3. **Part 1** — Move the stick in a figure-8 to calibrate the BNO055 (wait for `sys=3`).
4. **Part 2** — Point the stick at each drum zone position and press the button:
   - Snare → Hi-Hat → Tom → Crash → Ride
5. Copy the output values and paste them into `BoombaStick_Main.ino`.

### 3. Flash the Main Firmware

1. Open `firmware/BoombaStick_Main/BoombaStick_Main.ino`.
2. Replace the `DrumZone` values with your calibration output.
3. Upload to each Arduino.

### 4. Set Up the Host (PC)

```bash
# Install Python dependencies
pip install -r requirements.txt

# Edit the COM port in each script
# Then run one script per stick:
cd host
python playsound_left.py     # Terminal 1
python playsound_right.py    # Terminal 2
```

> **Tip:** To find your COM port, open **Device Manager → Ports (COM & LPT)** on Windows, or run `ls /dev/tty.*` on macOS.

---

## ⚙️ Configuration

### Tuning Hit Detection

In `BoombaStick_Main.ino`:

| Parameter | Default | Description |
|-----------|---------|-------------|
| `SWING_THRESHOLD_SQ` | `62500.0` | Minimum angular velocity² to register a swing (250°/s)² |
| `REST_DELAY` | `150` | Cooldown between hits in milliseconds |
| `zoneThresholdSq` | `2500.0` | How "big" each drum zone is (50°)² |

### Audio Latency

In the Python scripts:

| Parameter | Default | Description |
|-----------|---------|-------------|
| `AUDIO_BUFFER_SIZE` | `256` | Lower = less latency. Try `512` if you hear clicks. |

### Swapping Drum Samples

Replace the `.wav` files in `host/SAMPLES/` subdirectories. Use **44100 Hz WAV** format for best compatibility.

---

## 🤝 Contributing

Contributions are welcome! See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).
