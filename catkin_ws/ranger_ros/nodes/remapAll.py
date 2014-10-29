#!/usr/bin/env python
import roslib
import rospy

from sensor_msgs.msg import Image

def callback(msg):
  pub.publish(msg)

def image_remap():
  rospy.Subscriber("/camera/depth/image_raw", Image, callback)  
  

if __name__ == '__main__':
  rospy.init_node('remap')
  pub= rospy.Publisher("camera_depth", Image, queue_size=10)
  #while not rospy.is_shutdown():  
  image_remap()
  rospy.spin()
