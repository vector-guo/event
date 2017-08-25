#include"spi_op.h"
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <fcntl.h>        /* open */
#include<stdlib.h>
#include<stdio.h>
#include <string.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define LGW_BURST_CHUNK     1024
#define READ_ACCESS     0x00
#define WRITE_ACCESS    0x80
#define SPI_SPEED       800000
#define SPI_DEV_PATH    "/dev/spidev0.0"



unsigned char spi_read(int fd,unsigned char addr)
{
    //bt_devide_msg msg;
    unsigned char ucRegVal;
	int ret,i;
	unsigned char tx[20];

    tx[0] =addr &0x7f;
    tx[1] = 0x0;
	unsigned char rx[ARRAY_SIZE(tx)] = {0, };
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = 2,
		//.delay_usecs = udelay,
		//.speed_hz = speed,
		//.bits_per_word = bits,
	};
	

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);


        if (ret != (int)tr.len) {
            printf("read err\n");
            }

							
  return rx[1];
}
int spi_read_buffer(int fd, unsigned char addr, unsigned char *data, unsigned short size)
{
    int spi_device;
    unsigned char command[2];
    unsigned char command_size;
    struct spi_ioc_transfer k[2];
    int size_to_do, chunk_size, offset;
    int byte_transfered = 0;
    int i;

    /* check input parameters */
    
    if (size == 0) {
        printf("ERROR: BURST OF NULL LENGTH\n");
        return -1;
    }

   
        command[0] = READ_ACCESS | (addr & 0x7F);
        command_size = 1;
    size_to_do = size;

    /* I/O transaction */
    memset(&k, 0, sizeof(k)); /* clear k */
    k[0].tx_buf = (unsigned long) &command[0];
    k[0].len = command_size;
    k[0].cs_change = 0;
    k[1].cs_change = 0;
    for (i=0; size_to_do > 0; ++i) {
        chunk_size = (size_to_do < LGW_BURST_CHUNK) ? size_to_do : LGW_BURST_CHUNK;
        offset = i * LGW_BURST_CHUNK;
        k[1].rx_buf = (unsigned long)(data + offset);
        k[1].len = chunk_size;
        byte_transfered += (ioctl(fd, SPI_IOC_MESSAGE(2), &k) - k[0].len );
        printf("BURST READ: to trans %d # chunk %d # transferred %d \n", size_to_do, chunk_size, byte_transfered);
        size_to_do -= chunk_size;  /* subtract the quantity of data already transferred */
    }

    /* determine return code */
    if (byte_transfered != size) {
        printf("ERROR: SPI BURST READ FAILURE\n");
        return ERROR;
    } else {
        printf("Note: SPI burst read success\n");
        return SUCCESS;
    }
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Burst (multiple-byte) write */
int spi_write_fifo(int fd, unsigned char addr, unsigned char *data, unsigned short size) {
    int spi_device;
    unsigned char command[2];
    unsigned char command_size;
    struct spi_ioc_transfer k[2];
    int size_to_do, chunk_size, offset;
    int byte_transfered = 0;
    int i;

    /* check input parameters */
    

    /* prepare command byte */
   
        command[0] = WRITE_ACCESS | (addr & 0x7F);
        command_size = 1;
    
    size_to_do = size;

    /* I/O transaction */
    memset(&k, 0, sizeof(k)); /* clear k */
    k[0].tx_buf = (unsigned long) &command[0];
    k[0].len = command_size;
    k[0].cs_change = 0;
    k[1].cs_change = 0;
    for (i=0; size_to_do > 0; ++i) {
        chunk_size = (size_to_do < LGW_BURST_CHUNK) ? size_to_do : LGW_BURST_CHUNK;
        offset = i * LGW_BURST_CHUNK;
        k[1].tx_buf = (unsigned long)(data + offset);
        k[1].len = chunk_size;
        byte_transfered += (ioctl(fd, SPI_IOC_MESSAGE(2), &k) - k[0].len );
        printf("BURST WRITE: to trans %d # chunk %d # transferred %d \n", size_to_do, chunk_size, byte_transfered);
        size_to_do -= chunk_size; /* subtract the quantity of data already transferred */
    }

    /* determine return code */
    if (byte_transfered != size) {
        printf("ERROR: SPI BURST WRITE FAILURE\n");
        return ERROR;
    } else {
        printf("Note: SPI burst write success\n");
        return SUCCESS;
    }
}

int spi_write_buffer(int fd,unsigned char addr ,unsigned char * data,int size)
{

    unsigned char ucRegVal;
	int ret,i;
	unsigned char tx[1024];
    memset(tx,0,1024);

    tx[0] = 0x80 | (addr & 0x7F);

    memcpy(&tx[1],data,size);

    
	unsigned char rx[ARRAY_SIZE(tx)] = {0, };
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = size+2,
		.speed_hz = SPI_SPEED,
        .cs_change = 0,
        .bits_per_word = 8,
		//.delay_usecs = udelay,
		//.speed_hz = speed,
		//.bits_per_word = bits,
	};
	

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);


        if (ret != (int)tr.len) {
            printf("read err\n");
            }

	
							
  return 1;
}

int spi_write(int fd,unsigned char addr ,unsigned char data)
{
    //bt_devide_msg msg;
    unsigned char ucRegVal;
	int ret,i;
	unsigned char tx[20];

    tx[0] = 0x80 | (addr & 0x7F);
    tx[1] = data;
    //tx[2] = 0;
	unsigned char rx[ARRAY_SIZE(tx)] = {0, };
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = 2,
		.speed_hz = SPI_SPEED,
        .cs_change = 0,
        .bits_per_word = 8,
		//.delay_usecs = udelay,
		//.speed_hz = speed,
		//.bits_per_word = bits,
	};
	

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);


        if (ret != (int)tr.len) {
            printf("read err\n");
            }

	
							
  return 1;
}


int lgw_spi_open(void **spi_target_ptr) {
    int *spi_device = NULL;
    int dev;
    int a=0, b=0;
    int i;

    /* check input variables */
   // CHECK_NULL(spi_target_ptr); /* cannot be null, must point on a void pointer (*spi_target_ptr can be null) */

    /* allocate memory for the device descriptor */
    spi_device = malloc(sizeof(int));
    if (spi_device == NULL) {
        printf("ERROR: MALLOC FAIL\n");
        return ERROR;
    }

    /* open SPI device */
    dev = open(SPI_DEV_PATH, O_RDWR);
    if (dev < 0) {
        printf("ERROR: failed to open SPI device %s\n", SPI_DEV_PATH);
        return ERROR;
    }

    /* setting SPI mode to 'mode 0' */
    i = SPI_MODE_0;
    a = ioctl(dev, SPI_IOC_WR_MODE, &i);
    b = ioctl(dev, SPI_IOC_RD_MODE, &i);
    if ((a < 0) || (b < 0)) {
        printf("ERROR: SPI PORT FAIL TO SET IN MODE 0\n");
        close(dev);
        free(spi_device);
        return ERROR;
    }

    /* setting SPI max clk (in Hz) */
    i = SPI_SPEED;
    a = ioctl(dev, SPI_IOC_WR_MAX_SPEED_HZ, &i);
    b = ioctl(dev, SPI_IOC_RD_MAX_SPEED_HZ, &i);
    if ((a < 0) || (b < 0)) {
        printf("ERROR: SPI PORT FAIL TO SET MAX SPEED\n");
        close(dev);
        free(spi_device);
        return ERROR;
    }

    /* setting SPI to MSB first */
    i = 0;
    a = ioctl(dev, SPI_IOC_WR_LSB_FIRST, &i);
    b = ioctl(dev, SPI_IOC_RD_LSB_FIRST, &i);
    if ((a < 0) || (b < 0)) {
        printf("ERROR: SPI PORT FAIL TO SET MSB FIRST\n");
        close(dev);
        free(spi_device);
        return ERROR;
    }

    /* setting SPI to 8 bits per word */
    i = 0;
    a = ioctl(dev, SPI_IOC_WR_BITS_PER_WORD, &i);
    b = ioctl(dev, SPI_IOC_RD_BITS_PER_WORD, &i);
    if ((a < 0) || (b < 0)) {
        printf("ERROR: SPI PORT FAIL TO SET 8 BITS-PER-WORD\n");
        close(dev);
        return ERROR;
    }

    *spi_device = dev;
    *spi_target_ptr = (void *)spi_device;
    printf("Note: SPI port opened and configured ok\n");
    return SUCCESS;
}
