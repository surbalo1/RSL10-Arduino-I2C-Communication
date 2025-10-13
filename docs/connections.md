# Wiring Guide - RSL10 to Arduino I2C Connection

## Quick Visual Guide

```
   RSL10-002GEVB                              Arduino Nano
   ┌─────────────┐                           ┌─────────────┐
   │             │                           │             │
   │         3.3V├───────────────────────────┤3.3V         │
   │             │                           │             │
   │             │         ┌──[2.7kΩ]────┐   │             │
   │         DIO2├─────────┼──────────────┼───┤A4 (SDA)    │
   │        (SDA)│         │              │   │             │
   │             │         │              │   │             │
   │         DIO3├─────────┼──[2.7kΩ]────┼───┤A5 (SCL)    │
   │        (SCL)│         │              │   │             │
   │             │         │              │   │             │
   │          GND├─────────┴──────────────┴───┤GND          │
   │             │                           │             │
   └─────────────┘                           └─────────────┘
                   Both resistors connect to RSL10 3.3V
```

---

## Step-by-Step Connection Instructions

### What You Need

**Components:**
- ✅ RSL10-002GEVB board
- ✅ Arduino Nano board
- ✅ 2× 2.7kΩ resistors (pull-ups for I2C)
- ✅ 5× jumper wires (male-to-male or male-to-female)
- ✅ Breadboard (optional, for easier connections)

**Tools:**
- USB cable for RSL10 (programming and power)
- USB cable for Arduino (programming only)
- Multimeter (optional, for verification)

---

## Connection Steps

### Step 1: Power Connection ⚡

```
RSL10 Pin       →    Arduino Pin
─────────────────────────────────
3.3V output     →    3.3V pin

Color suggestion: RED wire
```

**⚠️ CRITICAL:** 
- Use Arduino's **3.3V pin** (NOT 5V!)
- This powers Arduino at 3.3V logic level
- Arduino will NOT work from USB while connected to RSL10 3.3V

---

### Step 2: Ground Connection ⏚

```
RSL10 Pin       →    Arduino Pin
─────────────────────────────────
GND             →    GND

Color suggestion: BLACK wire
```

**Important:** This is the reference voltage - must be connected!

---

### Step 3: I2C Data Line (SDA) 📊

```
RSL10 Pin       →    2.7kΩ Resistor    →    Arduino Pin
────────────────────────────────────────────────────────
DIO2 (SDA)      →    [one end]         →    A4 (SDA)

The other end of resistor connects to RSL10 3.3V

Color suggestion: BLUE or GREEN wire
```

**Connection Details:**
1. Connect one jumper from RSL10 DIO2 to breadboard row (e.g., row 10)
2. Connect Arduino A4 to the same breadboard row
3. Insert 2.7kΩ resistor: One leg in same row, other leg in +3.3V rail

---

### Step 4: I2C Clock Line (SCL) 🕐

```
RSL10 Pin       →    2.7kΩ Resistor    →    Arduino Pin
────────────────────────────────────────────────────────
DIO3 (SCL)      →    [one end]         →    A5 (SCL)

The other end of resistor connects to RSL10 3.3V

Color suggestion: YELLOW or WHITE wire
```

**Connection Details:**
1. Connect one jumper from RSL10 DIO3 to breadboard row (e.g., row 12)
2. Connect Arduino A5 to the same breadboard row
3. Insert 2.7kΩ resistor: One leg in same row, other leg in +3.3V rail

---

## Breadboard Layout (Top View)

```
    RSL10-002GEVB                    Breadboard                      Arduino Nano
    ┌─────────┐                     ───────────                     ┌─────────┐
    │         │                                                     │         │
    │ 3.3V ○──┼────────────────────→ (+) Rail ←────────────────────┼─○ 3.3V  │
    │         │                          │                          │         │
    │ DIO2 ○──┼─────→ Row 10: ═══════[2.7kΩ]═══════════════════════┼─○ A4    │
    │         │                          ↑                          │         │
    │ DIO3 ○──┼─────→ Row 12: ═══════[2.7kΩ]═══════════════════════┼─○ A5    │
    │         │                          ↑                          │         │
    │  GND ○──┼────────────────────→ (-) Rail ←────────────────────┼─○ GND   │
    │         │                                                     │         │
    └─────────┘                                                     └─────────┘

Legend:
═══ = Wire connection on breadboard
[2.7kΩ] = Resistor
→ = Direction of connection
○ = Pin
(+) = Positive power rail (connected to 3.3V)
(-) = Ground rail
```

---

## Without Breadboard (Direct Connection)

If connecting directly without breadboard:

```
1. Twist resistor legs with wire connections:

   [RSL10 DIO2]───┬───[Arduino A4]
                  │
              [2.7kΩ]
                  │
           [RSL10 3.3V]

2. Use electrical tape or heat shrink to insulate connections

3. Repeat for SCL line (DIO3 to A5)
```

---

## Connection Table

| Connection | RSL10 Pin | Arduino Pin | Resistor | Notes |
|------------|-----------|-------------|----------|-------|
| Power      | 3.3V      | 3.3V        | None     | Use 3.3V NOT 5V! |
| Ground     | GND       | GND         | None     | Must be connected |
| SDA (Data) | DIO2      | A4          | 2.7kΩ to 3.3V | I2C data line |
| SCL (Clock)| DIO3      | A5          | 2.7kΩ to 3.3V | I2C clock line |

**Total:** 5 physical connections (4 wires + 2 resistors)

---

## Pin Identification Guide

### Finding RSL10 Pins

```
RSL10-002GEVB Board (Top View):

   [USB]
    ___
   |   |
   |   |
   ─────
   │ │ │  ← Pin headers
   
Look for pin labels:
- 3.3V (usually marked with "3V3" or "VCC")
- GND (ground symbol ⏚)
- DIO2 (digital I/O pin 2)
- DIO3 (digital I/O pin 3)
```

### Finding Arduino Nano Pins

```
Arduino Nano (Top View):

   [USB]
    ___
   |___|
   
   Left Side:          Right Side:
   D13                 A0
   3.3V  ← Power       A1
   REF                 A2
   A0                  A3
   A1                  A4  ← SDA
   ...                 A5  ← SCL
                       A6
                       A7
                       5V
                       RST
   GND   ← Ground      GND
   ...                 ...
```

**Key Arduino Pins:**
- **3.3V:** Usually labeled, on left side near top
- **GND:** Ground, multiple pins available
- **A4:** Analog pin 4, also SDA for I2C
- **A5:** Analog pin 5, also SCL for I2C

---

## Verification Checklist

### Before Powering On

- [ ] RSL10 3.3V connected to Arduino 3.3V (NOT 5V!)
- [ ] RSL10 GND connected to Arduino GND
- [ ] RSL10 DIO2 connected to Arduino A4
- [ ] RSL10 DIO3 connected to Arduino A5
- [ ] 2.7kΩ resistor from SDA line to 3.3V
- [ ] 2.7kΩ resistor from SCL line to 3.3V
- [ ] All connections are secure (no loose wires)
- [ ] Arduino USB is DISCONNECTED

### With Multimeter (Optional)

```
# Continuity Test (power OFF):
1. RSL10 DIO2 to Arduino A4: Should beep
2. RSL10 DIO3 to Arduino A5: Should beep
3. RSL10 GND to Arduino GND: Should beep
4. RSL10 3.3V to Arduino 3.3V: Should beep

# Resistance Test (power OFF):
5. SDA line to 3.3V: ~2.7kΩ
6. SCL line to 3.3V: ~2.7kΩ

# Voltage Test (power ON):
7. Arduino 3.3V pin: 3.0-3.3V
8. Arduino A4 (no communication): 3.0-3.3V (pulled high)
9. Arduino A5 (no communication): 3.0-3.3V (pulled high)
```

---

## Common Mistakes to Avoid

### ❌ WRONG: Powering Arduino from 5V
```
RSL10 5V → Arduino 5V or VIN
This will cause 5V on I2C lines → Can damage RSL10!
```

### ❌ WRONG: Missing Pull-up Resistors
```
Direct connection without resistors:
RSL10 DIO2 ──────────── Arduino A4
RSL10 DIO3 ──────────── Arduino A5

I2C will NOT work without pull-ups!
```

### ❌ WRONG: Swapped SDA/SCL
```
RSL10 DIO2 → Arduino A5  ✗
RSL10 DIO3 → Arduino A4  ✗

Correct:
RSL10 DIO2 → Arduino A4  ✓
RSL10 DIO3 → Arduino A5  ✓
```

### ❌ WRONG: Missing Ground
```
Power connected but GND not connected
→ No common reference voltage
→ Communication will fail
```

### ❌ WRONG: Both USB and 3.3V Connected
```
Arduino USB plugged in + RSL10 3.3V connected
→ Two power sources fighting
→ Possible damage to boards
```

---

## Photos Reference

### Connection Points

**RSL10 Side:**
```
┌──────────────────┐
│   RSL10-002GEVB  │
│                  │
│  [3.3V]─→        │  Red wire
│  [DIO2]─→        │  Blue/Green wire (+ resistor to 3.3V)
│  [DIO3]─→        │  Yellow/White wire (+ resistor to 3.3V)
│  [GND]─→         │  Black wire
│                  │
└──────────────────┘
```

**Arduino Side:**
```
┌──────────────────┐
│   Arduino Nano   │
│                  │
│      ←─[3.3V]    │  Red wire
│      ←─[A4]      │  Blue/Green wire
│      ←─[A5]      │  Yellow/White wire
│      ←─[GND]     │  Black wire
│                  │
└──────────────────┘
```

---

## Next Steps

1. ✅ **Complete wiring** as shown above
2. ✅ **Verify connections** using checklist
3. ✅ **Upload Arduino code** (via USB, then disconnect)
4. ✅ **Upload RSL10 code** and power on
5. ✅ **Monitor communication** via SEGGER RTT

If communication doesn't work, see [troubleshooting.md](troubleshooting.md)

---

**Last Updated:** 2025-10-13  
**Tested Configuration:** RSL10-002GEVB + Arduino Nano (5V version powered at 3.3V)  
**Status:** ✅ Verified Working
```