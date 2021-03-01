#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "adc.h"


//ALIENTEK ̽����STM32F407������ ʵ��18
//ADCģ��ת��ʵ��-�⺯���汾  
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com  
//������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK



int main(void)
{ 
 	u16 adcx;
	float temp;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(48);    //��ʼ����ʱ����
	uart_init(115200);	//��ʼ�����ڲ�����Ϊ115200
	LED_Init();					//��ʼ��LED
	Adc_Init();         //��ʼ��ADC
	while(1)
	{ 
		adcx=Get_Adc_Average(ADC_Channel_5,20);//��ȡͨ��5��ת��ֵ��20��ȡƽ��
		temp=(float)adcx*(3.3/4096);          //��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111
		adcx=temp;                            //��ֵ�������ָ�adcx��������ΪadcxΪu16����
        printf("%dV\r\n", adcx);
        LED0=!LED0;
        delay_ms(250);
	}
}



