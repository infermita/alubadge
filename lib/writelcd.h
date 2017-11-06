#ifndef WRITELCD_H
#define WRITELCD_H

#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <string.h>

class WriteLcd
{
public:
    WriteLcd();
    void write_word(int data);
    void send_command(int comm);
    void send_data(int data);
    void init();
    void clear();
    void write(int x, int y, char data[]);
private:
    int LCDAddr;
    int BLEN;
    int fd;


};


#endif // WRITELCD_H
