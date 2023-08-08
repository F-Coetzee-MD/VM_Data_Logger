# compile current c++ backend code and store exacutables in bin file

import os           as os
import subprocess   as sub
# g++ main.cpp sqlite3.o -Ipath\to\sqlite3
os.system("clear")

directories = [
    "../core/libs/date_and_time/",
    "../core/libs/data_parsers/",
    "../core/libs/file_manager/",
    "../core/libs/sqlite_API/",
    "../core/libs/json_API/",
    "../core/libs/delays/",
    "../core/libs/tcp_ip/",
    "../core/libs/dsp/",
    "../core/src/"
]

bin_folder      = "../bin/"
exe_name        = "data_logger"
command         = "g++ -g -O -Wall -lpthread -ldl -lm "
sql_command     = "../core/libs/sqlite_API/sqlite3/sqlite3.o -I../core/libs/sqlite_API/sqlite3/ "
files           = []

req = input("Compile code? (y or n)")

if req == "N" or req == "n" :
    os.system("clear")
    sub.run(bin_folder + exe_name, shell=True)
    quit()

os.system("clear")

for current_dir in directories :
    src_code = os.listdir(current_dir)
    for current_file in src_code :
        if current_file.endswith(".cpp"):
            command += current_dir + current_file + " "
            files.append(current_dir + current_file)

if len(files) > 0 :
    print("Compiled files:\n")
    print("============================================================")
    for fl in files : print(fl)
    print("============================================================\n\n")
    sub.run(command + sql_command + "-o " + bin_folder + exe_name, shell=True)

# ask user if he would like to run the code now

a = input("\nRun code? (y or n)")
os.system("clear")

if a == "Y" or a == "y" :
    sub.run(bin_folder + exe_name, shell=True)
