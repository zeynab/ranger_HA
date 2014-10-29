#include <string>
#include <utility> // std::pair

#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Twist.h>
#include "aseba.h"
#include "odometry.h"

const double MAX_SPEED = .16; //m.s^-1 on the wheels for ranger2

using namespace std;


template <typename T>
T clamp(const T& n, const T& lower, const T& upper) {
      return std::max(lower, std::min(n, upper));
}

class Ranger {

public:

    RangerOdometry odom;

    Ranger(const string& aseba_target):aseba_node(aseba_target.c_str()) {
        // check whether connection was successful
        if (!aseba_node.isValid())
        {
            ROS_ERROR_STREAM("Could not connect to Aseba target " << aseba_target);
            exit(1);
        }
        else {ROS_INFO_STREAM("Connected to Aseba target " << aseba_target);}
    };

    void step() {
        aseba_node.Hub::step(); // check for incoming Aseba events

        if (aseba_node.is_charging) {
            odom.reset(0.35, 0., 0.);
        }
        else {
            odom.update(aseba_node.l_encoder, aseba_node.r_encoder);
        }
    }

    void set_speed(const geometry_msgs::Twist& msg) {

        pair<double, double> speeds = odom.twist_to_motors(msg.linear.x, msg.angular.z);
        int lspeed = clamp<int>(speeds.first * 100./ MAX_SPEED, -100, 100);
        int rspeed = clamp<int>(speeds.second * 100./ MAX_SPEED, -100, 100);

        aseba_node.setSpeed(lspeed, -rspeed);

    }

private:
    RangerAsebaBridge aseba_node;

};

int main(int argc, char** argv){
    ros::init(argc, argv, "ranger_odometry_publisher");

    ros::NodeHandle nh("~");
    string aseba_target = "";
    nh.getParam("aseba_target", aseba_target);

    Ranger ranger(aseba_target);

    ros::NodeHandle n;
    ros::Publisher odom_pub = n.advertise<nav_msgs::Odometry>("odom", 50);
    tf::TransformBroadcaster odom_broadcaster;

    ros::Subscriber cmd_vel_sub = n.subscribe<const geometry_msgs::Twist&>("cmd_vel", 1, &Ranger::set_speed, &ranger);


    ros::Time current_time, last_time;
    current_time = ros::Time::now();
    last_time = ros::Time::now();

    ros::Rate r(11.0); // Aseba low-level publishes encoders at ~10Hz

    double x, y, th, dx, dr;

    while(n.ok()){

        ros::spinOnce();               // check for incoming messages
        ranger.step();

        current_time = ros::Time::now();


        x = ranger.odom.get_x();
        y = ranger.odom.get_y();
        th = ranger.odom.get_th();
        dx = ranger.odom.get_dx();
        dr = ranger.odom.get_dr();

        //since all odometry is 6DOF we'll need a quaternion created from yaw
        geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(th);

        //first, we'll publish the transform over tf
        geometry_msgs::TransformStamped odom_trans;
        odom_trans.header.stamp = current_time;
        odom_trans.header.frame_id = "odom";
        odom_trans.child_frame_id = "base_link";

        odom_trans.transform.translation.x = x;
        odom_trans.transform.translation.y = y;
        odom_trans.transform.translation.z = 0.0;
        odom_trans.transform.rotation = odom_quat;

        //send the transform
        odom_broadcaster.sendTransform(odom_trans);

        //next, we'll publish the odometry message over ROS
        nav_msgs::Odometry odom;
        odom.header.stamp = current_time;
        odom.header.frame_id = "odom";

        //set the position
        odom.pose.pose.position.x = x;
        odom.pose.pose.position.y = y;
        odom.pose.pose.position.z = 0.0;
        odom.pose.pose.orientation = odom_quat;

        //set the velocity
        odom.child_frame_id = "base_link";
        odom.twist.twist.linear.x = dx;
        odom.twist.twist.linear.y = 0.0;
        odom.twist.twist.angular.z = dr;

        //publish the message
        odom_pub.publish(odom);

        last_time = current_time;
        r.sleep();
    }
}

