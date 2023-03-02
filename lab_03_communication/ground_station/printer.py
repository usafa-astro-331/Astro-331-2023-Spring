from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtPrintSupport import *
from PyQt5 import uic, QtPrintSupport

class printer():

    def __init__(self, parent = None, what_to_print = None) -> None:
        self.to_be_printed = what_to_print
        self.parent = parent

    def setDocument(self, what_to_print) -> None:
        self.to_be_printed = what_to_print

    def printDialog(self) -> None:
        if  self.to_be_printed == None:
            self.show_error_message(self, "A document must be selected prior to printing")
        else:
             printer = QPrinter(QPrinter.HighResolution)
             dialog = QPrintDialog(printer, self.parent)
             if dialog.exec_() == QPrintDialog.Accepted:
                 self.to_be_printed.print_(printer)

    def printpreviewDialog(self) -> None:
        if self.to_be_printed == None:
            self.show_error_message(self, "A document must be selected prior to printing")
        else:
             printer = QPrinter(QPrinter.HighResolution)
             previewDialog = QPrintPreviewDialog(printer, parent)
             previewDialog.paintRequested.connect(self.printPreview)
             previewDialog.exec_()

    def printPreview(self, printer) -> None:
        if  self.to_be_printed == None:
            self.show_error_message(self, "A document must be selected prior to printing")
        else:
             self.to_be_printed.print_(printer)

    def printPDF(self) -> None:
            fn, _ = QFileDialog.getSaveFileName(None, 'Export PDF', None, 'PDF files (.pdf);;All Files()')
            if fn != '':
                if QFileInfo(fn).suffix() == "" :
                    fn += '.pdf'
                    printer = QPrinter(QPrinter.HighResolution)
                    printer.setOutputFormat(QPrinter.PdfFormat)
                    printer.setOutputFileName(fn)
                    self.to_be_printed.document().print_(printer)

    def print_report(self) -> None:
        # Create printer
        printer = QtPrintSupport.QPrinter()
        # Create painter
        painter = QPainter()
        # Start painter
        painter.begin(printer)
        # Grab a widget you want to print
        screen = self.to_be_printed.grab()
        # Draw grabbed pixmap
        painter.drawPixmap(10, 10, screen)
        # End painting
        painter.end()


    def show_error_message(self, error):
        msg = QMessageBox()
        msg.setIcon(QMessageBox.Critical)
        msg.setText("Error!!")
        msg.setInformativeText(error)
        msg.setWindowTitle("Critical Error Message")
        msg.exec_()
