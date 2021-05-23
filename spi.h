#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <poll.h>
#include <string.h>

static const char   *device = "/dev/spidev0.0";
static uint8_t      mode;
static uint8_t      bits = 8;
static uint32_t     speed = 1125000;
static uint16_t     delay = 2000;
int                 fd1;
unsigned char       rd_buf[32];
int                 fd2,ret;
struct pollfd       pfd1;
char                c;

static void pabort(const char *s);
unsigned char transfer(int fd , unsigned char byte_tx);
char read_spi(int fd , char *rd_buf, char *flag);
void init_gpio(void);
int init_spi(void);