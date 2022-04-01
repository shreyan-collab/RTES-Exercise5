/* FreeRTOS 10 Tiva Demo
 *
 * main.c
 *
 * Andy Kobyljanec
 *
 * This is a simple demonstration project of FreeRTOS 8.2 on the Tiva Launchpad
 * EK-TM4C1294XL.  TivaWare driverlib sourcecode is included.
 */

#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include "drivers/pinout.h"
#include "utils/uartstdio.h"


// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"


#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c1294ncpdt.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include <stdlib.h>
//#include <time.h> --i am doubtful about this

#define ERROR (-1)
#define OK (0)
#define NSEC_PER_SEC (1000000000)
#define NSEC_PER_MSEC (1000000)
#define NSEC_PER_MICROSEC (1000)
#define USEC_PER_MSEC (1000)
#define NANOSEC_PER_SEC (1000000000)
#define NUM_CPU_CORES (1)
#define TRUE (1)
#define FALSE (0)

#define NUM_THREADS (7+1)

int abortTest=FALSE;
int abortS1=FALSE, abortS2=FALSE, abortS3=FALSE, abortS4=FALSE, abortS5=FALSE, abortS6=FALSE, abortS7=FALSE;
static SemaphoreHandle_t semS1, semS2, semS3, semS4, semS5, semS6, semS7;
TaskHandle_t xHandle1 = NULL;
TaskHandle_t xHandle2 = NULL;
TaskHandle_t xHandle3 = NULL;
TaskHandle_t xHandle4 = NULL;
TaskHandle_t xHandle5 = NULL;
TaskHandle_t xHandle6 = NULL;
TaskHandle_t xHandle7 = NULL;


// Demo Task declarations
void Service_1(void *threadp);
void Service_2(void *threadp);
void Service_3(void *threadp);
void Service_4(void *threadp);
void Service_5(void *threadp);
void Service_6(void *threadp);
void Service_7(void *threadp);
double getTimeMsec(void);
int worst_case_execution_time(long int arr[], int length);

void demoLEDTask(void *pvParameters);
void demoSerialTask(void *pvParameters);

uint32_t ui32Period, pom = 0;
uint32_t ui32SysClkFreq;

int interrupt_flag=0;

uint32_t idx = 0, jdx = 1;
uint32_t fib = 0, fib0 = 0, fib1 = 1;
QueueHandle_t queue1 = NULL;

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


int worst_case_execution_time(long int arr[], int length)
{
    int maximum_value = arr[0];
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

void Service_1(void *threadp)
{
    int iteration =0;
    int max_wcet= 0;
  //  struct timespec start_time = {0, 0};
  //  struct timespec finish_time = {0, 0};
  //  struct timespec thread_dt = {0, 0};
    long int wcet[90];
//    struct timeval current_time_val;
//    double current_time;
    unsigned long long S1Cnt=0;
//    threadParams_t *threadParams = (threadParams_t *)threadp;

 //   gettimeofday(&current_time_val, (struct timezone *)0);
 //   syslog(LOG_CRIT, "Frame Sampler thread @ sec=%d, msec=%d\n", (int)(current_time_val.tv_sec-start_time_val.tv_sec), (int)current_time_val.tv_usec/USEC_PER_MSEC);
 //   printf("Frame Sampler thread @ sec=%d, msec=%d\n", (int)(current_time_val.tv_sec-start_time_val.tv_sec), (int)current_time_val.tv_usec/USEC_PER_MSEC);


    while(!abortS1)
    {
        //sem_wait(&semS1);
        xSemaphoreTake(semS1, portMAX_DELAY);
    //    clock_gettime(CLOCK_REALTIME, &start_time);
        S1Cnt++;
        UARTprintf("HELLOOOOO\n");
      //  gettimeofday(&current_time_val, (struct timezone *)0);
      //  clock_gettime(CLOCK_REALTIME, &finish_time);
      //  delta_t(&finish_time, &start_time, &thread_dt);
   //     wcets1[iteration]= thread_dt.tv_nsec;
        //syslog(LOG_INFO, "Time Taken for Service 1: %ld for iteration %d:\n",thread_dt.tv_nsec, iteration);
        //syslog(LOG_CRIT, "Service 1: Frame Sampler release %llu @ sec=%d, msec=%d\n", S1Cnt, (int)(current_time_val.tv_sec-start_time_val.tv_sec), (int)current_time_val.tv_usec/USEC_PER_MSEC);
        iteration++;
    }
    max_wcet = worst_case_execution_time(wcet, iteration);
    UARTprintf("WCET S1: %ld\n\r",max_wcet);
    vTaskDelete( xHandle1 );
   // pthread_exit((void *)0);
}


// Main function
int main(void)
{
    // Initialize system clock to 120 MHz
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

        ui32Period = (ui32SysClkFreq /1000);
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

    semS1 = xSemaphoreCreateBinary();
    if (semS1 == NULL)
    {
        UARTprintf("Error in creating semaphore: sem1");
    }

    xTaskCreate(Service_1, (const portCHAR *)"S1",
                    configMINIMAL_STACK_SIZE,NULL,configMAX_PRIORITIES - 1, &xHandle1 );

    vTaskStartScheduler();
    
    // Code should never reach this point
    return 0;
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
