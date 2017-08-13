#include <iostream>
#include <fstream>
#include <cctype>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "dirent.h"
#include <unistd.h>
#include <string>
#include <typeinfo>
#include <ros/ros.h>
#include <ros/console.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/Imu.h>
#include <sensor_msgs/image_encodings.h>
#include <cv_bridge/cv_bridge.h>
#include <nav_msgs/Odometry.h>
#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <Eigen/Eigen>
#include <queue>

//for rand()
#include <stdlib.h>
#include <time.h>
#include <numeric>
#include <time.h>

using namespace cv;
using namespace Eigen;
using namespace std;

ros::Publisher pub_img;
ros::Publisher pub_imu;
queue<sensor_msgs::Imu> imu_iphone_buf;
queue<double> imu_iphone_time_buf;

int main(int argc, char **argv)
{
    ros::init(argc, argv, "data_bag_wapper");
    ros::NodeHandle n("~");

    //publish iphone sensor data
    pub_img = n.advertise<sensor_msgs::Image>("/iphone/cam", 1000);
    pub_imu = n.advertise<sensor_msgs::Imu>("/iphone/imu", 1000);

    string data_dir;
    n.getParam("data_dir", data_dir);
    cout << "data dir: " << data_dir << endl;
    //get images from folder
    std::string inputDirectory = data_dir + "raw_image/%01d.PNG";
    VideoCapture sequence(inputDirectory);
    if (!sequence.isOpened())
    {
      cerr << "Failed to open Image Sequence!\n" << endl;
      return 1;
    }
    //image timestamp file
    std::fstream imageTimeFile(data_dir + "image_time", std::ios_base::in);
    //imu data file
    std::fstream imuDataFile(data_dir + "imu_data", std::ios_base::in);
    Mat rawImage;
    namedWindow("track", CV_WINDOW_AUTOSIZE);
    //read imu
    while(true)
    {
        if(imuDataFile.eof())
        {
            printf("finish read imu %d\n",imu_iphone_buf.size());
            break;
        }
        else
        {
            sensor_msgs::Imu imu_msg;
            double imuTime,ax,ay,az,gx,gy,gz;
            imuDataFile >> imuTime;
            imuDataFile >> ax;
            imuDataFile >> ay;
            imuDataFile >> az;
            imuDataFile >> gx;
            imuDataFile >> gy;
            imuDataFile >> gz;
            cout << imuTime << endl;
            ros::Time timeImu(imuTime);
            imu_msg.header.stamp   = timeImu; 
            imu_msg.header.frame_id = "world";
            imu_msg.linear_acceleration.x = ax;
            imu_msg.linear_acceleration.y = ay;
            imu_msg.linear_acceleration.z = az;
            imu_msg.angular_velocity.x = gx;
            imu_msg.angular_velocity.y = gy;
            imu_msg.angular_velocity.z = gz;
            imu_iphone_time_buf.push(imuTime);
            imu_iphone_buf.push(imu_msg);
        }
    }
    while(true)
    {
        sequence >> rawImage;
        if(rawImage.empty())
        {
            cout << "End of Sequence" << endl;
            break;
        }
        else
        {
            cv::Mat grayImage;
            cv::cvtColor(rawImage, grayImage, CV_BGRA2GRAY);

            
            double imageTime;
            imageTimeFile >> imageTime;

            
            //publish imu data
            sensor_msgs::Imu imu_iphone;
            while(imu_iphone_time_buf.front() < imageTime)
            {
                pub_imu.publish(imu_iphone_buf.front());
                printf("pub imu %lf\n",imu_iphone_time_buf.front());
                imu_iphone_buf.pop();
                imu_iphone_time_buf.pop();
            }
            //image data publish    
            ros::Time timeImage(imageTime);
            cv_bridge::CvImage out_msg;
            out_msg.header.stamp   = timeImage; 
            out_msg.header.frame_id = "world";
            out_msg.encoding = sensor_msgs::image_encodings::MONO8; 
            out_msg.image    = grayImage;
            pub_img.publish(out_msg);
            printf("pub image %lf-----------------\n", imageTime);
            
            
            imshow("track",grayImage);
            waitKey(30);
            //sleep(0.3);
        }
    }
    imageTimeFile.close();
    imuDataFile.close();
    return 0;
    //ros::spin();
}
