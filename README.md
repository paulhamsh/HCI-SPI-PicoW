# HCI-SPI-PicoW
Micropython access to the CVW43439 SPI layer in the Pico Pi W

Creates a module HCI_SPI_PicoW   

```
>>> from HCI_SPI_PicoW import *
>>> bt = HCI_SPI()
>>> # 0x40_00_a0_04 in little endian byte order command: read, inc, SPI function, 0x14, 0x4
>>> r = bt.transfer(b'\x04\xa0\x00\x40', 4, 8)
>>> print(r)
b'\x00\x00\x00\x00\xad\xbe\xed\xfe'
```

## Edit needed in pico-sdk/src/rp2_common/pico_cyw43_driver/btstack_hci_transport_cyw43.c

Change ```static bool hci_transport_ready``` to ```bool hci_transport_ready``` (in ```pico-sdk/src/rp2_common/pico_cyw43_driver/btstack_hci_transport_cyw43.c```) then it can be accessed from Micropython code.   
Otherwise ensure that ```#define TRANSPORT_READY_CHECK``` is not defined.   

## How to build in WSL
```
sudo bash
apt update
apt upgrade

# Micropython installation
cd ~
git clone https://github.com/micropython/micropython.git

cd mircopython

make -C mpy-cross

# Clone this repo into micropython codebase

cd ~
git clone https://github.com/paulhamsh/HCI-PicoW.git
cd HCI-PicoW

mkdir ~/micropython/userc
cp -r HCI-PicoW ~/micropython/userc

# Build micropython with the user module
cd ~/micropython/ports/rp2

make BOARD=RPI_PICO_W submodules
make BOARD=RPI_PICO_W clean
make BOARD=RPI_PICO_W USER_C_MODULES=~/micropython/userc/HCI-PicoW/micropython.cmake
````

Copy *.uf2 to somewhere where you can drag to the Pico W, like:

````
cp build-RPI_PICO_W/firmware.uf2 /mnt/c/users/paul/Desktop
````
Then drag and drop in Windows    




## Notes
Micropython includes a Pico SDK here:
```micropython/lib/pcio-sdk```

You an delete it an replace with SDK direct from raspberry pi if you want.   
