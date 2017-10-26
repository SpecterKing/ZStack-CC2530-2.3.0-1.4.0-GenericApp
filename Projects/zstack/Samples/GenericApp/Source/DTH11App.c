/**************************************************************************************************
  Filename   : DTH11App.c
  Author     : Specter
  Email      : 330945035@qq.com
  Description: DTH11驱动OSAL相关代码
               头文件：DTH11App.h
**************************************************************************************************/
#include "DTH11.h"
#include "DTH11App.h"
#include "AF.h"

byte DTH11App_TaskID;  //DTH11OSAL驱动任务的任务ID号。

/*
* DTH11 OSAL驱动任务初始化函数
*/
void DTH11App_Init( byte task_id )
{
  DTH11App_TaskID = task_id;  //保存任务ID号
  DTH11_Init();   //调用硬件初始化函数
  
  osal_start_reload_timer(DTH11App_TaskID,DTH11APP_READ_DATA_EVT,3000);   //启动一个自动重载定时器，每隔3000ms测试一次数据（启动DTH11APP_READ_DATA_EVT事件）
}

UINT16 DTH11App_ProcessEvent( byte task_id, UINT16 events )
{
  afIncomingMSGPacket_t *MSGpkt;
  afDataConfirm_t *afDataConfirm;
 
  // Data Confirmation message fields
  byte sentEP;
  ZStatus_t sentStatus;
  byte sentTransID;       // This should match the value sent
  (void)task_id;  // Intentionally unreferenced parameter
  //以上为固定格式
  
  if ( events & SYS_EVENT_MSG )//判断是否是系统事件，除了系统事件其他都是自定义的  
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( DTH11App_TaskID );//接收消息
    while ( MSGpkt )//一直读取，直到数据读取完毕
    {
      switch ( MSGpkt->hdr.event )//根据MSGpkt->hdr.event判断消息的事件类型
      {
        /*
        //这些消息不是固定的，自己如果新建了一个任务，这些消息实际一个都没有，都要自己定义
        case ZDO_CB_MSG:   // 0xD3 ZDO incoming message callback
          //GenericApp_ProcessZDOMsgs( (zdoIncomingMsg_t *)MSGpkt );
          break;
        */
        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( DTH11App_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  //这个是我们自定义的事件，触发这个事件就会开始读取数据，当数据读取完毕之后就会调用
  if ( events & DTH11APP_READ_DATA_EVT )
  {
    DTH11_Start_Signal();   //发送一个开始信号
    // 开始信号之后要延时一段时间（最少18ms，这里延时了20ms）
    osal_start_timerEx( DTH11App_TaskID,
                        DTH11APP_START_READ_EVT,
                        20);

    // 清除已处理的事件
    return (events ^ DTH11APP_READ_DATA_EVT);
  }

  //这个是我们自定义的事件，这个不用手动触发，它是被DTH11APP_READ_DATA_EVT事件调用的
  if ( events & DTH11APP_START_READ_EVT )
  {
    dth11_data dth11_resulet;
    DTH11_Read_Data(&dth11_resulet);  //开始读取数据（调用DTH11_Start_Signal，18ms之后才能调用这个函数）
    //执行完上面的函数，此时我们就已经读取到数据了，如果数据有误，则温度和湿度都是0.
    if( 0 == dth11_resulet.temperature && 0 == dth11_resulet.humidity)
    {
      ;//如果温湿度都为0，则表示数据有误
    }else{
      ;//否则则表示数据正常
    }
    
    // 清除已处理的事件
    return (events ^ DTH11APP_START_READ_EVT);
  }
  
  // Discard unknown events(去除未知事件，这个只是一个容错的设置，实际我们要清楚自己定义的所有事件)
  return 0;
}