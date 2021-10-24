#include <FreeRTOS.h>
#include <mina_shield_mbed.h>
#include <portmacro.h>
#include <stm_core.h>
#include <system_stm32f4xx.h>
#include <task.h>

#define mainLED_TASK_PRIORITY ( tskIDLE_PRIORITY + 1 )

// pokuï to dobøe chápu tak prostì èasuju fuknci nezávisle na programu pomocí taskù, ale stejnì mi to sežere Systick tkže mi to moc smysl nedává

// každej task má svojí promìnou c (svùj zásobník, ale stejný kód)

void LEDFlashTask(void *params) // volám 3x pro každý task, vstupní argument void øetìzec, fuknce si vyzvedne prvni pismenko.. a fuknce bìží 3x a jednou je tam g jednou b a jednoun r
{
    char c = ((char *)params)[0];
    portTickType lastWakeTime = xTaskGetTickCount(); // poprvé se spustil a uloží si tik
    while(1)
    {
        switch(c)
        {
            case 'R':
                GPIOToggle(RGBLED_RED);
                break;
            case 'G':
                GPIOToggle(RGBLED_GREEN);
                break;
            case 'B':
                GPIOToggle(RGBLED_BLUE);
                break;
        }

        //vTaskDelay(10 * c); // urèuji èasování, øikám za jak dlouho má pøestat pracovat (èekej 10 x znak, kde b =66 g = nevim atd..)
        vTaskDelayUntil(&lastWakeTime, 10 * c); // bìž spát na a vstaò zase za dobu od posldního uložení tiku (aby mi do toho èasi nekecala doba bìhu fuknce)


    }
}

int main(void) {

    //SetClock100MHz(clockSourceHSE);

    SystemCoreClockUpdate();

    GPIOConfigurePin(RGBLED_BLUE,ioPortOutputPushPull);
    GPIOConfigurePin(RGBLED_GREEN,ioPortOutputPushPull);
    GPIOConfigurePin(RGBLED_RED,ioPortOutputPushPull);

    GPIOWrite(RGBLED_BLUE,1);
    GPIOWrite(RGBLED_GREEN,1);
    GPIOWrite(RGBLED_RED,1);

    xTaskCreate(LEDFlashTask, "LED_R", configMINIMAL_STACK_SIZE, "R" ,mainLED_TASK_PRIORITY, NULL); // nazev funkce (LEDFlashTask), nazev toho procesu (je libovolný ale pak je vidìt v procesech), stack size, parametr fuknce led..., priorita, nìjakej handller, nevím takže null
    xTaskCreate(LEDFlashTask, "LED_G", configMINIMAL_STACK_SIZE, "G" ,mainLED_TASK_PRIORITY, NULL);
    xTaskCreate(LEDFlashTask, "LED_B", configMINIMAL_STACK_SIZE, "B" ,mainLED_TASK_PRIORITY, NULL);

    // vyrobili jsme si tøi tasky

    vTaskStartScheduler();
    // sem se to nedostane pokud bìží alespoò jeden Task

}



/*
    uint32_t tm_BLUE = 0;
    uint32_t tm_GREEN = 0;
    uint32_t tm_RED = 0;
    uint32_t _ticks = 0;

    while (1)
    {
        for (int i = 0; i < 2000; i++); // pri 16MHz

        _ticks++;

        if (_ticks >= tm_BLUE)
        {
            tm_BLUE = _ticks + 50;
            GPIOToggle(RGBLED_BLUE);
        }

        if (_ticks >= tm_GREEN)
        {
            tm_GREEN = _ticks + 100;
            GPIOToggle(RGBLED_GREEN);
        }

        if (_ticks >= tm_RED)
        {
            tm_RED = _ticks + 150;
            GPIOToggle(RGBLED_RED);
        }

    }
}
 */
