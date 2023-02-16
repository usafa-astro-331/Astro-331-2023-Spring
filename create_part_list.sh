#!/bin/sh
# combine lab part lists into a consolidated part list
cat lab\ equipment/lab_equipment.csv > parts.csv
tail -n +2 lab\ 00\ metrology/parts00.csv >> parts.csv
tail -n +2 lab\ 01\ electrical\ power/parts01.csv >> parts.csv
tail -n +2 lab\ 02\ camera\ payload/parts02.csv >> parts.csv
tail -n +2 lab\ 03\ communication/parts03.csv >> parts.csv
tail -n +2 lab\ 04\ attitude/parts04.csv >> parts.csv 