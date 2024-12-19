# Atmel CLI Installation Guide

This guide will walk through the installation of the `atmel-cli` and `atmel-cli-cmake-wrapper` tools used to bridge F Prime and the Atmel build system. This assumes a virtual environment has been setup for your project.

> Activate the project virtual environment now.

## Install atmel-cli

This command downloads `atmel-cli` and installs the binary into the existing (and activated) virtual environment.
```sh
curl -fsSL https://raw.githubusercontent.com/atmel/atmel-cli/master/install.sh | BINDIR=$VIRTUAL_ENV/bin sh
```

## Install Required Packages and Atmel Libraries

### Pip

Install all required `pip` packages using the [`requirements.txt`](../requirements.txt) file
```sh
pip install -r fprime-atmel/requirements.txt
```

### Atmel

Required Atmel library dependencies:
```sh
atmel-cli lib install Time
```

If you will be using the `Atmel.TcpClient` component, install:
```sh
atmel-cli lib install WiFi
```

## List of Tested Boards

[The following list](./board-list.md) shows all the boards that successfully ran an F Prime deployment.

## Setup atmel-cli for select Atmel boards

Initialize the atmel-cli configuration file.
```sh
atmel-cli config init
```

Below are board manager URLs for tested Arduino boards. You are not required to add all of these boards, but you are free to do so. You can view the list of addtional boards [here](https://github.com/per1234/inoplatforms/blob/main/ino-hardware-package-list.tsv) if you wish to test F Prime on boards that were not tested above.
```sh
atmel-cli config add board_manager.additional_urls https://www.pjrc.com/teensy/package_teensy_index.json
atmel-cli config add board_manager.additional_urls https://adafruit.github.io/atmel-board-index/package_adafruit_index.json
atmel-cli config add board_manager.additional_urls https://espressif.github.io/atmel-esp32/package_esp32_index.json
atmel-cli config add board_manager.additional_urls https://atmel.esp8266.com/stable/package_esp8266com_index.json
atmel-cli config add board_manager.additional_urls https://mcudude.github.io/MegaCore/package_MCUdude_MegaCore_index.json
atmel-cli config add board_manager.additional_urls https://github.com/earlephilhower/atmel-pico/releases/download/global/package_rp2040_index.json
atmel-cli config add board_manager.additional_urls https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json
```

Install the new board packages. Only install the ones you have added to your board manager in the previous step.
```sh
atmel-cli core update-index
atmel-cli core install atmel:avr
atmel-cli core install teensy:avr
atmel-cli core install adafruit:samd
atmel-cli core install adafruit:nrf52
atmel-cli core install esp32:esp32@2.0.9
atmel-cli core install esp8266:esp8266
atmel-cli core install MegaCore:avr
atmel-cli core install rp2040:rp2040
atmel-cli core install STMicroelectronics:stm32
```

> ESP32: As of 10/27/2024, versions newer than `esp32:esp32@2.0.9` will not work.

If you are building the Adafruit NRF52 boards and are on Linux, you must also install:
```sh
pip install adafruit-nrfutil
```

## Adding udev rules (Linux Only)
Add udev rules. Download/save the `.rules` files located [here](https://github.com/fprime-community/fprime-atmel/tree/main/docs/rules) for your selected board(s) into `/etc/udev/rules.d/`.

## Uploading Deployment to Hardware

Upon successful build of an F Prime deployment, it is time to upload it to your board. The steps differ between boards. Refer to the [board list's](./board-list.md) `Upload Guide` column READMEs for guidance.
