### Download and run Kaa Sandbox

Download and import [Kaa Sandbox](https://jenkins.kaaproject.org:52001/job/sandbox_copy/8/artifact/kaa-sandbox-ubuntu-64bit-daily-build-126.ova)

### Download GPIO master application (Android)

1. Go to [http://localhost:9080/sandbox/#proj:projectId=gpiocontrol_demo_android_master](http://localhost:9080/sandbox/#proj:projectId=gpiocontrol_demo_android_master)
2. Press "Binary" button to download the APK file.

### Download GPIO slave application (ESP8266)

1. Go to [http://localhost:9080/sandbox/#proj:projectId=gpiocontrol_demo_c_slave](http://localhost:9080/sandbox/#proj:projectId=gpiocontrol_demo_c_slave)
2. Press "Source" button to download the archive file.

### Compilation guide for ESP8266 demo (tested against Ubuntu 14.04 and Node MCU)

1. Install Prerequisites

        sudo apt-get install autoconf libtool bison build-essential gawk git gperf flex texinfo libncurses5-dev libc6-dev-amd64 python-serial libexpat-dev python-setuptools

2. Download and install cmake:

        wget https://cmake.org/files/v3.7/cmake-3.7.2.tar.gz
        tar xvf cmake-3.7.2.tar.gz
        cd cmake-3.7.2/
        ./bootstrap && make
        sudo make install
    
3. Set the `ESPRESSIF_HOME` variable.
This variable will be used throughout the installation process, and denotes a directory where ESP8266 SDK and toolchain will be placed.
You are free to set it to whatever you like.

        sudo mkdir /opt/Espressif/
        sudo chown -R $USER:$USER /opt/Espressif/
        export ESPRESSIF_HOME=/opt/Espressif/

4. Install toolchain

        cd $ESPRESSIF_HOME
        git clone -b lx106-g++ git://github.com/jcmvbkbc/crosstool-NG.git
        cd crosstool-NG
        ./bootstrap && ./configure --prefix=$(pwd)
        make
        sudo make install
        ./ct-ng xtensa-lx106-elf
        ./ct-ng build

5. Add path to toolchain binaries to your `.bashrc`:

        echo "export PATH=$ESPRESSIF_HOME/crosstool-NG/builds/xtensa-lx106-elf/bin:\$PATH" >> ~/.bashrc
        source ~/.bashrc

6. Install ESP8266 RTOS SDK

        cd $ESPRESSIF_HOME
        export ESP_SDK_HOME=$ESPRESSIF_HOME/esp-rtos-sdk
        git clone https://github.com/espressif/esp_iot_rtos_sdk.git $ESP_SDK_HOME
        cd $ESP_SDK_HOME
        git checkout 169a436ce10155015d056eab80345447bfdfade5
        wget -O lib/libhal.a https://github.com/esp8266/esp8266-wiki/raw/master/libs/libhal.a
        cd $ESP_SDK_HOME/include/lwip/arch
        sed -i 's:#include "c_types.h"://#include "c_types.h":' $ESP_SDK_HOME/include/lwip/arch/cc.h

7. Install esptool.py

        cd $ESPRESSIF_HOME
        git clone https://github.com/espressif/esptool.git
        cd esptool
        sudo python setup.py install

8. Unpack the C application and the Kaa SDK

        tar xvf gpiocontrol_demo.tar.gz 
        cd CGPIODemo/
        tar xvf libs/kaa/kaa-c-ep-sdk-*.tar.gz -C libs/kaa/
 
9. Build an application

        mkdir build
        cd build
        cmake -DKAA_PLATFORM=esp8266 -DCMAKE_TOOLCHAIN_FILE=../libs/kaa/toolchains/esp8266.cmake -DWIFI_SSID=<your_AP_name> -DWIFI_PASSWORD=<your_AP_password> -DDEMO_ACCESS_TOKEN=<token> ..
        make
You need to specify your Wifi AP name and password. 
Also, you need to set the demo access token that you will use in the Android application. It can be any string.

10. Prepare the binary

        esptool.py elf2image demo_client

11. Flash the board

        sudo esptool.py write_flash 0x00000 demo_client-0x00000.bin 0x40000 demo_client-0x40000.bin

12. Connect LEDs to your ESP8266 board. The demo code uses the GPIO4 and GPIO5.
After flashing the ESP8266 will automatically connect to the AP and will listen for events from the Kaa server.

### Run Android application

1. Open the APK file on your Android device
2. Press "+" and enter the demo access token
3. Select the ESP8266 endpoint
4. Use GUI to turn on/off the leds on ESP board

