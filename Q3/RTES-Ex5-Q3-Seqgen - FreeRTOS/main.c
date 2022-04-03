
/*
 * @file-name       :   main.c
 *
 * @author          :   Shreyan Prabhhu & Tanmay Mahendra Kothale
 *
 * @date            :   01 April 2022
 *
 * @brief           :   A prototype of a sequencer running several services
 *                      using FreeRTOS.
 *
 * @references      :   Simple demonstration project of FreeRTOS 8.2 on the Tiva
 *                      Launchpad EK-TM4C1294XL by Andy Kobyljanec.  TivaWare
 *                      driverlib sourcecode is included.
 */

/*----------------------------------------------------------------------------------------------------*/
/*                                          LIBRARY FILES                                             */
/*----------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include "drivers/pinout.h"
#include "utils/uartstdio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#include "inc/tm4c1294ncpdt.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include <stdlib.h>
#include <time.h>
/*----------------------------------------------------------------------------------------------------*/
/*                                       DEFINES AND MACROS                                           */
/*----------------------------------------------------------------------------------------------------*/
#define ERROR                           (-1)
#define OK                              (0)
#define NSEC_PER_SEC                    (1000000000)
#define NSEC_PER_MSEC                   (1000000)
#define NSEC_PER_MICROSEC               (1000)
#define USEC_PER_MSEC                   (1000)
#define NANOSEC_PER_SEC                 (1000000000)
#define NUM_CPU_CORES                   (1)
#define TRUE                            (1)
#define FALSE                           (0)
#define NUM_THREADS                     (7+1)
#define PRIORITY_3_HZ                   (4)
#define PRIORITY_1_HZ                   (3)
#define PRIORITY_HALF_HZ                (2)
#define PRIORITY_ONE_TENTH_HZ           (1)

#define FIB_TEST(seqCnt, iterCnt)      \
   for(idx=0; idx < iterCnt; idx++)    \
   {                                   \
      fib = fib0 + fib1;               \
      while(jdx < seqCnt)              \
      {                                \
         fib0 = fib1;                  \
         fib1 = fib;                   \
         fib = fib0 + fib1;            \
         jdx++;                        \
      }                                \
   }                                   \
/*----------------------------------------------------------------------------------------------------*/
/*                                         GLOBAL VARIABLES                                           */
/*----------------------------------------------------------------------------------------------------*/
int abortTest=FALSE;
int abortS1=FALSE, abortS2=FALSE, abortS3=FALSE,
    abortS4=FALSE, abortS5=FALSE, abortS6=FALSE,
    abortS7=FALSE;
static SemaphoreHandle_t    semS1, semS2, semS3,
                            semS4, semS5, semS6,
                            semS7, print_sem;
TaskHandle_t                xHandle1 = NULL, xHandle2 = NULL, xHandle3 = NULL,
                            xHandle4 = NULL, xHandle5 = NULL, xHandle6 = NULL,
                            xHandle7 = NULL;
uint32_t ui32Period, count_333us = 0, ui32SysClkFreq;
uint32_t idx = 0, jdx = 1;
uint32_t fib = 0, fib0 = 0, fib1 = 1;
QueueHandle_t queue1 = NULL;
uint32_t    wcets1[90], wcets2[90], wcets3[90],
            wcets4[90], wcets5[90], wcets6[90], wcets7[90];
uint32_t    S1Cnt = 0, S2Cnt = 0, S3Cnt = 0,
            S4Cnt = 0, S5Cnt = 0, S6Cnt = 0, S7Cnt = 0;
uint32_t    max_wcet1, max_wcet2, max_wcet3,
            max_wcet4, max_wcet5, max_wcet6,
            max_wcet7;
/*----------------------------------------------------------------------------------------------------*/
/*                                         TASK DECLARATIONS                                          */
/*----------------------------------------------------------------------------------------------------*/
void Service_1(void *threadp);
void Service_2(void *threadp);
void Service_3(void *threadp);
void Service_4(void *threadp);
void Service_5(void *threadp);
void Service_6(void *threadp);
void Service_7(void *threadp);
/*----------------------------------------------------------------------------------------------------*/
/*                                         FUNCTION PROTOTYPES                                        */
/*----------------------------------------------------------------------------------------------------*/

/*
 * @brief       :   Calculates the maximum value in an array
 *                  of WCET.
 *
 * @parameters  :   arr[]  = Integer array in which various WCET are
 *                           stored.
 *                  length = Length of integer array arr[]
 *
 * @returns     :   maximum_value = WCET for the service
 */
uint32_t
worst_case_execution_time(uint32_t arr[], uint32_t length)
{
    uint32_t maximum_value = arr[0];
    int i=0;
    for(i=0; i < length ; i++)
    {
        if(arr[i] > maximum_value)
        {
            maximum_value = arr[i];
        }
    }
    return maximum_value;
}
/*----------------------------------------------------------------------------------------------------*/
/*
 * @brief       :   calculates time since last reboot
 *
 * @returns     :   time since last reboot
 */
static uint32_t
gettime()
{
    return (count_333us*333);
}
/*----------------------------------------------------------------------------------------------------*/
/*                                         SERVICE DEFINITIONS                                        */
/*----------------------------------------------------------------------------------------------------*/
void
Service_1(void *threadp)
{
    uint32_t before, after;
    int i;

    while(!abortS1)
    {
        xSemaphoreTake(semS1, portMAX_DELAY);
        before = xTaskGetTickCount();
        //FIB_TEST(47,500);
        for (i=0; i<10900; i++);
        after = xTaskGetTickCount();
        wcets1[S1Cnt] = after - before;
        S1Cnt++;
    }

    max_wcet1 = worst_case_execution_time(wcets1, S1Cnt);
    UARTprintf("WCET S1: %d\n\r", max_wcet1);
    xSemaphoreGive(print_sem);
    vTaskDelete(NULL);
}
/*----------------------------------------------------------------------------------------------------*/
void
Service_2(void *threadp)
{
    uint32_t before, after;
    int i;

    while(!abortS2)
    {
        xSemaphoreTake(semS2, portMAX_DELAY);
        before = xTaskGetTickCount();
        //FIB_TEST(47,500);
        for (i=0; i<10900; i++);
        after = xTaskGetTickCount();
        wcets2[S2Cnt] = after - before;
        S2Cnt++;
    }

    xSemaphoreTake(print_sem, portMAX_DELAY);
    max_wcet2 = worst_case_execution_time(wcets2, S2Cnt);
    UARTprintf("WCET S2: %d\n\r", max_wcet2);
    xSemaphoreGive(print_sem);
    vTaskDelete(NULL);

}
/*----------------------------------------------------------------------------------------------------*/
void
Service_3(void *threadp)
{
    uint32_t before, after;
    int i;

    while(!abortS3)
    {
        xSemaphoreTake(semS3, portMAX_DELAY);
        before = xTaskGetTickCount();
        //FIB_TEST(47,500);
        for (i=0; i<10900; i++);
        after = xTaskGetTickCount();
        wcets3[S3Cnt] = after - before;
        S3Cnt++;
    }

    xSemaphoreTake(print_sem, portMAX_DELAY);
    max_wcet3 = worst_case_execution_time(wcets3, S3Cnt);
    UARTprintf("WCET S3: %d\n\r", max_wcet3);
    xSemaphoreGive(print_sem);
    vTaskDelete(NULL);
}
/*----------------------------------------------------------------------------------------------------*/
void
Service_4(void *threadp)
{
    uint32_t before, after;
    int i;

    while(!abortS4)
    {
        xSemaphoreTake(semS4, portMAX_DELAY);
        before = xTaskGetTickCount();
        //FIB_TEST(47,500);
        for (i=0; i<10900; i++);
        after = xTaskGetTickCount();
        wcets4[S4Cnt] = after - before;
        S4Cnt++;
    }

    xSemaphoreTake(print_sem, portMAX_DELAY);
    max_wcet4 = worst_case_execution_time(wcets4, S4Cnt);
    UARTprintf("WCET S4: %d\n\r", max_wcet4);
    xSemaphoreGive(print_sem);
    vTaskDelete(NULL);
}
/*----------------------------------------------------------------------------------------------------*/
void
Service_5(void *threadp)
{
    uint32_t before, after;
    int i;

    while(!abortS5)
    {
        xSemaphoreTake(semS5, portMAX_DELAY);
        before = xTaskGetTickCount();
        //FIB_TEST(47,500);
        for (i=0; i<10900; i++);
        after = xTaskGetTickCount();
        wcets5[S5Cnt] = after - before;
        S5Cnt++;
    }

    xSemaphoreTake(print_sem, portMAX_DELAY);
    max_wcet5 = worst_case_execution_time(wcets5, S5Cnt);
    UARTprintf("WCET S5: %d\n\r", max_wcet5);
    xSemaphoreGive(print_sem);
    vTaskDelete(NULL);
}
/*----------------------------------------------------------------------------------------------------*/
void
Service_6(void *threadp)
{
    uint32_t before, after;
    int i;

    while(!abortS6)
    {
        xSemaphoreTake(semS6, portMAX_DELAY);
        before = xTaskGetTickCount();
        //FIB_TEST(47,500);
        for (i=0; i<10900; i++);
        after = xTaskGetTickCount();
        wcets6[S6Cnt] = after - before;
        S6Cnt++;
    }

    xSemaphoreTake(print_sem, portMAX_DELAY);
    max_wcet6 = worst_case_execution_time(wcets6, S6Cnt);
    UARTprintf("WCET S6: %d\n\r", max_wcet6);
    xSemaphoreGive(print_sem);
    vTaskDelete(NULL);
}
/*----------------------------------------------------------------------------------------------------*/
void
Service_7(void *threadp)
{
    uint32_t before, after;
    int i;

    while(!abortS7)
    {
        xSemaphoreTake(semS7, portMAX_DELAY);
        before = xTaskGetTickCount();
        //FIB_TEST(47,500);
        for (i=0; i<10900; i++);
        after = xTaskGetTickCount();
        wcets7[S7Cnt] = after - before;
        S7Cnt++;
    }

    xSemaphoreTake(print_sem, portMAX_DELAY);
    max_wcet7 = worst_case_execution_time(wcets7, S7Cnt);
    UARTprintf("WCET S7: %d\n\r", max_wcet7);
    xSemaphoreGive(print_sem);
    vTaskDelete(NULL);
}
/*----------------------------------------------------------------------------------------------------*/
/*                                        INIT FUNCTIONS                                              */
/*----------------------------------------------------------------------------------------------------*/
static void
init_peripherals()
{
    //Initialize system clock to 120 MHz
    uint32_t output_clock_rate_hz;
    output_clock_rate_hz = ROM_SysCtlClockFreqSet(
                               (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                                SYSTEM_CLOCK);
    ASSERT(output_clock_rate_hz == SYSTEM_CLOCK);

    ui32SysClkFreq = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    ui32Period = (ui32SysClkFreq / 3000); //timer will go off every 333.33usec
    TimerLoadSet(TIMER0_BASE, TIMER_A,  ui32Period);

    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();
    TimerEnable(TIMER0_BASE, TIMER_A);

    /*Configuring the UART*/
    ASSERT(output_clock_rate_hz == SYSTEM_CLOCK);
    UARTStdioConfig(0, 57600, output_clock_rate_hz);
    // Initialize the GPIO pins for the Launchpad
    PinoutSet(false, false);
}
/*----------------------------------------------------------------------------------------------------*/
static void
init_semaphores()
{
    semS1 = xSemaphoreCreateBinary();
    if (semS1 == NULL)
    {
        UARTprintf("Error in creating semaphore: sem1\n\r");
    }
    semS2 = xSemaphoreCreateBinary();
    if (semS2 == NULL)
    {
        UARTprintf("Error in creating semaphore: sem2\n\r");
    }
    semS3 = xSemaphoreCreateBinary();
    if (semS3 == NULL)
    {
        UARTprintf("Error in creating semaphore: sem3\n\r");
    }
    semS4 = xSemaphoreCreateBinary();
    if (semS4 == NULL)
    {
        UARTprintf("Error in creating semaphore: sem4\n\r");
    }
    semS5 = xSemaphoreCreateBinary();
    if (semS5 == NULL)
    {
        UARTprintf("Error in creating semaphore: sem5\n\r");
    }
    semS6 = xSemaphoreCreateBinary();
    if (semS6 == NULL)
    {
        UARTprintf("Error in creating semaphore: sem6\n\r");
    }
    semS7 = xSemaphoreCreateBinary();
    if (semS7 == NULL)
    {
        UARTprintf("Error in creating semaphore: sem7\n\r");
    }
    print_sem = xSemaphoreCreateBinary();
    if (print_sem == NULL)
    {
        UARTprintf("Error in creating semaphore: print_sem\n\r");
    }
}
/*----------------------------------------------------------------------------------------------------*/
static void
create_tasks()
{
    xTaskCreate(Service_1, (const portCHAR *)"S1",
                    configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY + PRIORITY_3_HZ , &xHandle1 );
    xTaskCreate(Service_2, (const portCHAR *)"S2",
                    configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY + PRIORITY_1_HZ  , &xHandle2 );
    xTaskCreate(Service_3, (const portCHAR *)"S3",
                    configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY + PRIORITY_HALF_HZ, &xHandle3 );
    xTaskCreate(Service_4, (const portCHAR *)"S4",
                    configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY + PRIORITY_1_HZ, &xHandle4 );
    xTaskCreate(Service_5, (const portCHAR *)"S5",
                    configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY + PRIORITY_HALF_HZ, &xHandle5 );
    xTaskCreate(Service_6, (const portCHAR *)"S6",
                    configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY + PRIORITY_1_HZ, &xHandle6 );
    xTaskCreate(Service_7, (const portCHAR *)"S7",
                    configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY + PRIORITY_ONE_TENTH_HZ, &xHandle7 );
}
/*----------------------------------------------------------------------------------------------------*/
/*  @brief: application entry point                                                                   */
/*----------------------------------------------------------------------------------------------------*/
// Main function
int
main(void)
{
    init_peripherals();

    init_semaphores();

    create_tasks();

    vTaskStartScheduler();

    // Code should never reach this point
    return 0;
}

void Timer0IntHandler(void)
{
    static int s1=0, s2=0, s3=0, s4=0, s5=0, s6=0, s7=0;
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    count_333us++;

    //UARTprintf("Inside ISR, count: %d\n\r",count_33ms);
    if ((count_333us % 10) == 0) //3 hz
    {
        s1++;
        xSemaphoreGive(semS1);
    }
    if ((count_333us % 30) == 0) //1 hz
    {
        s2++;
        xSemaphoreGive(semS2);
        s4++;
        xSemaphoreGive(semS4);
        s6++;
        xSemaphoreGive(semS6);
    }
    if ((count_333us % 60) == 0) //0.5 hz
    {
        s3++;
        xSemaphoreGive(semS3);
        s5++;
        xSemaphoreGive(semS5);
    }
    if ((count_333us % 300) == 0) //0.1 hz
    {
        s7++;
        xSemaphoreGive(semS7);
    }

    if (s1 == 90)
    {
        abortS1=TRUE;
    }
    if (s2 == 30 && s4 == 30 && s6 == 30)
    {
        abortS2=TRUE; abortS4=TRUE; abortS6=TRUE;
    }
    if (s3 == 15 && s5 == 15)
    {
        abortS3=TRUE; abortS5=TRUE;
    }
    if (s7 == 3)
    {
        abortS7=TRUE;
    }

    if (count_333us == 900)
    {
        TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
        TimerDisable(TIMER0_BASE, TIMER_A);
        abortS1=TRUE; abortS2=TRUE; abortS3=TRUE;
        abortS4=TRUE; abortS5=TRUE; abortS6=TRUE;
        abortS7=TRUE;
    }
}

/*  ASSERT() Error function
 *
 *  failed ASSERTS() from driverlib/debug.h are executed in this function
 */
void __error__(char *pcFilename, uint32_t ui32Line)
{
    // Place a breakpoint here to capture errors until logging routine is finished
    while (1)
    {
    }
}
