#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "hidapi.h"


// Write data of size len into register reg, return success
int write_register(hid_device *handle, unsigned char reg, unsigned char * data, unsigned char len) {
	unsigned char buf[len + 5];	
	int res;
	// Set a register on the mic -- 
	buf[0] = 0; // First byte is report number
	buf[1] = reg; // register #
	buf[2] = 0; // space
	buf[3] = len; // length
	buf[4] = 0; // space
	for(int i=0i;i<len;i++) buf[5+i] = data[i];
	res = hid_write(handle, buf, len+5);
	return res;
}

// Read data of size len into ret at register reg, return success
int read_register(hid_device *handle, unsigned char reg, unsigned char * ret, unsigned char len) {
	int res;
	unsigned char buf[9];
	buf[0] = 0;
	buf[1] = reg;
	buf[2] = 0x80;
	buf[3] = len;
	buf[4] = 0;
	buf[5] = 0;
	buf[6] = 0;
	res = hid_write(handle, buf, 7);
	res = hid_read(handle, buf, 7+len);
	if(buf[0] == reg) {
		for(int i=0;i<len;i++) {
			ret[i] = buf[4+i];
		}
		return 1;
	}
	return 0;

}

// Returns 1 if auto report was read, 0 otherwise
int read_auto_report(hid_device *handle, unsigned short * angle, unsigned char *vad_activity) {
	int res;
	unsigned char buf[9];
	angle[0] = 0;
	vad_activity[0] = 0;

	res = hid_read(handle, buf, 9);
	if(buf[0] == 0xFF) {
		angle[0] = buf[6]*256 + buf[5];
		vad_activity[0] = buf[4];
		return 1;
	}
	return 0;

}

int main(int argc, char* argv[])
{
	hid_device *handle;
	int res;
	#define MAX_STR 255
	wchar_t wstr[MAX_STR];
	unsigned char buf[4];

	handle = hid_open(0x2886, 0x07, NULL);

	read_register(handle, 0x10, buf, 1);
	printf("mic gain is %d\n", (char)buf[0]);

	buf[0] = 35;
	write_register(handle, 0x10, buf, 1);

	buf[0] = 0; // spk proc bypass
	write_register(handle, 0x13, buf, 1);

	buf[0] = 0; // no AGC
	write_register(handle, 0x2A, buf, 1);

	


	while(1) {
		unsigned short angle;
		unsigned char vad_activity;


		if(read_auto_report(handle, &angle, &vad_activity)) {
			printf("angle %d vad %d\n", angle, vad_activity);
		}
		usleep(100000);
	}
	return 0;
}


