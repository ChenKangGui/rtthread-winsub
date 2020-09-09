/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

/* defined the LED0 pin: PB1 */
#define LED_Y    GET_PIN(A, 1)
#define LED_R    GET_PIN(A, 2)
#define LED_B    GET_PIN(A, 3)



static struct rt_completion rx_completion,tx_completion;
static struct rt_thread usb_thread;
static char usb_thread_stack[0x1000 * 2];

static void usb_thread_entry(void * parameter)
{
	char buf[4096];
    while(1)
    {
		memset((void *)buf, 0, sizeof(buf));
		
		rt_tick_t startTick = rt_tick_get();
		rt_completion_init(&rx_completion);
		rt_device_read((rt_device_t)parameter,0, buf, 4096);
		rt_completion_wait(&rx_completion, 3000);
		rt_tick_t endTick = rt_tick_get();
		rt_kprintf("#####################%d %d %d %d %d %d %d\r\n", strlen(buf), strlen(buf), strlen(buf), strlen(buf), strlen(buf), strlen(buf), strlen(buf));
		rt_kprintf("*********  %d %d %d %d %d %d\r\n", endTick-startTick, endTick-startTick, endTick-startTick, endTick-startTick, endTick-startTick, endTick-startTick);
		
		startTick = rt_tick_get();
		rt_completion_init(&tx_completion);
		rt_device_write((rt_device_t)parameter, 0, buf, strlen(buf));
		rt_completion_wait(&tx_completion, 800);
		endTick = rt_tick_get();
		rt_kprintf("&&&&&&&&& = %d %d %d %d %d %d\r\n", endTick-startTick, endTick-startTick, endTick-startTick, endTick-startTick, endTick-startTick, endTick-startTick);

    }
}



static rt_err_t rx_indicate(rt_device_t dev, rt_size_t size)
{
//	rt_pin_write(LED_R, PIN_HIGH);
	
    rt_completion_done(&rx_completion);
    return RT_EOK;
}
static rt_err_t tx_complete(rt_device_t dev, void *buffer)
{
//	rt_pin_write(LED_B, PIN_HIGH);
	
    rt_completion_done(&tx_completion);
    return RT_EOK;
}

int main(void)
{
	rt_pin_mode(LED_Y, PIN_MODE_OUTPUT);
	rt_pin_mode(LED_R, PIN_MODE_OUTPUT);
	rt_pin_mode(LED_B, PIN_MODE_OUTPUT);
	
	rt_pin_write(LED_R, PIN_LOW);
	rt_pin_write(LED_B, PIN_LOW);
	rt_pin_write(LED_Y, PIN_LOW);
	
	rt_device_t device = rt_device_find("winusb");
    RT_ASSERT(device != RT_NULL);
    device->rx_indicate = rx_indicate;
    device->tx_complete = tx_complete;
    rt_device_open(device,RT_DEVICE_FLAG_RDWR);

    /* init usb device thread */
    rt_thread_init(&usb_thread,
                   "winusb",
                   usb_thread_entry, device,
                   usb_thread_stack, sizeof(usb_thread_stack),
                   6, 20);
    rt_thread_startup(&usb_thread);
			
	char buf[] = "hello world";			   
	while(1)
	{
		rt_pin_write(LED_Y, PIN_HIGH);
        rt_thread_mdelay(500);		
		rt_pin_write(LED_Y, PIN_LOW);
        rt_thread_mdelay(500);
		
//		rt_completion_init(&tx_completion);
//		rt_device_write(device, 0, buf, sizeof(buf));
//		rt_completion_wait(&tx_completion, 1000);
	}
				   
	return 0;
	
}


