#include <string.h>
#include <stdio.h>
#include "ros/ros.h"
#include <unistd.h>
#include "velacc_cal/Set.h"
#include "velacc_cal/Velo.h"
#include "velacc_cal/Mks.h"
#include "velacc_cal/velacc_cal.h"
#include <iostream> 
using namespace std;

int main(int argc,char* argv[]){
    ros::init(argc,argv,"Calculator");
	ros::NodeHandle n;
    calculate cal;
    ros::Publisher vel = n.advertise<velacc_cal::Velo>("/Velocity",1);
	ros::Publisher acc = n.advertise<velacc_cal::Velo>("/Accel",1);
    ros::Subscriber pos = n.subscribe<velacc_cal::Set>("/Data",100, &calculate::clbk3,&cal);
    ros::Rate r(70);
    velacc_cal::Velo velocity;
	velacc_cal::Velo acceleration;
    // ------------------Velocity/Acceleration-----------------
    while (ros::ok()){
        ros::spinOnce();
        if (!cal.skip){
            vel.publish(cal.velocity);
            acc.publish(cal.acceleration);
        }
        else cal.skip =0;
        r.sleep();
    }
    return 0;
}