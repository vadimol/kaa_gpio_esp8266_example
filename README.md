
1. Follow the guide to install dependencies:
		https://kaaproject.github.io/kaa/docs/v0.10.0/Programming-guide/Using-Kaa-endpoint-SDKs/C/SDK-ESP8266/

2. Install esptool:
		git clone https://github.com/espressif/esptool.git
		cd esptool
		sudo python setup.py install

3. Before building an application you need to download the C SDK from the Kaa server and unpack it into libs/kaa directory.
		mkdir -p libs/kaa
		tar xvf <your_KAA_SDK_TAR.GZ> -C libs/kaa

4. Build an application:
		mkdir build
		cd build
		cmake -DWIFI_SSID=<your_AP_name> -DWIFI_PASSWORD=<your_AP_password>  ..
		make

5. Prepare the binary:
		esptool.py elf2image demo_client

6. Flash the board:
		sudo esptool.py write_flash 0x00000 demo_client-0x00000.bin 0x40000 demo_client-0x40000.bin