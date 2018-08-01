import tensorflow as tf
import time
import matplotlib.pyplot as plt

import read_data

def conv_relu(inputs, filters, kernel_size, stride, padding, scope_name):
    with tf.variable_scope(scope_name, reuse=tf.AUTO_REUSE) as scope:
        input_channels = inputs.shape[-1] # the last dim indicates n_channels
        kernel = tf.get_variable('kernel', [kernel_size, kernel_size, input_channels, filters], initializer=tf.truncated_normal_initializer(mean=0.0, stddev=0.01))
        biases = tf.get_variable('biase', [filters], initializer=tf.constant_initializer(0.0))
        conv = tf.nn.conv2d(inputs, kernel, strides=[1, stride, stride, 1], padding=padding)
    return tf.nn.relu(conv + biases, name=scope.name)

def conv_bn_relu(inputs, filters, kernel_size, stride, padding, scope_name):
    with tf.variable_scope(scope_name, reuse=tf.AUTO_REUSE) as scope:
        input_channels = inputs.shape[-1] # the last dim indicates n_channels
        kernel = tf.get_variable('kernel', [kernel_size, kernel_size, input_channels, filters], initializer=tf.truncated_normal_initializer(mean=0.0, stddev=0.01), regularizer=tf.contrib.layers.l2_regularizer(scale=0.0002))
        biases = tf.get_variable('biase', [filters], initializer=tf.constant_initializer(0.0))
        conv = tf.nn.conv2d(inputs, kernel, strides=[1, stride, stride, 1], padding=padding)

        mean, variance = tf.nn.moments(conv + biases, axes=[0, 1, 2])
        beta = tf.get_variable('beta', filters, tf.float32, initializer=tf.constant_initializer(0.0, tf.float32))
        gamma = tf.get_variable('gamma', filters, tf.float32, initializer=tf.constant_initializer(1.0, tf.float32))
        bn = tf.nn.batch_normalization(conv + biases, mean, variance, beta, gamma, 0.001)
    return tf.nn.relu(bn, name=scope.name)


def maxpool(inputs, ksize, stride, padding='VALID', scope_name='pool'):
    with tf.variable_scope(scope_name, reuse=tf.AUTO_REUSE) as scope:
        pool = tf.nn.max_pool(inputs, ksize=[1, ksize, ksize, 1], strides=[1, stride, stride, 1], padding=padding)
    return pool

def fully_connected(inputs, out_dim, scope_name='fc'):
    with tf.variable_scope(scope_name, reuse=tf.AUTO_REUSE) as scope:
        in_dim = inputs.shape[-1]
        w = tf.get_variable('weights', [in_dim, out_dim], initializer=tf.contrib.layers.xavier_initializer(), regularizer=tf.contrib.layers.l2_regularizer(scale=0.0002))
        b = tf.get_variable('biases', [out_dim], initializer=tf.constant_initializer(0.0), regularizer=tf.contrib.layers.l2_regularizer(scale=0.0002))
        out = tf.matmul(inputs, w) + b
    return out


class vgg_cifar(object):
    def __init__(self):
        self.learning_rate = 0.001
        self.batch_size = 32 # 128 -> <70%, 32 -> <80%, 16 -> 70%
        self.keep_prob = tf.constant(0.75)
        self.eps = tf.constant(0.00001)
        self.global_step = tf.Variable(0, dtype=tf.int32, trainable=False, name='global_step')
        self.n_classes = 10
        self.skip_step = 1
        self.n_test = 10000
        self.n_train = 50000
        self.training = tf.constant(True, dtype=tf.bool)

    def get_data(self):
        with tf.name_scope('data'):
            train_data, test_data = read_data.get_data(self.batch_size)
            iterator = tf.data.Iterator.from_structure(train_data.output_types, train_data.output_shapes)
            self.img, self.label = iterator.get_next()
            #print(self.img)

            self.train_init = iterator.make_initializer(train_data)
            self.test_init = iterator.make_initializer(test_data)

    def inference(self):
        '''
        conv1 = conv_relu(self.img, 64, 3, 1, 'SAME', 'conv1')
        conv2 = conv_relu(conv1, 64, 3, 1, 'SAME', 'conv2')
        pool1 = maxpool(conv2, 2, 2, 'VALID', 'pool1')

        conv3 = conv_relu(pool1, 128, 3, 1, 'SAME', 'conv3')
        conv4 = conv_relu(conv3, 128, 3, 1, 'SAME', 'conv4')
        pool2 = maxpool(conv4, 2, 2, 'VALID', 'pool2')

        conv5 = conv_relu(pool2, 256, 3, 1, 'SAME', 'conv5')
        conv6 = conv_relu(conv5, 256, 3, 1, 'SAME', 'conv6')
        conv7 = conv_relu(conv6, 256, 3, 1, 'SAME', 'conv7')
        pool3 = maxpool(conv7, 2, 2, 'VALID', 'pool3')

        conv8 = conv_relu(pool3, 512, 3, 1, 'SAME', 'conv8')
        conv9 = conv_relu(conv8, 512, 3, 1, 'SAME', 'conv9')
        conv10 = conv_relu(conv9, 512, 3, 1, 'SAME', 'conv10')
        pool4 = maxpool(conv10, 2, 2, 'VALID', 'pool4')

        conv11 = conv_relu(pool4, 512, 3, 1, 'SAME', 'conv11')
        conv12 = conv_relu(conv11, 512, 3, 1, 'SAME', 'conv12')
        conv13 = conv_relu(conv12, 512, 3, 1, 'SAME', 'conv13')
        pool5 = maxpool(conv13, 2, 2, 'VALID', 'pool5')
        '''


        conv1 = conv_bn_relu(self.img, 64, 3, 1, 'SAME', 'conv1')
        conv2 = conv_bn_relu(conv1, 64, 3, 1, 'SAME', 'conv2')
        pool1 = maxpool(conv2, 2, 2, 'VALID', 'pool1')

        conv3 = conv_bn_relu(pool1, 128, 3, 1, 'SAME', 'conv3')
        conv4 = conv_bn_relu(conv3, 128, 3, 1, 'SAME', 'conv4')
        pool2 = maxpool(conv4, 2, 2, 'VALID', 'pool2')

        conv5 = conv_bn_relu(pool2, 256, 3, 1, 'SAME', 'conv5')
        conv6 = conv_bn_relu(conv5, 256, 3, 1, 'SAME', 'conv6')
        conv7 = conv_bn_relu(conv6, 256, 3, 1, 'SAME', 'conv7')
        pool3 = maxpool(conv7, 2, 2, 'VALID', 'pool3')

        conv8 = conv_bn_relu(pool3, 512, 3, 1, 'SAME', 'conv8')
        conv9 = conv_bn_relu(conv8, 512, 3, 1, 'SAME', 'conv9')
        conv10 = conv_bn_relu(conv9, 512, 3, 1, 'SAME', 'conv10')
        pool4 = maxpool(conv10, 2, 2, 'VALID', 'pool4')

        conv11 = conv_bn_relu(pool4, 512, 3, 1, 'SAME', 'conv11')
        conv12 = conv_bn_relu(conv11, 512, 3, 1, 'SAME', 'conv12')
        conv13 = conv_bn_relu(conv12, 512, 3, 1, 'SAME', 'conv13')
        pool5 = maxpool(conv13, 2, 2, 'VALID', 'pool5')


        feature_dim = pool5.shape[1] * pool5.shape[2] * pool5.shape[3]
        pool5 = tf.reshape(pool5, [-1, feature_dim])
        fc1 = fully_connected(pool5, 4096, 'fc1')
        dropout1 =tf.nn.dropout(tf.nn.relu(fc1), self.keep_prob, name='relu_dropout1')
        fc2 = fully_connected(fc1, 4096, 'fc2')
        dropout2 = tf.nn.dropout(tf.nn.relu(fc2), self.keep_prob, name='relu_dropout2')
        fc3 = fully_connected(dropout2, 1000, 'fc3')
        self.logits = fully_connected(fc3, self.n_classes, 'logits')

    def loss(self):
        with tf.name_scope('loss'):
            cross_entropy = tf.nn.softmax_cross_entropy_with_logits(logits=self.logits, labels=self.label)
            self.loss = tf.reduce_mean(cross_entropy, name='loss')
            self.regu_loss = tf.get_collection(tf.GraphKeys.REGULARIZATION_LOSSES)
            self.full_loss = tf.add_n([self.loss] + self.regu_loss)

    def optimize(self):
        #update_ops = tf.get_collection(tf.GraphKeys.UPDATE_OPS)
        #with tf.control_dependencies(update_ops):
        #self.opt = tf.train.AdamOptimizer(self.learning_rate).minimize(self.loss, global_step=self.global_step)
        ema = tf.train.ExponentialMovingAverage(0.99, self.global_step)
        self.train_ema_op = ema.apply([self.full_loss, self.loss])
        self.train_op = tf.train.MomentumOptimizer(learning_rate=self.learning_rate, momentum=0.9).minimize(self.full_loss, global_step=self.global_step)


    def eval(self):
        with tf.name_scope('perdict'):
            preds = tf.nn.softmax(self.logits)
            correct_preds = tf.equal(tf.argmax(preds, 1), tf.argmax(self.label, 1))
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
                _, _, l, f_l = sess.run([self.train_op, self.train_ema_op, self.loss, self.full_loss])
                #print(l)
                if (step + 1) % self.skip_step == 0:
                    print('Loss at step {0}: {1}, {2}'.format(step + 1, l, f_l))
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
                # self.eval_on_train_set(sess, self.train_init, epoch, step)

if __name__ == '__main__':
    model = vgg_cifar()
    model.build()
    model.train(50)
