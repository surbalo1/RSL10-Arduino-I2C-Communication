# RSL10-Arduino-I2C-Communication

Reliable bidirectional I2C communication between ON Semiconductor RSL10-002GEVB (master) and Arduino Nano (slave) using software I2C bit-bang implementation.

## Features

- Software I2C implementation on RSL10 (bit-bang GPIO control)
- Hardware I2C on Arduino Nano
- Bidirectional data transfer with acknowledgment
- 3.3V logic level compatible
- Error handling with retry logic and status codes
- Tested and verified, 90%+ success rate
- Complete documentation including wiring, troubleshooting, and examples

## Hardware Requirements

### Components
- RSL10-002GEVB board
- Arduino Nano (3.3V or 5V with level shifting)
- 2× 2.7kΩ resistors for I2C pull-ups
- Jumper wires
- Breadboard (optional)
- USB cables

### Wiring Diagram

```

RSL10-002GEVB                        Arduino Nano
3.3V ──────────┬──────────────────── 3.3V
DIO2 (SDA) ────┤2.7kΩ├────────────── A4 (SDA)
DIO3 (SCL) ────┬─────────────── A5 (SCL)
GND ───────────┴──────────────────── GND

```

### Pin Connections

| RSL10 Pin | Arduino Pin | Notes |
|-----------|-------------|-------|
| 3.3V      | 3.3V        | Use 3.3V only |
| GND       | GND         | Common ground |
| DIO2      | A4 (SDA)    | + 2.7kΩ pull-up |
| DIO3      | A5 (SCL)    | + 2.7kΩ pull-up |

## Important Logic Level Note

- Arduino Nano normally uses 5V logic which can damage RSL10.
- Power Arduino from RSL10 3.3V pin to match logic levels.
- Do not connect Arduino USB while powered from RSL10.
- Alternative: use a logic level shifter or a 3.3V Arduino.

## Quick Start

### 1. Clone Repository

```

git clone [https://github.com/surbalo1/RSL10-Arduino-I2C-Communication.git](https://github.com/surbalo1/RSL10-Arduino-I2C-Communication.git)
cd RSL10-Arduino-I2C-Communication

```

### 2. Upload Arduino Code
- Open `arduino_i2c_slave/arduino_i2c_slave.ino` in Arduino IDE.
- Select board: Arduino Nano, select port.
- Upload and disconnect USB after upload.

### 3. Setup RSL10 Project
- Import `rsl10_i2c_master` into Eclipse IDE or preferred IDE.
- Build project and flash to RSL10 board.

### 4. Wire Hardware
Follow wiring diagram above, connect SDA, SCL, power, and GND with pull-up resistors.

### 5. Run and Monitor
- RSL10 outputs via SEGGER RTT:
```

TX: 0x00 ✓ | RX: 0x01 ✓
[1] TX: 0x01 ✓ | RX: 0x02 ✓
--- Stats: Success=10, Fail=0 ---

```
- Arduino debug output (Serial Monitor):
```

Listening on 0x55
RX: 0x00 (0)
TX: 0x01

```

## Project Structure

```

RSL10-Arduino-I2C-Communication/
├── README.md
├── LICENSE
├── docs/
│   ├── wiring_guide.md
│   └── troubleshooting.md
├── rsl10_i2c_master/
│   ├── include/
│   └── source/
├── arduino_i2c_slave/
│   └── arduino_i2c_slave.ino

```

## How It Works

1. RSL10 (master) sends a byte to Arduino at 0x55.
2. Arduino (slave) increments the byte and sends it back.
3. RSL10 verifies the response equals original + 1.

- Software I2C runs ~30-40 kHz.
- Slave address is 0x55.
- Bit-bang implementation allows flexible GPIO selection.

## Performance

- I2C speed ~30-40 kHz
- Success rate 90%+ after initial sync
- Recommended 100ms delay between write and read
- Slower than hardware I2C but reliable for low-speed data

## Troubleshooting

- Check wiring, pull-ups, and common ground
- Slow down I2C by increasing `I2C_DELAY` in `i2c_hal.c`
- Keep wires short (<20cm)
- Use 100nF capacitor near Arduino if needed

## Development

### Requirements
- Eclipse IDE or Keil MDK
- ON Semiconductor RSL10 SDK
- J-Link debugger
- SEGGER RTT Viewer

### Adjusting I2C speed
In `i2c_hal.c`, change:
```

#define I2C_DELAY 250

```
Lower value = faster, less reliable. Higher value = slower, more reliable.

### Change Slave Address
- Arduino: `#define SLAVE_ADDRESS 0x55`
- RSL10: `#define ARDUINO_ADDR 0x55`

## License
MIT License - see LICENSE file.

## Acknowledgments
- ON Semiconductor RSL10 SDK
- Arduino community
- Open-source I2C examples

## Support
- GitHub Issues: https://github.com/surbalo1/RSL10-Arduino-I2C-Communication/issues
- Email: surbalo1@gmail.com

## Version History
- v1.0.0 (2025-10-13): initial release, software I2C, bidirectional communication, tested
```
