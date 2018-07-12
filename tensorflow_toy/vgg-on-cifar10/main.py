import os
import tensorflow as tf

import vgg_model

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2' # omit warings

if __name__ == '__main__':
    model = vgg_model.vgg_cifar()
    model.build()
    model.train(n_epochs=30)
