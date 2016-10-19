#!/usr/bin/python
import os
import time
a = 1
while(1):
	os.system("sshpass -p 'raspberry' scp data_from_sensor.db pi@192.168.1.147:~/git/EC544_12/Challenge2/")
#	os.system("sudo sshpass -p 'raspberry' ./data_server/public/files/file.txt pi@192.168.1.147:~/git/EC544_12/Challenge2/data_server/public/files/")
	time.sleep(2)

