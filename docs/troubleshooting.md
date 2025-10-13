# Troubleshooting Guide

Complete troubleshooting guide for RSL10–Arduino I2C communication issues.

## Table of Contents
- [No Communication](#no-communication)
- [Logic Level Issues](#logic-level-issues)
- [Data Corruption](#data-corruption)
- [Intermittent Communication](#intermittent-communication)
- [Hardware Issues](#hardware-issues)
- [Software Issues](#software-issues)

---

## No Communication

### Symptom
- RSL10 scanner detects no devices  
- All transactions return NACK  
- Arduino never receives data  

### Diagnosis Steps

1. **Check Power**
```

* Arduino LED should be ON
* Measure voltage at Arduino 3.3V pin: around 3.3V
* Measure voltage at RSL10 3.3V output: around 3.0–3.3V

```

2. **Check Ground Connection**
```

* Common ground is required
* Measure continuity between RSL10 GND and Arduino GND
* Resistance should be below 1Ω

```

3. **Verify Wiring**
```

✓ RSL10 DIO2 → Arduino A4 (SDA)
✓ RSL10 DIO3 → Arduino A5 (SCL)
✓ RSL10 GND → Arduino GND
✓ RSL10 3.3V → Arduino 3.3V (not 5V)

```

4. **Check Pull-up Resistors**
```

* Measure resistance from SDA to 3.3V: about 2.7kΩ
* Measure resistance from SCL to 3.3V: about 2.7kΩ

```

### Solutions

**Fix Power Connection**
```

RSL10 3.3V → Arduino 3.3V pin
(Do not power Arduino from USB while testing)

```

**Add Pull-up Resistors**
```

If missing:

* 2.7kΩ from SDA to 3.3V
* 2.7kΩ from SCL to 3.3V

```

**Check Arduino Code**
```

LED should blink at startup.
If not: re-upload code via USB, then disconnect USB before running.

````

---

## Logic Level Issues

### Symptom
- Works for a short time then fails  
- Random bit errors  
- RSL10 may get damaged after connection  

### Cause
Arduino Nano uses 5V logic, while RSL10 uses 3.3V. Direct connection can damage RSL10.

### Diagnosis
```cpp
void setup() {
  pinMode(A4, OUTPUT);
  digitalWrite(A4, HIGH);
}
void loop() { }

// Measure voltage at A4:
// ~5V → Problem (5V logic)
// ~3.3V → OK (3.3V logic)
````

### Solutions

**Run Arduino at 3.3V**

```
✓ RSL10 3.3V → Arduino 3.3V
✓ Do not connect 5V or USB during operation
```

**Use Logic Level Shifter**

```
Recommended for long-term use.

[5V Arduino]           [Level Shifter]           [3.3V RSL10]
A4 ────────────────> HV1 → LV1 ─────────────── DIO2
A5 ────────────────> HV2 → LV2 ─────────────── DIO3
5V ────────────────> HV VCC
3.3V ──────────────> LV VCC
GND ───────────────> Common GND

Suggested board: SparkFun BOB-12009 or similar
```

**Use a 3.3V Arduino**

```
Arduino Nano 33 IoT or Nano 33 BLE works natively at 3.3V.
```

---

## Data Corruption

### Symptom

* RSL10 receives wrong values (bit errors)
* Example: Sent 0x1D, received 0x8F
* Data correct only sometimes

### Cause

Timing problems in software I2C.

### Solutions

**Slow Down I2C Speed**

```c
// In i2c_hal.c
#define I2C_DELAY 350  // Try 350–500 for better stability
```

**Lower Arduino I2C Speed**

```c
Wire.begin(0x55);
Wire.setClock(10000); // 10 kHz
```

**Add Delay Between Transactions**

```c
Sys_Delay_ProgramROM((uint32_t)(0.1 * SystemCoreClock));  // 100 ms
```

**Reduce Noise**

```
- Keep wires under 20 cm
- Twist SDA and SCL together
- Keep away from power lines
- Add 100nF capacitor between Arduino VCC and GND
```

---

## Intermittent Communication

### Symptom

* Works sometimes, fails sometimes
* Success rate around 30–70%
* Fails more with certain byte values

### Common Causes

**Loose Connections**

```
Wiggle wires during test. If failures appear, rewire or solder connections.
```

**Initial Sync Errors**

```c
for(int retry = 0; retry < 3; retry++) {
    status = I2C_Write(addr, data, len);
    if(status == I2C_STATUS_OK) break;
    Sys_Delay_ProgramROM((uint32_t)(0.05 * SystemCoreClock));
}
```

**Power Conflicts**

```
Never power Arduino with both USB and RSL10 3.3V at the same time.
```

**Weak Pull-ups**

```
Try stronger ones:
2.2kΩ or 1.8kΩ
Don’t go below 1kΩ.
```

---

## Hardware Issues

### Arduino Not Responding

**Power Check**

```
LED must be ON.
Check RSL10 → Arduino 3.3V connection.
```

**Code Check**

```
LED should blink at startup.
If not, upload code again.
```

**I2C Address**

```c
Wire.begin(0x55);
```

**Pin Order**

```
Correct:
  DIO2 → A4 (SDA)
  DIO3 → A5 (SCL)
```

---

### RSL10 Output Pins Not Working

```c
Sys_DIO_Config(2, DIO_MODE_GPIO_OUT_0);
Sys_DIO_Config(3, DIO_MODE_GPIO_OUT_0);

while(1) {
    Sys_GPIO_Set_High(2);
    Sys_GPIO_Set_High(3);
    Sys_Delay_ProgramROM(SystemCoreClock);
    Sys_GPIO_Set_Low(2);
    Sys_GPIO_Set_Low(3);
    Sys_Delay_ProgramROM(SystemCoreClock);
}
```

Voltage should toggle between 0V and 3.3V on both pins.

---

## Software Issues

### Compilation Errors

**Missing Headers**

```c
#include <rsl10.h>
#include <rsl10_sys_gpio.h>
#include "i2c_hal.h"
```

**Undefined Functions**

```
Make sure RSL10 SDK is installed correctly.
Include paths:
- RSL10/include
- CMSIS/Include
```

### RTT Not Working

```c
SEGGER_RTT_Init();
```

**RTT Viewer Settings**

```
Connection: J-Link
Device: RSL10
Interface: SWD
Speed: 4000 kHz
RTT Control Block: Auto
```

### Arduino Serial Monitor Not Showing Output

```
- Baud rate mismatch: use 9600
- Add delay(1000) in setup()
- USB disconnected during test: reconnect temporarily
```

---

## Performance Optimization

**Improve Reliability**

```c
Sys_Delay_ProgramROM((uint32_t)(0.5 * SystemCoreClock)); // 500 ms
```

**Retry Failed Transactions**

```c
uint8_t attempts = 3;
while(attempts--) {
    status = I2C_Write(addr, data, len);
    if(status == I2C_STATUS_OK) break;
    Sys_Delay_ProgramROM((uint32_t)(0.1 * SystemCoreClock));
}
```

**Lower Speed for Stability**

```c
#define I2C_DELAY 500
```

**Increase Speed (risky)**

```c
#define I2C_DELAY 150  // ~60 kHz
#define I2C_DELAY 100  // ~90 kHz
```

---

## Advanced Debugging

**Logic Analyzer**

```
Channel 0 → SDA
Channel 1 → SCL
GND → Common ground
Sample rate: ≥1 MHz
Decoder: I2C
Threshold: 1.65V
```

Check for:

```
✓ START/STOP conditions
✓ 8 data bits + ACK
✗ Missing ACK or noise spikes
```

**Oscilloscope**

```
Rise time < 1µs
Overshoot < 0.5V
Ground noise < 50mV
```

---

## Getting Help

Before reporting issues:

* Check this guide
* Verify wiring
* Test with known good hardware
* Include oscilloscope or analyzer captures

**Include in reports:**

* Hardware setup details
* Software versions
* Logs or outputs
* What fixes you already tried

---

## Known Limitations

**Software I2C Limits**

1. Speed: ~30–40 kHz typical
2. High CPU usage during transfers
3. Timing varies with interrupts
4. No clock stretching

**Workarounds**

```
Use hardware I2C or DMA for faster or non-blocking communication.
```

---

## Success Criteria

Communication is **working** when:

```
✓ >90% success rate after first 10 transfers
✓ Data matches sent value + 1
✓ No bit errors
✓ Runs stable for 1000+ transfers
```

Some initial failures are normal.

---

## References

* [RSL10 Datasheet](https://www.onsemi.com/pdf/datasheet/rsl10-d.pdf)
* [Arduino Wire Library](https://www.arduino.cc/reference/en/language/functions/communication/wire/)
* [I2C Specification](https://www.nxp.com/docs/en/user-guide/UM10204.pdf)
* [Software I2C Guide](https://www.embedded.com/implementing-i2c-in-software/)

---

**Last Updated:** 2025-10-13
**Version:** 1.0
**Status:** Verified Working ✅