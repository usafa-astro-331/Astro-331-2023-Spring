
import sys
import os
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtPrintSupport import *
from PyQt5 import uic, QtPrintSupport
from jinja2 import *
from printer import *

class display_help(QDialog):

    def __init__(self, items, parent: QDialog=None) -> None:
         super().__init__(parent)
         self.items = items
         self.load_ui()
         self.load_help()
         self.setup_slots_and_signals()
         self.print_it = printer(self, self.display)

    def setup_slots_and_signals(self) -> None:
        self.print.clicked.connect(self.print_contents)
        self.save.clicked.connect(self.save_contents)
        self.close.clicked.connect(self.hide)

    def load_help(self) -> None:
        headers = ["Command", "Purpose and Paramaters"]
        rows = []
        for key in self.items.keys():
            rows.append([key, self.items[key][1]])
        table = """
        <style>
        table {
            font-family: arial, sans-serif;
            border-collapse: collapse;
            width: 100%;
        }

        th {
            font-family: "Times New Roman", Times, serif;
            border: 1px solid #dddddd;
            font-size: 14pt;
            text-align: center;
            padding: 10px;
        }

        td {
            font-family: "Times New Roman", Times, serif;
            border: 1px solid #dddddd;
            font-size: 12pt;
            text-align: left;
            vertical-align: center;
            padding: 10px;
        }
        </style>

        <table border="1" width="100%">
            <tr>{% for header in headers %}<th>{{header}}</th>{% endfor %}</tr>
            {% for row in rows %}<tr>
                {% for element in row %}<td>
                    {{element}}
                </td>{% endfor %}
            </tr>{% endfor %}
        </table>
        """
        self.display.setText(Template(table).render(headers=headers, rows=rows))
        #        path = os.path.join(os.path.dirname(__file__), "help.html")
#        self.display.setSource(QtCore.QUrl.fromLocalFile(path))

    def load_ui(self) -> None:
         path = os.path.join(os.path.dirname(__file__), "help.ui")
         uic.loadUi(path, self)

    @pyqtSlot()
    def print_contents(self) -> None:
        self.print_it.printpreviewDialog()

    @pyqtSlot()
    def save_contents(self) -> None:
        self.print_it.printPDF()

