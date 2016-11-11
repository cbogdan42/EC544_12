while true
do
	sshpass -p 'raspberry' scp img.jpg pi@192.168.1.147:~/position_display/public/images/
	sleep 1
done