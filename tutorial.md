# Collect data
`
python collect_data.py --task_name pick_little_bear --max_timesteps 500 --episode_idx 0
`

# Visualize data
`
python visualize_episodes.py --task_name pick_little_bear --episode_idx 0
`

# Training
`
python act/train.py --task_name pick_little_bear --batch_size 4 --num_epochs 3000 --num_episodes 50
`

# Inference
`
python act/inference.py --task_name pick_little_bear
`