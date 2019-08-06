#include "DDS9954.h"
#include "stdint.h"
#include "gpio.h"

void delay_us(unsigned int m)
{
	unsigned int  n;
	for (n=0;n<=m;n++){}
}
/*********************************************************************************************************
** �������� :void GPIO_AD9954_Init(void)
** �������� :AD9954�ӿ�IO��ʼ��
** ����˵�� :PS0----------------PE0	  OUT
**	   		   PS1----------------PE1	  OUT
**           IOUPDATE-----------PE5   OUT
**					 AD9954_RES---------PE6   OUT
**           AD9954_SDIO--------PC2   OUT
**	   			 AD9954_SCLK--------PC1	  OUT
**           AD9954_CS----------PC0	  OUT
** ��ڲ���:��
** ���ڲ���:��
*********************************************************************************************************/
//void GPIO_AD9954_Init(void)
//{
//	RCC->APB2ENR |= 1<<6;  //����ʱ��
//	GPIOE->CRL &= 0xF00FFF00;	
//	GPIOE->CRL |= 0x03300033; //PE0,1,5,6�������
//	
//	GPIOE->ODR |= 0x0063;  //�������
//	RCC->APB2ENR |= 1<<4; //����GPIOCʱ��
//	GPIOC->CRL &= 0xFFFFF000;	
//	GPIOC->CRL |= 0x00000333;//PC0-2�������
//	GPIOE->ODR |= 0x0007;//�������
//}

/*********************************************************************************************************
** �������� : void AD9954_RESET(void)
** �������� : ��λAD9954
** ����˵�� : ����λҲ����
** ��ڲ��� : ��
** ���ڲ��� : ��
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
** �������� :void AD9954_SENDBYTE(u8 dat)
** �������� :��AD9954����һ���ֽڵ�����
** ����˵�� :AD9954�Ĵ����ٶ����Ϊ25M,���Բ�����ʱҲ����
** ��ڲ��� :�������ֽ�
** ���ڲ��� :��
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
** �������� :void UPDATE(void)
** �������� :����һ�������ź�,����AD9954�ڲ��Ĵ���
** ����˵�� :���Բ����κ���ʱ
** ��ڲ��� :��
** ���ڲ��� :��
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
** �������� :void AD9954_Init(void)
** �������� :��ʼ��AD9954�Ĺܽź���򵥵��ڲ��Ĵ���������,�ⲿ����25M,16��Ƶ��Ϊ400M
** ����˵�� :Ĭ�ϹرձȽ����ĵ�Դ
** ��ڲ��� :��
** ���ڲ��� :��
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
	AD9954_SENDBYTE(CFR1);//��ַ0д����CFR1
	AD9954_SENDBYTE(0x00);//�ر�OSK
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);//�Ƚ���power down	  

	AD9954_SENDBYTE(CFR2);//��ַ1д����CFR2
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(Dou16);//0x84:16��Ƶ ��VCO���Ƹ�λ,ϵͳʱ�ӱ�Ƶ��Ϊ400M
//	AD9954_CS=1;
}

/*********************************************************************************************************
** �������� :void AD9954_SETFRE(float f)
** �������� :����AD9954��ǰƵ�ʵ����,���õ��ǵ�һƵ�����
** ����˵�� :��Ϊ���õĸ��������м���,ת�������л�������,ͨ���������Ծ�ȷ��0.1Hz����
** ��ڲ��� :Ԥ���õ�Ƶ��ֵ
** ���ڲ��� :��
*********************************************************************************************************/
#define float_fck 13.41331250//10.73065

void AD9954_SETFRE(uint32_t f)//single tone
{
	uint32_t date; 
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); //CS = 0;
	//date=(u32)(11.93046*f);//det=(f/fclk)x2^32=10.7374xf
	date=(uint32_t)(float_fck*f);//det=(f/fclk)x2^32=2^32/400xf=10.737418xf, ��������ѡ��10.73065��׼
	AD9954_SENDBYTE(FTW0);//FTW0��ַ
	AD9954_SENDBYTE((uint32_t)(date>>24));//Ƶ�ʿ�����
	AD9954_SENDBYTE((uint32_t)(date>>16));
	AD9954_SENDBYTE((uint32_t)(date>>8));
	AD9954_SENDBYTE((uint32_t)date);
//	AD9954_CS=1;
	UPDATE();
}

/*********************************************************************************************************
** �������� :void LINEARSWEEP(float f1,float f2)
** �������� :����ɨ�����ģʽ
** ����˵�� :ʹƵ�ʰ�Ԥ�õ�ģʽ����ɨ����ȥ,����μ��ٷ�PDF
** ��ڲ��� :float f1:��ʼƵ��
** 			 		 float f2:��ֹƵ��
** ���ڲ��� :��
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
	AD9954_SENDBYTE(CFR1);//��ַ0д����
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x20);//Enable Linear Sweep mode
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);//�Ƚ���power down  �ɹ�
	

	AD9954_SENDBYTE(CFR2);//��ַ1д����
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(Dou16);//16��Ƶ,��VCO���Ƹ�λ,ϵͳʱ�ӱ�ƵΪ400M

	date=(uint32_t)(float_fck*f1);
	AD9954_SENDBYTE(FTW0);//FTW0��ַ
	AD9954_SENDBYTE((uint8_t)(date>>24));//Ƶ�ʿ�����
	AD9954_SENDBYTE((uint8_t)(date>>16));
	AD9954_SENDBYTE((uint8_t)(date>>8));
	AD9954_SENDBYTE((uint8_t)date);
	date=(uint32_t)(float_fck*f2);
	AD9954_SENDBYTE(FTW1);//FTW1��ַ
	AD9954_SENDBYTE((uint8_t)(date>>24));//Ƶ�ʿ�����
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
//	AD9954_SENDBYTE(0x9F);		//����б���ʺͲ��� 
	AD9954_SENDBYTE(0x01);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x01);
	AD9954_SENDBYTE(0xa3);
	AD9954_SENDBYTE(0x6f);
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET); //CS = 1;
	UPDATE();
}


/*********************************************************************************************************
** �������� :void DirectSwitchFSK(float f1,float f2,float f3,float f4)
** �������� :����FSK�ź����
** ����˵�� :���ĸ�RAM����������һ��Ƶ��ֵ,ͨ���ı�PS0��PS1�ĵ�ƽѡ���Ӧ��RAM�������Ӧ��Ƶ��ֵ��ʵ��FSK,Ҳ����ʵ�ֶ����FSK;
**            ͨ�����ö�ʱ���жϿ���PS0,PS1�ܽŵĵ�ƽ�Ϳ��Խ������Ʊ���ת��ΪFSK�ź����
** ��ڲ��� :float f1:Ƶ��1
** 	      	 float f2:Ƶ��2
** 	      float f3:Ƶ��3
** 	      float f4:Ƶ��4
** �������� :   PS0: 0   1	0	 1
   �ܽŲ��� : PS1: 0   0	1	 0
** ��Ӧ���� RAM��:0   1	2	 3
** ���ڲ��� :��
*********************************************************************************************************/
void DirectSwitchFSK(uint32_t  f1,uint32_t  f2,uint32_t  f3,uint32_t  f4)
{
	uint32_t date;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); //CS = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET); //UPD = 0;
	AD9954_SENDBYTE(CFR1);//��ַ0д����
	AD9954_SENDBYTE(0x00);//��RAM����λ����FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	//
	AD9954_SENDBYTE(CFR2);//��ַ1д����
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(Dou16);//16��Ƶ,��VCO���Ƹ�λ,ϵͳʱ�ӱ�Ƶ��Ϊ400M
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET); //PS0 = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET); //PS1 = 0;
	AD9954_SENDBYTE(RSCW0);
	AD9954_SENDBYTE(0x01);//ramp rate=0x0010
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);//final address:0x000
	AD9954_SENDBYTE(0x00);//start address:0x000;
	AD9954_SENDBYTE(0x00);//RAM0������ģʽ0,��ͣ��λ������
	UPDATE();
	date=(uint32_t)(float_fck*f1);//det=(f/fclk)x2^32=10.7374xf
	AD9954_SENDBYTE(RAM);
	AD9954_SENDBYTE((uint8_t)(date>>24));//Ƶ�ʿ�����
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
	AD9954_SENDBYTE(0x00);//RAM1������ģʽ0,��ͣ��λ������
	UPDATE();
	date=(uint32_t)(float_fck*f2);//det=(f/fclk)x2^32=10.7374xf
	AD9954_SENDBYTE(RAM);
	AD9954_SENDBYTE((uint8_t)(date>>24));//Ƶ�ʿ�����
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
	AD9954_SENDBYTE((uint8_t)(date>>24));//Ƶ�ʿ�����
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
	AD9954_SENDBYTE((uint8_t)(date>>24));//Ƶ�ʿ�����
	AD9954_SENDBYTE((uint8_t)(date>>16));
	AD9954_SENDBYTE((uint8_t)(date>>8));
	AD9954_SENDBYTE((uint8_t)date);
	
	AD9954_SENDBYTE(CFR1);//��ַ0д����
	AD9954_SENDBYTE(0x80);
//	AD9954_SENDBYTE(0x80);//��RAM����λ����FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	UPDATE();  
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET); //CS = 1;
}

/*********************************************************************************************************
** �������� :void DirectSwitchPSK(float f,float phase1,float phase2,float phase3,float phase4)
** �������� :����PSK�ź����
** ����˵�� :���ĸ�RAM����������һ��Ƶ��ֵ,ͨ���ı�PS0��PS1�ĵ�ƽѡ���Ӧ��RAM�������Ӧ��Ƶ��ֵ��ʵ��PSK,Ҳ����ʵ�ֶ����PSK;
**            ͨ�����ö�ʱ���жϿ���PS0,PS1�ܽŵĵ�ƽ�Ϳ��Խ������Ʊ���ת��ΪPSK�ź����
** ��ڲ��� :float f1:Ƶ��1
** 	      	 float f2:Ƶ��2
** 	      float f3:Ƶ��3
** 	      float f4:Ƶ��4
** �������� :   PS0: 0   1	0	 1
   �ܽŲ��� : PS1: 0   0	1	 0
** ��Ӧ���� RAM��:0   1	2	 3
** ���ڲ��� :��
*********************************************************************************************************/
void DirectSwitchPSK(uint32_t	f,float phase1,float phase2,float phase3,float phase4)
{
	uint16_t date;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); //CS = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET); //UPD = 0;
	
	delay_us(1);	
	AD9954_SENDBYTE(CFR1);//��ַ0д����
	AD9954_SENDBYTE(0x00);//��RAM����λ����FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	//
	AD9954_SENDBYTE(CFR2);//��ַ1д����
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(Dou16);//16��Ƶ,��VCO���Ƹ�λ,ϵͳ��Ƶ��Ϊ400M
	AD9954_SETFRE(f);//�ز�Ƶ��
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET); //PS0 = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET); //PS1 = 0;
	AD9954_SENDBYTE(RSCW0);
	AD9954_SENDBYTE(0x01);//ramp rate=0x0010
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);//final address:0x000
	AD9954_SENDBYTE(0x00);//start address:0x000;
	AD9954_SENDBYTE(0x00);//RAM0������0,��ͣ��λ������
	UPDATE();
	date=45.51*phase1;
	date=date<<2;
	AD9954_SENDBYTE(RAM);
	AD9954_SENDBYTE((uint8_t)(date>>8));//Ƶ�ʿ�����
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
	AD9954_SENDBYTE(0x00);//RAM1������ģʽ0,��ͣ��λ������
	UPDATE();
	date=45.51*phase2;
	date=date<<2;
	AD9954_SENDBYTE(RAM);
	AD9954_SENDBYTE((uint8_t)(date>>8));//Ƶ�ʿ�����
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
	AD9954_SENDBYTE((uint8_t)(date>>8));//Ƶ�ʿ�����
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
	AD9954_SENDBYTE((uint8_t)(date>>8));//Ƶ�ʿ�����
	AD9954_SENDBYTE((uint8_t)date);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);

	AD9954_SENDBYTE(CFR1);//��ַ0д����
	AD9954_SENDBYTE(0xa0);//��RAM����λ����FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET); //CS = 1;
	UPDATE();  
}

/*********************************************************************************************************
** �������� :void Generate_PSK(float f,u16 *phase)
** �������� :PSK�����ź����
** ����˵�� :ͨ��RAM�������źŵ���λ,�Ӷ�ʵ��PSK�ĵ���
**            ����ʹ��һ��RAM�ε�ѭ��ģʽ,PSK�ź�Ϊ8λ����λ
**            RAM�ĸ���ram rate��ʱ�Ĵ�������,�Ĵ���Ϊ16λ,�ṩ��ϵͳ�ṩ��100Mʱ��1~65535��Ƶ,д0����Ч��
**			  �������PSK�ز���Ƶ����ram rate����Ƶ�ʵ�������ʱ,���ܱ�֤ÿ�ε���λ�ı�һ��
** ������� :float f   :PSK�ز���Ƶ��
** 			  u16 *phase: д����λ�ۼ�������λֵ,180ʵ����λ��ת,0���ı���λ,PSK�ź�Ϊ8λ,��Ϊ8λ������
** ������� :��
*********************************************************************************************************/
void Generate_PSK(uint32_t f,uint16_t *phase)
{
	uint8_t i;
	uint16_t date;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); //CS = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET); //UPD = 0;

	AD9954_SENDBYTE(CFR1);//��ַ0д����
	AD9954_SENDBYTE(0x00);//��RAM����λ����FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	//
	AD9954_SENDBYTE(CFR2);//��ַ1д����
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(Dou16);//16��Ƶ,��VCO���Ƹ�λ,ϵͳʱ�ӱ�Ƶ��Ϊ400M
//	AD9954_SETFRE(f);//�ز�Ƶ��

	AD9954_SENDBYTE(FTW0);//FTW0��ַ
	AD9954_SENDBYTE(0x00);//Ƶ�ʿ�����
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
	AD9954_SENDBYTE(0x80);//RAM0������ģʽ4,��ͣ��λ������
	UPDATE();

	AD9954_SENDBYTE(0x0b);	
	for(i=0;i<8;i++)
	{
		date=45.51*phase[i];
		date=date<<2;	
		AD9954_SENDBYTE((uint8_t)(date>>8));//Ƶ�ʿ�����
		AD9954_SENDBYTE((uint8_t)date);
		AD9954_SENDBYTE(0x00);
		AD9954_SENDBYTE(0x00);			
	}
	AD9954_SENDBYTE(CFR1);//��ַ0д����
	AD9954_SENDBYTE(0xc0);//��RAM����λ����FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET); //CS = 1;
	UPDATE();  
}

/*********************************************************************************************************
** �������� :void Generate_FM(u32 *fre)
** �������� :FM�����ź����
** ����˵�� :ͨ��RAM���������źŵ�Ƶ��,�Ӷ�ʵ��FM�źŵĵ���
**            ����ʹ��һ��RAM���źŵ�ѭ��ģʽ,PSK�ź�Ϊ8λ����λ
**            RAM�ĸ����ٶ���ram rate��ʱ�Ĵ�������,�Ĵ���Ϊ16λ,ʵ�ֶ�ϵͳ�ṩ��100Mʱ��1~65535��Ƶ,д0����Ч��
**			 			ram rate�ĸ����ٶ�Ϊ�����źŵ�Ƶ��,������1KHz,д��ram rate��ֵΪ0X0C35,
** ��ڲ��� :u32 *fre:FM�ź�Ƶ��ֵ,������õ���32��Ƶ�ʲ���,���ô˺���ʱ�����ú�Ƶ�ʱ��ֵ,�䰴���ҹ��ɸı�
**            for(i=0;i<32;i++)
**						{
**							fre[i]=Fc+Fshift*sin(wt);
**						}
** ���ڲ��� :��
*********************************************************************************************************/

void Generate_FM(uint32_t *fre)
{
	uint8_t i;
	uint32_t date;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); //CS = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET); //UPD = 0;

	AD9954_SENDBYTE(CFR1);//��ַ0д����
	AD9954_SENDBYTE(0x00);//��RAM����λ����FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);	

	AD9954_SENDBYTE(CFR2);//��ַ1д����
	AD9954_SENDBYTE(0x00);//
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(Dou16);//16��Ƶ,��VCO���Ƹ�λ,ϵͳʱ�ӱ�Ƶ��Ϊ400M

	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET); //PS0 = 0;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET); //PS1 = 0;
	AD9954_SENDBYTE(RSCW0);
	AD9954_SENDBYTE(0x35);//ramp rate=32kHz
	AD9954_SENDBYTE(0x0C);
	AD9954_SENDBYTE(0x1F);//final address:0x000
	AD9954_SENDBYTE(0x00);//start address:0x000;
	AD9954_SENDBYTE(0x80);//RAM0������ģʽ0,��ͣ��λ������
	UPDATE();
	
	AD9954_SENDBYTE(0x0b);	
	for(i=0;i<32;i++)
	{
		date=float_fck*fre[i];	
		AD9954_SENDBYTE((uint8_t)(date>>24));//Ƶ�ʿ�����
		AD9954_SENDBYTE((uint8_t)(date>>16));
		AD9954_SENDBYTE((uint8_t)(date>>8));
		AD9954_SENDBYTE((uint8_t)date);		
	}
	AD9954_SENDBYTE(CFR1);//��ַ0д����
	AD9954_SENDBYTE(0x80);//��RAM����λ����FTW
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x40);	 
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET); //CS = 1;
	UPDATE();  
}

/*********************************************************************************************************
** �������� :void ASK_Init(void)
** �������� :ASK�����ź������ʼ��
** ����˵�� :ASK����,��ҪԤ�����ú��ز�Ƶ��,Ȼ��ı�DAC�������� scale factor��Ϊ14λ,ͨ������Ϊ����0����ֵʵ��ASK�źŵĵ���
**			  ����Write_ASF(u16 factor)���ı��ֵ
** ��ڲ��� :��
** ���ڲ��� :��
*********************************************************************************************************/
void ASK_Init(void)
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); //CS = 0;
	AD9954_SENDBYTE(CFR1);//��ַ0д����
	AD9954_SENDBYTE(0x02);//�ֶ���OSK��
	AD9954_SENDBYTE(0x00);

	AD9954_SENDBYTE(0x20);//���ܵ�UPDATE�ź�,�Զ������λ�ۼ���
//	AD9954_SENDBYTE(0x00);//���ı��κ�

	AD9954_SENDBYTE(0x40);

	//������λƫ��,��Ĭ����������Ҳ�,������Ҫ������λ�ۼ�������λΪ270
	AD9954_SENDBYTE(POW0);
	AD9954_SENDBYTE(0X30);
	AD9954_SENDBYTE(0X00);
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET); //CS = 1;
	UPDATE();
}

/*********************************************************************************************************
** �������� :void AM_Init(void)
** �������� :AM�����ź������ʼ��
** ����˵�� :AM����,�������ú��ز�Ƶ��,Ȼ��ı�DAC�������� scale factor��Ϊ14λ,AM�ķ���ֵ�����ҹ��ɱ任,����ʵ��AM
**            ��߿��Բ���32�����
**			  ����Write_ASF(u16 factor)���ı��ֵ
** ��ڲ��� :��
** ���ڲ��� :��
*********************************************************************************************************/
void AM_Init(void)
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); //CS = 0;	AD9954_SENDBYTE(CFR1);//��ַ0д����
	AD9954_SENDBYTE(0x02);//�ֶ���OSK��
	AD9954_SENDBYTE(0x00);
	AD9954_SENDBYTE(0x00);//���ı��κ�
	AD9954_SENDBYTE(0x40);
	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET); //CS = 1;
	UPDATE();
}

/*********************************************************************************************************
** �������� :void Write_ASF(u16 factor)
** �������� :�ı�scale factor��ֵ,�ı�DAC�������
** ����˵�� :д�����ֵΪ0X3FFF,��СΪ0
** ��ڲ��� :��
** ���ڲ��� :��
*********************************************************************************************************/
void Write_ASF(uint16_t factor)  //2ASK
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET); //CS = 0;	AD9954_SENDBYTE(CFR1);//��ַ0д����
	AD9954_SENDBYTE(CFR1);//��ַ0д����
	AD9954_SENDBYTE(0x02);//�ֶ���OSK��
	AD9954_SENDBYTE(0x00);

	AD9954_SENDBYTE(0x20);//���ܵ�UPDATE�ź�,�Զ������λ�ۼ���		//
//	AD9954_SENDBYTE(0x00);//���ı��κ�

	AD9954_SENDBYTE(0x40);

	//������λƫ��
//	AD9954_SENDBYTE(POW0);
//	AD9954_SENDBYTE(0X30);
//	AD9954_SENDBYTE(0X00);	  

	AD9954_SENDBYTE(ASF);//����
	AD9954_SENDBYTE(factor>>8);
	AD9954_SENDBYTE(factor);	
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET); //CS = 1;
	UPDATE(); 

}



