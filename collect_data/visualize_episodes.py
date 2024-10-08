#coding=utf-8
import os
import numpy as np
import cv2
import h5py
import argparse
import matplotlib.pyplot as plt

from utils import load_hdf5

DT = 0.02
# JOINT_NAMES = ["waist", "shoulder", "elbow", "forearm_roll", "wrist_angle", "wrist_rotate"]
JOINT_NAMES = ["joint0", "joint1", "joint2", "joint3", "joint4", "joint5"]
STATE_NAMES = JOINT_NAMES + ["gripper"]
BASE_STATE_NAMES = ["linear_vel", "angular_vel"]

def main(args):
    dataset_dir = os.path.join(args['dataset_dir'], args['task_name'])
    episode_idx = args['episode_idx']
    task_name   = args['task_name']
    dataset_name = f'episode_{episode_idx}'

    qpos, qvel, effort, action, base_action, image_dict = load_hdf5(dataset_dir, dataset_name)
    
    print('hdf5 loaded!!')
    
    vis_dir = os.path.join(dataset_dir, "visualization")
    if not os.path.exists(vis_dir):
        os.makedirs(vis_dir)

    save_videos(image_dict, action, DT,  video_path=os.path.join(vis_dir, dataset_name + '_video.mp4'))
    visualize_joints(qpos, action, plot_path=os.path.join(vis_dir, dataset_name + '_qpos.png'))
    visualize_base(base_action, plot_path=os.path.join(vis_dir, dataset_name + '_base_action.png'))

def save_videos(video, actions, dt, video_path=None):
    cam_names = list(video.keys())
    all_cam_videos = []
    for cam_name in cam_names:
        all_cam_videos.append(video[cam_name])
    all_cam_videos = np.concatenate(all_cam_videos, axis=2) # width dimension

    n_frames, h, w, _ = all_cam_videos.shape
    fps = int(1 / dt)
    out = cv2.VideoWriter(video_path, cv2.VideoWriter_fourcc(*'mp4v'), fps, (w, h))
    for t in range(n_frames):
        image = all_cam_videos[t]
        image = image[:, :, [2, 1, 0]]  # swap B and R channel
        cv2.imshow("images",image)
        cv2.waitKey(30)
        print("episode_id: ", t, "left: ", np.round(actions[t][:7], 3), "right: ", np.round(actions[t][7:], 3), "\n")
        out.write(image)
    out.release()
    print(f'Saved video to: {video_path}')


def visualize_joints(qpos_list, command_list, plot_path=None, ylim=None, label_overwrite=None):
    if label_overwrite:
        label1, label2 = label_overwrite
    else:
        label1, label2 = 'Observation (puppet)', 'Action (master)'

    qpos = np.array(qpos_list) # ts, dim
    command = np.array(command_list)
    
    num_ts, num_dim = qpos.shape
    h, w = 2, num_dim
    num_figs = num_dim
    fig, axs = plt.subplots(num_figs, 1, figsize=(8, 2 * num_dim))

    # plot joint state
    all_names = [name + '_left' for name in STATE_NAMES] + [name + '_right' for name in STATE_NAMES]
    for dim_idx in range(num_dim):
        ax = axs[dim_idx]
        ax.plot(qpos[:, dim_idx], label=label1, color='orangered')
        ax.set_title(f'Joint {dim_idx}: {all_names[dim_idx]}')
        ax.legend()

    ## plot arm command
    ## HACK: for gripper, the command (master) is 12 times the state (puppet)
    # command[:, -1] = command[:, -1]/12
    for dim_idx in range(num_dim):
        ax = axs[dim_idx]
        ax.plot(command[:, dim_idx], label=label2)
        ax.legend()

    if ylim:
        for dim_idx in range(num_dim):
            ax = axs[dim_idx]
            ax.set_ylim(ylim)

    plt.tight_layout()
    plt.savefig(plot_path)
    print(f'Saved qpos plot to: {plot_path}')
    plt.close()


def visualize_base(readings, plot_path=None):
    readings = np.array(readings) # ts, dim
    num_ts, num_dim = readings.shape
    num_figs = num_dim
    fig, axs = plt.subplots(num_figs, 1, figsize=(8, 2 * num_dim))

    # plot joint state
    all_names = BASE_STATE_NAMES
    for dim_idx in range(num_dim):
        ax = axs[dim_idx]
        ax.plot(readings[:, dim_idx], label='raw')
        ax.plot(np.convolve(readings[:, dim_idx], np.ones(20)/20, mode='same'), label='smoothed_20')
        ax.plot(np.convolve(readings[:, dim_idx], np.ones(10)/10, mode='same'), label='smoothed_10')
        ax.plot(np.convolve(readings[:, dim_idx], np.ones(5)/5, mode='same'), label='smoothed_5')
        ax.set_title(f'Joint {dim_idx}: {all_names[dim_idx]}')
        ax.legend()


    plt.tight_layout()
    plt.savefig(plot_path)
    print(f'Saved effort plot to: {plot_path}')
    plt.close()

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--dataset_dir', action='store', type=str, help='Dataset_dir.',
                        default="./data", required=False)
    parser.add_argument('--task_name', action='store', type=str, help='Task name.', required=True)
    parser.add_argument('--episode_idx', action='store', type=int, help='Episode index.', required=True)
    
    main(vars(parser.parse_args()))
