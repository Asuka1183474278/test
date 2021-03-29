#ifndef _ADC_H_
#define _ADC_H_
#include "sys.h"

#define ADC1_CHN         1                  //要使用的ADC通道数
#define ADC1_Count      4			        //AD滤波采样次数
#define TIM1_CH1_MODE    1                  //1:定时器触发
#if TIM1_CH1_MODE==1
#define  TIM1_Pwm_Init_Data  1679,99        //加上滤波后实际采样周期=(arr+1)*(psc+1)/APB2clk=1680*100/84000000s=2ms*ADC1_Count=8ms
#endif                                      //定时器采样时间不能快过adc的转换时间，转换时间=[采样时间+ 12个周期(固定值)]/(ADC频率/分频) =(480[实际根据自己设定值]+12)/(84m/4[实际根据自己设定值])=23.62us *ADC1_CHN=23.62us *2=47.24us
//采样时间=3 15	28 56 84 112 144 480     ADC频率 =84M       分频 =2 4 6 8

extern vu16 ADC1_Data[ADC1_CHN];                      //AD采集滤波后的值
extern vu16 ADC1_ConvertedValue[ADC1_Count][ADC1_CHN];//DMA缓存

void Adc1_Init(void); 				//ADC通道初始化
#if TIM1_CH1_MODE==1
void    TIM1_Pwm_Adc_Init(u16 arr,u16 psc);
#endif
u16     Get_Adc1(u8 ch); 				//获得某个通道值
void    ADC1_Filter(void);
void    Adc1_Test(void);
#endif
















