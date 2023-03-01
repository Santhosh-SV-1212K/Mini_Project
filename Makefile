obj-m += oled_display_drv.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

get_file:
	# change permission from root to user, group, other
	
	sudo chmod ugo+w /sys/bus/i2c/devices/1-003c/ssd1306
	# create soft link to device file
	
	ln -s /sys/bus/i2c/devices/1-003c/ssd1306 ./ssd1306

remove_file:
	rm ./ssd1306
	

clean: 
	make remove_file
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
