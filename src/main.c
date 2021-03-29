#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "adc.h"


//ALIENTEK ̽����STM32F407������ ʵ��19
//�ڲ��¶ȴ�����ʵ�� -�⺯���汾 
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com  
//������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK
float ADCSRData;

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);     //��ʼ����ʱ����
	uart_init(115200);	 //��ʼ�����ڲ�����Ϊ115200

	LED_Init();					//��ʼ��LED
    Adc1_Init();
    while(1)
    {
        LED0 = !LED0;
        delay_ms(250);
    }
}
