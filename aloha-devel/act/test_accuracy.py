# from inference import RosOperator, get_arguments
# import time

# left0 = [-0.00133514404296875, 0.00209808349609375, 0.01583099365234375, -0.032616615295410156, -0.00286102294921875, 0.00095367431640625, 3.557830810546875]
# right0 = [-0.00133514404296875, 0.00438690185546875, 0.034523963928222656, -0.053597450256347656, -0.00476837158203125, -0.00209808349609375, 3.557830810546875]
# left1 = [-0.00133514404296875, 0.00209808349609375, 0.01583099365234375, -0.032616615295410156, -0.00286102294921875, 0.00095367431640625, 0.0]
# right1 = [-0.00133514404296875, 0.00438690185546875, 0.034523963928222656, -0.053597450256347656, -0.00476837158203125, -0.00209808349609375, 0.0]

# left2 = [-0.00133514404296875, 0.00209808349609375, 0.01583099365234375, -0.032616615295410156, -0.00286102294921875, 0.00095367431640625, 0.0]
# right2 = [-0.02765655517578125, 1.516937255859375, 1.2731742858886719, -1.1087589263916016, -0.13065528869628906, 0.02193450927734375, -0.010474681854248047]

# def main():
#     args = get_arguments()
#     ros_operator = RosOperator(args)
#     time.sleep(1)
#     ros_operator.puppet_arm_publish_continuous(left0, right0)
#     time.sleep(0.5)
#     ros_operator.puppet_arm_publish_continuous(left1, right1)
#     input("Go to desired configuration.")
#     ros_operator.puppet_arm_publish_continuous(left2, right2)


# if __name__ == '__main__':
#     main()

# --------------------------------------------------------------------------------------------------------------------------------
# import cv2
# from cv2 import aruco
# import rospy
# import numpy as np
# from sensor_msgs.msg import CameraInfo
# import time

# def extract_camera_params():
#     rospy.init_node('camera_info_reader', anonymous=True)
#     time.sleep(1)
#     camera_info_msg = rospy.wait_for_message('/camera_r/color/camera_info', CameraInfo)
#     # 提取相机内参矩阵
#     K = camera_info_msg.K
#     camera_matrix = np.array(K).reshape(3, 3)
    
#     # 提取畸变系数
#     D = camera_info_msg.D
#     distortion_coefficients = np.array(D)
    
#     return camera_matrix, distortion_coefficients


# # Initialize ArUco detector
# aruco_dict = aruco.getPredefinedDictionary(aruco.DICT_5X5_250)
# parameters = aruco.DetectorParameters()


# frame = cv2.imread("/home/aloha/Documents/cobot_magic/aloha-devel/act/marker.png", cv2.IMREAD_GRAYSCALE)

# # Convert frame to grayscale
# # gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

# # Detect markers
# detector = aruco.ArucoDetector(aruco_dict, parameters)
# corners, ids, _ = detector.detectMarkers(frame)

# # Assuming `mtx` and `dist` are camera intrinsic parameters from calibration
# camera_matrix, distortion_coefficients = extract_camera_params()
# # Estimate pose of detected markers
# if ids is not None:
#     rvecs, tvecs, _ = aruco.estimatePoseSingleMarkers(corners, 6, camera_matrix, distortion_coefficients)
#     frame_markers = aruco.drawDetectedMarkers(frame.copy(), corners, ids)
#     # Print pose information
#     for i in range(len(ids)):
#         print(f"Marker {ids[i]}: rvec={rvecs[i]}, tvec={tvecs[i]}")
#         # aruco.drawAxis(frame_markers, camera_matrix, distortion_coefficients, rvecs[i], tvecs[i], length=0.1)
#         # cv2.drawFrameAxes(frame, camera_matrix, distortion_coefficients, rvecs[i], tvecs[i], 4, 4)

# # Draw detected markers
# aruco.drawDetectedMarkers(frame, corners, ids)

# # Display the frame
# cv2.imshow('Frame', frame)

# cv2.waitKey(0)
# cv2.destroyAllWindows()

# --------------------------------------------------------------------------------------------------------------------------------
# --------------------------------------------------------------------------------------------------------------------------------
import numpy as np
import cv2 as cv
import rospy
import time
from sensor_msgs.msg import CameraInfo, Image, JointState
from cv_bridge import CvBridge
from inference import RosOperator, get_arguments
import time
import h5py

def extract_camera_params():
    camera_info_msg = rospy.wait_for_message('/camera_r/color/camera_info', CameraInfo)

    K = camera_info_msg.K
    camera_matrix = np.array(K).reshape(3, 3)
    
    D = camera_info_msg.D
    distortion_coefficients = np.array(D)
    
    return camera_matrix, distortion_coefficients

def draw(img, corners, imgpts):
    corner = tuple(corners[0].astype(np.int32).ravel())
    img = cv.line(img, corner, tuple(imgpts[0].astype(np.int32).ravel()), (0,0,255), 5)
    img = cv.line(img, corner, tuple(imgpts[1].astype(np.int32).ravel()), (0,255,0), 5)
    img = cv.line(img, corner, tuple(imgpts[2].astype(np.int32).ravel()), (255,0,0), 5)
    return img

def save_image(image_topic, file_path):
    rospy.init_node('image_saver', anonymous=True)
    

    image_msg = rospy.wait_for_message(image_topic, Image)
    
    cv_image = CvBridge().imgmsg_to_cv2(image_msg, desired_encoding='bgr8')
    
    cv.imshow('img',cv_image)
    k = cv.waitKey(0) & 0xFF
    if k == ord('s'):
        cv.imwrite(file_path, cv_image)
    print(f"Image saved to {file_path}")

def record_joints_images(ros_operator: RosOperator, num_records: int, data_save_path: str, vis: bool):
    left_initial = [-0.00133514404296875, 0.00209808349609375, 0.01583099365234375, -0.032616615295410156, -0.00286102294921875, 0.00095367431640625, 0.0]
    right_initial = [-0.00133514404296875, 0.00438690185546875, 0.034523963928222656, -0.053597450256347656, -0.00476837158203125, -0.00209808349609375, 0.0]
    left_desired = left_initial
    right_desired = [-0.02918243408203125, 1.5287628173828125, 1.2594413757324219, -1.1087589263916016, -0.12760353088378906, 0.02269744873046875, -0.011046886444091797]
    i = 0
    images = []
    joints = []
    time.sleep(0.5)
    ros_operator.puppet_arm_publish_continuous(left_initial, right_initial)
    while i < num_records:
        ros_operator.puppet_arm_publish_continuous(left_desired, right_desired)
        ros_operator.puppet_arm_publish_continuous(left_desired, right_desired)
        ros_operator.puppet_arm_publish_continuous(left_desired, right_desired)
        time.sleep(1)
        input()
        joint_msg = rospy.wait_for_message(ros_operator.args.puppet_arm_right_topic, JointState)
        image_msg = rospy.wait_for_message(ros_operator.args.img_right_topic, Image)
        cv_image = CvBridge().imgmsg_to_cv2(image_msg, desired_encoding='bgr8')
        if vis:
            cv.imshow('image', cv_image)
            k = cv.waitKey(0) & 0xFF
            if k == ord('s'):
                images.append(cv_image)
                joints.append(np.array(joint_msg.position))
                i += 1
                print(f"{i+1} recorded")
            cv.destroyWindow('image')
        else:
            images.append(cv_image)
            joints.append(np.array(joint_msg.position))
            i += 1
            print(f"{i+1} recorded")
        ros_operator.puppet_arm_publish_continuous(left_initial, right_initial)
        time.sleep(0.5)
    with h5py.File(data_save_path, 'w') as f:
        f.create_dataset('images', data=np.array(images))
        f.create_dataset('joints', data=np.array(joints))
    print("data saved")
    return images, joints

def calculate_multiple_camera_position(data_save_path: str):
    camera_positions = []
    with h5py.File(data_save_path, 'r') as f:
        images = list(f['images'])
        joints = list(f['joints'])
    for image in images:
        camera_positions.append(calculate_camera_position(image, vis=False))
    return np.array(camera_positions) # N*3

def calculate_camera_position(image: np.ndarray, vis: bool):
    camera_matrix, distortion_coefficients = extract_camera_params()
    # Criteria for termination of the iterative process of corner refinement.
    criteria = (cv.TERM_CRITERIA_EPS + cv.TERM_CRITERIA_MAX_ITER, 30, 0.001)
    pattern_size = (11, 8)
    # Arrays to store object points
    objp = np.zeros((pattern_size[0]*pattern_size[1], 3), np.float32)
    objp[:,:2] = np.mgrid[0:pattern_size[0],0:pattern_size[1]].T.reshape(-1,2)*15

    axis = np.float32([[45,0,0], [0,45,0], [0,0,45]]).reshape(-1,3)

    # image = cv.imread("/home/aloha/Documents/cobot_magic/aloha-devel/act/truth_image.png")
    gray_image = cv.cvtColor(image, cv.COLOR_BGR2GRAY)
    # Find the chess board corners
    ret, corners = cv.findChessboardCorners(gray_image, pattern_size)
    # If found, add object points, image points (after refining them)
    if ret == True:
        corners2 = cv.cornerSubPix(gray_image, corners, (11,11), (-1,-1), criteria)
        # Find the rotation and translation vectors, from object to camera frame
        ret, rvecs, tvecs = cv.solvePnP(objp, corners2, camera_matrix, distortion_coefficients)
        # Find the rotation matrix and translation vector from camera to obj frame
        rotation_matrix, _ = cv.Rodrigues(rvecs)
        R_inv = rotation_matrix.T
        t_inv = -np.dot(R_inv, tvecs) # the pose of camera in obj frame

        # camera_points = np.array([[0, 120, 89], [0, 163, 88]], dtype=np.float32)  # 示例相机坐标系中的点
        # object_points_from_camera = np.dot(R_inv, camera_points.T) + t_inv
        # print("Object Points from Camera Coordinates:\n", object_points_from_camera.T)

        if vis:
            # project 3D points to image plane
            axis_on_image, _ = cv.projectPoints(axis, rvecs, tvecs, camera_matrix, distortion_coefficients)
            # Draw and display the corners
            cv.drawChessboardCorners(image, pattern_size, corners2, ret)
            image = draw(image, corners2, axis_on_image)
            cv.imshow('Chessboard Corners and Axes', image)
            cv.waitKey(0)
            cv.destroyWindow('Chessboard Corners and Axes')
    else:
        raise ValueError("Chessboard corners not found")
    return np.squeeze(t_inv)

def calculate_position_accuracy(camera_positions):
    mean_values = np.mean(camera_positions, axis=0)
    std_values = np.std(camera_positions, axis=0)
    print(f"mean of camera positions: {mean_values}, std of camera positions: {std_values}")
    print(f"max of camera positions: {np.max(camera_positions, axis=0)}, min of camera positions: {np.min(camera_positions, axis=0)}")
    truth_image = cv.imread("/home/aloha/Documents/cobot_magic/aloha-devel/act/truth_image.png")
    camera_position_truth = calculate_camera_position(truth_image, vis=False)
    print(f"camera positon ground truth: {camera_position_truth}")
    pos_diff = np.sqrt(np.sum((camera_positions-camera_position_truth)**2, axis=1))
    mean_values = np.mean(pos_diff, axis=0)
    std_values = np.std(pos_diff, axis=0)
    print(f"mean of position difference: {mean_values}, std of position difference: {std_values}") # 9.282026019188258 0.07207788434716376


def main():
    ros_operator = RosOperator(get_arguments())
    data_save_path = '/home/aloha/Documents/cobot_magic/aloha-devel/act/image_joint.h5'
    num_records = 50
    # record_joints_images(ros_operator, num_records, data_save_path, vis=False)
    camera_positions = calculate_multiple_camera_position(data_save_path)
    calculate_position_accuracy(camera_positions)

if __name__ == '__main__':
    main()