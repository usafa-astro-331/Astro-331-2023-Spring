import sys
import subprocess

# implement pip as a subprocess:
subprocess.check_call([sys.executable, '-m', 'pip', 'install', 
'pyqt5'])
subprocess.check_call([sys.executable, '-m', 'pip', 'install', 
'jinja2'])

# pip install pyqt5
# pip install jinja2