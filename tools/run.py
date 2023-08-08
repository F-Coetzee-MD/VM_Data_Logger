import os           as os
import subprocess   as sub

bin_folder = "../backend/bin/"
exe_name = "LIDAR-SBS"

while(True):
    os.system("clear")
    sub.run(bin_folder + exe_name, shell=True)


