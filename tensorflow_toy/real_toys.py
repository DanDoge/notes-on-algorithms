import tensorflow as tf
import time
import os

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'

x = [1, 2, 3, 4, 5]
y = [1, 2, 3, 4, 5]
a = tf.constant(2, name='name') # 'name' to appear in tensorboard
b = tf.constant([[0, 1], [2, 3]], name='b')
c = tf.constant([2, 3], name='c')
mul_op = tf.multiply(b, c)

s = tf.Variable(2, name='s')
s = tf.get_variable("scalar", initializer=tf.constant(2))
w = tf.get_variable("big_matrix", shape=(784, 10), initializer=tf.zeros_initializer())

# tf.zero()
# tf.zero_like() ==> similar to numpy
# tf.fill(dims,value, name='')

# tf.random_normal
# tf.truncated_normal
# tf.random_shuffle
# tf.

writer = tf.summary.FileWriter('./graphs', tf.get_default_graph());
writer.close()

# w = tf.Variable(10)
# assign_op = w.assign(100)
# with tf.Session() as sess:
#     sess.run(assign_op) # otherwise w.eval() returns 10, w.assign() returns a operator, you have to run it

# with tf.Graph.control_dependencies([a, b, c]):
#     d = ... # d must be executed after a, b, and c

with tf.Session() as sess:
    sess.run(tf.global_variables_initializer()) # a must, if you use 'get_variable()'
    #print(sess.run(mul_op));
    print(sess.graph.as_graph_def())


a = tf.placeholder(tf.float32, shape=[3])
b = tf.constant([1, 2, 3], tf.float32)

c = a + b

with tf.Session() as sess:
    print(sess.run(c, feed_dict={a: [1, 2, 3]}))
