#python script for EIC builds

###########################################
# INCLUDES
###########################################

import string
import subprocess
import pandas

###########################################
# USER VARIABLES
###########################################

PROJECT_NAME = "EIC"

FILE_TO_READ = "Configs.txt"
FILE_TO_WRITE = "../Logic/Configs/Configs.cpp"

STRING_TO_CHANGE = "stConfig.udDevice_ID ="

CSV_FILE_LOCATION = "MTAHandheldXbee.csv"

###########################################
# PRIVATE VARIABLES (DONT CHANGE)
###########################################

input_csv = pandas.read_csv(CSV_FILE_LOCATION)
dimensions = input_csv.shape

###########################################
# FUNCTIONS
###########################################

def modify(key, value):
	global counter
	DidSomethingChange = False
	
	print("Opening Read File")
	read_file = open(FILE_TO_READ,"r")
	
	print("Opening Write File")
	write_file = open(FILE_TO_WRITE,"w+")
	
	for line in read_file.readlines():
		if STRING_TO_CHANGE not in line: 
			write_file.write(line)
		else:
			write_file.write("    stConfig.udDevice_ID = 0x"+value+";\n")
			DidSomethingChange = True
	
	read_file.close()	
	write_file.close()
	
	if DidSomethingChange == True:
		print("DONE WRITING FILE: "+key)
		
		
###########################################
# MAIN
###########################################

#print(input_csv) #debug
print("Data Size is:", dimensions)

for counter in range(0,dimensions[0]):
	temp_name = input_csv.iloc[counter][0]
	name = "".join(temp_name)
	
	temp_addr = input_csv.iloc[counter][1]
	mac_address = "".join(temp_addr)
	
	mac_address = mac_address[-5:]
	
	print(name, mac_address)
	modify(name, mac_address)
	#subprocess.call(["del", file_location_read+'.o'], shell=True)
	subprocess.call(["make", ".\makefile", "-j8", "clean"])
	#subprocess.call(["make", ".\makefile", "-j8", "all"])
	subprocess.call(["studio_c", "-nosplash", "-application", "org.eclipse.cdt.managedbuilder.core.headlessbuild", "-data", "F:\Millers\RWPS\v5_workspace\Development_New_Handheld_1.5_Firmware\Development_New\EIC", "-build", "EIC"])
	subprocess.call(["rename", PROJECT_NAME + ".bin", name + ".bin"], shell = True)
	#break
	
print("------------------------------------------")	
print("ALL DONE!")
print("------------------------------------------")
print("TOTAL NO OF BINARIES BUILT: "+str(counter+1))
print("------------------------------------------")

#F:\Millers\RWPS\v5_workspace\Development_New_Handheld_1.5_Firmware\Development_New\EIC\studio.exe -nosplash -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data F:\Millers\RWPS\v5_workspace\Development_New_Handheld_1.5_Firmware\Development_New\EIC -build EIC