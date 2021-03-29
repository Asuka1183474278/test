#include "adc.h"
#include "usart.h"

vu16 ADC1_Data[ADC1_CHN];                      //AD采集滤波后的值
vu16 ADC1_ConvertedValue[ADC1_Count][ADC1_CHN];//DMA缓存

void  Adc1_Test(void)
{
    Adc1_Init();
    while(1)
    {

    }
}

//初始化ADC
void  Adc1_Init(void)
{
    GPIO_InitTypeDef      GPIO_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    ADC_InitTypeDef       ADC_InitStructure;
    DMA_InitTypeDef       DMA_InitStructure;
    NVIC_InitTypeDef      NVIC_InitStructure;

    //始化ADC1通道IO口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;                                //模拟输入
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;                           //不带上下拉

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //ADC设置
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);                         //使能ADCx时钟
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	                        //ADCx复位
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	                      //复位结束

    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;                    //独立模式
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间的延迟5个时钟
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;     //DMA失能(对于多通道ADC模式)
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;                 //预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz
    ADC_CommonInit(&ADC_CommonInitStructure);

    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;                      //12位模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;                                //扫描模式(多通道ADC采集要使能扫描模式)
#if TIM1_CH1_MODE==0
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                          //连续转换
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //禁止触发检测，使用软件触发
#else
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                         //模数转换工作在单次转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;       //外部触发转换开启
    ADC_InitStructure.ADC_ExternalTrigConvEdge =ADC_ExternalTrigConvEdge_Rising;//上升沿触发
#endif
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                      //右对齐
    ADC_InitStructure.ADC_NbrOfConversion =ADC1_CHN;                              //x个转换规则序列
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_15Cycles );

    ADC_DMACmd(ADC1, ENABLE);                                                   //使能ADC_DMA

    ADC_Cmd(ADC1, ENABLE);                                                      //开启AD转换器

    //DMA设置
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

    DMA_InitStructure.DMA_Channel = DMA_Channel_0;                              //选择通道号
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);           //外围设备地址,ADC_DR_DATA规则数据寄存器
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&ADC1_ConvertedValue;           //DMA存储器地址,自己设置的缓存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                     //传输方向,外设->存储器
    DMA_InitStructure.DMA_BufferSize = ADC1_CHN * ADC1_Count;                       //DMA缓存大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //外设是否为增量模式
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //存储器是否为增量模式
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //外设数据长度半个字(16位)
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         //存储器数据长度半字(16位)
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                             //DMA模式,普通模式、循环模式,还有双缓冲模式,需要特殊设置
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                         //DMA优先级,高优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                      //不使用FIFO
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;           //FIFO阈值
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                 //存储器突发,单次传输
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;         //外设突发,单次传输
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);                                 //初始化DMA2_Streamx

    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);                           //连续使能DMA
    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TC);                             //清除中断标志
    DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);                              //传输完成中断
    DMA_Cmd(DMA2_Stream0, ENABLE);                                              //使能DMA

    //设置DMA中断
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;                     //DMA2_Streamx中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;                     //抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;                           //子优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                             //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);

#if TIM1_CH1_MODE==0
    ADC_SoftwareStartConv(ADC1);                                                //软件启动ADCx
#else
    TIM1_Pwm_Adc_Init(TIM1_Pwm_Init_Data);                                      //定时器初始化,输出比较模式好像不太行，就用这个吧
#endif
}
void TIM1_Pwm_Adc_Init(u16 arr,u16 psc)//TIM1_Pwm_Init(7200,0);PWM周期=(arr+1)*(psc+1)/Tclk=7200*1/72000000s=0.1ms
{
    //TIM1_PWM_Init(16799,0);//不分频。PWM频率=168000000/16800/（0+1）=10000hz
    GPIO_InitTypeDef         GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef        TIM_OCInitStructure;

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);               //设置缺省值,这一步最好加上防止放到串口初始化后出问题
    TIM_OCStructInit(&TIM_OCInitStructure);                       //设置缺省值,这一步最好加上防止放到串口初始化后出问题
    //TIM_ICStructInit(&TIM_ICInitStructure);                     //设置缺省值,这一步最好加上防止放到串口初始化后出问题

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);  	      //TIMx时钟使能

    TIM_TimeBaseStructure.TIM_Prescaler    =psc;                  //定时器分频
    TIM_TimeBaseStructure.TIM_CounterMode  =TIM_CounterMode_Up;   //向上计数模式
    TIM_TimeBaseStructure.TIM_Period       =arr;                  //自动重装载值
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);                //初始化定时器

    //初始化TIMx PWM模式
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;        //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_Low;     //输出极性:TIM输出比较极性低

    //初始化TIMx Channelx
    TIM_OCInitStructure.TIM_Pulse = (arr+1)/2;                    //不为0和最大值，pwm占空比不为0和100 都可以
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);                      //根据T指定的参数初始化外设TIMx OCx
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);             //使能TIMx在CCR1上的预装载寄存器

    TIM_ARRPreloadConfig(TIM1,ENABLE);                            //ARPE使能
    TIM_Cmd(TIM1, ENABLE);                                        //使能TIMx

    TIM_CtrlPWMOutputs(TIM1, ENABLE);                             //使能TIMx的PWM输出,高级定时器需要
}
void ADC1_Filter(void)
{
    u16 i, j;
    u32 temp_sum;//单独写出来省的不注意用u16溢出
    for(i=0; i<ADC1_CHN; i++)/*取值求和取平均*/
    {
        temp_sum = 0;
        for(j=0; j<ADC1_Count; j++)
            temp_sum += ADC1_ConvertedValue[j][i];
        ADC1_Data[i] = temp_sum / ADC1_Count;
    }
    //最好main里面打印,printf太慢了,这里测试采样周期这里打印
    for(i=0; i<ADC1_CHN; i++)
    {
        printf("%f  ",ADC1_Data[i] * 3.3 / 4096);
    }
        printf("\r\n");
}


//获得ADC值
//通道值 0~16取值范围为:ADC_Channel_0~ADC_Channel_16
//Get_Adc1(ADC_Channel_5)
u16 Get_Adc1(u8 ch)
{
    //设置指定ADC的规则组通道，一个序列，采样时间
    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度
    ADC_SoftwareStartConv(ADC1);		                                  //使能指定的ADC1的软件转换启动功能
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));                   //等待转换结束
    return ADC_GetConversionValue(ADC1);	                            //返回最近一次ADC1规则组的转换结果
}


void DMA2_Stream0_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0))  //判断DMA传输完成中断
    {
        ADC1_Filter();//通道多,滤波长处理慢的自己不能接受，最好在main里面处理，其他就放在这问题不大
        DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
    }
}
