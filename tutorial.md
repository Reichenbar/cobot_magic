# Collect data
```
python collect_data.py --task_name pick_little_bear --max_timesteps 500 --episode_idx 0
```

* Observation topic: `/puppet/joint_left` and `/puppet/joint_right`
* Action topic: `/master/joint_left` and `/master/joint_right`

```python
data_dict = {
    # 一个是奖励里面的qpos，qvel， effort ,一个是实际发的acition
    '/observations/qpos': [],
    '/observations/qvel': [],
    '/observations/effort': [],
    '/action': [],
    '/base_action': [],
    # '/base_action_t265': [],
}
```

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

# Training
```
python act/train.py --task_name pick_little_bear --batch_size 4 --num_epochs 3000 --num_episodes 50
```

# Inference
```
python act/inference.py --task_name pick_little_bear
```