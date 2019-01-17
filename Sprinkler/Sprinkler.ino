/*
 Name:		sprinkler.ino
 Created:	06.11.2018 22:41:02
 Author:	Ksiw
*/
//значени¤ и сна указываютс¤ кратно 4м секундам

#define LED 13          //нога светодиода
#define WORK 8			//сек качать воду	
#define SLEEP 3600     //1 ед - 8 сек // 8ч
#define WORKLOAD 5		//пин насоса


#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>


uint32_t timerWORK = 0, timerSLEEP = 0;



void setup()
{
	digitalWrite(2, HIGH);    // кнопка с подт¤гивающим резистором
	pinMode(WORKLOAD, OUTPUT);
	pinMode(LED, OUTPUT);
	
	func_Work();
	
}

void loop()
{
	if (SLEEP <= timerSLEEP)
		func_Work();
	
	
	

	// шаблон битов sleep:
	//  1 секунда:  0b000110
	//  2 секунды:	0b000111
	//  4 секунды:	0b100000
	//  8 секунд:	0b100001

	 // засыпаем на x секунд
	myWatchdogEnable(0b100001);  // 1 секунд
	timerSLEEP++;
}
//--------------------------------------------------------------------------------------
// процедура обработки прерывани¤ по нажатию кнопки
void wake()
{
	wdt_disable();  // отключаем сторожевой таймер
}
//--------------------------------------------------------------------------------------
// прерывание сторожевого таймера
ISR(WDT_vect)
{
	wake();
}
//----------------------------------------------------------------------------------------
void myWatchdogEnable(const byte interval)
{
	noInterrupts();

	MCUSR = 0;                          // сбрасываем различные флаги
	WDTCSR |= 0b00011000;               // устанавливаем WDCE, WDE
	WDTCSR = 0b01000000 | interval;    // устанавливаем WDIE, и соответсвующую задержку
	wdt_reset();

	byte adcsra_save = ADCSRA;
	ADCSRA = 0;  // запрещаем работу ј÷ѕ
	power_all_disable();   // выключаем все модули
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // устанавливаем режим сна
	sleep_enable();
	attachInterrupt(0, wake, LOW);   // позвол¤ем заземлить pin 2 дл¤ выхода из сна
	interrupts();
	sleep_cpu();            // переходим в сон и ожидаем прерывание
	detachInterrupt(0);     // останавливаем прерывание LOW

	ADCSRA = adcsra_save;  // останавливаем понижение питани¤
	power_all_enable();   // включаем все модули
}
//---------------------------------------------------------------------------------------------
void func_Work()
{	
	/*Serial.begin(115200);
	Serial.println("Work");*/

	digitalWrite(WORKLOAD, HIGH);
	digitalWrite(LED, HIGH);
	delay(WORK * 1000);

	digitalWrite(WORKLOAD, LOW);
	digitalWrite(LED, LOW);
	timerSLEEP = 0;

}
