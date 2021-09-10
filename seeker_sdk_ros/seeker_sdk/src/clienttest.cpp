/*=========================================================
 // 
 // File: ClientTest.cpp
 //
 =============================================================================*/

#include <string.h>
#include <stdio.h>
#include "ros/ros.h"
#include <unistd.h>
#include <signal.h>
#include "seeker_two/SeekerSDK.h"
#include "seeker_two/Set.h"
#include "seeker_two/Acce.h"
#include "seeker_two/Velo.h"
#include <float.h>
#include <iostream>
using namespace std;



void MyErrorMsgHandler(int iLevel, char *szMsg)
{
	const char *szLevel = NULL;

	if (iLevel == VL_Debug) {
		szLevel = "Debug";
	} else if (iLevel == VL_Info) {
		szLevel = "Info";
	} else if (iLevel == VL_Warning) {
		szLevel = "Warning";
	} else if (iLevel == VL_Error) {
		szLevel = "Error";
	}
	printf("    %s: %s\n", szLevel, szMsg);
}
bool loop=1;

void MySigintHandler(int sig){
	printf("\n****** Seeker_Exit ******\n");
	Seeker_Exit();
	ROS_INFO("Forced ShutDown!");
	ros::shutdown();
	loop =0;
}

int main(int argc, char* argv[])
{
	int retval = RC_Okay;
	int userKey;

	printf("Start Nokov_SDK_Client\n\n");
	Seeker_SetVerbosityLevel(VL_Info);
	//Seeker_SetErrorMsgHandlerFunc(MyErrorMsgHandler);  
	//Seeker_SetSeekerPort(1050);
	//Seeker_SetMultiCastPort(1060);
	printf("****** Seeker_Initialize ******\n");
	retval = Seeker_Initialize("10.1.1.194","10.1.1.198");
	if (retval != RC_Okay)
	{
		printf("****** Seeker_Initialize Failed ******\n");
		return 0;
	}

	printf("Please input char 'c' or 'C' to receive motion data from Seeker...\n\n");
	userKey=getchar();
	ros::init(argc,argv,"seeker_node");
	ros::NodeHandle n;
	ros::Rate r(70);
	signal(SIGINT,MySigintHandler);
	ros::Publisher pos = n.advertise<seeker_two::Set>("/Data",1);


	if (userKey=='c'||userKey=='C')
	{

		sFrameOfData* pFrameOfData=NULL; 
		sFrameOfData  CapFrameOfData;  
		
		int pf;
		while(loop)
		{
			memset(&CapFrameOfData, 0, sizeof(sFrameOfData));   
			pFrameOfData = Seeker_GetCurrentFrame();            
			seeker_two::Set markers;
			
			if (pFrameOfData !=NULL)
			{
				
				// ------------------Position-----------------
				//retval = Seeker_CopyFrame(pFrameOfData,&CapFrameOfData);
				Seeker_CopyFrame(pFrameOfData,&CapFrameOfData);
				markers.n_frame = CapFrameOfData.iFrame;
				if (markers.n_frame==pf){
					continue;
				}
				//float de = CapFrameOfData.fDelay;
				//int dof = CapFrameOfData.BodyData[0].nDofs;
				printf("Received multi-cast frame no %d", markers.n_frame);
				for (int iBody=0 ; iBody<CapFrameOfData.nBodies ; ++iBody)
				{
					sBodyData *Body = &CapFrameOfData.BodyData[iBody]; 
					// Markers
					markers.n_mk= Body->nMarkers;
					//printf( "\tnMarkers=%d\n", markers.n_mk);  
					for (int i=0 ; i<Body->nMarkers ; ++i)  
					{
						seeker_two::Coor m;
						for (int j=0;j<3;++j)m.coor.push_back(Body->Markers[i][j]);
						markers.mkset.push_back(m);
					}
					// Segments
					markers.n_seg = Body->nSegments;  
					for (int i=0 ; i<markers.n_seg ; ++i)
					{
						seeker_two::Seg s;
						for (int j=0;j<7;++j)s.seg.push_back(Body->Segments[i][j]);
						markers.segset.push_back(s);
					}
					
				}
				
				
				// Unidentified Markers
				markers.n_unmk = CapFrameOfData.nUnidentifiedMarkers;
				for (int i=0 ; i<markers.n_unmk ; ++i)
				{
					seeker_two::Coor un_m;
					for (int j=0;j<7;++j)un_m.coor.push_back(CapFrameOfData.UnidentifiedMarkers[i][j]);
					markers.undef_mk.push_back(un_m);
				}				
				
				pos.publish(markers);
				
				
				pf = markers.n_frame;
			}
			else{
				
				printf("No data captured...\n\n");
			}
			r.sleep();   
			printf("\n");
			Seeker_FreeFrame(&CapFrameOfData);
		}
	} 

	printf("****** Seeker_Exit ******\n");
	Seeker_Exit();

	return 0;
}




// upgrade in the future
// 1. set multiple topics to divide data groups, to avoid large pack of data
// 2. shrink the header file, delete unused function
// 3. check the header file, to get more data that we need