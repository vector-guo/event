#ifndef SPI__H__
#define SPI__H__
#include"common.h"

int lgw_spi_open(void **spi_target_ptr);

unsigned char spi_read(int fd,unsigned char addr);
int spi_read_buffer(int fd, unsigned char addr, unsigned char *data, unsigned short size);

int spi_write_fifo(int fd, unsigned char addr, unsigned char *data, unsigned short size);
int spi_write_buffer(int fd,unsigned char addr ,unsigned char * data,int size);
int spi_write(int fd,unsigned char addr ,unsigned char data);

#endif
