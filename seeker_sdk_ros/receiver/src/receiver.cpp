/***********************************************************************
Copyright 2020 GuYueHome (www.guyuehome.com).
***********************************************************************/

/**
 * 该例程将订阅/person_info话题，自定义消息类型learning_topic::Person
 */
 
#include <ros/ros.h>
#include "receiver/Set.h"
#include "receiver/Velo.h"

// 接收到订阅的消息后，会进入消息回调函数
void Callback(const receiver::Set::ConstPtr& msg)
{
    // 将接收到的消息打印出来
    receiver::Set data ;
    data.n_frame = msg->n_frame;
    data.n_mk = msg->n_mk;
    printf("Frame No.%d:\n",data.n_frame);
    for (int i=0;i<data.n_mk;++i){
        data.mkset.push_back(msg->mkset[i]);
        printf("Marker No.%d with (%f, %f, %f)\n",i+1,data.mkset[i].coor[0],data.mkset[i].coor[1],data.mkset[i].coor[2]);
    }
    printf("\n\n");                   
    
}
void velo_callback(const receiver::Velo::ConstPtr& msg){
    receiver::Velo data;
    data.n_frame = msg->n_frame;
    printf("Frame No.%d:\n",data.n_frame);
    for (int i=0;i<data.n_mk;++i){
        data.velocity.push_back(msg->velocity[i]);
        printf("Marker No.%d with velocity xyzM(%f, %f, %f,%f)\n",i+1,data.velocity[i].xyzM[0],data.velocity[i].xyzM[1],data.velocity[i].xyzM[2],data.velocity[i].xyzM[3]);
    }
    printf("\n\n");       

}

int main(int argc, char **argv)
{
    // 初始化ROS节点
    ros::init(argc, argv, "receiver");

    // 创建节点句柄
    ros::NodeHandle n;

    // 创建一个Subscriber，订阅名为/person_info的topic，注册回调函数personInfoCallback
    //ros::Subscriber pos = n.subscribe<receiver::Set>("/Data",1, Callback);
    ros::Subscriber vel = n.subscribe<receiver::Velo>("/Velocity",1,velo_callback);

    // 循环等待回调函数
    ros::spin();

    return 0;
}
