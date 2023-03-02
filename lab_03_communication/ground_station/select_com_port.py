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


class select_com_port(QWidget):
    """Select Com Port Widget."""
    port_ready = pyqtSignal(str, str, name='port_ready')

    def __init__(self, parent: QWidget=None) -> None:
        super().__init__(parent)
        self.make_combo_box()
        self.make_connect_btn()
        self.make_layout()
        self.setWindowModality(Qt.ApplicationModal)
        self.setFocusPolicy(Qt.StrongFocus)

    def make_combo_box(self):
        # Port Combobox
        self.port_label = QLabel(self.tr('COM Port:'))
        self.port_combobox = QComboBox()
        self.port_label.setBuddy(self.port_combobox)
        self.baud_label = QLabel(self.tr('Baud Rate:'))
        self.baud_combobox = QComboBox()
        self.baud_label.setBuddy(self.baud_combobox)
        self.update_com_ports()
        self.update_baud_rates()

    def make_connect_btn(self):
        # Connect Buttons
        self.connect_btn = QPushButton(self.tr('Select'))
        self.connect_btn.pressed.connect(self.on_select_btn_pressed)

    def make_layout(self):
        # Arrange Layout
        layout = QGridLayout()
        layout.addWidget(self.port_label, 0, 0)
        layout.addWidget(self.port_combobox, 0, 1)
        layout.addWidget(self.baud_label, 1, 0)
        layout.addWidget(self.baud_combobox, 1, 1)
        layout.addWidget(self.connect_btn, 0, 2)
        self.setLayout(layout)

    @pyqtSlot()
    def on_select_btn_pressed(self):
        self.port_ready.emit(self.port_combobox.currentText(), self.baud_combobox.currentText())
        self.close()

    def show_error_message(self, msg: str) -> None:
        """Show a Message Box with the error message."""
        QMessageBox.critical(self, QApplication.applicationName(), str(msg))

    def update_com_ports(self) -> None:
        """Update COM Port list in GUI."""
        available_ports = self.gen_serial_ports()
        for name, device in available_ports:
            self.port_combobox.addItem(device)

    def update_baud_rates(self) -> None:
        self.baud_combobox.addItem('300')
        self.baud_combobox.addItem('1200')
        self.baud_combobox.addItem('2400')
        self.baud_combobox.addItem('4800')
        self.baud_combobox.addItem('9600')
        self.baud_combobox.addItem('19200')
        self.baud_combobox.addItem('38400')
        self.baud_combobox.addItem('57600')
        self.baud_combobox.addItem('74880')
        self.baud_combobox.addItem('115200')
        self.baud_combobox.addItem('230400')
        self.baud_combobox.addItem('250000')
        self.baud_combobox.addItem('1000000')
        self.baud_combobox.addItem('2000000')


    @property
    def port(self) -> str:
        """Return the current serial port."""
        return self.port_combobox.currentText()

    @property
    def baud(self) -> str:
        """Return the current serial port."""
        return self.baud_combobox.currentText()

    def gen_serial_ports(self) -> Iterator[Tuple[str, str]]:
        """Return all available serial ports."""
        ports = comports()
        return ((p.description, p.device) for p in ports)

