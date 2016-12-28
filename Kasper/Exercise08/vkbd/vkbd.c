#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/pci.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/module.h>

static unsigned char vkbd_keycode[256 /*0x72*/] = {	/* American layout */
   /*   0*/ 0, 0, 0, 0, 0, 0, 0, KEY_BACKSPACE, 0, 0, 
   /*  10*/ KEY_ENTER, KEY_TAB, 0, 0, 0, 0, 0, 0, 0, 0,
   /*  20*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   /*  30*/ 0, KEY_SPACE, 0, 0, 0, 0, 0, 0, 0, 0, 
   /*  40*/ 0, 0, 0, 0, 0, 0, 0, 0, KEY_0, KEY_0,
   /*  50*/ KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, 0, 0, 
   /*  60*/ 0, 0, 0, 0, 0, KEY_A, KEY_B, KEY_C, KEY_D, KEY_E,
   /*  70*/ KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, 
   /*  80*/ KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, 
   /*  90*/ KEY_Z, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   /* 100*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   /* 110*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   /* 120*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   /* 130*/ KEY_LEFTSHIFT, KEY_RIGHTSHIFT, 0, 0, 0, 0, 0, 0, 0, 0, 
   /* 140*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   /* 150*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   /* 160*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   /* 170*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   /* 180*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   /* 190*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   /* 200*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   /* 210*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   /* 220*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   /* 230*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
   /* 240*/ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   /* 250*/ 0, 0, 0, 0, 0, 0 };
/*

	[0]	 = KEY_GRAVE,
	[1]	 = KEY_ESC,
	[2]	 = KEY_1,
	[3]	 = KEY_2,
	[4]	 = KEY_3,
	[5]	 = KEY_4,
	[6]	 = KEY_5,
	[7]	 = KEY_6,
	[8]	 = KEY_7,
	[9]	 = KEY_8,
	[10]	 = KEY_9,
	[11]	 = KEY_0,
	[12]	 = KEY_MINUS,
	[13]	 = KEY_EQUAL,
	[14]	 = KEY_BACKSPACE,
	[15]	 = KEY_TAB,
	[16]	 = KEY_Q,
	[17]	 = KEY_W,
	[18]	 = KEY_E,
	[19]	 = KEY_R,
	[20]	 = KEY_T,
	[21]	 = KEY_Y,
	[22]	 = KEY_U,
	[23]	 = KEY_I,
	[24]	 = KEY_O,
	[25]	 = KEY_P,
	[26]	 = KEY_LEFTBRACE,
	[27]	 = KEY_RIGHTBRACE,
	[28]	 = KEY_ENTER,
	[29]	 = KEY_LEFTCTRL,
	[30]	 = KEY_A,
	[31]	 = KEY_S,
	[32]	 = KEY_D,
	[33]	 = KEY_F,
	[34]	 = KEY_G,
	[35]	 = KEY_H,
	[36]	 = KEY_J,
	[37]	 = KEY_K,
	[38]	 = KEY_L,
	[39]	 = KEY_SEMICOLON,
	[40]	 = KEY_APOSTROPHE,
	[41]	 = KEY_BACKSLASH,	
	[42]	 = KEY_LEFTSHIFT,
	[43]	 = KEY_GRAVE,		
	[44]	 = KEY_Z,
	[45]	 = KEY_X,
	[46]	 = KEY_C,
	[47]	 = KEY_V,
	[48]	 = KEY_B,
	[49]	 = KEY_N,
	[50]	 = KEY_M,
	[51]	 = KEY_COMMA,
	[52]	 = KEY_DOT,
	[53]	 = KEY_SLASH,
	[54]	 = KEY_RIGHTSHIFT,
	[55]	 = KEY_KPASTERISK,
	[56]	 = KEY_LEFTALT,
	[57]	 = KEY_SPACE,
	[58]	 = KEY_CAPSLOCK,
	[59]	 = KEY_F1,
	[60]	 = KEY_F2,
	[61]	 = KEY_F3,
	[62]	 = KEY_F4,
	[63]	 = KEY_F5,
	[64]	 = KEY_F6,
	[65]	 = KEY_F7,
	[66]	 = KEY_F8,
	[67]	 = KEY_F9,
	[68]	 = KEY_F10,
	[69]	 = KEY_ESC,
	[70]	 = KEY_DELETE,
	[71]	 = KEY_KP7,
	[72]	 = KEY_KP8,
	[73]	 = KEY_KP9,
	[74]	 = KEY_KPMINUS,
	[75]	 = KEY_KP4,
	[76]	 = KEY_KP5,
	[77]	 = KEY_KP6,
	[78]	 = KEY_KPPLUS,
	[79]	 = KEY_KP1,
	[80]	 = KEY_KP2,
	[81]	 = KEY_KP3,
	[82]	 = KEY_KP0,
	[83]	 = KEY_KPDOT,
	[90]	 = KEY_KPLEFTPAREN,
	[91]	 = KEY_KPRIGHTPAREN,
	[92]	 = KEY_KPASTERISK,	
	[93]	 = KEY_KPASTERISK,
	[94]	 = KEY_KPPLUS,
	[95]	 = KEY_HELP,
	[96]	 = KEY_BACKSLASH,	
	[97]	 = KEY_KPASTERISK,	
	[98]	 = KEY_KPSLASH,
	[99]	 = KEY_KPLEFTPAREN,
	[100]	 = KEY_KPRIGHTPAREN,
	[101]	 = KEY_KPSLASH,
	[102]	 = KEY_KPASTERISK,
	[103]	 = KEY_UP,
	[104]	 = KEY_KPASTERISK,
	[105]	 = KEY_LEFT,
	[106]	 = KEY_RIGHT,
	[107]	 = KEY_KPASTERISK,	
	[108]	 = KEY_DOWN,
	[109]	 = KEY_KPASTERISK,	
	[110]	 = KEY_KPASTERISK,	
	[111]	 = KEY_KPASTERISK,	
	[112]	 = KEY_KPASTERISK,	
	[113]	 = KEY_KPASTERISK	
        };*/


struct input_dev* vkbd_input_dev;
static struct platform_device* vkbd_dev;



static ssize_t write_vkbd(struct device* dev,
                         struct device_attribute* attr,
                         const char* buffer, size_t count)
{
   /*
     int xs, xd;
   
     char scancode, down;
     sscanf(buffer, "%x%x", &xs, &xd);

     scancode = (char)xs;
     down = (char)xd;
   */   
   /*
     input_report_key(vkbd_input_dev, 0x1e, 1);
     input_report_key(vkbd_input_dev, 0x9e, 0);

     input_report_key(vkbd_input_dev, 0x30, 1);
     input_report_key(vkbd_input_dev, 0xb0, 0);

     input_report_key(vkbd_input_dev, 0x2e, 1);
     input_report_key(vkbd_input_dev, 0xae, 0);
   */
   int i;
   
   for(i = 0; i < count; ++i)
   {
      unsigned char c = buffer[i];
      if((buffer[i] >= 'a') && (buffer[i] <= 'z'))
      {
        input_report_key(vkbd_input_dev, KEY_RIGHTSHIFT, 1);
         c = c - ('a' - 'A');
         //printk("scancode = %d, down = %d\n", scancode, down);
         printk("Shift pressed!\n");
      }

      printk("char c = %x\n", c);
      if(vkbd_keycode[c])
      {
        input_report_key(vkbd_input_dev, vkbd_keycode[c], 1);
        input_report_key(vkbd_input_dev, vkbd_keycode[c], 0);
      }
      

      if((buffer[i] >= 'a') && (buffer[i] <= 'z'))
      {
        input_report_key(vkbd_input_dev, KEY_RIGHTSHIFT, 0);
      }
      input_sync(vkbd_input_dev);
   }
   


   //printk("scancode = %d, down = %d\n", scancode, down);
   
   return count;
}

DEVICE_ATTR(keyinput, 0666, NULL, write_vkbd);

static struct attribute* vkbd_attr[] = 
{
   &dev_attr_keyinput.attr,
   NULL
};

static struct attribute_group vkbd_attr_group = 
{
   .attrs = vkbd_attr,
};


int __init vkbd_init(void)
{
   int i = 0;
   
   vkbd_dev = platform_device_register_simple("vkbd", -1, NULL, 0);
   if(IS_ERR(vkbd_dev))
   {
     //PTR_ERR(vkbd_dev);
      printk("vkbd_init: error\n");
   }
   
   (void)sysfs_create_group(&vkbd_dev->dev.kobj, &vkbd_attr_group);
   
   vkbd_input_dev = input_allocate_device();

   if(!vkbd_input_dev)
   {
      printk("Bad input_alloc_device()\n");
   }

   vkbd_input_dev->name = "Virtual KBD";
   vkbd_input_dev->phys = "vkbd/input0";
   vkbd_input_dev->id.bustype = BUS_HOST;
   vkbd_input_dev->id.vendor = 0x0001;
   vkbd_input_dev->id.product = 0x0001;
   vkbd_input_dev->id.version = 0x0100;

   vkbd_input_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP);
   vkbd_input_dev->keycode = vkbd_keycode;
   vkbd_input_dev->keycodesize = sizeof(unsigned char);
   vkbd_input_dev->keycodemax = ARRAY_SIZE(vkbd_keycode);

   for (i = 0; i < 255; i++) {
      set_bit(vkbd_keycode[i], vkbd_input_dev->keybit);
   }

   

   vkbd_input_dev->name = "Virtual KBD";

   vkbd_input_dev->evbit[0] = BIT(EV_KEY) | BIT(EV_REP);
   vkbd_input_dev->keycode = vkbd_keycode;
   vkbd_input_dev->keycodesize = sizeof(unsigned char);
   vkbd_input_dev->keycodemax = ARRAY_SIZE(vkbd_keycode);

   set_bit(EV_REL, vkbd_input_dev->evbit);   
   set_bit(REL_X, vkbd_input_dev->relbit);
   set_bit(REL_Y, vkbd_input_dev->relbit);

   input_set_capability(vkbd_input_dev, EV_KEY, BTN_LEFT);
   input_set_capability(vkbd_input_dev, EV_KEY, BTN_MIDDLE);
   input_set_capability(vkbd_input_dev, EV_KEY, BTN_RIGHT);

   input_register_device(vkbd_input_dev);
   
   printk("Virtual Mouse Driver Initialized.\n");
   
   return 0;
}

void __exit vkbd_cleanup(void)
{
  input_unregister_device(vkbd_input_dev);
   
  sysfs_remove_group(&vkbd_dev->dev.kobj, &vkbd_attr_group);
   
  platform_device_unregister(vkbd_dev);
   
   return;
}


module_init(vkbd_init);
module_exit(vkbd_cleanup);

MODULE_AUTHOR("IHA");
MODULE_LICENSE("GPL");
