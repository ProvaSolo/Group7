/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT or OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
* Application specific definitions.
* Adjusted for balanced priorities and stable timer operation.
*----------------------------------------------------------*/

#ifndef __NVIC_PRIO_BITS
    #define __NVIC_PRIO_BITS 4  /* Cortex-M7 uses 4 priority bits */
#endif

/* Trace and runtime stats configuration */
#define configUSE_TRACE_FACILITY                 0
#define configGENERATE_RUN_TIME_STATS            0

/* Scheduler configuration */
#define configUSE_PREEMPTION                     1
#define configUSE_IDLE_HOOK                      0
#define configUSE_TICK_HOOK                      0
#define configCPU_CLOCK_HZ                       ( ( unsigned long ) 240000000 )
#define configTICK_RATE_HZ                       ( ( TickType_t ) 1000 )
#define configMINIMAL_STACK_SIZE                 ( ( unsigned short ) 160 )
#define configTOTAL_HEAP_SIZE                    ( ( size_t ) ( 16 * 1024 ) )
#define configMAX_TASK_NAME_LEN                  ( 12 )
#define configUSE_16_BIT_TICKS                   0
#define configIDLE_SHOULD_YIELD                  0

/* Co-routine configuration */
#define configUSE_CO_ROUTINES                    0
#define configMAX_CO_ROUTINE_PRIORITIES          ( 2 )

/* Mutex and semaphore configuration */
#define configUSE_MUTEXES                        1
#define configUSE_RECURSIVE_MUTEXES              1
#define configUSE_QUEUE_SETS                     1
#define configUSE_COUNTING_SEMAPHORES            1

/* Stack and memory allocation configuration */
#define configCHECK_FOR_STACK_OVERFLOW           2
#define configUSE_MALLOC_FAILED_HOOK             0

/* Task and queue configuration */
#define configMAX_PRIORITIES                     ( 9UL )
#define configQUEUE_REGISTRY_SIZE                10

/* Timer-related defines: Balanced priorities for stable operation. */
#define configUSE_TIMERS                         0
#define configTIMER_TASK_PRIORITY                (configMAX_PRIORITIES - 4 )
#define configTIMER_QUEUE_LENGTH                 20
#define configTIMER_TASK_STACK_DEPTH             (configMINIMAL_STACK_SIZE * 2)

/* Task notifications configuration */
#define configUSE_TASK_NOTIFICATIONS             1
#define configTASK_NOTIFICATION_ARRAY_ENTRIES    3

/* Include API functions for required functionality. */
#define INCLUDE_vTaskPrioritySet                 1
#define INCLUDE_uxTaskPriorityGet                1
#define INCLUDE_vTaskDelete                      1
#define INCLUDE_vTaskCleanUpResources            0
#define INCLUDE_vTaskSuspend                     1
#define INCLUDE_vTaskDelayUntil                  1
#define INCLUDE_vTaskDelay                       1
#define INCLUDE_uxTaskGetStackHighWaterMark      1
#define INCLUDE_xTaskGetSchedulerState           1
#define INCLUDE_xTimerGetTimerDaemonTaskHandle   1
#define INCLUDE_xTaskGetIdleTaskHandle           1
#define INCLUDE_xSemaphoreGetMutexHolder         1
#define INCLUDE_eTaskGetState                    1
#define INCLUDE_xTimerPendFunctionCall           1
#define INCLUDE_xTaskAbortDelay                  1
#define INCLUDE_xTaskGetHandle                   1

/* Statistics formatting configuration */
#define configUSE_STATS_FORMATTING_FUNCTIONS     0

/* Interrupt priority configuration */
#define configKERNEL_INTERRUPT_PRIORITY          ( 255 )  /* Lowest priority for kernel interrupt */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY     ( 5 << (8 - __NVIC_PRIO_BITS) )  /* NVIC priority level */

#ifndef __IASMARM__
    #define configASSERT( x ) if( ( x ) == 0 ) while(1);
#endif

/* Task priorities for queues and timers */
#define intqHIGHER_PRIORITY      ( configMAX_PRIORITIES - 5 ) /* High priority for important inter-task queues */
#define bktPRIMARY_PRIORITY      ( configMAX_PRIORITIES - 3 ) /* Primary background tasks */
#define bktSECONDARY_PRIORITY    ( configMAX_PRIORITIES - 4 ) /* Secondary background tasks */

/* Port optimized task selection configuration */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION         1

/* Additional test configuration */
#define configRUN_ADDITIONAL_TESTS                      1
#define configSTREAM_BUFFER_TRIGGER_LEVEL_TEST_MARGIN   4

/* Backward compatibility configuration */
#define configENABLE_BACKWARD_COMPATIBILITY             0

/* Memory Protection Unit (MPU) configuration */
#define configENABLE_MPU                                1
#define configUSE_MPU                                   1
#define configTOTAL_MPU_REGIONS                         8     /* Match your MPU hardware */
#define configUSE_MPU_WRAPPERS                          1
#define configUSE_MPU_WRAPPERS_V1                       0
// #define portUSING_MPU_WRAPPERS                          1

/* Kernel object pool configuration */
#define configPROTECTED_KERNEL_OBJECT_POOL_SIZE         150

/* System call stack configuration */
#define configSYSTEM_CALL_STACK_SIZE                    128   /* Adjust the size as needed */
#define configENFORCE_SYSTEM_CALLS_FROM_KERNEL_ONLY     1

/* Dynamic memory allocation configuration */
#define configSUPPORT_DYNAMIC_ALLOCATION                1

/* Static memory allocation configuration */
#define configSUPPORT_STATIC_ALLOCATION                 0
#define configUSE_STATIC_ALLOCATION                     0
// #define portHAS_STACK_OVERFLOW_CHECKING                 1

/* Privilege level configuration */
#define configALLOW_UNPRIVILEGED_CRITICAL_SECTIONS      0

/* Access Control List Configuration */
#define configENABLE_ACCESS_CONTROL_LIST                1

#endif /* FREERTOS_CONFIG_H */
