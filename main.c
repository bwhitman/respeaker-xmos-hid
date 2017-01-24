#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hidapi.h"

//  gcc -L/usr/local/lib -I/usr/local/include/hidapi hidapitest.c -o hidapitest -lhidapi

/*
unsigned short read_short(char register) {

}
*/

int main(int argc, char* argv[])
{
	int res;
	unsigned char buf[65];
	#define MAX_STR 255
	wchar_t wstr[MAX_STR];
	hid_device *handle;
	int i;

	// Enumerate and print the HID devices on the system
	struct hid_device_info *devs, *cur_dev;
	/*
	devs = hid_enumerate(0x0, 0x0);
	cur_dev = devs;	
	while (cur_dev) {
		printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls",
			cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
		printf("\n");
		printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
		printf("  Product:      %ls\n", cur_dev->product_string);
		printf("\n");
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);
	*/

	// Open the device using the VID, PID,
	// and optionally the Serial number.
	handle = hid_open(0x2886, 0x07, NULL);

	// Read the Manufacturer String
	res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	printf("Manufacturer String: %ls\n", wstr);

	// Read the Product String
	res = hid_get_product_string(handle, wstr, MAX_STR);
	printf("Product String: %ls\n", wstr);

	// Read the Serial Number String
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	printf("Serial Number String: %ls", wstr);
	printf("\n");
	/*
	// Send a Feature Report to the device
	buf[0] = 0x2; // First byte is report number
	buf[1] = 0xa0;
	buf[2] = 0x0a;

	res = hid_send_feature_report(handle, buf, 17);
	// Read a Feature Report from the device
	buf[0] = 0x2;
	res = hid_get_feature_report(handle, buf, sizeof(buf));

	// Print out the returned buffer.
	printf("Feature Report\n   ");
	for (i = 0; i < res; i++)
		printf("%02hhx ", buf[i]);
	printf("\n");
	*/
	// Set the hid_read() function to be non-blocking.
/*
	var tmp = [
        reportID //0
        ,reg,0,length,0
		,arr[0],arr[1],arr[2],arr[3]
        ];
        device.write(tmp);
*/
	hid_set_nonblocking(handle, 1);
	/*
	// Set a register on the mic -- 
	buf[0] = 0; // First byte is report number
	buf[1] = 0x00; // register #
	buf[2] = 0; // space
	buf[3] = 0x04; // length
	buf[4] = 0; // space

	buf[5] = 0x01; // inverted from the command -- R, G, B, (1=on/=0off)
	buf[6] = 0x00; // b
	buf[7] = 0x00; // g
	buf[8] = 0x11; // r

	res = hid_write(handle, buf, 9);
	*/

	// To get something,, say voice angle...
	// 2 bytes at 0x45
	// we first write somethign then read it back
	// we write
	// reportID, reg,0x80,length,0,0,0
    // and read it back
	while(1) {
		buf[0] = 0;
		buf[1] = 0x45;
		buf[2] = 0x80;
		buf[3] = 2;
		buf[4] = 0;
		buf[5] = 0;
		buf[6] = 0;
		//res = hid_write(handle, buf, 7);
		res = hid_read(handle, buf, 9);
		//printf("angle %d %d %d %d %d %d %d %d %d %d\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8]);
		if(buf[0]==0xFF) {
			printf("%d° (%d)\n", buf[6]*256 + buf[5], buf[4]);
		}
		usleep(10000);
	}

	// Send an Output report to request the state (cmd 0x81)
	//buf[1] = 0x81;
	//hid_write(handle, buf, 65);
	/*
	// Read requested state
	res = hid_read(handle, buf, 65);
	if (res < 0)
		printf("Unable to read()\n");

	// Print out the returned buffer.
	for (i = 0; i < res; i++)
		printf("buf[%d]: %d\n", i, buf[i]);
	*/
	return 0;
}


