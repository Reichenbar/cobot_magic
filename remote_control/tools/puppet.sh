#!/bin/bash
source ~/.bashrc
workspace=$(pwd)
password=103

# 2 启动roscore
gnome-terminal -t "roscore" -- bash -c "roscore;exec bash;"
sleep 2


# 3 启动臂
gnome-terminal -t "launcher" -- bash -c "source ~/.bashrc;source /opt/ros/${ROS_DISTRO}/setup.bash;cd ${workspace}/follow1;source ${workspace}/follow1/devel/setup.bash;roslaunch arm_control arx.launch; exec bash;"
sleep 2
gnome-terminal -t "launcher" -- bash -c "source ~/.bashrc;source /opt/ros/${ROS_DISTRO}/setup.bash;cd ${workspace}/follow2;source ${workspace}/follow2/devel/setup.bash;roslaunch arm_control arx.launch; exec bash;"
