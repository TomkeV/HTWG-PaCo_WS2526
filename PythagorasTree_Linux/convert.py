#!/bin/python3

import sys
import os


logfile = open("log.log", 'r')

SERD = {}
OMPD = {}
OCLD = {}
BOCT = []

for line in logfile:
    stuff = line.strip().split(':')
    measurement_type = stuff[0]
    if measurement_type == "SERD" or measurement_type == "OMPD" or measurement_type == "OCLD":
        depth = stuff[1]
        time = stuff[3]

        if measurement_type  == "SERD": 
            if depth not in SERD:
                SERD[depth] = []
            SERD[depth].append(time)
        elif measurement_type == "OMPD": 
            if depth not in OMPD:
                OMPD[depth] = []
            OMPD[depth].append(time)
        elif measurement_type == "OCLD": 
            if depth not in OCLD:
                OCLD[depth] = []
            OCLD[depth].append(time)

        if measurement_type == "SERD":
            measurement_type = "Serial Depth"
        elif measurement_type == "OMPD":
            measurement_type = "OpenMP Depth"
        elif measurement_type == "OCLD":
            measurement_type = "OpenCL Depth"
        
        print(f"{measurement_type}={depth} Time={time}")
    elif measurement_type == "BOCT":
        build_time = stuff[1]
        print(f"Build OpenCL Time={build_time}")
        BOCT.append(build_time)

# Write to CSV files
with open("Messwerte.csv", 'w') as csvfile:
    csvfile.write(f"Serial measurements\n")
    for depth in SERD:
        csvfile.write(f"{depth},,")
        for time in SERD[depth]:
            csvfile.write(f"{time},")
        csvfile.write("\n")
    
    csvfile.write(f"\nOpenMP measurements\n")
    for depth in OMPD:
        csvfile.write(f"{depth},,")
        for time in OMPD[depth]:
            csvfile.write(f"{time},")
        csvfile.write("\n")

    csvfile.write(f"\nOpenCL measurements\n")
    for depth in OCLD:
        csvfile.write(f"{depth},,")
        for time in OCLD[depth]:
            csvfile.write(f"{time},")
        csvfile.write("\n")
        
    csvfile.write(f"\nBuild OpenCL times\n")
    for build_time in BOCT:
        csvfile.write(f"{build_time},")

logfile.close()