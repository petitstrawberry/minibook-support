# minibook-support
Softwares for CHUWI MiniBook (8-inch UMPC)

## Softwares

- [x] trackpointerd
- [x] keyboardd
- [x] tabletmoded

### trackpointerd

Trackpointerd is a daemon that manages the trackpointer of the MiniBook. 

- Calibrate the trackpointer
- Switch enable/disable the trackpointer

### keyboardd

Keyboardd is a daemon that manages the keyboard of the MiniBook.

- Switch enable/disable the keyboard

### tabletmoded

Tabletmoded is a daemon that triggers the tablet mode of the MiniBook.

- Auto detect the tablet mode
- Switch enable/disable the tablet mode
  - Trigger the tablet mode when the MiniBook is folded
  - Untrigger the tablet mode when the MiniBook is unfolded
  - Disable the keyboard using keyboardd when the tablet mode is triggered and enable the keyboard when the tablet mode is untriggered
  - Disable the trackpointer using trackpointerd when the tablet mode is triggered and enable the trackpointer when the tablet mode is untriggered

## Requirements

- CHUWI MiniBook (8-inch UMPC)
- Linux

## Installation

```bash
git clone https://github.com/petitstrawberry/minibook-support.git
cd minibook-support
sudo make install
```
