#!/usr/bin/env python
import roslib
import rospy

from sensor_msgs.msg import Image
from sensor_msgs.msg import CameraInfo

def callback(msg):
  pub.publish(msg)

def callback_info(msg):
  pub_info.publish(msg)

def image_remap():
  rospy.Subscriber("/camera/depth/image_raw", Image, callback)  
  
def image_remap_info():
  rospy.Subscriber("/camera/depth/camera_info", CameraInfo, callback_info)



if __name__ == '__main__':
  rospy.init_node('remap')
  pub= rospy.Publisher("camera_depth", Image, queue_size=10)
  pub_info= rospy.Publisher("camera_info", CameraInfo, queue_size=1)

  #while not rospy.is_shutdown():  
  image_remap()
  image_remap_info()
  rospy.spin() 

