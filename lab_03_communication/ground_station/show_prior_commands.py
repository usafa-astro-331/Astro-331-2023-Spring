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

class show_prior_commands(QDialog):
    selected_commands = pyqtSignal(list, name='selected_commands')

    def __init__(self, parent: QWidget=None) -> None:
        super().__init__(parent)
        self.setupUi(self)
        self.setWindowModality(Qt.ApplicationModal)
        self.setFocusPolicy(Qt.StrongFocus)

    def append(self, command):
        self.prior_commands.addItem(command)

    def return_selected_items(self):
        return self.prior_commands.selectedItems()

    def setupUi(self, Dialog):
        Dialog.setObjectName("Dialog")
        Dialog.setWindowModality(Qt.ApplicationModal)
        Dialog.resize(362, 381)
        font = QFont()
        font.setBold(False)
        font.setUnderline(False)
        font.setWeight(75)
        Dialog.setFont(font)
        self.verticalLayoutWidget = QWidget(Dialog)
        self.verticalLayoutWidget.setGeometry(QRect(20, 20, 321, 351))
        self.verticalLayoutWidget.setObjectName("verticalLayoutWidget")
        self.verticalLayout = QVBoxLayout(self.verticalLayoutWidget)
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout.setObjectName("verticalLayout")
        self.label = QLabel(self.verticalLayoutWidget)
        self.label.setAlignment(Qt.AlignCenter)
        self.label.setObjectName("label")
        self.verticalLayout.addWidget(self.label)
        self.label.setObjectName("label")
        self.verticalLayout.addWidget(self.label)
        self.prior_commands = QListWidget(self.verticalLayoutWidget)
        self.prior_commands.setObjectName("prior_commands")
        self.verticalLayout.addWidget(self.prior_commands)
        self.accept_deny = QDialogButtonBox(self.verticalLayoutWidget)
        self.accept_deny.setOrientation(Qt.Horizontal)
        self.accept_deny.setStandardButtons(QDialogButtonBox.Cancel|QDialogButtonBox.Ok)
        self.accept_deny.setObjectName("accept_deny")
        self.verticalLayout.addWidget(self.accept_deny)
        self.retranslateUi(Dialog)
        self.accept_deny.accepted.connect(Dialog.accept)
        self.accept_deny.rejected.connect(Dialog.reject)
        QMetaObject.connectSlotsByName(Dialog)
#        self.prior_commands.itemSelectionChanged.connect(self.selectionChanged)
        self.prior_commands.setSelectionMode(QAbstractItemView.ExtendedSelection)

    @pyqtSlot()
    def selectionChanged(self):
        items = self.prior_commands.selectedItems()
        self.selected_commands.emit(items)


    def retranslateUi(self, Dialog):
        _translate = QCoreApplication.translate
        Dialog.setWindowTitle(_translate("Dialog", "Prior Commands"))
        self.label.setText(_translate("Dialog", "Valid Prior Commands Sent To Arduino"))

