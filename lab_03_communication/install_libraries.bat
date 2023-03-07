..\computer_environment\arduino-cli lib install RTCZero@1.6.0
..\computer_environment\arduino-cli lib install "XBee-Arduino library"@0.6.0
..\computer_environment\arduino-cli lib install "wifi101"@0.16.1
..\computer_environment\arduino-cli lib install "wifi"@1.2.7

..\computer_environment\arduino-cli config init
..\computer_environment\arduino-cli config set library.enable_unsafe_install true
..\computer_environment\arduino-cli lib install --zip-path ..\computer_environment\BreezyArduCAM_single_jpeg.zip
..\computer_environment\arduino-cli config set library.enable_unsafe_install false

