/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-5-10      ShiHao       first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "aht10.h"
#include "ap3216c.h"
#include <drv_lcd.h>
#include <rttlogo.h>

#define DBG_TAG "main"
#define DBG_LVL         DBG_LOG
#include <rtdbg.h>

/* 配置 KEY 输入引脚  */
#define PIN_KEY1        GET_PIN(C, 1)      // PC1:  KEY1         --> KEY
#define PIN_WK_UP       GET_PIN(C, 5)      // PC5:  WK_UP        --> KEY

/* 配置蜂鸣器引脚 */
#define PIN_BEEP        GET_PIN(B, 0)      // PA1:  BEEP         --> BEEP (PB1)

/* 中断回调 */
void irq_callback(void *args)
{
    rt_uint32_t sign = (rt_uint32_t) args;
    switch (sign)
    {
    case PIN_WK_UP :
        rt_pin_write(PIN_BEEP,PIN_HIGH);
        LOG_D("WK_UP interrupt. beep on.");
        break;
    case PIN_KEY1 :
        rt_pin_write(PIN_BEEP,PIN_LOW);
        LOG_D("KEY1 interrupt. beep off.");
        break;
    default:
        LOG_E("error sign= %d !", sign);
        break;
    }
}

int main(void)
{

    /* 设置按键引脚为输入模式 */
                              rt_pin_mode(PIN_KEY1, PIN_MODE_INPUT_PULLUP);
                              rt_pin_mode(PIN_WK_UP, PIN_MODE_INPUT_PULLUP);

                              /* 设置蜂鸣器引脚为输出模式 */
                              rt_pin_mode(PIN_BEEP, PIN_MODE_OUTPUT);

                              /* 设置按键中断模式与中断回调函数 */
                              rt_pin_attach_irq(PIN_KEY1, PIN_IRQ_MODE_FALLING, irq_callback, (void *) PIN_KEY1);
                              rt_pin_attach_irq(PIN_WK_UP, PIN_IRQ_MODE_FALLING, irq_callback, (void *) PIN_WK_UP);

                              /* 使能中断 */
                              rt_pin_irq_enable(PIN_KEY1, PIN_IRQ_ENABLE);
                              rt_pin_irq_enable(PIN_WK_UP, PIN_IRQ_ENABLE);

    float humidity, temperature;
       aht10_device_t dev;

       /* 总线名称 */
       const char *i2c_bus_name = "i2c3";
       int count = 0;

       ap3216c_device_t dev1;
       const char *i2c_bus_name1 = "i2c2";


       /* 等待传感器正常工作 */
       rt_thread_mdelay(2000);

       /* 初始化 aht10 */
       dev = aht10_init(i2c_bus_name);
       if (dev == RT_NULL)
       {
           LOG_E(" The sensor initializes failure");
           return 0;
       }

       /* 初始化 ap3216c */
           dev1 = ap3216c_init(i2c_bus_name1);
           if (dev1 == RT_NULL)
           {
               LOG_E("The sensor initializes failure.");
               return 0;
           }

       while (count++ < 100)
       {
           rt_uint16_t ps_data;
            /* 读接近感应值 */
           ps_data = ap3216c_read_ps_data(dev1);
           if (ps_data == 0)
           {
               LOG_D("object is not proximity of sensor.");
           }
           else
           {
               LOG_D("current ps data   : %d.", ps_data);
           }


           /* 读取湿度 */
           humidity = aht10_read_humidity(dev);


           /* 读取温度 */
           temperature = aht10_read_temperature(dev);


           rt_thread_mdelay(1000);

           if(temperature>35)
                      {
                          rt_pin_write(PIN_BEEP, PIN_HIGH);

                      }

           if(ps_data>=7)
           {
                lcd_clear(WHITE);
                /* show RT-Thread logo */
                lcd_show_image(0, 0, 240, 69, image_rttlogo);

                /* set the background color and foreground color */
                lcd_set_color(WHITE, BLACK);
                /* show some string on lcd */
                lcd_show_string(10, 69, 24, "Temperature:");
                lcd_show_string(10, 69+24, 24, "Humidity:");
                lcd_show_num(170,69,temperature,4,24);
                lcd_show_num(170,69+24,humidity,7,24);



                /* draw a line on lcd */
                lcd_draw_line(0, 69 + 16 + 24 + 32, 240, 69 + 16 + 24 + 32);

                /* draw a concentric circles */
                lcd_draw_point(120, 194);
                for (int i = 0; i < 46; i += 4)
                {
                    lcd_draw_circle(120, 194, i);
                }
           }
           else
               lcd_clear(BLACK);


        }

       return 0;

}

