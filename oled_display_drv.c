#define pr_fmt(fmt) "%s: " fmt, KBUILD_MODNAME

#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/delay.h>

#define I2C_BUS_AVAILABLE 1
#define DEVICE_NAME "oled_ssd1306_i2c"
#define SSD1306_ADDRESS 0x3C
#define SSD1306_MAX_SEG	128
#define SSD1306_MAX_LINE 7
#define SSD1306_DEF_FONT_SIZE 5

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("I2C OLED SSD1306 DRIVER");

static struct i2c_adapter *ssd1306_i2c_adapter = NULL;
static struct i2c_client *ssd1306_i2c_client = NULL;
static int line_nu = 0;

/* array to store letters */
static const unsigned char SSD1306_font[][SSD1306_DEF_FONT_SIZE]= 
{
    {0x00, 0x00, 0x00, 0x00, 0x00},   // space
    {0x00, 0x00, 0x2f, 0x00, 0x00},   // !
    {0x00, 0x07, 0x00, 0x07, 0x00},   // "
    {0x14, 0x7f, 0x14, 0x7f, 0x14},   // #
    {0x24, 0x2a, 0x7f, 0x2a, 0x12},   // $
    {0x23, 0x13, 0x08, 0x64, 0x62},   // %
    {0x36, 0x49, 0x55, 0x22, 0x50},   // &
    {0x00, 0x05, 0x03, 0x00, 0x00},   // '
    {0x00, 0x1c, 0x22, 0x41, 0x00},   // (
    {0x00, 0x41, 0x22, 0x1c, 0x00},   // )
    {0x14, 0x08, 0x3E, 0x08, 0x14},   // *
    {0x08, 0x08, 0x3E, 0x08, 0x08},   // +
    {0x00, 0x00, 0xA0, 0x60, 0x00},   // ,
    {0x08, 0x08, 0x08, 0x08, 0x08},   // -
    {0x00, 0x60, 0x60, 0x00, 0x00},   // .
    {0x20, 0x10, 0x08, 0x04, 0x02},   // /

    {0x3E, 0x51, 0x49, 0x45, 0x3E},   // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00},   // 1
    {0x42, 0x61, 0x51, 0x49, 0x46},   // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31},   // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10},   // 4
    {0x27, 0x45, 0x45, 0x45, 0x39},   // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30},   // 6
    {0x01, 0x71, 0x09, 0x05, 0x03},   // 7
    {0x36, 0x49, 0x49, 0x49, 0x36},   // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E},   // 9

    {0x00, 0x36, 0x36, 0x00, 0x00},   // :
    {0x00, 0x56, 0x36, 0x00, 0x00},   // ;
    {0x08, 0x14, 0x22, 0x41, 0x00},   // <
    {0x14, 0x14, 0x14, 0x14, 0x14},   // =
    {0x00, 0x41, 0x22, 0x14, 0x08},   // >
    {0x02, 0x01, 0x51, 0x09, 0x06},   // ?
    {0x32, 0x49, 0x59, 0x51, 0x3E},   // @

    {0x7C, 0x12, 0x11, 0x12, 0x7C},   // A
    {0x7F, 0x49, 0x49, 0x49, 0x36},   // B
    {0x3E, 0x41, 0x41, 0x41, 0x22},   // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C},   // D
    {0x7F, 0x49, 0x49, 0x49, 0x41},   // E
    {0x7F, 0x09, 0x09, 0x09, 0x01},   // F
    {0x3E, 0x41, 0x49, 0x49, 0x7A},   // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F},   // H
    {0x00, 0x41, 0x7F, 0x41, 0x00},   // I
    {0x20, 0x40, 0x41, 0x3F, 0x01},   // J
    {0x7F, 0x08, 0x14, 0x22, 0x41},   // K
    {0x7F, 0x40, 0x40, 0x40, 0x40},   // L
    {0x7F, 0x02, 0x0C, 0x02, 0x7F},   // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F},   // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E},   // O
    {0x7F, 0x09, 0x09, 0x09, 0x06},   // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E},   // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46},   // R
    {0x46, 0x49, 0x49, 0x49, 0x31},   // S
    {0x01, 0x01, 0x7F, 0x01, 0x01},   // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F},   // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F},   // V
    {0x3F, 0x40, 0x38, 0x40, 0x3F},   // W
    {0x63, 0x14, 0x08, 0x14, 0x63},   // X
    {0x07, 0x08, 0x70, 0x08, 0x07},   // Y
    {0x61, 0x51, 0x49, 0x45, 0x43},   // Z

    {0x00, 0x7F, 0x41, 0x41, 0x00},   // [
    {0x55, 0xAA, 0x55, 0xAA, 0x55},   // Backslash (Checker pattern)
    {0x00, 0x41, 0x41, 0x7F, 0x00},   // ]
    {0x04, 0x02, 0x01, 0x02, 0x04},   // ^
    {0x40, 0x40, 0x40, 0x40, 0x40},   // _
    {0x00, 0x03, 0x05, 0x00, 0x00},   // `

    {0x20, 0x54, 0x54, 0x54, 0x78},   // a
    {0x7F, 0x48, 0x44, 0x44, 0x38},   // b
    {0x38, 0x44, 0x44, 0x44, 0x20},   // c
    {0x38, 0x44, 0x44, 0x48, 0x7F},   // d
    {0x38, 0x54, 0x54, 0x54, 0x18},   // e
    {0x08, 0x7E, 0x09, 0x01, 0x02},   // f
    {0x18, 0xA4, 0xA4, 0xA4, 0x7C},   // g
    {0x7F, 0x08, 0x04, 0x04, 0x78},   // h
    {0x00, 0x44, 0x7D, 0x40, 0x00},   // i
    {0x40, 0x80, 0x84, 0x7D, 0x00},   // j
    {0x7F, 0x10, 0x28, 0x44, 0x00},   // k
    {0x00, 0x41, 0x7F, 0x40, 0x00},   // l
    {0x7C, 0x04, 0x18, 0x04, 0x78},   // m
    {0x7C, 0x08, 0x04, 0x04, 0x78},   // n
    {0x38, 0x44, 0x44, 0x44, 0x38},   // o
    {0xFC, 0x24, 0x24, 0x24, 0x18},   // p
    {0x18, 0x24, 0x24, 0x18, 0xFC},   // q
    {0x7C, 0x08, 0x04, 0x04, 0x08},   // r
    {0x48, 0x54, 0x54, 0x54, 0x20},   // s
    {0x04, 0x3F, 0x44, 0x40, 0x20},   // t
    {0x3C, 0x40, 0x40, 0x20, 0x7C},   // u
    {0x1C, 0x20, 0x40, 0x20, 0x1C},   // v
    {0x3C, 0x40, 0x30, 0x40, 0x3C},   // w
    {0x44, 0x28, 0x10, 0x28, 0x44},   // x
    {0x1C, 0xA0, 0xA0, 0xA0, 0x7C},   // y
    {0x44, 0x64, 0x54, 0x4C, 0x44},   // z

    {0x00, 0x10, 0x7C, 0x82, 0x00},   // {
    {0x00, 0x00, 0xFF, 0x00, 0x00},   // |
    {0x00, 0x82, 0x7C, 0x10, 0x00},   // }
    {0x00, 0x06, 0x09, 0x09, 0x06}    // ~ (Degrees)
};

/* SSD1306 write */
static void SSD1306_Write(bool is_cmd, unsigned char data)
{
  unsigned char buf[2] = {0}; /* 0-command 1-Data */
  int ret;

  if( is_cmd == true ) {
      buf[0] = 0x00;
  }
  else {
      buf[0] = 0x40;
  }
  buf[1] = data;
  ret = i2c_master_send(ssd1306_i2c_client, buf, 2); /* write data to i2c drive */
}

/* fill oled with data */
static void SSD1306_Fill(unsigned char data)
{
  unsigned int total  = 128 * 8;  // 8 pages x 128 segments x 8 bits of data
  unsigned int i      = 0;

  //Fill the Display
  SSD1306_Write(true, 0x21);              // cmd for the column start and end address
  SSD1306_Write(true, 0);         // column start addr
        SSD1306_Write(true, SSD1306_MAX_SEG-1); // column end addr
        SSD1306_Write(true, 0x22);              // cmd for the page start and end address
        SSD1306_Write(true, 0);            // page start addr
        SSD1306_Write(true, SSD1306_MAX_LINE);
  for(i = 0; i < total; i++)
  {
      SSD1306_Write(false, data);
  }
  line_nu=0;

}

/* set new Line upto 7*/
static void setNewLine(void) {
	SSD1306_Write(true, 0x21);              // cmd for the column start and end address
	SSD1306_Write(true, 0);         // column start addr
  	SSD1306_Write(true, SSD1306_MAX_SEG-1); // column end addr

  	SSD1306_Write(true, 0x22);              // cmd for the page start and end address
  	SSD1306_Write(true, (++line_nu)%(SSD1306_MAX_LINE+1));            // page start addr
  	SSD1306_Write(true, SSD1306_MAX_LINE);  // page end addr
}

//static void display_text(char *str) {
static void printd(char *str) {
	unsigned char buf;
	char space;
        int i, j;
	
	space = ' ';
	for(j = 0; j < strlen(str); ++j) {
		if(str[j] == '\n') {
			setNewLine();
		}
		else {
			for(i = 0; i < SSD1306_DEF_FONT_SIZE; ++i) {
                        	//buf[0] = 0x40;
                        	buf = SSD1306_font[str[j]-32][i]; // Get the data to be displayed from LookUptable
                        	SSD1306_Write(false, buf);  // write data to the OLED
			}
                }
        }
}

/* display init */
static int SSD1306_DisplayInit(void)
{
  msleep(100);               // delay

  /*
  ** Commands to initialize the SSD_1306 OLED Display
  */
  SSD1306_Write(true, 0xAE); // Entire Display OFF
  SSD1306_Write(true, 0xD5); // Set Display Clock Divide Ratio and Oscillator Frequency
  SSD1306_Write(true, 0x80); // Default Setting for Display Clock Divide Ratio and Oscillator Frequency that is recommended
  SSD1306_Write(true, 0xA8); // Set Multiplex Ratio
  SSD1306_Write(true, 0x3F); // 64 COM lines
  SSD1306_Write(true, 0xD3); // Set display offset
  SSD1306_Write(true, 0x00); // 0 offset
  SSD1306_Write(true, 0x40); // Set first line as the start line of the display
  SSD1306_Write(true, 0x8D); // Charge pump
  SSD1306_Write(true, 0x14); // Enable charge dump during display on
  SSD1306_Write(true, 0x20); // Set memory addressing mode
  SSD1306_Write(true, 0x00); // Horizontal addressing mode
  SSD1306_Write(true, 0xA1); // Set segment remap with column address 127 mapped to segment 0
  SSD1306_Write(true, 0xC8); // Set com output scan direction, scan from com63 to com 0
  SSD1306_Write(true, 0xDA); // Set com pins hardware configuration
  SSD1306_Write(true, 0x12); // Alternative com pin configuration, disable com left/right remap
  SSD1306_Write(true, 0x81); // Set contrast control
  SSD1306_Write(true, 0x80); // Set Contrast to 128
  SSD1306_Write(true, 0xD9); // Set pre-charge period
  SSD1306_Write(true, 0xF1); // Phase 1 period of 15 DCLK, Phase 2 period of 1 DCLK
  SSD1306_Write(true, 0xDB); // Set Vcomh deselect level
  SSD1306_Write(true, 0x20); // Vcomh deselect level ~ 0.77 Vcc
  SSD1306_Write(true, 0xA4); // Entire display ON, resume to RAM content display
  SSD1306_Write(true, 0xA6); // Set Display in Normal Mode, 1 = ON, 0 = OFF
  SSD1306_Write(true, 0x2E); // Deactivate scroll
  SSD1306_Write(true, 0xAF); // Display ON in normal mode

  //Clear the display
  SSD1306_Fill(0x00);

  return 0;
}

//display the string from file(overlap) 
static ssize_t ssd1306_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
	int len, i;
	char str[27];
	
/*	if(strcmp(buf,"clear")==0){
		SSD1306_Fill(0x00);
		pr_info("cleared.....\n");
	}
	pr_info("buffer:%s\n",buf);*/


	snprintf(str, 26, "%s", buf);
	len = strlen(str);
	/*if(len < 26)
                for(i = len; i < 26; ++i)
                        printd(" ");*/
	pr_info("write on file0: %s\n", str);
	printd(str); // str to print 

		
	if(strncmp(str,"clear\n",6)==0){

		SSD1306_Fill(0x00);
		pr_info("cleared....\n");
	}
	return count;
}
DEVICE_ATTR_WO(ssd1306);

/* display the string from file(new) */
static ssize_t ssd1306_new_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count) {
        char str[4028];

        snprintf(str, 4028, "%s", buf);
        pr_info("write on file1: %s\n", str);
	/* set curser */
        SSD1306_Write(true, 0x21);              // cmd for the column start and end address
        SSD1306_Write(true, 0);         // column start addr
        SSD1306_Write(true, SSD1306_MAX_SEG-1); // column end addr

        SSD1306_Write(true, 0x22);              // cmd for the page start and end address
        SSD1306_Write(true, 0);            // page start addr
        SSD1306_Write(true, SSD1306_MAX_LINE);

        /* clear display */
	SSD1306_Fill(0x00);
	
	printd(str); /* str to print */
        return count;
}
DEVICE_ATTR_WO(ssd1306_new);

/* ssd1306_i2c_probe */
static int ssd1306_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id) {
	struct device *dev;

	SSD1306_DisplayInit();
	
	/**************************************************/
	printd("Welcome\n");

	/* create device file */
	dev = &client -> dev;
	device_create_file(dev, &dev_attr_ssd1306); /* overlap */
	//device_create_file(dev, &dev_attr_ssd1306_new); /* flush */


	/**************************************************/
		
	pr_info("OLED Probed.\n");
	return 0;
}

/* ssd1306_i2c_remove */
static int ssd1306_i2c_remove(struct i2c_client *client) {
	
	/* set curser */
	SSD1306_Write(true, 0x21);              // cmd for the column start and end address
	SSD1306_Write(true, 0);         // column start addr
	SSD1306_Write(true, SSD1306_MAX_SEG-1); // column end addr

	SSD1306_Write(true, 0x22);              // cmd for the page start and end address
	SSD1306_Write(true, 0);            // page start addr
	SSD1306_Write(true, SSD1306_MAX_LINE);

	/* clear display */
	SSD1306_Fill(0x00);
	
	/* display off */
	SSD1306_Write(true, 0xAE);

	pr_info("OLED Removed.\n");
	return 0;
}

/* ssd1306 id_table */
static const struct i2c_device_id ssd1306_i2c_id[] = {
	{DEVICE_NAME, 0},
	{ }
};
MODULE_DEVICE_TABLE(i2c, ssd1306_i2c_id);

/* ssd1306 i2c_driver structure */
static struct i2c_driver ssd1306_i2c_driver = {
	.driver = {
		.name 	= DEVICE_NAME,
		.owner 	= THIS_MODULE,
	},
	.probe 	= ssd1306_i2c_probe,
	.remove = ssd1306_i2c_remove,
	.id_table = ssd1306_i2c_id,
};

/* ssd1306 board info */
static struct i2c_board_info ssd1306_i2c_board_info = {
	I2C_BOARD_INFO(DEVICE_NAME, SSD1306_ADDRESS)
};

/* init function */
static int __init ssd1306_driver_init(void) {
	ssd1306_i2c_adapter = i2c_get_adapter(I2C_BUS_AVAILABLE);
	if(ssd1306_i2c_adapter == NULL) {
		pr_info("Failed to find I2C bus\n");
		goto out;
	}
	ssd1306_i2c_client = i2c_new_client_device(ssd1306_i2c_adapter, &ssd1306_i2c_board_info);
	if(ssd1306_i2c_client == NULL) {
                pr_info("Failed to register the I2C device\n");
                goto out;
        }
	i2c_add_driver(&ssd1306_i2c_driver);
	i2c_put_adapter(ssd1306_i2c_adapter);

	pr_info("Starting SSD1306 I2C Driver...\n");
	return 0;
out:
	return -1;
}

/* exit function */
static void __exit ssd1306_driver_exit(void) {
	i2c_unregister_device(ssd1306_i2c_client);
	i2c_del_driver(&ssd1306_i2c_driver);
	pr_info("Exiting SSD1306 I2C Driver...\n");

}

module_init(ssd1306_driver_init);
module_exit(ssd1306_driver_exit);
