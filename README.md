这个例子是从TI的ZStack-CC2530-2.3.0-1.4.0移植过来的，我们用的是Projects\zstack\Samples\GenericApp这个例子为模板的
我们先将zstack的源码全部拷贝过来，然后删除以下无关的代码
	1、进入ZStack-CC2530-2.3.0-1.4.0-GenericApp目录
	   删除Documents（帮助文档）、Tools（一些工具）、Getting Started Guide - CC2530.pdf（用户手册）
	2、进入ZStack-CC2530-2.3.0-1.4.0-GenericApp\Projects\zstack
	   只保留Samples目录，其他都删除
	3、进入ZStack-CC2530-2.3.0-1.4.0-GenericApp\Projects\zstack\Samples
	   只保留GenericApp目录，其他都删除
	   
注意：
	本git工程没有管理ZStack-CC2530-2.3.0-1.4.0-GenericApp\Projects\zstack\Samples\GenericApp\CC2530DB目录（该目录下保存的是工程相关的文件）

如果想要修改工程的名字参考以下步骤	   
1、进入Projects\zstack\Samples\
   将GenericApp重命名为SDAPP
2、进入SDAPP\Source
   将GenericApp.c、GenericApp.h、OSAL_GenericApp.c这三个文件打开，将里面的GenericApp用SDAPP替换
3、将GenericApp.c、GenericApp.h、OSAL_GenericApp.c这三个文件的名字改成SDAPP.c、SDAPP.h、OSAL_SDAPP.c
4、进入Projects\zstack\Samples\SDAPP\CC2530DB
   将GenericApp.ewd、GenericApp.ewp、GenericApp.eww、GenericApp.ewd.dep这几个文件的GenericApp都重命名为SDAPP
   然后用记事本打开这几个文件，将里面的GenericApp全部替换为SDAPP
   此时工程重命名就完成了