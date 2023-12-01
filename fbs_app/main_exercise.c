 /* FreeRTOS Kernel V10.1.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/******************************************************************************
 * NOTE: Windows will not be running the FreeRTOS demo threads continuously, so
 * do not expect to get real time behaviour from the FreeRTOS Windows port, or
 * this demo application.  Also, the timing information in the FreeRTOS+Trace
 * logs have no meaningful units.  See the documentation page for the Windows
 * port for further information:
 * http://www.freertos.org/FreeRTOS-Windows-Simulator-Emulator-for-Visual-Studio-and-Eclipse-MingW.html
 *
 ******************************************************************************
 *
 * NOTE:  Console input and output relies on Windows system calls, which can
 * interfere with the execution of the FreeRTOS Windows port.  This demo only
 * uses Windows system call occasionally.  Heavier use of Windows system calls
 * can crash the port.
 */

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#define PRIORITY_SCHEDULER_TASK (tskIDLE_PRIORITY + 2)
#define PRIORITY_WORKER_TASK (tskIDLE_PRIORITY + 1)
#define schedulerTask_MS 120

//void WellBehavedWorkerTask(void *pvParameters);
void WellBehavedWorkerTask00(void *pvParameters);
void WellBehavedWorkerTask01(void *pvParameters);
void WellBehavedWorkerTask02(void *pvParameters);
void WellBehavedWorkerTask03(void *pvParameters);
void WellBehavedWorkerTask04(void *pvParameters);
void MisBehavingWorkerTask(void *pvParameters);
void SchedulerTask(void *pvParameters);
float fsafeDivide(float divisor);

// struct frame_t
// {
//     uint8_t frameNum;
//     uint16_t frameStart;
//     uint16_t frameStop;
//     uint16_t numTasksInFrame;
//     uint16_t *tasks;
// };

struct frame_t
{
    uint8_t frameNum;
    uint16_t frameStart;
    uint16_t frameStop;
    uint16_t numTasksInFrame;
    uint16_t *tasks;
    boolean deleteTask5; // flag to track misbehaving task [Given: Task 5] Also, irst Run Indicator
    uint16_t currentTask; //Stores task under execution

};

struct frame_t frameList[5];

SemaphoreHandle_t xSemaphore;

/*Task Handles*/
// TaskHandle_t workerTaskHandle;
TaskHandle_t workerTaskHandle00; //Handle for Well behaved Tasks
TaskHandle_t MBworkerTaskHandle; // Handle for misbehaving Task [Given: Task 5]

static boolean firstEntry = TRUE;

/*Frame List Initialization WRT the table given in the problem*/
void initializeFrameList(struct frame_t frameList[]);


void main_exercise(void)
{
    printf("Before task creation\n");
    printf("Initializing Frame List\n");
    initializeFrameList(frameList);
    printf("Frame List Initialized!\n");

    xSemaphore = xSemaphoreCreateBinary();

    if (xSemaphore == NULL)
    {
        printf("Failed to create semaphore\n");
        vTaskEndScheduler();
    }

    if (xTaskCreate(SchedulerTask, "SchedulerTask", configMINIMAL_STACK_SIZE, NULL, PRIORITY_SCHEDULER_TASK, NULL) != pdPASS)
    {
        printf("Failed to create the scheduler task\n");
    }

    if (xTaskCreate(WellBehavedWorkerTask00, "WorkerTask00", configMINIMAL_STACK_SIZE, NULL, PRIORITY_WORKER_TASK, &workerTaskHandle00) != pdPASS) // Well Behaved
    {
        printf("Failed to create the worker task\n");
        exit(1);
    }

    if (xTaskCreate(WellBehavedWorkerTask01, "WorkerTask01", configMINIMAL_STACK_SIZE, NULL, PRIORITY_WORKER_TASK, &workerTaskHandle00) != pdPASS) // Well Behaved
    {
        printf("Failed to create the worker task\n");
        exit(1);
    }

    if (xTaskCreate(WellBehavedWorkerTask02, "WorkerTask02", configMINIMAL_STACK_SIZE, NULL, PRIORITY_WORKER_TASK, &workerTaskHandle00) != pdPASS) // Well Behaved
    {
        printf("Failed to create the worker task\n");
        exit(1);
    }

    if (xTaskCreate(WellBehavedWorkerTask03, "WorkerTask03", configMINIMAL_STACK_SIZE, NULL, PRIORITY_WORKER_TASK, &workerTaskHandle00) != pdPASS) // Well Behaved
    {
        printf("Failed to create the worker task\n");
        exit(1);
    }

    if (xTaskCreate(WellBehavedWorkerTask04, "WorkerTask04", configMINIMAL_STACK_SIZE, NULL, PRIORITY_WORKER_TASK, &workerTaskHandle00) != pdPASS) // Well Behaved
    {
        printf("Failed to create the worker task\n");
        exit(1);
    }

    /* Misbehaving Task [Given: Task 5]*/
    if (xTaskCreate(MisBehavingWorkerTask, "MBWorkerTask", configMINIMAL_STACK_SIZE, NULL, PRIORITY_WORKER_TASK, &MBworkerTaskHandle) != pdPASS) // MissBehaved
    {
        printf("Failed to create the worker task\n");
        exit(1);
    }

    vTaskStartScheduler();
    printf("After starting the scheduler (this point should not be reached)\n");

    for (;;)
    {
        printf("Inside infinite loop (this point should not be reached)\n");
    }
}

void SchedulerTask(void *pvParameters)
{
    printf("SchedulerTask is running\n");

    uint8_t currentFrameIndex = 0;

    for (;;)
    {
        struct frame_t *currentFrame = &frameList[currentFrameIndex];

        printf("SchedulerTask scheduling tasks for frame %d\n", currentFrame->frameNum);

        for (uint16_t j = 0; j < currentFrame->numTasksInFrame; j++)
        {
            currentFrame->currentTask = currentFrame->tasks[j];

            if (xTaskNotify(workerTaskHandle00, currentFrame->frameNum, eSetValueWithOverwrite) != pdPASS)
            {
                printf("Failed to give notification to WorkerTask\n");
                exit(1);
            }

            if (j < currentFrame->numTasksInFrame - 1)
            {
                vTaskDelay(pdMS_TO_TICKS(60));
            }
        }

        if (currentFrame->frameNum == 0 && !currentFrame->deleteTask5 && firstEntry)
        {
            if (xTaskNotify(MBworkerTaskHandle, currentFrame->frameNum, eSetValueWithOverwrite) != pdPASS)
            {
                printf("Failed to give notification to WorkerTask\n");
                exit(1);
            }
            currentFrame->deleteTask5 = TRUE;
            firstEntry = FALSE;
        }

        vTaskDelay(schedulerTask_MS);

        // Printing message when the scheduler runs for 120ms
        printf("SchedulerTask ran for 120ms\n");
        printf("Checking Overun...\n");
        // Removal from list            // Hardcoded
        printf("%d\n",currentFrame->deleteTask5);
        if (currentFrame->deleteTask5)
        {   
            printf("Task %d has overrun, Deleting...\n",currentFrame->currentTask);
            // Excluding Task 5 from the tasks array
            currentFrame->numTasksInFrame = 5;
            currentFrame->deleteTask5 = FALSE; // Reset
        }

        // Printing the remaining tasks in the frame
        printf("Remaining tasks in frame %d: ", currentFrame->frameNum);
        for (uint16_t k = 0; k < currentFrame->numTasksInFrame; k++)
        {
            printf("%d ", currentFrame->tasks[k]);
        }
        printf("\n");
        // Moving to the next frame 
        currentFrameIndex = (currentFrameIndex + 1) % (sizeof(frameList) / sizeof(frameList[0]));
    }
}

void WellBehavedWorkerTask00(void *pvParameters)
{
    uint8_t loopControlFrame;

    for (;;)
    {
        if (xTaskNotifyWait(0, 0, &loopControlFrame, portMAX_DELAY) == pdTRUE)
        {
            uint8_t frameIndex = (uint8_t)loopControlFrame;
            struct frame_t *currentFrame = &frameList[frameIndex];

            printf("Running Task: %d\n", currentFrame->currentTask);

            for(uint32_t count=0;count<=1000000;count++)
            {
                if(count == 1000000)
                {
                    //printf("counted for %d, ", count);
                }
            }
        }
        
    }
}
void WellBehavedWorkerTask01(void *pvParameters)
{
    uint8_t loopControlFrame;

    for (;;)
    {
        if (xTaskNotifyWait(0, 0, &loopControlFrame, portMAX_DELAY) == pdTRUE)
        {
            uint8_t frameIndex = (uint8_t)loopControlFrame;
            struct frame_t *currentFrame = &frameList[frameIndex];

            printf("Running Task: %d\n", currentFrame->currentTask);

            for(uint32_t count=0;count<=1000000;count++)
            {
                if(count == 1000000)
                {
                    //printf("counted for %d, ", count);
                }
            }
        }
        
    }
}
void WellBehavedWorkerTask02(void *pvParameters)
{
    uint8_t loopControlFrame;

    for (;;)
    {
        if (xTaskNotifyWait(0, 0, &loopControlFrame, portMAX_DELAY) == pdTRUE)
        {
            uint8_t frameIndex = (uint8_t)loopControlFrame;
            struct frame_t *currentFrame = &frameList[frameIndex];

            printf("Running Task: %d\n", currentFrame->currentTask);

            for(uint32_t count=0;count<=1000000;count++)
            {
                if(count == 1000000)
                {
                    //printf("counted for %d, ", count);
                }
            }
        }
        
    }
}
void WellBehavedWorkerTask03(void *pvParameters)
{
    uint8_t loopControlFrame;

    for (;;)
    {
        if (xTaskNotifyWait(0, 0, &loopControlFrame, portMAX_DELAY) == pdTRUE)
        {
            uint8_t frameIndex = (uint8_t)loopControlFrame;
            struct frame_t *currentFrame = &frameList[frameIndex];

            printf("Running Task: %d\n", currentFrame->currentTask);

            for(uint32_t count=0;count<=1000000;count++)
            {
                if(count == 1000000)
                {
                    //printf("counted for %d, ", count);
                }
            }
        }
        
    }
}
void WellBehavedWorkerTask04(void *pvParameters)
{
    uint8_t loopControlFrame;

    for (;;)
    {
        if (xTaskNotifyWait(0, 0, &loopControlFrame, portMAX_DELAY) == pdTRUE)
        {
            uint8_t frameIndex = (uint8_t)loopControlFrame;
            struct frame_t *currentFrame = &frameList[frameIndex];

            printf("Running Task: %d\n", currentFrame->currentTask);

            for(uint32_t count=0;count<=1000000;count++)
            {
                if(count == 1000000)
                {
                    //printf("counted for %d, ", count);
                }
            }
        }
        
    }
}

void MisBehavingWorkerTask(void *pvParameters)
{
    uint8_t currentFrameNum;
    uint8_t currentFrame; // Incoming

    for (;;)
    {
        if (xTaskNotifyWait(0, 0, &currentFrame, portMAX_DELAY) == pdTRUE)
        {
            currentFrameNum = (uint8_t)currentFrame;

            // Getting Frame
            struct frame_t *currentFrame = &frameList[currentFrameNum];

            // Introducing a delay to simulate misbehaved task
            for (uint32_t count = 0; count < 2000000; count++)
            {
                // Intentional delay causing overrun
            }
            vTaskDelete(NULL);
            break;
        }
    }
}


void initializeFrameList(struct frame_t frameList[])
{
    uint8_t numFrames = 5U;
    uint16_t frameStartTimes[] = {0U, 120U, 240U, 360U, 480U};
    uint16_t frameStopTimes[] = {120U, 240U, 360U, 480U, 600U};
    float numTasksInFrames[] = {6U, 0U, 3U, 2U, 3U};
    uint16_t *tasksInFrames[] = {(uint16_t[]){0U, 1U, 2U, 3U, 4U, 5U}, NULL, (uint16_t[]){0U, 1U, 4U}, (uint16_t[]){2U, 3U}, (uint16_t[]){0U, 1U, 4U}};

    for (uint8_t i = 0; i < numFrames; ++i)
    {
        frameList[i].frameNum = i;
        frameList[i].frameStart = frameStartTimes[i];
        frameList[i].frameStop = frameStopTimes[i];
        frameList[i].numTasksInFrame = numTasksInFrames[i];

        frameList[i].tasks = malloc(numTasksInFrames[i] * sizeof(uint16_t));

        if (frameList[i].tasks == NULL)
        {
            printf("Failed to allocate memory for tasks in frame %d\n", i);
            for (uint8_t j = 0; j < i; ++j)
            {
                free(frameList[j].tasks);
            }
            exit(1);
        }

        for (uint16_t j = 0; j < numTasksInFrames[i]; ++j)
        {
            frameList[i].tasks[j] = tasksInFrames[i][j];
        }
    }
}
/*Safe Divide for Frames with no task!*/
float fsafeDivide(float divisor)
{
    const float epsilon = 0.0001;

    if (fabs(divisor) < epsilon)
    {
        return (divisor < 0) ? -epsilon : epsilon;
    }
    else
    {
        return divisor;
    }
}
