#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "aht10.h"
#define DBG_TAG "get_humi"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#define THREAD_PRIORITY 25
#define THREAD_STACK_SIZE 2048
#define THREAD_TIMESLICE 5
static rt_thread_t tid1 = RT_NULL;
float humidity=0;//此处作为全局变量与onenet线程进行通信
float temperature=0;
static void thread1_entry(void *parameter)
{
aht10_device_t dev;
/* 总线名称 */
const char *i2c_bus_name = "i2c3";
/* 等待传感器正常工作 */
rt_thread_mdelay(2000);
/* 初始化 aht10 */
dev = aht10_init(i2c_bus_name);
if (dev == RT_NULL)
{

    return;
}
while (1)
{
/* 读取湿度 */
humidity = aht10_read_humidity(dev);
/* 读取温度 */
temperature = aht10_read_temperature(dev);
rt_thread_mdelay(1000);
}
return;
}
int thread_sample(void)
{
    /* 创建线程 1，名称是 thread1，入口是 thread1_entry*/
    tid1 = rt_thread_create("thread1",
    thread1_entry, RT_NULL,
    THREAD_STACK_SIZE,
    THREAD_PRIORITY, THREAD_TIMESLICE);

if (tid1 != RT_NULL)
rt_thread_startup(tid1);
return 0;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(thread_sample, thread sample);
