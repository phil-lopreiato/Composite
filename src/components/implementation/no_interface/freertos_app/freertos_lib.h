#ifndef FREERTOS_LIB_H
#define FREERTOS_LIB_H

//#include <freertos_klib.h>
#include <frt.h>
#include "frt_rb.h"

#define MAX_FRT_THDS 10

typedef int portBASE_TYPE;
typedef unsigned int UBaseType_t;
typedef long portTickType;
typedef void* xQueueHandle;
typedef xQueueHandle SemaphoreHandle_t;
typedef void* xTaskHandle;

static volatile int memory_created = 0;

struct frt_thd_mapping {
        int tid;
        vaddr_t fptr;
        vaddr_t tptr;
} frt_thd_array[MAX_FRT_THDS] = {NULL, NULL, NULL};

struct rb_head *app_rb;
struct rb_head *k_rb;
//Because FreeRTOS is a steaming pile of ....

#define vSemaphoreBinaryCreate (xSemaphore); { xSemaphore = frt_vSemaphoreBinaryCreate(); }

int
xSemaphoreTake(SemaphoreHandle_t xSemaphore, portTickType xTicksToWait)
{
        return frt_xSemaphoreTake((int) xSemaphore, (int) xTicksToWait);
}

int
xSemaphoreGive(SemaphoreHandle_t xSemaphore)
{
        return frt_xSemaphoreGive((int) xSemaphore);
}

//Tasks

int
xTaskCreate(void* pvTaskCode, const char * const pcName, unsigned short usStackDepth, void *pvParameters, int uxPriority, xTaskHandle *pvCreatedTask)
{
        printc("passed in %d with prio %d\n", pvTaskCode, uxPriority);
        int i = 0;
        unsigned int ret = 0;
        unsigned short tid = 0;
        unsigned short ref = 0;

        while(frt_thd_array[i].tid != NULL)
                i++;
        printc("tid is %d\n", cos_get_thd_id());
        frt_thd_array[i].fptr = pvTaskCode;
        frt_thd_array[i].tptr = pvCreatedTask;
        printc("thd_ar %d\n", frt_thd_array[i].fptr);
        if ((ret = frt_xTaskCreate(pcName, usStackDepth, pvParameters, uxPriority)) == -1) BUG();
        printc("yes\n");
        tid = ret >> 16;
        ref = ret &  0x0000FFFF;
        printc("hope: %d\n", (int) tid);
        frt_thd_array[i].tid = (int) tid;
        *pvCreatedTask = ref;
        return ref;
}

void
vTaskDelete(xTaskHandle xTask)
{
        frt_vTaskDelete((int)xTask);
}

// What about task priority set and get?

void
vTaskSuspend(xTaskHandle xTaskToSuspend)
{
        frt_vTaskSuspend((int)xTaskToSuspend);
}

void
vTaskDelay(portTickType xTicksToDelay)
{
        frt_vTaskDelay((int)xTicksToDelay);
}

/*
void
vTaskDelayUntil(portTickType *pxPreviousWakeTime, portTickTime xTimeIncrement)
{
        frt_vTaskDelayUntil((int *)pxPreviousWakeTime, (int)xTimeIncrement);
}
*/

void
vTaskResume(xTaskHandle xTaskToResume)
{
        frt_vTaskDelay((int)xTaskToResume);
}

portBASE_TYPE
xTaskResumeFromISR(xTaskHandle xTaskToResume)
{
        return frt_xTaskResumeFromISR((int)xTaskToResume);
}

xTaskHandle
xTaskGetCurrentTaskHandle(void)
{
        return frt_xTaskGetCurrentTaskHandle();
}

xTaskHandle
xTaskGetIdleTaskHandle(void)
{
        return frt_xTaskGetIdleTaskHandle();
}

UBaseType_t
uxTaskGetStackHighWaterMark(xTaskHandle xTask)
{
        return frt_uxTaskGetStackHighWaterMark((int)xTask);
}

/*
 eTaskState
 eTaskGetState(xTaskHandle xTask)
 {
        return frt_eTaskGetState((int)xTask);
 }
 */

// Can't do pointers, returns a char*
/*
char*
pcTaskGetName(xTaskHandle xTaskToQuery)
{
        return frt_pcTaskGetName((int)xTaskToQuery);
}
*/

portTickType
xTaskGetTickCount(void)
{
        return frt_xTaskGetTickCount();
}

portTickType
xTaskGetTickCountFromISR(void)
{
        return frt_xTaskGetTickCountFromISR();
}

portBASE_TYPE
xTaskGetSchedulerState(void)
{
        return frt_xTaskGetSchedulerState();
}

UBaseType_t
uxTaskGetNumberOfTasks(void)
{
        return frt_uxTaskGetNumberOfTasks();
}

// char * pointer
/*
void
vTaskList(char *pcWriteBuffer)
{
}
*/

// char * pointer
/*
void
frt_vTaskStartTrace(char *pcBuffer, unsigned long ulBufferSize)
{
}
*/

unsigned long
ulTaskEndTrace(void)
{
        return frt_ulTaskEndTrace();
}
//Queues

int
xQueueCreate(portBASE_TYPE uxQueueLength, portBASE_TYPE uxItemSize)
{
        int wat = 0;
        printc("In queue create\n");
        assert(uxQueueLength == 10 && uxItemSize == sizeof(int)); //if (unlikely(memory_created == 0)) {
        if (memory_created == 0) {
                int ret = -1;
                vaddr_t ptr = -1;
                memory_created = 1;

                ptr = cos_get_heap_ptr();
                cos_set_heap_ptr(ptr + 8192);
                if (cos_get_heap_ptr() != ptr + 8192) BUG();
                ret = frt_xQueueCreate((int) uxQueueLength, (int) uxItemSize, (int) ptr, cos_spd_id());
                app_rb = ptr;
                k_rb = ptr + 4096;
                printc("wat: %d\n", app_rb->prod);
                return ret;
        } else
                return frt_xQueueCreate((int) uxQueueLength, (int) uxItemSize, NULL, NULL);
}

int
xQueueSend(xQueueHandle xQueue, const void * pvItemToQueue, portTickType xTicksToWait)
{
        assert((int) xQueue >= 0 && pvItemToQueue);
        unsigned short int item = app_rb->prod + sizeof(struct rb_item_head);
        rb_insert(app_rb, pvItemToQueue, sizeof(int));
        return frt_xQueueSend((int)xQueue, (int)item, (int)xTicksToWait);
}

int
xQueueReceive(xQueueHandle xQueue, void *pvBuffer, portTickType xTicksToWait)
{
        int ret;
        assert((int) xQueue >= 0 && pvBuffer);
        unsigned short int buff = k_rb->prod;
        ret = frt_xQueueReceive((int) xQueue, (int)buff, (int)xTicksToWait);
        rb_remove(k_rb, pvBuffer, sizeof(int));
        return ret;
}

#endif
