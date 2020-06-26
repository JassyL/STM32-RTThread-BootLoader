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
#include <fal.h>
#include "signal_led.h"

/* defined the LED0 pin: PB1 */
#define LED0_PIN    GET_PIN(C, 13)

//定义信号灯对象句柄
led_t *led0 =  NULL;
//定义内存操作函数接口
led_mem_opreation_t led_mem_opreation;

char *led_blink_mode_0 = "500,500,"; //1Hz闪烁
char *led_blink_mode_1 = "50,50,";   //10Hz闪烁
char *led_blink_mode_2 = "0,100,";   //常灭
char *led_blink_mode_3 = "100,0,";   //常亮
char *led_blink_mode_4 = "100,100,100,1000,";//非固定时间
char *led_blink_mode_5 = "500,100,";

//定义开灯函数
void led0_switch_on(void)
{
    rt_pin_write(LED0_PIN, PIN_HIGH);
}


//定义关灯函数
void led0_switch_off(void)
{
    rt_pin_write(LED0_PIN, PIN_LOW);
}

//中途切换模式测试
void led_switch (void *param)
{
    rt_thread_mdelay(5000);
    led_set_mode(led0, LOOP_PERMANENT, led_blink_mode_4);   
    rt_thread_mdelay(5000);
    led_set_mode(led0, 10, led_blink_mode_1);  
}

void blink_over_callback(led_t *led_handler)
{
    led_set_mode(led_handler, LOOP_PERMANENT, led_blink_mode_4);
    led_start(led_handler);
}

static void led_run(void *parameter)
{
    while(1)
    {
        led_ticks();
        rt_thread_mdelay(LED_TICK_TIME);
    }
}

int rt_led_timer_init(void)
{
    rt_pin_mode(GET_PIN(C, 13),PIN_MODE_OUTPUT);
    
/*自定义内存操作接口
 *注意：若要进行自定义内存操作，必须要在调用任何软件包内接口之前作设置，
 *      否则会出现不可意料的错误！！！
 */
    led_mem_opreation.malloc_fn = (void* (*)(size_t))rt_malloc;
    led_mem_opreation.free_fn = rt_free;
    led_set_mem_operation(&led_mem_opreation);
    
    //初始化信号灯对象
    led0 = led_create(led0_switch_on, led0_switch_off);
  
    //设置信号灯工作模式，循环十次
    led_set_mode(led0, LOOP_PERMANENT, led_blink_mode_0);
    //设置信号灯闪烁结束回调函数
    led_set_blink_over_callback(led0,blink_over_callback);
    
    //开启信号灯
    led_start(led0);
    
     
    rt_thread_t tid = RT_NULL;
    tid = rt_thread_create("signal_led",
                            led_run, 
                            RT_NULL,
                            512,
                            RT_THREAD_PRIORITY_MAX-2,
                            100);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
    
    /* 创建线程1 */
    tid = rt_thread_create("led_switch_test",
                            led_switch, 
                            RT_NULL,
                            512,
                            RT_THREAD_PRIORITY_MAX-2,
                            100);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
    return RT_EOK;
}
INIT_APP_EXPORT(rt_led_timer_init);

int main(void)
{
    int count = 1;
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
		fal_init(); 

    while (count++)
    {
        // rt_pin_write(LED0_PIN, PIN_HIGH);
        // rt_thread_mdelay(500);
        // rt_pin_write(LED0_PIN, PIN_LOW);
         rt_thread_mdelay(500);
    }

    return RT_EOK;
}
