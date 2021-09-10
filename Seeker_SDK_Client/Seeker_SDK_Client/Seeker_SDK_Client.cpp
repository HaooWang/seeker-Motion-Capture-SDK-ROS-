// Seeker_SDK_Client.cpp : 定义控制台应用程序的入口点。

#include"stdafx.h"
#include "SeekerSDK.h"
#include <windows.h>
#include <stdio.h> 
#include <conio.h>

#pragma comment(lib, "SeekerSDK.lib")

void MyErrorMsgHandler(int iLevel, char *szMsg)
{
    char *szLevel;

    if (iLevel == VL_Debug)
    {
        szLevel = "Debug";
    }
    else
    if (iLevel == VL_Info)
    {
        szLevel = "Info";
    }
    else
    if (iLevel == VL_Warning)
    {
        szLevel = "Warning";
    }
    else
    if (iLevel == VL_Error)
    {
        szLevel = "Error";
    }

    printf("    %s: %s\n", szLevel, szMsg);
}

//------------------------------------------------- time code added by frank.shao
char TimeCodeMode[5][20]= { "None", "SMPTE", "Film", "EBU", "SystemClock" };

int _tmain(int argc, _TCHAR* argv[])
{
	sHostInfo Seeker_HostInfo;
	int retval1;
	char userKey;
	printf("开启Seeker_SDK_Client程序\n\n");
	//int retval;
	//初始化网络  
	Seeker_SetErrorMsgHandlerFunc(MyErrorMsgHandler);
	int retval=Seeker_Initialize("10.1.1.198","10.1.1.198"); 	//参数1：客户端IP（本机IP）；参数2：服务器IP（运行动捕软件的电脑IP）
	if (retval != RC_Okay)
	{
		printf("未能成功初始化以太网\n\n");
	}
	else
	{
		printf("成功初始化以太网\n\n");
	}
	
    //获取主机信息	
	retval1 = Seeker_GetHostInfo(&Seeker_HostInfo);
	if (retval1 != RC_Okay|| !Seeker_HostInfo.bFoundHost)
	{
		printf("未能成功找到服务端.\n\n");
	}
	else
	{
		printf("成功找到服务端.\n\n");
	}
	
	printf("输入【c/C】接收服务器动捕数据.\n\n");
	userKey=getchar();

	//----------------------------------- prevent frame No. repeat conflict
	int preFrmNo = 0;
	int curFrmNo = 0;
			
	if (userKey=='c'||userKey=='C')
	{
		sFrameOfData* pFrameOfData=NULL; 
		sFrameOfData  CapFrameOfData;       //动捕数据
		while(1)
		{
			memset(&CapFrameOfData, 0, sizeof(sFrameOfData));   //清零
			pFrameOfData = Seeker_GetCurrentFrame();            //获取当前帧动捕数据

			if (pFrameOfData !=NULL)
			{
				retval = Seeker_CopyFrame(pFrameOfData,&CapFrameOfData);
				curFrmNo = CapFrameOfData.iFrame;
				//-------------------- remove duplicate frames
				if (curFrmNo != preFrmNo){
					preFrmNo = curFrmNo;

					//----------------------------------------------- time code 
					printf( "FrameNO: %d ", CapFrameOfData.iFrame);  //print current frame NO.
					printf("TimeCode, %s : ", TimeCodeMode[(CapFrameOfData.TimeCode.iStandard)] ); //print timecode stardard
					printf( "%02d:%02d:%02d - %03d\n", (CapFrameOfData.TimeCode.iHours),
													   (CapFrameOfData.TimeCode.iMinutes),
													   (CapFrameOfData.TimeCode.iSeconds),
													   (CapFrameOfData.TimeCode.iFrames) );
					
					printf( "nMarkerset=%d\n", CapFrameOfData.nBodies);  //CapFrameOfData.nBodies：动捕数据Markerset（骨架）的数量

					for (int iBody=0 ; iBody<CapFrameOfData.nBodies ; iBody++)
					{
						sBodyData *Body = &CapFrameOfData.BodyData[iBody]; //当前帧动捕数据中，提取第iBody个Markerset的数据
						printf( "Markerset %d: %s\n", iBody+1, Body->szName);  //动捕数据第iBody个Markerset的名字
						printf( "{\n");
					
						// Markers
						printf( "\tnMarkers=%d\n", Body->nMarkers);  //动捕数据第iBody个Markerset（骨架），包含的Marker点数量
						printf( "\t{\n");
						for (int i=0 ; i<Body->nMarkers ; i++)  //输出包含的Marker点坐标
						{
							printf("\t\tMarker %d：X:%f Y:%f Z:%f\n",
								i+1,
								Body->Markers[i][0],
								Body->Markers[i][1],
								Body->Markers[i][2]);
						}
						printf( "\t}\n");
							
						// Segments
						printf("\tnSegments=%d\n", Body->nSegments);  //动捕数据第iBody个Markerset（骨架），包含的Segments（骨骼）数量
						printf("\t{\n");
						for (int i=0 ; i<Body->nSegments ; i++) //输出包含的Segments（骨骼）的六个自由度信息（Tx,Ty,Tz,Rx,Ry,Rz）+长度（Length）
						{
							printf("\t\tSegment %d：Tx:%lf Ty:%lf Tz:%lf Rx:%lf Ry:%lf Rz:%lf Length:%lf\n",
								i+1,
								Body->Segments[i][0],
								Body->Segments[i][1],
								Body->Segments[i][2],
								Body->Segments[i][3],
								Body->Segments[i][4],
								Body->Segments[i][5],
								Body->Segments[i][6]);
						}
						printf( "\t}\n");
					
						printf("}\n");  //第iBody个Markerset的数据输出完成
					}

					// Unidentified Markers
					printf("nUnidentifiedMarkers=%d\n", CapFrameOfData.nUnidentifiedMarkers);  //动捕数据中，未识别的Marker点数量
					printf( "{\n");
					for (int i=0 ; i<CapFrameOfData.nUnidentifiedMarkers ; i++) //输出未识别的Marker点坐标
					{
						printf( "\tUnidentifiedMarkers %d：X:%f Y:%f Z:%f\n",
							i+1,
							CapFrameOfData.UnidentifiedMarkers[i][0],
							CapFrameOfData.UnidentifiedMarkers[i][1],
							CapFrameOfData.UnidentifiedMarkers[i][2]);
					}
					printf( "}\n");  
				}
			}
			else{
				printf("未获取到当前帧动捕数据.\n\n");
			}

//			Sleep(10);   //降低CPU负荷
//控制台信息打印多，降低CPU负荷；减少控制台信息打印，也会降低CPU负荷

			//printf( "\n");
			//printf( "\n");
			Seeker_FreeFrame(&CapFrameOfData);
		}
	} 
	printf("与服务端断开连接.\n\n");
	Seeker_Exit();

	return 0;
}

