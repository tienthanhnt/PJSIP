#include "spi.h"

static void pabort(const char *s)
{
	perror(s);
	abort();
}
/* tranfer one byte */
unsigned char transfer(int fd , unsigned char byte_tx)
{
	int ret;
	uint8_t tx[1];
	uint8_t rx[1];
	tx[0] = byte_tx;
	rx[0] = 0;
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = 1,
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");

	printf("gia tri nhan duoc %x\n", rx[0]);
	return rx[0];
}
/* read channel state and number phone from stm32 */
char read_spi(int fd , char *rd_buf, char *flag)
{
	int i, num_byte, tmp, count_rx = 0;
    int channel;
	memset(rd_buf, 0, sizeof rd_buf);
    /*tranfer first byte for nothing*/
	transfer(fd,0xFF);

    /* read channel and state */
	tmp = transfer(fd,0xFF);
    channel = tmp >> 3;
    *flag = tmp & 0x07;
	printf("read_spi channel = %d, flag = %d \n", channel, *flag);

    /* calling state get number from stm32*/
    if (*flag == 0x01){
        while (1){
            tmp = transfer(fd,0xFF);
            printf("%d ", tmp);
            /* Get MinSQ and first digit */
            if (count_rx == 0){
                    num_byte = tmp >> 4;
                    printf("numbyte %d\n", num_byte);
                    rd_buf[0] = tmp & 0x0F;
                    count_rx++;
                    if (num_byte == 0){
                            printf("get wrong minSQ\n");
                            break;
                    }
                    //printf("%d ", rd_buf[0]);
            }
            /* Get rest digit */
            else if ((count_rx > 0) && (count_rx < num_byte)){
                    if ((count_rx % 2 != 0) && (count_rx < num_byte)){
                    rd_buf[count_rx] = tmp >> 4;
                            printf("rd_buf[%d]%d\n",count_rx, rd_buf[count_rx]);
                    count_rx++;
                            printf("%d\n", count_rx);
                    }
                    if ((count_rx % 2 == 0) && (count_rx < num_byte)) {
                            rd_buf[count_rx] = tmp & 0x0F;
                            printf("rd_buf[%d]%d\n",count_rx, rd_buf[count_rx]);
                            count_rx++;
                            printf("%d\n", count_rx);
                    }
            }
            /* get enough digit */
            if (count_rx == num_byte){
                    count_rx = 0;
                    for(i = 0; i< num_byte; i++){
                        if (rd_buf[i] ==10) rd_buf[i] = 0;      //digit 0 in analog phone = 10
                        printf("%d ",rd_buf[i]);
                        rd_buf[i] += 48;
                    }
                    break;
            }
        }
    }
    return channel;
}
void init_gpio(){
    int fd;
    fd = open("/sys/class/gpio/export", O_WRONLY);
	write(fd, "340", 3);
	close(fd);

	//Set gpio25 as input
	fd = open("/sys/class/gpio/gpio340/direction", O_WRONLY);
	write(fd, "in", 2);
	close(fd);

	//Set gpio25 interrupt
	fd = open("/sys/class/gpio/gpio340/edge", O_WRONLY);
	//write(fd1, "falling", 7);
	write(fd, "rising", 6);
	close(fd);
}
int init_spi(){
    int ret = 0;
	int fd;

	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	printf("spi mode: %d\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
    return fd;
}