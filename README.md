
## Follow the guide to install dependencies
		https://kaaproject.github.io/kaa/docs/v0.10.0/Programming-guide/Using-Kaa-endpoint-SDKs/C/SDK-ESP8266/

## Install esptool
		git clone https://github.com/espressif/esptool.git
		cd esptool
		sudo python setup.py install

## Download and untar the C SDK from the Kaa server
		mkdir -p libs/kaa
		tar xvf <your_KAA_SDK_TAR.GZ> -C libs/kaa

## Build an application
		mkdir build
		cd build
		cmake -DWIFI_SSID=<your_AP_name> -DWIFI_PASSWORD=<your_AP_password>  ..
		make

## Prepare the binary
		esptool.py elf2image demo_client

## Flash the board
		sudo esptool.py write_flash 0x00000 demo_client-0x00000.bin 0x40000 demo_client-0x40000.bin