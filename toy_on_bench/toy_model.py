import tensorflow as tf
import time
import matplotlib as plt
import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'

import read_data

def conv_relu(inputs, filters, kernel_size, stride, padding, scope_name):
    with tf.variable_scope(scope_name, reuse=tf.AUTO_REUSE) as scope:
        input_channels = inputs.shape[-1] # the last dim indicates n_channels
        kernel = tf.get_variable('kernel', [kernel_size, kernel_size, input_channels, filters], initializer=tf.truncated_normal_initializer(mean=0.0, stddev=0.01))
        biases = tf.get_variable('biase', [filters], initializer=tf.constant_initializer(0.0))
        conv = tf.nn.conv2d(inputs, kernel, strides=[1, stride, stride, 1], padding=padding)
    return tf.nn.relu(conv + biases, name=scope.name)

def maxpool(inputs, ksize, stride, padding='VALID', scope_name='pool'):
    with tf.variable_scope(scope_name, reuse=tf.AUTO_REUSE) as scope:
        pool = tf.nn.max_pool(inputs, ksize=[1, ksize, ksize, 1], strides=[1, stride, stride, 1], padding=padding)
    return pool

def fully_connected(inputs, out_dim, scope_name='fc'):
    with tf.variable_scope(scope_name, reuse=tf.AUTO_REUSE) as scope:
        in_dim = inputs.shape[-1]
        #w = tf.get_variable('weights', [in_dim, out_dim], initializer=tf.truncated_normal_initializer(mean=0.0, stddev=0.01))
        w = tf.get_variable('weights', [in_dim, out_dim], initializer=tf.contrib.layers.xavier_initializer())
        b = tf.get_variable('biases', [out_dim], initializer=tf.constant_initializer(0.0))
        out = tf.matmul(inputs, w) + b
    return out


class toy_model(object):
    def __init__(self):
        self.learning_rate = 0.001
        self.batch_size = 32
        self.keep_prob = tf.constant(0.75)
        self.eps = tf.constant(0.00001)
        self.global_step = tf.Variable(0, dtype=tf.int32, trainable=False, name='global_step')
        self.n_classes = 24
        self.skip_step = 1
        self.n_test = 600 # do i need test data?
        self.n_train = 3000
        self.training = True

    def get_data(self):
        with tf.name_scope('data'):
            train_data, test_data = read_data.get_data(self.batch_size, self.n_train, self.n_test)
            iterator = tf.data.Iterator.from_structure(train_data.output_types, train_data.output_shapes)
            self.img, self.labels = iterator.get_next() # this img has 24 channels!

            self.train_init = iterator.make_initializer(train_data)
            self.test_init = iterator.make_initializer(test_data)

    def inference(self):
        # img is a 64 * 64 * 24 image
        conv1 = conv_relu(self.img, 64, 3, 1, 'SAME', 'conv1') # 64 * 64 * 32
        conv2 = conv_relu(conv1, 64, 3, 1, 'SAME', 'conv2') # 64 * 64 * 64
        pool1 = maxpool(conv2, 2, 2, 'VALID', 'pool1') # 32 * 32 * 64

        conv3 = conv_relu(pool1, 32, 3, 1, 'SAME', 'conv3') # 32 * 32 * 32
        conv4 = conv_relu(conv3, 32, 3, 1, 'SAME', 'conv4') # 32 * 32 * 32
        pool2 = maxpool(conv4, 2, 2, 'VALID', 'pool2') # 16 * 16 * 32

        conv5 = conv_relu(pool2, 16, 3, 1, 'SAME', 'conv5') # 16 * 16 * 16
        conv6 = conv_relu(conv5, 16, 3, 1, 'SAME', 'conv6') # 16 * 16 * 16
        pool3 = maxpool(conv6, 2, 2, 'VALID', 'pool3') # 8 * 8 * 16

        feature_dim = pool3.shape[1] * pool3.shape[2] * pool3.shape[3]
        pool3 = tf.reshape(pool3, [-1, feature_dim])
        fc1 = fully_connected(pool3, 1024, 'fc1')
        dropout = tf.nn.dropout(tf.nn.relu(fc1), self.keep_prob, name='relu1')
        self.logits = fully_connected(dropout, self.n_classes, 'logits')

    def loss(self):
        with tf.name_scope('loss'):
            cross_entropy = tf.nn.softmax_cross_entropy_with_logits(labels=self.labels, logits=self.logits)
            self.loss = tf.reduce_mean(cross_entropy, name='loss')

            # try log_softmax, hope more numerically stable
            # self.logits.shape ==> [batch_size, n_classes]
            #log_softmax = tf.nn.log_softmax(self.logits, axis=-1)
            #cross_entropy = -tf.reduce_mean(self.label * )

    def optimize(self):
        self.opt = tf.train.AdamOptimizer(self.learning_rate).minimize(self.loss, global_step=self.global_step)

    def eval(self):
        with tf.name_scope('predict'):
            preds = tf.nn.softmax(self.logits)
            correct_preds = tf.equal(tf.argmax(preds, 1), tf.argmax(self.labels, 1))
            self.accuracy = tf.reduce_sum(tf.cast(correct_preds, tf.float32))

    def build(self):
        self.get_data()
        self.inference()
        self.loss()
        self.optimize()
        self.eval()

    def train_one_epoch(self, sess, init, epoch, step):
        start_time = time.time()
        sess.run(init)
        self.training = True
        tot_loss = 0
        n_batches = 0
        try:
            while True:
                _, l = sess.run([self.opt, self.loss])
                if (step + 1) % self.skip_step == 0:
                    print('Loss at step {0}: {1}'.format(step + 1, l))
                step += 1
                tot_loss += l
                n_batches += 1
        except tf.errors.OutOfRangeError:
            pass
        print('Average loss at epoch {0}: {1}'.format(epoch, tot_loss/n_batches))
        print('Took: {0} seconds'.format(time.time() - start_time))
        return step

    def eval_once(self, sess, init, epoch, step):
        start_time = time.time()
        sess.run(init)
        self.training = False
        tot_corr_preds = 0
        try:
            while True:
                accuracy_batch = sess.run(self.accuracy)
                tot_corr_preds += accuracy_batch
        except tf.errors.OutOfRangeError:
            pass
        print('Accuracy at epoch {0}: {1} '.format(epoch, tot_corr_preds/self.n_test))
        print('Took: {0} seconds'.format(time.time() - start_time))

    def eval_on_train_set(self, sess, init, epoch, step):
        start_time = time.time()
        sess.run(init)
        self.training = False
        tot_corr_preds = 0
        try:
            while True:
                acc_batch = sess.run(self.accuracy)
                tot_corr_preds += acc_batch
        except tf.errors.OutOfRangeError:
            pass
        print('Accuracy at epoch {0}: {1} '.format(epoch, tot_corr_preds/self.n_train))
        print('Took: {0} seconds'.format(time.time() - start_time))

    def train(self, n_epochs):
        with tf.Session() as sess:
            sess.run(tf.global_variables_initializer())
            step = self.global_step.eval()

            for epoch in range(n_epochs):
                step = self.train_one_epoch(sess, self.train_init, epoch, step)
                self.eval_once(sess, self.test_init, epoch, step)
                self.eval_on_train_set(sess, self.train_init, epoch, step)

if __name__ == '__main__':
    model = toy_model()
    model.build()
    model.train(50)
