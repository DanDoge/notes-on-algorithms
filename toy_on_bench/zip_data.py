import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import numpy as np
import tensorflow as tf
import os
import cv2

models_path = "./models_origin/plane"
depth_map_path = "./depth_maps"

#files = os.listdir(path)

az = ["0", "60", "120", "180", "240", "300"]
el = ["-60", "-20", "20", "60"]

permutation_none = np.array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23])
permutation_up_down = np.array([3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12, 19, 18, 17, 16, 23, 22, 21, 20])
permutation_rot_60 = np.array([4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 0, 1, 2, 3])
permutation_rot_120 = np.array([8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 0, 1, 2, 3, 4, 5, 6, 7])
permutation_rot_180 = np.array([12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11])
permutation_rot_240 = np.array([16, 17, 18, 19, 20, 21, 22, 23, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15])
permutation_rot_300 = np.array([20, 21, 22, 23, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19])
permutation_rot_60_ud = permutation_rot_60[permutation_up_down]
permutation_rot_120_ud = permutation_rot_120[permutation_up_down]
permutation_rot_180_ud = permutation_rot_180[permutation_up_down]
permutation_rot_240_ud = permutation_rot_240[permutation_up_down]
permutation_rot_300_ud = permutation_rot_300[permutation_up_down]

per = [permutation_none, permutation_up_down, permutation_rot_60, permutation_rot_180, permutation_rot_120, permutation_rot_240, permutation_rot_300, permutation_rot_60_ud, permutation_rot_120_ud, permutation_rot_180_ud, permutation_rot_240_ud, permutation_rot_300_ud]


def get_data():
    model_files = os.listdir(models_path)
    raw_data = np.empty(shape=[0, 24, 64, 64])
    raw_label = []
    idx = 0
    for file in model_files:
        if not os.path.isdir(file):
            input_data_origin = np.empty(shape=[0, 64, 64])
            input_label_origin = 12
            for a in az:
                for e in el:
                    channel_name = depth_map_path + "/" + file + "_" + a + "_" + e + ".jpg"
                    channel = cv2.imread(channel_name, 0)
                    channel = cv2.copyMakeBorder(channel, 11, 11, 4, 4, cv2.BORDER_CONSTANT, value=(0, 0, 0))
                    channel = np.array(channel)
                    input_data_origin = np.concatenate((input_data_origin, np.expand_dims(channel, axis=0)), axis=0)
            input_data_origin = np.array(input_data_origin)
            for permut in per:
                input_data = input_data_origin[permut]
                input_label = np.where(permut == input_label_origin)[0][0]
                raw_data = np.concatenate((raw_data, np.expand_dims(input_data, axis=0)), axis=0)
                raw_label.append(input_label)
                print(raw_data.shape)
        if raw_data.shape[0] == 1200:
            idx += 1
            data_file_name = 'data' + str(idx)
            np.save(data_file_name, raw_data.astype(np.int8))
            label_file_name = 'label' + str(idx)
            np.save(label_file_name, np.array(raw_label))

            raw_data = np.empty(shape=[0, 24, 64, 64])
            raw_label = []



if __name__ == '__main__':
    get_data() # passed
