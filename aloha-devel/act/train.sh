cd ~/Documents/cobot_magic/aloha-devel
# conda activate aloha

# python act/train.py --task_name slide_ziploc --batch_size 8 --lr 1e-5 --chunk_size 16 --num_epochs 8000 --num_episodes 50 --kl_weight 10 # 2:20
# python act/train.py --task_name slide_ziploc --batch_size 8 --lr 1e-5 --chunk_size 32 --num_epochs 8000 --num_episodes 50 --kl_weight 10
# python act/train.py --task_name slide_ziploc --batch_size 8 --lr 1e-5 --chunk_size 48 --num_epochs 8000 --num_episodes 50 --kl_weight 10
# python act/train.py --task_name slide_ziploc --batch_size 8 --lr 1e-5 --chunk_size 64 --num_epochs 8000 --num_episodes 50 --kl_weight 10
# python act/train.py --task_name slide_ziploc --batch_size 8 --lr 1e-5 --chunk_size 80 --num_epochs 8000 --num_episodes 50 --kl_weight 10
# python act/train.py --task_name slide_ziploc --batch_size 8 --lr 1e-5 --chunk_size 8 --num_epochs 8000 --num_episodes 50 --kl_weight 10
# python act/train.py --task_name slide_ziploc --batch_size 8 --lr 1e-5 --chunk_size 96 --num_epochs 8000 --num_episodes 50 --kl_weight 10
# python act/train.py --task_name slide_ziploc --batch_size 8 --lr 1e-5 --chunk_size 40 --num_epochs 8000 --num_episodes 50 --kl_weight 10
# python act/train.py --task_name slide_ziploc --batch_size 8 --lr 1e-5 --chunk_size 56 --num_epochs 8000 --num_episodes 50 --kl_weight 10
# python act/train.py --task_name slide_ziploc --batch_size 16 --lr 2e-5 --chunk_size 32 --num_epochs 8000 --num_episodes 50 --kl_weight 10
# python act/train.py --task_name slide_ziploc --batch_size 16 --lr 2e-5 --chunk_size 48 --num_epochs 8000 --num_episodes 50 --kl_weight 10
# python act/train.py --task_name slide_ziploc --batch_size 8 --lr 1e-5 --chunk_size 48 --num_epochs 8000 --num_episodes 50 --kl_weight 100

# python act/train.py --task_name transfer_cube --batch_size 8 --lr 1e-5 --chunk_size 48 --num_epochs 8000 --num_episodes 90 --kl_weight 10 # 3:50

# python act/train.py --task_name move_ziploc --batch_size 8 --lr 1e-5 --chunk_size 48 --num_epochs 8000 --num_episodes 50 --kl_weight 10 # 2:15
# python act/train.py --task_name move_ziploc --batch_size 8 --lr 1e-5 --chunk_size 48 --num_epochs 8000 --num_episodes 50 --kl_weight 100

# python act/train.py --task_name pick_bolt --batch_size 8 --lr 1e-5 --chunk_size 48 --num_epochs 8000 --num_episodes 50 --kl_weight 10 # 2:15
# python act/train.py --task_name pick_bolt --batch_size 8 --lr 1e-5 --chunk_size 48 --num_epochs 8000 --num_episodes 50 --kl_weight 100

python act/train.py --task_name pick_long_bolt --batch_size 8 --lr 1e-5 --chunk_size 48 --num_epochs 8000 --num_episodes 50 --kl_weight 10 #
