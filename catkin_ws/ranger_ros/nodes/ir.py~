#!/usr/bin/env python

from math import sin, cos
import rospy




from sensor_msgs.msg import Range
from tf.broadcaster import TransformBroadcaster

import ranger

RATE = 40 #Hz
BASE_FRAME = "base_link"

range_left = Range()
range_left.header.frame_id = "ir_left"
range_left.radiation_type = Range.INFRARED
range_left.field_of_view = 0.4 # ~20deg
range_left.min_range = 0.025 # according to my measurments
range_left.max_range = 0.30 # according to my measurments

range_center = Range()
range_center.header.frame_id = "ir_center"
range_center.radiation_type = Range.INFRARED
range_center.field_of_view = 0.4 # ~20 deg
range_center.min_range = 0.35 # according to my measurments
range_center.max_range = 2.0 # according to my measurments

range_right = Range()
range_right.header.frame_id = "ir_right"
range_right.radiation_type = Range.INFRARED
range_right.field_of_view = 0.4 # ~20deg
range_right.min_range = 0.025 # according to my measurments
range_right.max_range = 0.30 # according to my measurments



with ranger.Ranger() as robot:

    r = rospy.Rate(RATE)




    irPub = [rospy.Publisher("ir_left", Range, queue_size=1),
            rospy.Publisher("ir_center", Range, queue_size=1),
            rospy.Publisher("ir_right", Range, queue_size=1)]


    rospy.loginfo("ROS Ranger interface started. Starting to broadcast odometry and TF transform of base_link")

    

    state = robot.state

    while not rospy.is_shutdown():
        now = rospy.Time.now()

        # we get all the state once to ensure (as much as possible) temporal
        # consistency
       
        ir_left, ir_center, ir_right = state.ir_left, state.ir_center, state.ir_right

       

        if robot.state.w < 0.5: # only publish IR sensor when not rotating too fast
            # IR SENSORS
            range_left.range = ir_left
            range_center.range = ir_center
            range_right.range = ir_right
            irPub[0].publish(range_left)
            irPub[1].publish(range_center)
            irPub[2].publish(range_right)



        # DONE!
        r.sleep()
