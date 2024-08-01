cd master1
catkin_make -DPYTHON_EXECUTABLE=/usr/bin/python3

cd ../master2
catkin_make -DPYTHON_EXECUTABLE=/usr/bin/python3

cd ../follow1
catkin_make -DPYTHON_EXECUTABLE=/usr/bin/python3

cd ../follow2
catkin_make -DPYTHON_EXECUTABLE=/usr/bin/python3

cd ..