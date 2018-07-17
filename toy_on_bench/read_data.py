import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import numpy as np
import tensorflow as tf
import os
import cv2

models_path = "./models_origin"
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


def get_data(batch_size, train_size, test_size):
    model_files = os.listdir(models_path)
    train_data = np.empty(shape=[0, 24, 64, 64])
    test_data = np.empty(shape=[0, 24, 64, 64])
    train_label = []
    test_label = []
    idx = 0
    for file in model_files:
        if not os.path.isdir(file):
            input_data_origin = np.empty(shape=[0, 64, 64])
            input_label_origin = 11
            for a in az:
                for e in el:
                    channel_name = depth_map_path + "/" + file + "_" + a + "_" + e + ".jpg"
                    channel = cv2.imread(channel_name, 0)
                    channel = cv2.copyMakeBorder(channel, 11, 11, 4, 4, cv2.BORDER_CONSTANT, value=(0, 0, 0))
                    channel = np.array(channel)
                    input_data_origin = np.concatenate((input_data_origin, np.expand_dims(channel, axis=0)), axis=0)
            #
            input_data_origin = np.array(input_data_origin)
            for permut in per:
                input_data = input_data_origin[permut]
                input_label = np.where(permut == input_label_origin)[0][0]
                if idx < train_size:
                    train_data = np.concatenate((train_data, np.expand_dims(input_data, axis=0)), axis=0)
                    train_label.append(input_label)
                elif idx < train_size + test_size:
                    test_data = np.concatenate((test_data, np.expand_dims(input_data, axis=0)), axis=0)
                    test_label.append(input_label)
                idx += 1
            if idx > train_size + test_size:
                break

            # log...keep track of reading process
            print(train_data.shape, test_data.shape)


    train_data = np.transpose(train_data, (0, 2, 3, 1)).astype(np.float32)
    test_data = np.transpose(test_data, (0, 2, 3, 1)).astype(np.float32)
    train_label = tf.one_hot(train_label, 24)
    test_label = tf.one_hot(test_label, 24)
    print(train_data.shape, test_data.shape, train_label.shape, test_label.shape)


    mean = np.round(np.mean(train_data))
    train_data -= mean
    test_data -= mean

    train = (train_data, train_label)
    test = (test_data, test_label)
    train = tf.data.Dataset.from_tensor_slices(train).shuffle(10000).batch(batch_size)
    test = tf.data.Dataset.from_tensor_slices(test).batch(batch_size)

    return train, test

if __name__ == '__main__':
    get_data(32, 2500, 500) # passed
