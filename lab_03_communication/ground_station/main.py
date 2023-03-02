# This Python file uses the following encoding: utf-8
from digi.xbee.devices import XBeeDevice
from digi.xbee.models.atcomm import *
import sys
import time
import os
import serial
import json
from are_you_sure import *
from display_help import *
from show_prior_commands import *
from select_com_port import *
from show_picture import *
from typing import Iterator, Tuple
from serial.tools.list_ports import comports
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtPrintSupport import *
from PyQt5 import uic, QtPrintSupport
from printer import *


class MainWindow(QMainWindow):

    tool_bar_items = []
    menu_items = []
    timer = None
    transmit_btn = None
    commands = {}
    radio_config = {}
    ser = serial.Serial(timeout=0)     # Object for access to the serial port
    device = None
    remote_device = None
    rssi = 0
    record = False
    csv_file = None
    csv_file_name = ''
    perform = {}
    file_names = {}
    current_output_file = ''

    def __init__(self):
        super(MainWindow, self).__init__()
        self.load_program_config()
        self.load_ui()
        self.setup_slots_and_signals()
        self.setWindowTitle("Cube Sat")
        self.setup_transmit_button()
        self.setup_statusBar()
        self.setup_slots_and_signals()
        self.setup_menu()
        self.setup_toolbar()
        self.setup_help()
        self.setup_command_map()

    def setup_command_map(self) -> None:
        self.perform = {
                            '1' : self.get_sensor_data_for_printing,
                            '2' : self.get_sensor_data_for_printing,
                            '7' : self.get_picture_from_arduino,
                            '10': self.get_sensor_data_for_printing,
                            '11': self.display_rssi,
                            '12': self.record_data,
                            '13': self.record_data,
                            '14': self.record_data,
                            '22': self.get_sat_rssi
                        }

    def load_program_config(self) -> None:
        with open('config.json') as json_file:
            config = json.load(json_file)
            self.setup_commands(self.radio_config, config['radio_config'])
            self.setup_commands(self.commands, config['commands'])
#            self.setup_commands(self.write_commands, config['write_commands'])
            self.setup_commands(self.file_names, config['file_names'])

    def setup_help(self) -> None:
        self.help_on_program = display_help(self.commands)

    def setup_commands(self, command_dict, dictionary) -> None:
        for key, value in dictionary.items():
            command_dict[key] = value

    def setup_slots_and_signals(self) -> None:
        #self.port_selection = select_com_port()
        #self.port_selection.port_ready.connect(self.on_connect_btn_pressed)
        self.prior_commands = show_prior_commands()
        #self.prior_commands.selected_commands.connect(self.execute_prior_commands)

    def setup_prior_commands(self) -> None:
        self.prior_commands = QListView()
        self.prior_commands.setWindowTitle('Prior Commands')
        model = QStandardItemModel(self.prior_commands)
        self.prior_commands.setModel(model)
        self.prior_commands.show()
        self.gridLayout.addWidget(self.prior_commands, 0, 1)

    def setup_transmit_button(self) -> None:
        #self.transmit_btn = QPushButton(self.tr('Transmit'))
        self.transmit_btn.pressed.connect(self.on_transmit_btn_pressed)
        self.transmit_btn.setEnabled(False)
        #self.gridLayout.addWidget(self.transmit_btn, 1, 1)
        self.ser = serial.Serial(timeout=0.05)

    def setup_timer(self) -> None:
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.receive_serial_data)
        self.timer.start(5)

    def setup_statusBar(self) -> None:
        self.statusBar().showMessage('Ready')

    def make_menu_actions(self, icon_file,menu_name, short_cut, tool_tip, when_triggered, enabled = False) -> QAction:
        action = QAction(QIcon(icon_file), menu_name, self)
        action.setShortcut(short_cut)
        action.setStatusTip(tool_tip)
        action.triggered.connect(when_triggered)
        action.setEnabled(enabled)
        return action

    def setup_menu(self) -> None:
        exitAction = self.make_menu_actions('exit.png', ' &Exit','Ctrl+Q', 'Exit Application', self.exit_button_pressed, True)
        printAction = self.make_menu_actions('print.png', '&Print', 'Ctrl+P','Print transcript of session.', self.printDialog)
        saveAction = self.make_menu_actions('save.png','&Save','Ctrl+S','Save file.', self.printPDF)
        arduinoAction = self.make_menu_actions('arduino.jpg', '&Connect to Arduino','Ctrl+C','Connect to Arduino',self.connect_to_arduino, True)
        disconnectAction = self.make_menu_actions('redx.jpg', '&Disconnect from Arduino', 'Ctrl+D','Disconnect from Arduino',self.disconnect_from_arduino)
        viewAction = self.make_menu_actions('view.png', '&View', 'Ctrl+V','View prior commands',self.view_prior_commands)
        helpAction = self.make_menu_actions('help.png', '&Help', 'Ctrl+H','View help',self.view_help_on_commands)

        menubar = self.menuBar()
        file_menu = menubar.addMenu('&File')
        self.menu_items.append(saveAction)
        file_menu.addAction(saveAction)
        self.menu_items.append(printAction)
        file_menu.addAction(printAction)

        comm_menu = menubar.addMenu('&Arduino Connection')
        self.menu_items.append(arduinoAction)
        comm_menu.addAction(arduinoAction)
        self.menu_items.append(disconnectAction)
        comm_menu.addAction(disconnectAction)

        view_menu = menubar.addMenu('&View')
        self.menu_items.append(viewAction)
        view_menu.addAction(viewAction)
        help_menu = menubar.addMenu('&Help')
        self.menu_items.append(helpAction)
        help_menu.addAction(helpAction)

        self.menu_items.append(exitAction)
        file_menu.addAction(exitAction)

    def tool_bar_button(self, icon_image, button_name, status_tip, connect_to) -> QAction :
        button_action = QAction(QIcon(icon_image), button_name, self)
        button_action.setStatusTip(status_tip)
        button_action.triggered.connect(connect_to)
        button_action.setEnabled(False)
        self.tool_bar_items.append(button_action)
        return button_action

    def setup_toolbar(self) -> None:
        tool_bar = QToolBar("Main Toolbar")
        tool_bar.setIconSize(QSize(30,30))
        self.addToolBar(tool_bar)
        button_action = self.tool_bar_button('arduino.jpg', 'Connect', 'Connect to Arduino', self.connect_to_arduino)
        tool_bar.addAction(button_action)
        button_action = self.tool_bar_button('redx.jpg', 'Disconnect', 'Disconnect from Arduino', self.disconnect_from_arduino)
        tool_bar.addAction(button_action)
        tool_bar.addSeparator()
        button_action = self.tool_bar_button('save.png', 'Save', 'Save to file', self.printPDF)
        tool_bar.addAction(button_action)
        button_action = self.tool_bar_button('print.png', 'Print', 'Print Transcript', self.printDialog)
        tool_bar.addAction(button_action)
        tool_bar.addSeparator()
        button_action = self.tool_bar_button('view.png', 'View', 'View prior commands sent to Arduino', self.view_prior_commands)
        tool_bar.addAction(button_action)
        button_action = self.tool_bar_button('help.png', 'Help', 'View help on commands to be sent to Arduino', self.view_help_on_commands)
        tool_bar.addAction(button_action)
        tool_bar.addSeparator()
        button_action = self.tool_bar_button('exit.png', 'Exit', 'Exit Program', self.exit_button_pressed)
        tool_bar.addAction(button_action)
        self.tool_bar_items[0].setEnabled(True)
        self.tool_bar_items[-1].setEnabled(True)

    def view_prior_commands(self) -> None:
        count_of_failed_attemps = 0
        self.prior_commands.show()
        response = self.prior_commands.exec_()
        if response == QDialog.Accepted:
            list_of_commands = self.prior_commands.return_selected_items()
            all_sent = False
            count_of_failed_attempts = 0
            while not all_sent:
                try:
                    for item in list_of_commands:
                        self.send_serial(item.text())
                        time.sleep(2)
                    all_sent = True
                except:
                    count_of_failed_attemps += 1
                    if count_of_failed_attempts > 10:
                        status_update('Exceed 10 attempts at re-issuing commands. Timeout occurred with radio communications')
                        all_sent = True

    def view_help_on_commands(self) -> None:
        self.help_on_program.show()

    def connect_to_arduino(self) -> None:
        #self.port_selection.show()
        self.on_connect_btn_pressed()

    def disconnect_from_arduino(self) -> None:
        self.on_disconnect_btn_pressed()
        self.ser = serial.Serial(timeout=0)
        self.toggle_tool_bar_and_menu_enabled_status()
        self.timer.stop()

    def closeEvent(self, event: QCloseEvent) -> None:
        """Handle Close event of the Widget."""
        exit_dialog = are_you_sure_dialog(self)
        if exit_dialog.exec_():
            #if self.ser.is_open:
            #    self.ser.close()
            if self.timer != None:
                self.timer.stop()

            if self.device != None and self.device.is_open:
                self.device.close()
            event.accept()

    def exit_button_pressed(self) -> None:
        exit_dialog = are_you_sure_dialog(self)
        if exit_dialog.exec_():
            #if self.ser.is_open:
            #    self.ser.close()
            if self.device.is_open:
                self.device.close()
            qApp.quit()        

    def are_there_more_than_one_routers(self) -> None:
        try:
            xnet = self.ser.get_network()
            xnet.start_discovery_process(deep=True, n_deep_scans=1)
            while xnet.is_discovery_running():
                  time.sleep(0.5)

        # Get the list of the nodes in the network.
            nodes = xnet.get_devices()
            if len(nodes) != 1:
                self.show_error_message("There are more than one satellites on this network!\nPlease check XBee network configuration!")
            else:
                mkr1000 = nodes[0]

        except:
             self.show_error_message("Something went wrong when scanning for satellites")                   # This indicates the response was incorrect.

    def config_xbee(self) -> bool:
        # The XBee 3 has to be configured as a controller,
        # 9600 baud, and in AT mode
        # put the radio in command mode:
            self.ser.flush();            # Clear the serial buffer
            self.ser.write(b"+++")       # Put the XBee 3 into 'Command Mode'
            time.sleep(0.100)            # Wait for the XBee to finish
            ok_response = b"OK\r"        # The ﻿response we expect.
            #  Read the text of the response into the response variable
            response = ""                #Create an empty string
            while len(response) < len(ok_response):    # As long as we did not get a response from the XBee
                #print(self.ser.in_waiting)
                if self.ser.in_waiting > 0:
                    response += str(self.ser.read(1)) # Read a single character at a time
            # If we got the right response, configure the radio and return true.
            response = self.ser.read(4)
            #print('xbee response:', response, "len(response)", len(response) , len(ok_response))
            if response == ok_response:
                self.ser.write(b"ATDH0013A200\r") # destination high- this will never be changed for the XBee 3 Series of radios
                time.sleep(0.100)
                msg = b"ATDL" + self.node_low + b"\r"
                self.ser.write(msg)                # destination low-REPLACE with the lower 32 bits of the address of the Coordinator for each separate network
                time.sleep(0.100);                 # Wait for the XBee
                self.ser.write(b"ATCN\r")          # Switch back to data mode
                return True
            else:
                return False                      # This indicates the response was incorrect.

    def start_comm(self) -> None:
        self.arduino_response.append("%s: Attempting to connect to %s" % (QDateTime.currentDateTime().toUTC().toString(Qt.ISODate), self.radio_config["sat_name"]))
        self.arduino_response.update()
        self.device = XBeeDevice(self.radio_config["port"], self.radio_config["baud"])
        self.device.open()
        self.device.set_pan_id(self.radio_config["pan"].encode())
        self.device.set_node_id(self.radio_config["name"])
        xbee_network = self.device.get_network()
        self.remote_device = xbee_network.discover_device(self.radio_config["sat_name"])
        #print(self.remote_device, type(self.remote_device))
        if type(self.remote_device) is None:
           self.arduino_response.append("%s: Unable to connect to %s\n\n" % (QDateTime.currentDateTime().toUTC().toString(), self.radio_config["sat_name"]))
           self.arduino_response.update()
           self.show_error_message("%s is not responding to connection request.\n\nSatellite may be out of range or unable to communicate at this time." % (self.radio_config["sat_name"]))
           self.device.close()
        else:
            epoch = QDateTime.currentDateTime().toSecsSinceEpoch()
            #print(epoch)
            self.send_serial('%s=%s' % ('start_comm', str(epoch)))

    @pyqtSlot()
    def on_connect_btn_pressed(self) -> None:
        """Open serial connection to the specified port."""
        if self.device is not None and self.device.is_open():
                  self.device.close()
        try:
            self.start_comm()
            self.toggle_tool_bar_and_menu_enabled_status()
            self.setup_timer()
        except Exception as e:
            self.show_error_message(str(e))
            self.toggle_tool_bar_and_menu_enabled_status()

    def show_error_message(self, error):
        msg = QMessageBox()
        msg.setIcon(QMessageBox.Critical)
        msg.setText("Error!!")
        msg.setInformativeText(error)
        msg.setWindowTitle("Critical Error Message")
        msg.exec_()
        self.toggle_tool_bar_and_menu_enabled_status()

    def on_disconnect_btn_pressed(self) -> None:
        """Close current serial connection."""
        if self.device is not None and self.device.is_open():
            self.device.close()
            self.device = None
            self.arduino_response.append("%s: Disconnected from %s\n\n" % (QDateTime.currentDateTime().toUTC().toString(Qt.ISODate), self.radio_config["sat_name"]))
            self.arduino_response.update()

    def toggle_tool_bar_and_menu_enabled_status(self):
        self.transmit_btn.setEnabled(not self.transmit_btn.isEnabled())
        for i in range(0, len(self.tool_bar_items) - 1):
            self.tool_bar_items[i].setEnabled( not self.tool_bar_items[i].isEnabled())
            self.menu_items[i].setEnabled( not self.menu_items[i].isEnabled())

    def remove_spaces(self, message: str) -> str:
        message.strip()
        message.replace(' ', '')
        return message

    def validate_command(self,arduino_msg: str) -> Tuple:
        value = ''
        arduino_msg = self.remove_spaces(arduino_msg)
        pos = arduino_msg.find('=')
        if pos > -1:
            value = arduino_msg[pos + 1:]
            arduino_msg = arduino_msg[0:pos]
        if arduino_msg in self.commands.keys():
            if value == '':
                return (True, self.commands[arduino_msg][0])
            else:
                return (True, self.commands[arduino_msg][0] + ' ' + value)
        else:
            return (False, None)

    def get_picture_from_arduino(self):
#        print('Starting get_picture_from_arduino')
        self.QByteArray2QPixelmap(self.grap_jpeg_from_arduino())
        new_pic = show_picture(image = self.image)
        new_pic.show()
        response = new_pic.exec_()
        if response == QDialog.Accepted:
            file_path, _ = QFileDialog.getSaveFileName(self, "Save Image", "",
                                    "PNG(*.png);;JPEG(*.jpg *.jpeg);;All Files(*.*) ")
            # if file path is blank return back
            if file_path == "":
                return
            # saving canvas at desired path
            self.image.save(file_path)

    def get_sensor_data_for_printing(self) -> None:
#        print("Starting get_sensor...")
        not_done = True
        text = ''
        self.timer.stop()
        while not_done:
            xbee_message = self.device.read_data()
            if xbee_message is not None:
                msg = xbee_message.data
                if msg != b'':
                    try:
                        text += msg.decode()
                        self.device.flush_queues()
                        pos = text.find('!')
                        if pos > -1:
                            text = text[:pos]
                            not_done = False
                    except Exception as e:
                        print(e)
                        print('An error occurred!')
        text = '%s: %s'%(QDateTime.currentDateTime().toUTC().toString(Qt.ISODate), text)
        self.get_rssi()
        self.arduino_response.append(text)
        self.arduino_response.update()
        self.setup_timer()

    def get_rssi(self) -> None:
        print('Starting get_rssi', flush = True)
        self.rssi = self.device.get_parameter("DB")
        print("Ending get_rssi", flush = True)

    def get_sat_rssi(self) -> None:
        print('Starting get_sat_rssi', flush = True)
        value = self.getack()
        text = '%s: Satellite RSSI is at -%d dbm'%(QDateTime.currentDateTime().toUTC().toString(Qt.ISODate),  int(value,16))
        self.arduino_response.append(text)
        self.arduino_response.update()
        self.repaint()
        print("Ending get_sat_rssi", flush = True)

            
    def display_rssi(self) -> None:
        text = '%s: Base RSSI is at -%d dbm'%(QDateTime.currentDateTime().toUTC().toString(Qt.ISODate), ord(self.rssi))
        self.arduino_response.append(text)
        self.arduino_response.update()
        self.repaint()

    def record_data(self) -> None:
        self.status_update('Started recording data...')
        self.csv_file_name = self.file_names[self.current_output_file][0] + '_recording_on_' + QDateTime.currentDateTime().toUTC().toString() + '.csv'
        self.csv_file = open(self.csv_file_name,'w')
        self.csv_file.write('UTC,' + self.file_names[self.current_output_file][1] + '\n')
        self.record = True

    def process_special_commands(self, validated_command) -> None:
        pos = validated_command.find(' ')
        if pos > -1:
            validated_command = validated_command[0:pos]
        if validated_command in self.perform.keys():
            self.current_output_file = validated_command
            self.perform[validated_command]()

    def getack(self) -> str:
        done = False
        while not done:
            xbee_message = self.device.read_data()
            if xbee_message != None:
                    message = xbee_message.data.decode()
                    done = True
                    self.get_rssi()
        self.device.flush_queues()
        return message


    def sendbyte(self, value) -> None:
        self.device.send_data(self.remote_device, value)

    def status_update(self, msg) -> None:
        response = '%s: %s'%(QDateTime.currentDateTime().toUTC().toString(Qt.ISODate), msg)
        self.arduino_response.append(response)
        self.arduino_response.update()
        self.repaint()

    def grap_jpeg_from_arduino(self):
        done = False
        self.timer.stop()
        received_bytes = bytearray()
        self.status_update( 'Asking ' + self.radio_config["sat_name"] + ' to connect to its camera')
        response = self.getack()
        print(response,flush=True)
        self.status_update(self.radio_config["sat_name"] + ':' + response)
        print('Starting grab', flush=True)
        self.device.flush_queues()
        while not done:
            xbee_message = self.device.read_data()
            if xbee_message is None:
                continue
            print("jpeg: len(received:",len(xbee_message.data))
            print("b'" + ''.join('\\x{:02x}'.format(x) for x in xbee_message.data) + "'")
            for data in xbee_message.data:
                received_bytes.append(data)
                if received_bytes[-1] == 0xd9 and received_bytes[-2] == 0xff:
                        done = True
                        break
        print('Stopped grabbing picture', flush=True)
        self.device.flush_queues()
        self.get_rssi()
        self.setup_timer()
        self.status_update(self.radio_config["sat_name"] + ': Picture transmission completed.')
        return QByteArray(received_bytes[0:-2])

    def QByteArray2QPixelmap(self, byte_array):
#        print('Starting QByteArray2QPixelmap')
        buffer = QBuffer(byte_array)
        buffer.open(QIODevice.ReadOnly)
        reader = QImageReader(buffer)
        self.image = QPixmap.fromImage(reader.read())

    def send_serial(self, arduino_msg: str) -> None:
        """Send a message to serial port (async)."""
        try:
            validated_command = self.validate_command(arduino_msg.lower())
            if validated_command[0]:
                self.prior_commands.append(arduino_msg)
                self.device.send_data(self.remote_device, validated_command[1].encode())
                self.device.flush_queues()
                self.process_special_commands(validated_command[1])
            else:
                self.arduino_response.append('%s: %s is not a valid command. Please re-enter.' % (QDateTime.currentDateTime().toUTC().toString(), arduino_msg))
                self.repaint()
        except Exception as e:
            print(e)
            self.arduino_response.append('%s: Satellite communication error on transmit.' % (QDateTime.currentDateTime().toUTC().toString(Qt.ISODate)))
            self.repaint()

    @pyqtSlot()
    def receive_serial_data(self) -> None:
        xbee_message = self.device.read_data()
        if xbee_message is not None:
            msg = xbee_message.data
            if msg != b'':
                try:
                    text = msg.decode()
                    self.device.flush_queues()
                    if self.record:
                        if text.find('EOT') > -1:
                            self.record = False;
                            text = text.replace('EOT','')
                        if len(text) > 0:
                            self.csv_file.write(text)
                        if not self.record:
                            self.csv_file.close()
                            text = "Recording stored in: " + self.csv_file_name
                            self.status_update(text)
                    else:
                        self.status_update(text)
                        self.arduino_response.update()
                    self.get_rssi()
                except:
                    for i in range(len(sys.exc_info())):
                                       print(sys.exc_info()[i])
                    print('An error occurred!')

    def on_transmit_btn_pressed(self) -> None:
        """Send message to serial port."""
        msg = self.user_input.text()
        self.user_input.clear()
        if msg != '':
            self.send_serial(msg)

    def fontDialog(self)-> None:
        font, ok = QFontDialog.getFont()
        if ok:
            self.arduino_response.setFont(font)

    def colorDialog(self) -> None:
        color = QColorDialog.getColor()
        self.arduino_response.setTextColor(color)

    def printDialog(self) -> None:
        self.print_it.printDialog()

    def printPDF(self) -> None:
        self.print_it.printPDF()

    def print_report(self) -> None:
        self.print_it.print_report()

    def load_ui(self):
        path = os.path.join(os.path.dirname(__file__), "form.ui")
        uic.loadUi(path, self)
        self.print_it = printer(self, self.arduino_response)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    main_window = MainWindow()
    main_window.show()
    app.exec()
