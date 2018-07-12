import numpy as np
import tensorflow as tf
import matplotlib.pyplot as plt

def unpickle(file):
    import pickle
    with open(file, 'rb') as fp:
        dict = pickle.load(fp, encoding='bytes')
    return dict

def get_data(batch_size):
    path = './cifar-10-batches-py'
    train_file_name = ['data_batch_1', 'data_batch_2', 'data_batch_3', 'data_batch_4', 'data_batch_5']
    test_file_name = ['test_batch']

    train_data = np.concatenate(list(unpickle('/'.join([path, train_path]))[b'data'] for train_path in train_file_name))
    train_label =np.concatenate(list(unpickle('/'.join([path, train_path]))[b'labels'] for train_path in train_file_name))
    train_label = tf.one_hot(train_label, 10)

    test_data = np.concatenate(list(unpickle('/'.join([path, test_path]))[b'data'] for test_path in test_file_name))
    test_label =np.concatenate(list(unpickle('/'.join([path, test_path]))[b'labels'] for test_path in test_file_name))
    test_label = tf.one_hot(test_label, 10)

    train_data = np.transpose(np.reshape(train_data, (50000, 3, 32, 32)), (0, 2, 3, 1)).astype(np.float32)
    test_data = np.transpose(np.reshape(test_data, (10000, 3, 32, 32)), (0, 2, 3, 1)).astype(np.float32)

    mean = np.round(np.mean(train_data))
    train_data -= mean
    test_data -= mean

    train = (train_data, train_label)
    test = (test_data, test_label)

    train_ = tf.data.Dataset.from_tensor_slices(train)
    train_ = train_.shuffle(10000)
    train_ = train_.batch(batch_size)

    test_ = tf.data.Dataset.from_tensor_slices(test)
    test_ = test_.batch(batch_size)

    return train_, test_

if __name__ == '__main__':
    get_data(127)
