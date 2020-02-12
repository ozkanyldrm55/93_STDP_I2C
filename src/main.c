#include "stm32f4xx.h"
#include "stm32f4_discovery.h"

/*structure tanimlamlalari*/
GPIO_InitTypeDef GPIO_InitStruct;
I2C_InitTypeDef I2C_InitStruct;

uint8_t  m_address = 0x4E; // PCF8574 I2C Adresi

int i = 0;

void delay(uint32_t time)
{
	while(time--);
}

void GPIO_Config()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); // PA0 BUTTON
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); // PB6 , PB7 = SCL , SDA

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOA,&GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOB , GPIO_PinSource6, GPIO_AF_I2C1);	// SCL Pin
	GPIO_PinAFConfig(GPIOB , GPIO_PinSource7, GPIO_AF_I2C1);	// SDA Pin

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_Init(GPIOB,&GPIO_InitStruct);
}

void I2C_Config()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);

	I2C_InitStruct.I2C_Ack = ENABLE; 				// slave cihaz adresi kabul ettigine dair mesaj gondermesi icin aktif ediyoruz
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // set address length to 7 bit addresses , ACK adresi 7 bit olarak sectik
	I2C_InitStruct.I2C_ClockSpeed = 400000; 		// 400kHz , i2c hattý max 400khz kadar calisir
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2; // 50% duty cycle --> standard
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C; 		// I2C mode
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;			// own address, not relevant in master mode , cihazýn kendi adresini belirtir.

	I2C_Init(I2C1 ,& I2C_InitStruct);
	I2C_Cmd(I2C1,ENABLE);
}

void I2C_Write(uint8_t address , uint8_t  data) /* gonderecegimiz data yý cihazlara yazdirmaya yarar.
                                                   ornegin 5 cihaz var adresini yazdýgimiz cihaz ile iletisime gececek */
{
	while(I2C_GetFlagStatus(I2C1 , I2C_FLAG_BUSY)); // i2c bayragi uygun olana kadar bekle.
													//nedeni ise biz yeni bir data gonderecegiz ama i2c hattý musait mi baska bir cihaz ile iletisim halinde mi diye

	I2C_GenerateSTART(I2C1 , ENABLE); 				// Send I2C1 START condition , I2C1 baslatiyoruz.

	while(!(I2C_CheckEvent(I2C1 , I2C_EVENT_MASTER_MODE_SELECT))); // master mod un secilmesini kontrol ediyoruz (veri gonderecegimiz icin master olmaliyiz), yani cihazimiz master olmalý
																   // master oldugu anda 1 olacak ve tersi olup while den cikacak

	I2C_Send7bitAddress(I2C1 , address , I2C_Direction_Transmitter); // hangi slave cihazla iletisime baslayacagimizi seciyoruz(address) , receive deil de veri gonderecegimizi soyluyoruz.

	while(!(I2C_CheckEvent(I2C1 , I2C_EVENT_MASTER_BYTE_TRANSMITTED))); // gonderici olarak hazir hale geldim mi diye sorguluyorum.
	I2C_SendData(I2C1 ,data); // data gonderiyorum.

	I2C_GenerateSTOP(I2C1 , ENABLE); // i2c1 i durduruyoruz.
}

int main(void)
{
	GPIO_Config();
	I2C_Config();

  while (1)
  {
	  if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)) // butonu okunuyor.
	  {
		  while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)) // ark olmamasi icin buton dan el cekilene kadar bekleniyor.
		  i++;
		  delay(1680000);
	  }

	  switch(i)
	  {
	  case 0 :
		  I2C_Write(m_address , 0x00);
		  break;
	  case 1 :
		  I2C_Write(m_address , 0x01); // led 1 on
		  break;
	  case 2 :
		  I2C_Write(m_address , 0x02);
		  break;
	  case 3 :
		  I2C_Write(m_address , 0x04);
		  break;
	  case 4 :
		  I2C_Write(m_address , 0x08);
		  break;
	  case 5 :
		  I2C_Write(m_address , 0x10);
		  break;
	  case 6 :
		  I2C_Write(m_address , 0x20);
		  break;
	  case 7 :
		  I2C_Write(m_address , 0x40);
		  break;
	  case 8 :
		  I2C_Write(m_address , 0x80);
		  break;
	  default :
		  i = 0;
		  break;
	  }
  }
}

/*
 * Callback used by stm32f4_discovery_audio_codec.c.
 * Refer to stm32f4_discovery_audio_codec.h for more info.
 */
void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size){
  /* TODO, implement your code here */
  return;
}

/*
 * Callback used by stm324xg_eval_audio_codec.c.
 * Refer to stm324xg_eval_audio_codec.h for more info.
 */
uint16_t EVAL_AUDIO_GetSampleCallBack(void){
  /* TODO, implement your code here */
  return -1;
}
