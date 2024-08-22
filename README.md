# minibook-support
Softwares for CHUWI MiniBook (8-inch UMPC)

## Softwares

- [x] moused
- [x] keyboardd
- [x] tabletmoded

### moused

moused is a daemon that manages the trackpointer / trackpad of the MiniBook. 

- Calibrate the trackpointer / trackpad
- Switch enable/disable the trackpointer / trackpad

### keyboardd

keyboardd is a daemon that manages the keyboard of the MiniBook.

- Switch enable/disable the keyboard

### tabletmoded

tabletmoded is a daemon that triggers the tablet mode of the MiniBook.

- Auto detect the tablet mode
- Switch enable/disable the tablet mode
  - Trigger the tablet mode when the MiniBook is folded
  - Untrigger the tablet mode when the MiniBook is unfolded
  - Disable the keyboard using keyboardd when the tablet mode is triggered and enable the keyboard when the tablet mode is untriggered
  - Disable the mouse using moused when the tablet mode is triggered and enable the mouse when the tablet mode is untriggered

## Requirements

- CHUWI MiniBook (8-inch UMPC)
- Linux

## Installation

```bash
git clone https://github.com/petitstrawberry/minibook-support.git
cd minibook-support
make
sudo make install
```
