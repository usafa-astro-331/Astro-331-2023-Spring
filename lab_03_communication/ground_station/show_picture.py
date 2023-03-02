import sys
import time
import os
import serial
from typing import Iterator, Tuple
from serial.tools.list_ports import comports
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtPrintSupport import *
from PyQt5 import uic, QtPrintSupport
from PyQt5.QtGui import QPixmap
from helpers import *


class show_picture(QDialog):


    def __init__(self, *args, **kwargs):
        super(show_picture, self).__init__()
        self.image = kwargs['image']
        self.load_ui()
        self.setWindowTitle("Received Picture")

    def load_ui(self):
        path = os.path.join(os.path.dirname(__file__), "show_picture.ui")
        uic.loadUi(path, self)
        self.picture.setPixmap(self.image)
