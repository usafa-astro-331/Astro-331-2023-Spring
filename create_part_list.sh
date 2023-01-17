#!/bin/sh
# combine lab part lists into a consolidated part list
cat lab\ 01\ electrical\ power/parts.csv > parts.csv
cat lab\ 02\ camera\ payload/parts.csv >> parts.csv
cat lab\ 03\ communication/parts.csv >> parts.csv
cat lab\ 04\ attitude/parts.csv >> parts.csv
