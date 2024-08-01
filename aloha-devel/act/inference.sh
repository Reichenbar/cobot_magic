# Disable input() in the inference process
cd /home/aloha/Documents/cobot_magic/aloha-devel

python act/inference.py --task_name pick_long_bolt_2024_07_11_17_31_12 --temporal_agg --chunk_size 48 --kl_weight 10 --max_publish_step 375
python act/inference.py --task_name slide_ziploc_2024_07_09_02_42_29 --temporal_agg --chunk_size 48 --kl_weight 10 --max_publish_step 700
python act/inference.py --task_name move_ziploc_2024_07_11_00_16_39 --temporal_agg --chunk_size 48 --kl_weight 10 --max_publish_step 500