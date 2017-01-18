## Installing dependencies

Before developing Kaa applications for ESP8266, some dependencies should be installed.
The detailed installation instructions can be found below.

1. Prerequisites

        sudo apt-get install autoconf libtool libtool-bin bison build-essential gawk git gperf flex texinfo libncurses5-dev libc6-dev-amd64 python-serial libexpat-dev python-setuptools

2. Set the `ESPRESSIF_HOME` variable.
This variable will be used throughout the installation process, and denotes a directory where ESP8266 SDK and toolchain will be placed.
You are free to set it to whatever you like.

        sudo mkdir /opt/Espressif/
        sudo chown -R $USER:$USER /opt/Espressif/
        export ESPRESSIF_HOME=/opt/Espressif/

3. Install toolchain
<!-- TODO: KAA-928 -->

        cd $ESPRESSIF_HOME
        git clone -b lx106-g++ git://github.com/jcmvbkbc/crosstool-NG.git
        cd crosstool-NG
        ./bootstrap && ./configure --prefix=$(pwd)
        make
        sudo make install
        ./ct-ng xtensa-lx106-elf
        ./ct-ng build

4. Add path to toolchain binaries to your `.bashrc`:

        echo "export PATH=$ESPRESSIF_HOME/crosstool-NG/builds/xtensa-lx106-elf/bin:\$PATH" >> ~/.bashrc
        source ~/.bashrc

5. Install ESP8266 RTOS SDK

        cd $ESPRESSIF_HOME
        export ESP_SDK_HOME=$ESPRESSIF_HOME/esp-rtos-sdk
        git clone https://github.com/espressif/esp_iot_rtos_sdk.git $ESP_SDK_HOME
        cd $ESP_SDK_HOME
        git checkout 169a436ce10155015d056eab80345447bfdfade5
        wget -O lib/libhal.a https://github.com/esp8266/esp8266-wiki/raw/master/libs/libhal.a
        cd $ESP_SDK_HOME/include/lwip/arch
        sed -i 's:#include "c_types.h"://#include "c_types.h":' $ESP_SDK_HOME/include/lwip/arch/cc.h

6. Install esptool.py

        cd $ESPRESSIF_HOME
        git clone https://github.com/espressif/esptool.git
        cd esptool
        sudo python setup.py install

## Build an application
        mkdir build
        cd build
        cmake -DWIFI_SSID=<your_AP_name> -DWIFI_PASSWORD=<your_AP_password> -DKAA_PLATFORM=esp8266 -DCMAKE_TOOLCHAIN_FILE=../libs/kaa/toolchains/esp8266.cmake ..
        make

## Prepare the binary
        esptool.py elf2image demo_client

## Flash the board
        sudo esptool.py write_flash 0x00000 demo_client-0x00000.bin 0x40000 demo_client-0x40000.bin