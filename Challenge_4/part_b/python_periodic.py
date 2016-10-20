#!/usr/bin/python
import os
import time
a = 1
while(1):
	#os.system("cp ../data_from_sensor.db .")
	os.system("sshpass -p 'raspberry' scp ../data_from_sensor.db pi@192.168.1.147:~/git/EC544_12/Challenge_4/part_b/")
	os.system("sshpass -p 'raspberry' scp ../data_server/public/files/file.txt pi@192.168.1.147:~/git/EC544_12/Challenge_4/part_b/data_server/public/files/")
	time.sleep(2)

