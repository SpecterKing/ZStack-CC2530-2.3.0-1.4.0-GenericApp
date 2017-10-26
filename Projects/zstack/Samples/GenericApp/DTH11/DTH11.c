/**************************************************************************************************
  Filename   : DTH11.c
  Author     : Specter
  Email      : 330945035@qq.com
  Description: DTH11驱动函数，头文件DTH11.h
               本程序是根据CC2530来写的，调用部分在zstack中,
               其他平台可能需要适配
**************************************************************************************************/
#include"DTH11.h"
#include"OnBoard.h"   //用到这个函数里的延时函数MicroWait()了

uint8 read_byte();
/*
 * 这个是用来初始化一下端口的，这个要在初始化的时候调用一下
 */
void DTH11_Init(void)
{
  clrbit(DTH11_DATA_SEL,DTH11_DATA_PIN_NU);  //将我们指定的IO口SEL位清零，设置为普通IO口功能
  setbit(DTH11_DATA_INP,DTH11_DATA_PIN_NU);  //将我们指定的IO口INP位置1，将IO口设置为高阻态
}

/*
 * 发送启动信号，因为DTH11启动信号要求18ms以上的低电平，如果一直延时效率太低，
 * 所以先发一个启动信号，等时间到了之后再开始读取数据
 * 注意：这个函数并不会阻塞，调用之后要在指定的时间之（最少18ms,可以延时20ms左右）后再开始读取数据
 */
void DTH11_Start_Signal(void)
{
  //DTH11的开始信号是主机发送至少18ms的低电平，然后拉高20-40us，然后等待DTH11响应
  setbit(DTH11_DATA_DIR,DTH11_DATA_PIN_NU);  //将数据IO口设置为输出
  DTH11_DATA_PIN=0 ;  //主机把电平拉低
  //此时本函数就结束了，调用函数要在18ms之后调用读取函数DTH11_Read_Data
}

/*
 * 读取数据的函数，首先要调用DTH11_Start_Signal，18ms之后才能调用这个函数
 * 将dth11_data结构体变量地址传进去之后会填充数据
 * 如果出错则会将温湿度数据都置为0
 */
void DTH11_Read_Data(dth11_data *resulet)
{
  uint8 byte1,byte2,byte3,byte4,byte5;
  if(DTH11_DISABLE_IRQ){
    EA=0;  //关中断，避免数据读取被打断
  }
  
  DTH11_DATA_PIN = 1 ;  //主机把电平拉高
  MicroWait(32);  //延时20-40us
  clrbit(DTH11_DATA_DIR,DTH11_DATA_PIN_NU);  //将数据IO口设置为输入
  while(DTH11_DATA_PIN);    //等待DTH11把电平拉低（拉低40-50us）
  while(!DTH11_DATA_PIN);    //等待DTH11把电平拉高（拉高40-50us）
  //此时开始起始信号就读取完毕了，下面开始正式读取数据
  
  byte1 = read_byte();  //读取一个byte的数据(湿度的整数部分)
  byte2 = read_byte();  //读取一个byte的数据(湿度的小数部分，这里是估读的不是真实测量的数据)
  byte3 = read_byte();  //读取一个byte的数据(温度的整数部分)
  byte4 = read_byte();  //读取一个byte的数据(温度的小数部分，这里是估读的不是真实测量的数据)
  byte5 = read_byte();  //读取一个byte的数据(这个是校验位)
  uint8 byte6;
  byte6 = byte1 + byte2 + byte3 + byte4;
  setbit(DTH11_DATA_DIR,DTH11_DATA_PIN_NU);  //将数据IO口设置为输出
  if(DTH11_DISABLE_IRQ){
    EA=1;  //开中断
  }
  if(byte5 == (byte1 + byte2 + byte3 + byte4) ){
    //这里表示数据读取正确
    resulet->humidity = byte1;     //填充湿度数据
    resulet->temperature = byte3;  //填充温度数据
  }else{
    //这里表示数据读取错误，将温湿度都设置为0
    resulet->humidity = 0;
    resulet->temperature = 0;
  }
}

/*
 * 从DTH11读取一个byte的数据
 */
uint8 read_byte()
{
  uint8 tmp_byte=0,i=0;
  for(i=0;i<8;i++){
    tmp_byte = (tmp_byte<<1);  //把数据左移一位，空出最低位，用来存储本次读取到的bit，第一次的时候因为全是0，所以移位没有任何影响
    while(DTH11_DATA_PIN);    //等待DTH11把电平拉低（拉低12-14us）,这个是一个数据开始传送的标志
    while(!DTH11_DATA_PIN);   //等待DTH11把电平拉高（拉高40-50us），这个是数据位，26-28us表示0；116-118us表示1
    /* 如果第一个数据是0，那么下一个数据会在38-42us后将数据线拉高发送下一帧数据，所以我们取28us-38us的中间值33us作为监测点，为了给下面的运算留一些时间，我们延时32us
     * 如果数据位拉高33us后此时数据线为低电平则数据为0（此时这个电平在下一个数据位开始的标志位处）
     * 如果数据位拉高33us后此时数据线为高电平则数据为1（此时这个电平是上一个数据位的数据1还没有发送完毕）
     */
    MicroWait(32);
    tmp_byte += DTH11_DATA_PIN;  //将本次读取的的bit填充到数据中
  }
  return tmp_byte;
}
