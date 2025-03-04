#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <string.h>
#include <time.h>

int LCDAddr = 0x27;
int BLEN = 1;
int fd;


void write_word(int data){
	int temp = data;
	if ( BLEN == 1 )
		temp |= 0x08;
	else
		temp &= 0xF7;
	wiringPiI2CWrite(fd, temp);
}

void send_command(int comm){
	int buf;
	// Send bit7-4 firstly
	buf = comm & 0xF0;
	buf |= 0x04;			// RS = 0, RW = 0, EN = 1
	write_word(buf);
	delay(2);
	buf &= 0xFB;			// Make EN = 0
	write_word(buf);

	// Send bit3-0 secondly
	buf = (comm & 0x0F) << 4;
	buf |= 0x04;			// RS = 0, RW = 0, EN = 1
	write_word(buf);
	delay(2);
	buf &= 0xFB;			// Make EN = 0
	write_word(buf);
}

void send_data(int data){
	int buf;
	// Send bit7-4 firstly
	buf = data & 0xF0;
	buf |= 0x05;			// RS = 1, RW = 0, EN = 1
	write_word(buf);
	delay(2);
	buf &= 0xFB;			// Make EN = 0
	write_word(buf);

	// Send bit3-0 secondly
	buf = (data & 0x0F) << 4;
	buf |= 0x05;			// RS = 1, RW = 0, EN = 1
	write_word(buf);
	delay(2);
	buf &= 0xFB;			// Make EN = 0
	write_word(buf);
}

void init(){
	send_command(0x33);	// Must initialize to 8-line mode at first
	delay(5);
	send_command(0x32);	// Then initialize to 4-line mode
	delay(5);
	send_command(0x28);	// 2 Lines & 5*7 dots
	delay(5);
	send_command(0x0C);	// Enable display without cursor
	delay(5);
	send_command(0x01);	// Clear Screen
	wiringPiI2CWrite(fd, 0x08);
}

void clear(){
	send_command(0x01);	//clear Screen
}

void write(int x, int y, char data[]){
	int addr, i;
	int tmp;
	if (x < 0)  x = 0;
	if (x > 15) x = 15;
	if (y < 0)  y = 0;
	if (y > 1)  y = 1;

	// Move cursor
	addr = 0x80 + 0x40 * y + x;
	send_command(addr);

	tmp = strlen(data);
	for (i = 0; i < tmp; i++){
		send_data(data[i]);
	}
}

void temp_display(){
    char letters[10];
    float temp, hum; 
    srand(time(0));
    temp = 0 + rand() % 300 + (rand() % 10) / 10.0;
    hum = 0 + rand() %100 + (rand() % 10) / 10.0;

    write(0, 0 , "temp");
    write(6, 0 , "hum");
    write(11, 0 , "time");
 



    int hour = rand() %24;
    int minute = rand()%60;


    // display temperature
     char str[7]; 
     sprintf(str, "%.1f", temp);
     write(0, 1, str);
	

    // display humidity
    sprintf(str, "%.1f", hum);
    write(6, 1, str);

    //  display time in HH:MM format


    sprintf(str, "%02d:%02d", hour, minute);
    write(11, 1, str);
	

}


void main(){
	fd = wiringPiI2CSetup(LCDAddr);
	init();

while (1) {
    temp_display();
    
	delay(6000);
    }
	// clear();
}
