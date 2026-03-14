#include <geometry_msgs/Quaternion.h>
#include <geometry_msgs/Transform.h>
#include <geometry_msgs/Twist.h>
#include <quadrotor_msgs/PositionCommand.h>
#include <ros/ros.h>
#include <tf2/LinearMath/Quaternion.h>
#include <trajectory_msgs/MultiDOFJointTrajectory.h>
#include <trajectory_msgs/MultiDOFJointTrajectoryPoint.h>

class RotorsTrajectoryBridge {
public:
  RotorsTrajectoryBridge() {
    ros::NodeHandle nh;
    ros::NodeHandle pnh("~");

    pnh.param("yaw_source", yaw_source_, std::string("command"));

    cmd_sub_ = nh.subscribe("/planning/pos_cmd", 50,
                            &RotorsTrajectoryBridge::cmdCallback, this,
                            ros::TransportHints().tcpNoDelay());
    traj_pub_ = nh.advertise<trajectory_msgs::MultiDOFJointTrajectory>(
        "/firefly/command/trajectory", 10);
  }

private:
  void cmdCallback(const quadrotor_msgs::PositionCommand::ConstPtr &msg) {
    trajectory_msgs::MultiDOFJointTrajectory traj_msg;
    traj_msg.header = msg->header;
    traj_msg.header.frame_id =
        msg->header.frame_id.empty() ? "world" : msg->header.frame_id;
    traj_msg.joint_names.push_back("base_link");

    trajectory_msgs::MultiDOFJointTrajectoryPoint point;

    geometry_msgs::Transform transform;
    transform.translation.x = msg->position.x;
    transform.translation.y = msg->position.y;
    transform.translation.z = msg->position.z;

    const double yaw = yaw_source_ == "velocity"
                           ? std::atan2(msg->velocity.y, msg->velocity.x)
                           : msg->yaw;
    tf2::Quaternion q;
    q.setRPY(0.0, 0.0, yaw);
    transform.rotation.x = q.x();
    transform.rotation.y = q.y();
    transform.rotation.z = q.z();
    transform.rotation.w = q.w();
    point.transforms.push_back(transform);

    geometry_msgs::Twist velocity;
    velocity.linear.x = msg->velocity.x;
    velocity.linear.y = msg->velocity.y;
    velocity.linear.z = msg->velocity.z;
    velocity.angular.z = msg->yaw_dot;
    point.velocities.push_back(velocity);

    geometry_msgs::Twist acceleration;
    acceleration.linear.x = msg->acceleration.x;
    acceleration.linear.y = msg->acceleration.y;
    acceleration.linear.z = msg->acceleration.z;
    point.accelerations.push_back(acceleration);

    point.time_from_start = ros::Duration(0.0);
    traj_msg.points.push_back(point);

    traj_pub_.publish(traj_msg);
  }

  ros::Subscriber cmd_sub_;
  ros::Publisher traj_pub_;
  std::string yaw_source_;
};

int main(int argc, char **argv) {
  ros::init(argc, argv, "rotors_trajectory_bridge");
  RotorsTrajectoryBridge bridge;
  ros::spin();
  return 0;
}
