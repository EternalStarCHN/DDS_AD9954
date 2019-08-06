#include "DDS9954.h"
#include "stdint.h"
#include "gpio.h"

void delay_us(unsigned int m)
{
	unsigned int  n;
	for (n=0;n<=m;n++){}
}
/*********************************************************************************************************
** 函数名称 :void GPIO_AD9954_Init(void)
** 函数功能 :AD9954接口IO初始化
** 函数说明 :PS0----------------PE0	  OUT
**	   		   PS1----------------PE1	  OUT
**           IOUPDATE-----------PE5   OUT
**					 AD9954_RES---------PE6   OUT
**           AD9954_SDIO--------PC2   OUT
**	   			 AD9954_SCLK--------PC1	  OUT
**           AD9954_CS----------PC0	  OUT
** 入口参数:无
** 出口参数:无
*********************************************************************************************************/
//void GPIO_AD9954_Init(void)
//{
//	RCC->APB2ENR |= 1<<6;  //开启时钟
//	GPIOE->CRL &= 0xF00FFF00;	
//	GPIOE->CRL |= 0x03300033; //PE0,1,5,6推挽输出
//	
//	GPIOE->ODR |= 0x0063;  //上拉输出
//	RCC->APB2ENR |= 1<<4; //开启GPIOC时钟
//	GPIOC->CRL &= 0xFFFFF000;	
//	GPIOC->CRL |= 0x00000333;//PC0-2推挽输出
//	GPIOE->ODR |= 0x0007;//上拉输出
//}

/*********************************************************************************************************
** 函数名称 : void AD9954_RESET(void)
** 函数功能 : 复位AD9954
** 函数说明 : 不复位也可以
** 入口参数 : 无
** 出口参数 : 无
*********************************************************************************************************/
void AD9954_RESET(void)
{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET);
		delay_us(100);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET);
}

/*********************************************************************************************************
** 函数名称 :void AD9954_SENDBYTE(u8 dat)
** 函数功能 :往AD9954发送一个字节的内容
** 函数说明 :AD9954的传输速度最大为25M,所以不加延时也可以
** 入口参数 :待发送字节
** 出口参数 :无
*********************************************************************************************************/
void AD9954_SENDBYTE(uint8_t dat)
{
	uint8_t i;
	for(i=0;i<8;i++){
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET); //SCLK = 0;
		if( dat & 0x80)
		{
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET); //SDIO = 1;
		}
		else
		{
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET); //SDIO = 0;	
		}
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET); //SCLK = 1;
		dat <<= 1;
	}
}

/*********************************************************************************************************
** 函数名称 :void UPDATE(void)
** 函数功能 :产生一个更新信号,更新AD9954内部寄存器
** 函数说明 :可以不加任何延时
** 入口参数 :无
** 出口参数 :无
*********************************************************************************************************/
void UPDATE(void)
{
//	IOUPDATE=0;
//	delay_us(10);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET); //UPD = 1;
//	delay_us(1);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET); //UPD = 0;
}

/*********************************************************************************************************
** 函数名称 :void AD9954_Init(void)
** 函数功能 :初始化AD9954的管脚和最简单的内部寄存器的配置,外部晶振25M,16倍频后为400M
** 函数说明 :默认关闭比较器的电源
** 入口参数 :无
** 出口参数 :无
*********************************************************************************************************/
void AD9954_Init(void)
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET); //SCLK = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET); //RES = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET); //UPD = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET); //PS0 = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET); //PS1 = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); //CS = 0;
	delay_us(1);
	//single tone
	AD9954_SENDBYTE(CFR1);//地址0写操作CFR1
	AD9954_SENDBYTE(0x00);//关闭OSK
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);//比较器power down	  

	AD9954_SENDBYTE(CFR2);//地址1写操作CFR2
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(Dou16);//0x84:16倍频 打开VCO控制高位,系统时钟倍频后为400M
//	AD9954_CS=1;
}

/*********************************************************************************************************
** 函数名称 :void AD9954_SETFRE(float f)
** 函数功能 :设置AD9954当前频率的输出,采用的是单一频率输出
** 函数说明 :因为采用的浮点数进行计算,转换过程中会出现误差,通过调整可以精确到0.1Hz以内
** 入口参数 :预设置的频率值
** 出口参数 :无
*********************************************************************************************************/
#define float_fck 13.41331250//10.73065

void AD9954_SETFRE(uint32_t f)//single tone
{
	uint32_t date; 
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); //CS = 0;
	//date=(u32)(11.93046*f);//det=(f/fclk)x2^32=10.7374xf
	date=(uint32_t)(float_fck*f);//det=(f/fclk)x2^32=2^32/400xf=10.737418xf, 经调整后选择10.73065最准
	AD9954_SENDBYTE(FTW0);//FTW0地址
	AD9954_SENDBYTE((uint32_t)(date>>24));//频率控制字
	AD9954_SENDBYTE((uint32_t)(date>>16));
	AD9954_SENDBYTE((uint32_t)(date>>8));
	AD9954_SENDBYTE((uint32_t)date);
//	AD9954_CS=1;
	UPDATE();
}

/*********************************************************************************************************
** 函数名称 :void LINEARSWEEP(float f1,float f2)
** 函数功能 :线性扫描输出模式
** 函数说明 :使频率按预置的模式线性扫描上去,详情参见官方PDF
** 入口参数 :float f1:起始频率
** 			 		 float f2:终止频率
** 出口参数 :无
*********************************************************************************************************/
void LINEARSWEEP(uint32_t f1,uint32_t f2)//linear sweep mode
{
	uint32_t date;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET); //SCLK = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET); //RES = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET); //UPD = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET); //PS0 = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET); //PS1 = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); //CS = 0;

	//linear sweep mode
	AD9954_SENDBYTE(CFR1);//地址0写操作
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x20);//Enable Linear Sweep mode
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);//比较器power down  可关
	

	AD9954_SENDBYTE(CFR2);//地址1写操作
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(Dou16);//16倍频,打开VCO控制高位,系统时钟倍频为400M

	date=(uint32_t)(float_fck*f1);
	AD9954_SENDBYTE(FTW0);//FTW0地址
	AD9954_SENDBYTE((uint8_t)(date>>24));//频率控制字
	AD9954_SENDBYTE((uint8_t)(date>>16));
	AD9954_SENDBYTE((uint8_t)(date>>8));
	AD9954_SENDBYTE((uint8_t)date);
	date=(uint32_t)(float_fck*f2);
	AD9954_SENDBYTE(FTW1);//FTW1地址
	AD9954_SENDBYTE((uint8_t)(date>>24));//频率控制字
	AD9954_SENDBYTE((uint8_t)(date>>16));
	AD9954_SENDBYTE((uint8_t)(date>>8));
	AD9954_SENDBYTE((uint8_t)date);
	AD9954_SENDBYTE(NLSCW);//NLSCW
//	AD9954_SENDBYTE(0xFF);
//	AD9954_SENDBYTE(0x00);
//	AD9954_SENDBYTE(0x00);
//	AD9954_SENDBYTE(0x00);
//	AD9954_SENDBYTE(0x9F);	
	AD9954_SENDBYTE(0x01);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x1b);
	AD9954_SENDBYTE(0xf4);
	AD9954_SENDBYTE(PLSCW);//PLSCW
//	AD9954_SENDBYTE(0xFF);
//	AD9954_SENDBYTE(0x00);
//	AD9954_SENDBYTE(0x00);		//Setting the rising Delta Frequency Tuning word=0.0001MHz
//	AD9954_SENDBYTE(0x00);		//setting the rising Sweep Ramp Rate Word=FFH
//	AD9954_SENDBYTE(0x9F);		//设置斜坡率和步进 
	AD9954_SENDBYTE(0x01);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x01);
	AD9954_SENDBYTE(0xa3);
	AD9954_SENDBYTE(0x6f);
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET); //CS = 1;
	UPDATE();
}


/*********************************************************************************************************
** 函数名称 :void DirectSwitchFSK(float f1,float f2,float f3,float f4)
** 函数功能 :四相FSK信号输出
** 函数说明 :在四个RAM区各设置了一个频率值,通过改变PS0和PS1的电平选择对应的RAM端输出相应的频率值来实现FSK,也可以实现二相的FSK;
**            通过设置定时器中断控制PS0,PS1管脚的电平就可以将二进制编码转化为FSK信号输出
** 入口参数 :float f1:频率1
** 	      	 float f2:频率2
** 	      float f3:频率3
** 	      float f4:频率4
** 隐含控制 :   PS0: 0   1	0	 1
   管脚参数 : PS1: 0   0	1	 0
** 对应控制 RAM段:0   1	2	 3
** 出口参数 :无
*********************************************************************************************************/
void DirectSwitchFSK(uint32_t  f1,uint32_t  f2,uint32_t  f3,uint32_t  f4)
{
	uint32_t date;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); //CS = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET); //UPD = 0;
	AD9954_SENDBYTE(CFR1);//地址0写操作
	AD9954_SENDBYTE(0x00);//打开RAM控制位驱动FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	//
	AD9954_SENDBYTE(CFR2);//地址1写操作
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(Dou16);//16倍频,打开VCO控制高位,系统时钟倍频后为400M
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET); //PS0 = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET); //PS1 = 0;
	AD9954_SENDBYTE(RSCW0);
	AD9954_SENDBYTE(0x01);//ramp rate=0x0010
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);//final address:0x000
	AD9954_SENDBYTE(0x00);//start address:0x000;
	AD9954_SENDBYTE(0x00);//RAM0工作于模式0,无停留位不激活
	UPDATE();
	date=(uint32_t)(float_fck*f1);//det=(f/fclk)x2^32=10.7374xf
	AD9954_SENDBYTE(RAM);
	AD9954_SENDBYTE((uint8_t)(date>>24));//频率控制字
	AD9954_SENDBYTE((uint8_t)(date>>16));
	AD9954_SENDBYTE((uint8_t)(date>>8));
	AD9954_SENDBYTE((uint8_t)date);
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET); //PS0 = 1;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET); //PS1 = 0;
	
	AD9954_SENDBYTE(RSCW1);
	AD9954_SENDBYTE(0x01);//ramp rate=0x0010
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x01);//final address:0x0001
	AD9954_SENDBYTE(0x04);//start address:0x0001
	AD9954_SENDBYTE(0x00);//RAM1工作于模式0,无停留位不激活
	UPDATE();
	date=(uint32_t)(float_fck*f2);//det=(f/fclk)x2^32=10.7374xf
	AD9954_SENDBYTE(RAM);
	AD9954_SENDBYTE((uint8_t)(date>>24));//频率控制字
	AD9954_SENDBYTE((uint8_t)(date>>16));
	AD9954_SENDBYTE((uint8_t)(date>>8));
	AD9954_SENDBYTE((uint8_t)date);
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET); //PS0 = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET); //PS1 = 1;
	
	AD9954_SENDBYTE(RSCW2);
	AD9954_SENDBYTE(0x01);//ramp rate=0x0010
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x02);//final address:0x0002
	AD9954_SENDBYTE(0x08);//start address:0x0002
	AD9954_SENDBYTE(0x00);
	UPDATE();
	date=(uint32_t)(float_fck*f3);//det=(f/fclk)x2^32=10.7374xf
	AD9954_SENDBYTE(RAM);
	AD9954_SENDBYTE((uint8_t)(date>>24));//频率控制字
	AD9954_SENDBYTE((uint8_t)(date>>16));
	AD9954_SENDBYTE((uint8_t)(date>>8));
	AD9954_SENDBYTE((uint8_t)date);
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET); //PS0 = 1;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET); //PS1 = 1;
	
	AD9954_SENDBYTE(RSCW3);
	AD9954_SENDBYTE(0x01);//ramp rate=0x0010
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x03);//final address:0x0003
	AD9954_SENDBYTE(0x0c);//start address:0x0003
	AD9954_SENDBYTE(0x00);  
	UPDATE();
	date=(uint32_t)(float_fck*f4);//det=(f/fclk)x2^32=10.7374xf
	AD9954_SENDBYTE(RAM);
	AD9954_SENDBYTE((uint8_t)(date>>24));//频率控制字
	AD9954_SENDBYTE((uint8_t)(date>>16));
	AD9954_SENDBYTE((uint8_t)(date>>8));
	AD9954_SENDBYTE((uint8_t)date);
	
	AD9954_SENDBYTE(CFR1);//地址0写操作
	AD9954_SENDBYTE(0x80);
//	AD9954_SENDBYTE(0x80);//打开RAM控制位驱动FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	UPDATE();  
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET); //CS = 1;
}

/*********************************************************************************************************
** 函数名称 :void DirectSwitchPSK(float f,float phase1,float phase2,float phase3,float phase4)
** 函数功能 :四相PSK信号输出
** 函数说明 :在四个RAM区各设置了一个频率值,通过改变PS0和PS1的电平选择对应的RAM端输出相应的频率值来实现PSK,也可以实现二相的PSK;
**            通过设置定时器中断控制PS0,PS1管脚的电平就可以将二进制编码转化为PSK信号输出
** 入口参数 :float f1:频率1
** 	      	 float f2:频率2
** 	      float f3:频率3
** 	      float f4:频率4
** 隐含控制 :   PS0: 0   1	0	 1
   管脚参数 : PS1: 0   0	1	 0
** 对应控制 RAM段:0   1	2	 3
** 出口参数 :无
*********************************************************************************************************/
void DirectSwitchPSK(uint32_t	f,float phase1,float phase2,float phase3,float phase4)
{
	uint16_t date;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); //CS = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET); //UPD = 0;
	
	delay_us(1);	
	AD9954_SENDBYTE(CFR1);//地址0写操作
	AD9954_SENDBYTE(0x00);//打开RAM控制位驱动FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	//
	AD9954_SENDBYTE(CFR2);//地址1写操作
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(Dou16);//16倍频,打开VCO控制高位,系统倍频后为400M
	AD9954_SETFRE(f);//载波频率
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET); //PS0 = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET); //PS1 = 0;
	AD9954_SENDBYTE(RSCW0);
	AD9954_SENDBYTE(0x01);//ramp rate=0x0010
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);//final address:0x000
	AD9954_SENDBYTE(0x00);//start address:0x000;
	AD9954_SENDBYTE(0x00);//RAM0工作于0,无停留位不激活
	UPDATE();
	date=45.51*phase1;
	date=date<<2;
	AD9954_SENDBYTE(RAM);
	AD9954_SENDBYTE((uint8_t)(date>>8));//频率控制字
	AD9954_SENDBYTE((uint8_t)date);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET); //PS0 = 1;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET); //PS1 = 0;
	
	AD9954_SENDBYTE(RSCW1);
	AD9954_SENDBYTE(0x01);//ramp rate=0x0010
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x01);//final address:0x0001
	AD9954_SENDBYTE(0x04);//start address:0x0001
	AD9954_SENDBYTE(0x00);//RAM1工作于模式0,无停留位不激活
	UPDATE();
	date=45.51*phase2;
	date=date<<2;
	AD9954_SENDBYTE(RAM);
	AD9954_SENDBYTE((uint8_t)(date>>8));//频率控制字
	AD9954_SENDBYTE((uint8_t)date);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET); //PS0 = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET); //PS1 = 1;
	
	AD9954_SENDBYTE(RSCW2);
	AD9954_SENDBYTE(0x01);//ramp rate=0x0010
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x02);//final address:0x0002
	AD9954_SENDBYTE(0x08);//start address:0x0002
	AD9954_SENDBYTE(0x00);
	UPDATE();
	date=45.51*phase3;
	date=date<<2;
	AD9954_SENDBYTE(RAM);
	AD9954_SENDBYTE((uint8_t)(date>>8));//频率控制字
	AD9954_SENDBYTE((uint8_t)date);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET); //PS0 = 1;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET); //PS1 = 1;
	AD9954_SENDBYTE(RSCW3);
	AD9954_SENDBYTE(0x01);//ramp rate=0x0010
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x03);//final address:0x0003
	AD9954_SENDBYTE(0x0c);//start address:0x0003
	AD9954_SENDBYTE(0x00);  
	UPDATE();
	date=45.51*phase4;
	date=date<<2;
	AD9954_SENDBYTE(RAM);
	AD9954_SENDBYTE((uint8_t)(date>>8));//频率控制字
	AD9954_SENDBYTE((uint8_t)date);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);

	AD9954_SENDBYTE(CFR1);//地址0写操作
	AD9954_SENDBYTE(0xa0);//打开RAM控制位驱动FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET); //CS = 1;
	UPDATE();  
}

/*********************************************************************************************************
** 函数名称 :void Generate_PSK(float f,u16 *phase)
** 函数功能 :PSK调制信号输出
** 函数说明 :通过RAM来控制信号的相位,从而实现PSK的调制
**            这里使用一个RAM段的循环模式,PSK信号为8位数据位
**            RAM的更新ram rate定时寄存器控制,寄存器为16位,提供对系统提供的100M时钟1~65535分频,写0是无效的
**			  当跟输出PSK载波的频率是ram rate更新频率的整数倍时,才能保证每次的相位改变一致
** 输入参数 :float f   :PSK载波的频率
** 			  u16 *phase: 写入相位累加器的相位值,180实现相位翻转,0不改变相位,PSK信号为8位,需为8位的数组
** 输出参数 :无
*********************************************************************************************************/
void Generate_PSK(uint32_t f,uint16_t *phase)
{
	uint8_t i;
	uint16_t date;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); //CS = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET); //UPD = 0;

	AD9954_SENDBYTE(CFR1);//地址0写操作
	AD9954_SENDBYTE(0x00);//打开RAM控制位驱动FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	//
	AD9954_SENDBYTE(CFR2);//地址1写操作
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(Dou16);//16倍频,打开VCO控制高位,系统时钟倍频后为400M
//	AD9954_SETFRE(f);//载波频率

	AD9954_SENDBYTE(FTW0);//FTW0地址
	AD9954_SENDBYTE(0x00);//频率控制字
	AD9954_SENDBYTE(0x10);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	UPDATE();

	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET); //PS0 = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET); //PS1 = 0;
	AD9954_SENDBYTE(RSCW0);
	AD9954_SENDBYTE(0x00);//ramp rate=0x0400
	AD9954_SENDBYTE(0x04);
	AD9954_SENDBYTE(0x07);//final address:0x007
	AD9954_SENDBYTE(0x00);//start address:0x000;
	AD9954_SENDBYTE(0x80);//RAM0工作于模式4,无停留位不激活
	UPDATE();

	AD9954_SENDBYTE(0x0b);	
	for(i=0;i<8;i++)
	{
		date=45.51*phase[i];
		date=date<<2;	
		AD9954_SENDBYTE((uint8_t)(date>>8));//频率控制字
		AD9954_SENDBYTE((uint8_t)date);
		AD9954_SENDBYTE(0x00);
		AD9954_SENDBYTE(0x00);			
	}
	AD9954_SENDBYTE(CFR1);//地址0写操作
	AD9954_SENDBYTE(0xc0);//打开RAM控制位驱动FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET); //CS = 1;
	UPDATE();  
}

/*********************************************************************************************************
** 函数名称 :void Generate_FM(u32 *fre)
** 函数功能 :FM调制信号输出
** 函数说明 :通过RAM端来控制信号的频率,从而实现FM信号的调制
**            这里使用一个RAM段信号的循环模式,PSK信号为8位数据位
**            RAM的更新速度由ram rate定时寄存器控制,寄存器为16位,实现对系统提供的100M时钟1~65535分频,写0是无效的
**			 			ram rate的更新速度为调制信号的频率,这里如1KHz,写入ram rate的值为0X0C35,
** 入口参数 :u32 *fre:FM信号频率值,这里采用的是32点频率采样,调用此函数时先设置好频率表的值,其按正弦规律改变
**            for(i=0;i<32;i++)
**						{
**							fre[i]=Fc+Fshift*sin(wt);
**						}
** 出口参数 :无
*********************************************************************************************************/

void Generate_FM(uint32_t *fre)
{
	uint8_t i;
	uint32_t date;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); //CS = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET); //UPD = 0;

	AD9954_SENDBYTE(CFR1);//地址0写操作
	AD9954_SENDBYTE(0x00);//打开RAM控制位驱动FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);	

	AD9954_SENDBYTE(CFR2);//地址1写操作
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(Dou16);//16倍频,打开VCO控制高位,系统时钟倍频后为400M

	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET); //PS0 = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET); //PS1 = 0;
	AD9954_SENDBYTE(RSCW0);
	AD9954_SENDBYTE(0x35);//ramp rate=32kHz
	AD9954_SENDBYTE(0x0C);
	AD9954_SENDBYTE(0x1F);//final address:0x000
	AD9954_SENDBYTE(0x00);//start address:0x000;
	AD9954_SENDBYTE(0x80);//RAM0工作于模式0,无停留位不激活
	UPDATE();
	
	AD9954_SENDBYTE(0x0b);	
	for(i=0;i<32;i++)
	{
		date=float_fck*fre[i];	
		AD9954_SENDBYTE((uint8_t)(date>>24));//频率控制字
		AD9954_SENDBYTE((uint8_t)(date>>16));
		AD9954_SENDBYTE((uint8_t)(date>>8));
		AD9954_SENDBYTE((uint8_t)date);		
	}
	AD9954_SENDBYTE(CFR1);//地址0写操作
	AD9954_SENDBYTE(0x80);//打开RAM控制位驱动FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);	 
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET); //CS = 1;
	UPDATE();  
}

/*********************************************************************************************************
** 函数名称 :void ASK_Init(void)
** 函数功能 :ASK调制信号输出初始化
** 函数说明 :ASK调制,需要预先设置好载波频率,然后改变DAC比例因子 scale factor其为14位,通过设置为最大和0两种值实现ASK信号的调制
**			  调用Write_ASF(u16 factor)来改变幅值
** 入口参数 :无
** 出口参数 :无
*********************************************************************************************************/
void ASK_Init(void)
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); //CS = 0;
	AD9954_SENDBYTE(CFR1);//地址0写操作
	AD9954_SENDBYTE(0x02);//手动打开OSK打开
	AD9954_SENDBYTE(0x00);

	AD9954_SENDBYTE(0x20);//当受到UPDATE信号,自动清除相位累加器
//	AD9954_SENDBYTE(0x00);//不改变任何

	AD9954_SENDBYTE(0x40);

	//控制相位偏移,因默认输出是正弦波,所以需要控制相位累加器的相位为270
	AD9954_SENDBYTE(POW0);
	AD9954_SENDBYTE(0X30);
	AD9954_SENDBYTE(0X00);
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET); //CS = 1;
	UPDATE();
}

/*********************************************************************************************************
** 函数名称 :void AM_Init(void)
** 函数功能 :AM调制信号输出初始化
** 函数说明 :AM调制,需先设置好载波频率,然后改变DAC比例因子 scale factor其为14位,AM的幅度值按正弦规律变换,可以实现AM
**            最高可以采用32点采样
**			  调用Write_ASF(u16 factor)来改变幅值
** 入口参数 :无
** 出口参数 :无
*********************************************************************************************************/
void AM_Init(void)
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); //CS = 0;	AD9954_SENDBYTE(CFR1);//地址0写操作
	AD9954_SENDBYTE(0x02);//手动打开OSK打开
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);//不改变任何
	AD9954_SENDBYTE(0x40);
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET); //CS = 1;
	UPDATE();
}

/*********************************************************************************************************
** 函数名称 :void Write_ASF(u16 factor)
** 函数功能 :改变scale factor的值,改变DAC输出幅度
** 函数说明 :写入最大值为0X3FFF,最小为0
** 入口参数 :无
** 出口参数 :无
*********************************************************************************************************/
void Write_ASF(uint16_t factor)  //2ASK
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); //CS = 0;	AD9954_SENDBYTE(CFR1);//地址0写操作
	AD9954_SENDBYTE(CFR1);//地址0写操作
	AD9954_SENDBYTE(0x02);//手动打开OSK打开
	AD9954_SENDBYTE(0x00);

	AD9954_SENDBYTE(0x20);//当受到UPDATE信号,自动清除相位累加器		//
//	AD9954_SENDBYTE(0x00);//不改变任何

	AD9954_SENDBYTE(0x40);

	//控制相位偏移
//	AD9954_SENDBYTE(POW0);
//	AD9954_SENDBYTE(0X30);
//	AD9954_SENDBYTE(0X00);	  

	AD9954_SENDBYTE(ASF);//幅度
	AD9954_SENDBYTE(factor>>8);
	AD9954_SENDBYTE(factor);	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET); //CS = 1;
	UPDATE(); 

}



