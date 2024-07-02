# Collect data
```
python collect_data.py --task_name pick_little_bear --max_timesteps 500 --episode_idx 0
```

* Observation topic: `/puppet/joint_left` and `/puppet/joint_right`
* Action topic: `/master/joint_left` and `/master/joint_right`

```python
data_dict = {
    # puppet states as observation, master states as action
    '/observations/qpos': [],
    '/observations/qvel': [],
    '/observations/effort': [],
    '/action': [],
    '/base_action': [],
    # '/base_action_t265': [],
}
```
* The data is typically stored in the shape of (num_steps, num_dims)

# Visualize data
```
python visualize_episodes.py --task_name pick_little_bear --episode_idx 10
```
* read the data from observation and action topics and plot it

# Replay data
```
python replay_data.py --task_name pick_little_bear --episode_idx 0
```
* read the data from observation (optional) and action topics and plot it

# Replay data for simulation
```
python replay_data.py --simulation --frame_rate 3 --task_name pick_little_bear --episode_idx 0
```

# Training
NOTE: Check task name!!! Otherwise, the previous trained model maybe overwritten.
```
python act/train.py --task_name pick_blue_clamp --batch_size 8 --num_epochs 9000 --num_episodes 50 --lr 1e-5
```

# Inference
```
python act/inference.py --task_name pick_blue_clamp
```

# ACT Tuning Tips
- Chunk size is the most important param to tune when applying ACT to a new environment. One chunk should correspond to ~1 secs wall-clock robot motion. In our case, the default value of chunk size is 32, which meets this requirement.
- High KL weight (10 or 100), or train **without** CVAE encoder.
- Consider removing temporal_agg and increase query frequency [here](https://github.com/tonyzhaozh/act/blob/main/imitate_episodes.py#L193) to be the same as your chunk size. I.e. each chunk is executed fully.
- train for **very long** (well after things plateaus, see picture)
- Try to increase batch size as much as possible, and increase lr accordingly. E.g. batch size 64 with learning rate 5e-5 versus batch size 8 and learning rate 1e-5
- Have separate backbones for each camera (requires changing the code, see [this commit](https://github.com/tonyzhaozh/act/commit/20fc6e990698534b89a41c2c85f54b7ff4b0c280))
- L1 loss > L2 loss (not precise enough)
- Abs position control > delta/velocity control (harder to recover)
- Try multiple checkpoints

For real-world experiments:
- Train for even longer (5k - 8k steps, especially if multi-camera)
- If inference is too slow -> robot moving slowly: disable temporal_agg and increase query frequency [here](https://github.com/tonyzhaozh/act/blob/main/imitate_episodes.py#L193). We tried as high as 20.

Example loss curve (L1)

![Example loss curve](./collect_data/docs/example_l1.png)