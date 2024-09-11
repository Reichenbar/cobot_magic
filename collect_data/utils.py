import os
import h5py
import cv2
import numpy as np

def load_hdf5(dataset_dir, dataset_name):
    dataset_path = os.path.join(dataset_dir, dataset_name + '.hdf5')
    if not os.path.isfile(dataset_path):
        print(f'Dataset does not exist at \n{dataset_path}\n')
        exit()

    with h5py.File(dataset_path, 'r') as root:
        is_sim = root.attrs['sim']
        compressed = root.attrs.get('compress', False)
        qpos = root['/observations/qpos'][()] # automatically return np.ndarray type
        qvel = root['/observations/qvel'][()]
        if 'effort' in root.keys():
            effort = root['/observations/effort'][()]
        else:
            effort = None
        action = root['/action'][()]
        base_action = root['/base_action'][()]
        image_dict = dict()
        for cam_name in root[f'/observations/images/'].keys():
            image_dict[cam_name] = root[f'/observations/images/{cam_name}'][()]
        if compressed:
            compress_len = root['/compress_len'][()]

    if compressed:
        for cam_id, cam_name in enumerate(image_dict.keys()):
            # un-pad and uncompress
            padded_compressed_image_list = image_dict[cam_name]
            image_list = []
            for frame_id, padded_compressed_image in enumerate(padded_compressed_image_list): # [:1000] to save memory
                image_len = int(compress_len[frame_id, cam_id])
                compressed_image = padded_compressed_image
                # image = cv2.imdecode(compressed_image, 1)
                image = cv2.imdecode(np.frombuffer(compressed_image, dtype=np.uint8), cv2.IMREAD_COLOR) # compressed as bytes
                image_list.append(image)
            # image_dict[cam_name] = image_list
            image_dict[cam_name] = np.array(image_list) # should be np.ndarray type
    return  qpos, qvel, effort, action, base_action, image_dict