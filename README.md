# 🔑 iButton Emulator — ESP8266 Firmware

A firmware for ESP8266 that emulates and reads Dallas/Maxim iButton (DS1990A / 1-Wire) keys used in intercoms. Built with PlatformIO on the Arduino framework.

---

## ✨ Features

- 🎠 **Key carousel** — browse built-in factory keys and your saved keys on a 128×64 OLED display
- 📡 **Emulation** — emulate any key via the 1-Wire bus
- 📖 **Key reading** — read a physical iButton key and save it to flash storage
- 🏭 **10 built-in factory keys** — common intercom codes for Metakom, Cyfral, and Vizit systems
- 💾 **Up to 20 user keys** — saved persistently on LittleFS
- 🖥️ **UART control** — full command interface over serial for scripting or companion apps
- 🛡️ **Duplicate detection** — prevents saving the same key twice (checked against both user and factory keys)

---

## 🔧 Hardware

### Required

| Component | Details |
|-----------|---------|
| ESP8266 + OLED combo board | All-in-one module with ESP8266 and SSD1306 128×64 display already on board — [AliExpress](https://www.aliexpress.com/item/1005006211265531.html). Just solder a wire to the iButton data pad (GPIO 2 / D4) & GND and you're good to go |
| OLED display | Adafruit SSD1306 128×64 (I²C, address `0x3C`) |

### Optional (for battery-powered build) 🔋

| Component | Purpose |
|-----------|---------|
| TP4056 module | LiPo charging |
| LiPo battery | Power source |
| Slide switch | Power on/off |

---

## ⚡ Wiring

```
ESP8266 GPIO 2 (D4)  ──── iButton data pin (1-Wire bus)
ESP8266 SDA          ──── OLED SDA
ESP8266 SCL          ──── OLED SCL
ESP8266 3.3V / GND   ──── OLED VCC / GND
```

---

## 🚀 Building & Flashing

### Prerequisites

- [PlatformIO](https://platformio.org/) (CLI or VS Code extension)

### Dependencies (defined in `platformio.ini`)

```ini
lib_deps =
    orgua/OneWireHub @ ^3.0.0
    paulstoffregen/OneWire @ ^2.3.8
    adafruit/Adafruit SSD1306 @ ^2.5.16
```

### Build & upload

```bash
pio run --target upload
```

LittleFS is used for key storage — no separate filesystem upload step is needed; it initialises automatically on first boot.

---

## 🕹️ Usage

### Button controls

| Gesture | Context | Action |
|---------|---------|--------|
| 1× click | Carousel | Next key |
| 2× click | Carousel | Emulate selected key |
| Hold | Carousel | Enter key-reading mode |
| 2× click | Emulating | Stop emulation, return to carousel |
| 2× click | Reading | Cancel, return to carousel |

### Key reading

Hold the button from the carousel to enter reading mode, then touch a physical iButton key to the data pin. If the key is new it will be saved automatically under the name `My Key N`.

---

## 🖥️ UART Command Interface

Connect at **115200 baud**. Commands are newline-terminated (`\n`).

### Commands

| Command | Description |
|---------|-------------|
| `LIST` | 📋 List all keys (user + factory) |
| `EMULATE:AA:BB:CC:DD:EE:FF:GG:HH` | 📡 Emulate a key by raw 8-byte address |
| `STOPEMULATE` | ⏹️ Stop current emulation |
| `READ` | 📖 Enter key-reading mode |
| `STOPREAD` | ⏹️ Exit key-reading mode |
| `SAVE:AA:BB:CC:DD:EE:FF:GG:HH:Name` | 💾 Save a key with a custom name |

### Responses

| Response | Meaning |
|----------|---------|
| `OK` | ✅ Command succeeded |
| `ERR:<reason>` | ❌ Command failed |
| `COUNT:<n>` | Total key count (response to `LIST`) |
| `KEY:<i>:USER:<bytes>:<name>` | User key entry |
| `KEY:<i>:FACTORY:<bytes>:<name>` | Factory key entry |

### Example session

```
LIST
COUNT:11
KEY:0:FACTORY:01:FF:FF:FF:FF:FF:FF:2F:Standard Key
...

EMULATE:01:19:90:00:00:00:00:FF
OK

STOPEMULATE
OK

SAVE:01:AB:CD:EF:00:11:22:33:Front Door
OK
```

---

## 🏭 Factory Keys

| Name | Systems |
|------|---------|
| Standard Key | Universal |
| Metakom, Cyfral, Vizit | Multi-system |
| Cyfral, Vizit | Cyfral / Vizit |
| Metakom, Cyfral | Metakom / Cyfral |
| Vizit | Vizit |
| Cyfral | Cyfral |
| Metakom 98% | Metakom |
| Metakom 95% | Metakom |
| Vizit 99% | Vizit |
| Metakom, Cyfral (alt) | Metakom / Cyfral |

---

## 📁 Project Structure

```
├── config.h              # Pin definitions, timing constants, factory keys
├── main.cpp              # App state machine, setup/loop
├── onewire/
│   ├── Key.h             # OneWireKey struct
│   ├── Emulator.h/.cpp   # 1-Wire emulation via OneWireHub
│   └── Reader.h/.cpp     # 1-Wire key reading via OneWire
├── interface/
│   ├── display/
│   │   └── Display.h/.cpp    # SSD1306 UI rendering
│   ├── button/
│   │   └── Button.h/.cpp     # Gesture detection (click / double / hold)
│   └── serial/
│       └── SerialComm.h/.cpp # UART command parser
└── storage/
    └── Storage.h/.cpp    # LittleFS persistence for user keys
```

---

## 📄 License

MIT
