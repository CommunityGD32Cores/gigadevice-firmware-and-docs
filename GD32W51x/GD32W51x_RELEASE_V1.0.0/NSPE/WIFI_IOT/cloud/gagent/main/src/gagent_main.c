#include "stdio.h"
#include "gagent_typedef.h"
#include "gagent.h"
#include "wrapper_os.h"

extern pgcontext pgContextData;

void gagent_main_thread(void *parm)
{
	GAgent_Printf(GAGENT_INFO,"GAgent_Init\r\n");
	GAgent_Init( &pgContextData );
	GAgent_Printf(GAGENT_INFO,"GAgent_dumpInfo\r\n");
	GAgent_dumpInfo( pgContextData );
#ifdef CONFIG_GAGENT_CLOUD_TASK
	if(xTaskCreate(GAgent_Cloud_Task_Handle, ((const char*)"gagent_cloud"), 1024, (void*)pgContextData, tskIDLE_PRIORITY + 4, NULL) != pdPASS)
		GAgent_Printf(GAGENT_INFO, "%s xTaskCreate(GAgent_CloudConfigDataHandle) failed.", __FUNCTION__);
#endif
	while(1)
	{
		GAgent_Tick( pgContextData );
		GAgent_SelectFd( pgContextData,1,0 );

		GAgent_Lan_Handle( pgContextData, pgContextData->rtinfo.Rxbuf , GAGENT_BUF_LEN );
		GAgent_Local_Handle( pgContextData, pgContextData->rtinfo.Rxbuf, GAGENT_BUF_LEN );
#ifdef CONFIG_GAGENT_CLOUD_TASK
		xSemaphoreGive(pgContextData->rtinfo.waninfo.cloud_config_sema);
#else
		GAgent_Cloud_Handle( pgContextData, pgContextData->rtinfo.Rxbuf, GAGENT_BUF_LEN );
#endif
	}

}

void cmd_gagent_test(int argc, char **argv)
{
	void *handle;

	handle = sys_task_create(&gagent_task_tcb, (const u8 *)"gagent_task", &gagent_task_stk[0],
					GAGENT_STACK_SIZE, 0, GAGENT_TASK_PRIO,
					(task_func_t)gagent_main_thread, NULL);
	if (handle == NULL) {
		GAgent_Printf(GAGENT_ERROR, "\n\rGAgent: create gagent task failed.\n");
	}
}

