#!/usr/bin/env python
import os
import sys
import time
retCode = os.system("python src/build.py")

time.sleep( 2 )

if retCode == 0 :
    print("UPLOADING")
    retCode = os.system("arduino-cli upload -v -b arduino:avr:uno --programmer arduinoasisp -p COM4 -i ./build/arduino.avr.uno/handController.ino.hex")
    if retCode == 1 :
        print("UPLOADING FAILED!!! ")
    else :
        print("UPLOADING SUCCES!!! ")
