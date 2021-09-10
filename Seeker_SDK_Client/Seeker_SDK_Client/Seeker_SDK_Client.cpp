// Seeker_SDK_Client.cpp : �������̨Ӧ�ó������ڵ㡣

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
	printf("����Seeker_SDK_Client����\n\n");
	//int retval;
	//��ʼ������  
	Seeker_SetErrorMsgHandlerFunc(MyErrorMsgHandler);
	int retval=Seeker_Initialize("10.1.1.198","10.1.1.198"); 	//����1���ͻ���IP������IP��������2��������IP�����ж�������ĵ���IP��
	if (retval != RC_Okay)
	{
		printf("δ�ܳɹ���ʼ����̫��\n\n");
	}
	else
	{
		printf("�ɹ���ʼ����̫��\n\n");
	}
	
    //��ȡ������Ϣ	
	retval1 = Seeker_GetHostInfo(&Seeker_HostInfo);
	if (retval1 != RC_Okay|| !Seeker_HostInfo.bFoundHost)
	{
		printf("δ�ܳɹ��ҵ������.\n\n");
	}
	else
	{
		printf("�ɹ��ҵ������.\n\n");
	}
	
	printf("���롾c/C�����շ�������������.\n\n");
	userKey=getchar();

	//----------------------------------- prevent frame No. repeat conflict
	int preFrmNo = 0;
	int curFrmNo = 0;
			
	if (userKey=='c'||userKey=='C')
	{
		sFrameOfData* pFrameOfData=NULL; 
		sFrameOfData  CapFrameOfData;       //��������
		while(1)
		{
			memset(&CapFrameOfData, 0, sizeof(sFrameOfData));   //����
			pFrameOfData = Seeker_GetCurrentFrame();            //��ȡ��ǰ֡��������

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
					
					printf( "nMarkerset=%d\n", CapFrameOfData.nBodies);  //CapFrameOfData.nBodies����������Markerset���Ǽܣ�������

					for (int iBody=0 ; iBody<CapFrameOfData.nBodies ; iBody++)
					{
						sBodyData *Body = &CapFrameOfData.BodyData[iBody]; //��ǰ֡���������У���ȡ��iBody��Markerset������
						printf( "Markerset %d: %s\n", iBody+1, Body->szName);  //�������ݵ�iBody��Markerset������
						printf( "{\n");
					
						// Markers
						printf( "\tnMarkers=%d\n", Body->nMarkers);  //�������ݵ�iBody��Markerset���Ǽܣ���������Marker������
						printf( "\t{\n");
						for (int i=0 ; i<Body->nMarkers ; i++)  //���������Marker������
						{
							printf("\t\tMarker %d��X:%f Y:%f Z:%f\n",
								i+1,
								Body->Markers[i][0],
								Body->Markers[i][1],
								Body->Markers[i][2]);
						}
						printf( "\t}\n");
							
						// Segments
						printf("\tnSegments=%d\n", Body->nSegments);  //�������ݵ�iBody��Markerset���Ǽܣ���������Segments������������
						printf("\t{\n");
						for (int i=0 ; i<Body->nSegments ; i++) //���������Segments�����������������ɶ���Ϣ��Tx,Ty,Tz,Rx,Ry,Rz��+���ȣ�Length��
						{
							printf("\t\tSegment %d��Tx:%lf Ty:%lf Tz:%lf Rx:%lf Ry:%lf Rz:%lf Length:%lf\n",
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
					
						printf("}\n");  //��iBody��Markerset������������
					}

					// Unidentified Markers
					printf("nUnidentifiedMarkers=%d\n", CapFrameOfData.nUnidentifiedMarkers);  //���������У�δʶ���Marker������
					printf( "{\n");
					for (int i=0 ; i<CapFrameOfData.nUnidentifiedMarkers ; i++) //���δʶ���Marker������
					{
						printf( "\tUnidentifiedMarkers %d��X:%f Y:%f Z:%f\n",
							i+1,
							CapFrameOfData.UnidentifiedMarkers[i][0],
							CapFrameOfData.UnidentifiedMarkers[i][1],
							CapFrameOfData.UnidentifiedMarkers[i][2]);
					}
					printf( "}\n");  
				}
			}
			else{
				printf("δ��ȡ����ǰ֡��������.\n\n");
			}

//			Sleep(10);   //����CPU����
//����̨��Ϣ��ӡ�࣬����CPU���ɣ����ٿ���̨��Ϣ��ӡ��Ҳ�ή��CPU����

			//printf( "\n");
			//printf( "\n");
			Seeker_FreeFrame(&CapFrameOfData);
		}
	} 
	printf("�����˶Ͽ�����.\n\n");
	Seeker_Exit();

	return 0;
}

