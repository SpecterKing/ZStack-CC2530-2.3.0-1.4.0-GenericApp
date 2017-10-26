/**************************************************************************************************
  Filename   : DTH11App.c
  Author     : Specter
  Email      : 330945035@qq.com
  Description: DTH11����OSAL��ش���
               ͷ�ļ���DTH11App.h
**************************************************************************************************/
#include "DTH11.h"
#include "DTH11App.h"
#include "AF.h"

byte DTH11App_TaskID;  //DTH11OSAL�������������ID�š�

/*
* DTH11 OSAL���������ʼ������
*/
void DTH11App_Init( byte task_id )
{
  DTH11App_TaskID = task_id;  //��������ID��
  DTH11_Init();   //����Ӳ����ʼ������
  
  osal_start_reload_timer(DTH11App_TaskID,DTH11APP_READ_DATA_EVT,3000);   //����һ���Զ����ض�ʱ����ÿ��3000ms����һ�����ݣ�����DTH11APP_READ_DATA_EVT�¼���
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
  //����Ϊ�̶���ʽ
  
  if ( events & SYS_EVENT_MSG )//�ж��Ƿ���ϵͳ�¼�������ϵͳ�¼����������Զ����  
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( DTH11App_TaskID );//������Ϣ
    while ( MSGpkt )//һֱ��ȡ��ֱ�����ݶ�ȡ���
    {
      switch ( MSGpkt->hdr.event )//����MSGpkt->hdr.event�ж���Ϣ���¼�����
      {
        /*
        //��Щ��Ϣ���ǹ̶��ģ��Լ�����½���һ��������Щ��Ϣʵ��һ����û�У���Ҫ�Լ�����
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

  //����������Զ�����¼�����������¼��ͻῪʼ��ȡ���ݣ������ݶ�ȡ���֮��ͻ����
  if ( events & DTH11APP_READ_DATA_EVT )
  {
    DTH11_Start_Signal();   //����һ����ʼ�ź�
    // ��ʼ�ź�֮��Ҫ��ʱһ��ʱ�䣨����18ms��������ʱ��20ms��
    osal_start_timerEx( DTH11App_TaskID,
                        DTH11APP_START_READ_EVT,
                        20);

    // ����Ѵ�����¼�
    return (events ^ DTH11APP_READ_DATA_EVT);
  }

  //����������Զ�����¼�����������ֶ����������Ǳ�DTH11APP_READ_DATA_EVT�¼����õ�
  if ( events & DTH11APP_START_READ_EVT )
  {
    dth11_data dth11_resulet;
    DTH11_Read_Data(&dth11_resulet);  //��ʼ��ȡ���ݣ�����DTH11_Start_Signal��18ms֮����ܵ������������
    //ִ��������ĺ�������ʱ���Ǿ��Ѿ���ȡ�������ˣ���������������¶Ⱥ�ʪ�ȶ���0.
    if( 0 == dth11_resulet.temperature && 0 == dth11_resulet.humidity)
    {
      ;//�����ʪ�ȶ�Ϊ0�����ʾ��������
    }else{
      ;//�������ʾ��������
    }
    
    // ����Ѵ�����¼�
    return (events ^ DTH11APP_START_READ_EVT);
  }
  
  // Discard unknown events(ȥ��δ֪�¼������ֻ��һ���ݴ�����ã�ʵ������Ҫ����Լ�����������¼�)
  return 0;
}