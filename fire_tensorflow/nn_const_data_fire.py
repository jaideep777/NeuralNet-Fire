import os
os.environ['TF_CPP_MIN_LOG_LEVEL']='2'
import tensorflow as tf
from tensorflow.contrib.data import Dataset, Iterator
import numpy as np
from numpy import genfromtxt


#"""

#  This code demonstrates 2 neuron NN
#  
#"""

__learn_rate = 0.001
__batch_size = 10000
__n_steps = 10000


# functions to initialize weights and biases
def weight_variable(shape):
  initial = tf.truncated_normal(stddev=0.5, shape=shape)
  return tf.Variable(initial)

def bias_variable(shape):
  initial = tf.truncated_normal(stddev=0.5, shape=shape)
  return tf.Variable(initial)


def parse_csv(x):
  record_defaults = [[""], [0.0], [0.0], [0.0], [0.0], [0.0], [0.0], [0.0], [0], [0], [0]]
  _, temp, S, fuel,_, _, _, _, nf, _, _  = tf.decode_csv(x, record_defaults=record_defaults, field_delim="\t")
  return [temp,S,fuel], tf.one_hot(nf, depth=2, dtype=tf.float32)


def create_dataset(filename, map_fun, batch_size, rep=1, buffer_size=0):
  dat = tf.contrib.data.TextLineDataset(filename)
  dat = dat.skip(1)
  dat = dat.map(map_fun) 
  dat = dat.repeat(rep)
  if (buffer_size>0): 
    dat = dat.shuffle(buffer_size)  # for each iteration, refills the buffer with new data and chooses random elements to put into batch <-- this behaviour is a bit counterintuitive
  dat = dat.batch(batch_size)  
  return dat  

# ~~~~~~ forward prop ~~~~~~~~~~
def denseNet(x, W1,b1,Wo,bo):
  y1 = tf.nn.sigmoid(tf.matmul(x,W1) + b1)  # first layer neurons with sigmoid activation
#  y2 = tf.nn.sigmoid(tf.matmul(y1,W2) + b2)  # first layer neurons with sigmoid activation
  y = tf.matmul(y1,Wo) + bo
  
  return y



# Prepare training dataset
#dat_train = create_dataset("fireIndex.txt", parse_csv, batch_size=100, rep=1)

print("Reading data...")
my_data = genfromtxt('fireIndex_FMAM.csv', delimiter=',',skip_header=1)
my_data[:,2] = my_data[:,2]/35
my_data[:,4] = my_data[:,4]/300
print("DONE")
xin = tf.cast(my_data[:,(2,3,4)], tf.float32)
yin = tf.one_hot(my_data[:,12], depth=2, dtype=tf.int32)
dat_train = tf.contrib.data.Dataset.from_tensor_slices((xin,yin))
dat_train = dat_train.repeat(1000)
dat_train = dat_train.shuffle(100000)
dat_train = dat_train.batch(__batch_size)

# prepare data iterator
it_handle = tf.placeholder(tf.string, shape=[])

iterator = Iterator.from_string_handle(it_handle, dat_train.output_types, dat_train.output_shapes)

next_batch = iterator.get_next()

training_iterator = dat_train.make_one_shot_iterator()
validation_iterator = dat_train.make_one_shot_iterator()


x  = tf.reshape(next_batch[0], shape=[-1,3])
y_ = tf.reshape(next_batch[1], shape=[-1,2])



# init weights
# Layer 1 : with a single neuron
W1 = weight_variable([3,5])	
b1 = bias_variable([5])

## Layer 1 : with a single neuron
#W2 = weight_variable([3,5])	
#b2 = bias_variable([5])

# output layer
Wo = weight_variable([5,2])	
bo = bias_variable([2])

# net
y = denseNet(x, W1,b1,Wo,bo)

# training operation
cross_entropy = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(labels=y_, logits=y))
#cross_entropy = tf.reduce_mean(-y_*tf.log(y)-(1-y_)*tf.log(1-y))
#ssq_loss = tf.reduce_sum((tf.nn.softmax(y)-y_)**2)
train_op = tf.train.AdamOptimizer(__learn_rate).minimize(cross_entropy)

# evaluation operation
y_soft = tf.nn.softmax(denseNet(x, W1,b1,Wo,bo))
correct_prediction = tf.equal(tf.argmax(y_soft,1), tf.argmax(y_,1))
accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))


with tf.Session() as sess:

  training_handle = sess.run(training_iterator.string_handle())
  validation_handle = sess.run(validation_iterator.string_handle())

  sess.run(tf.global_variables_initializer())

  for i in range(__n_steps):
  #  iterator = dat.make_one_shot_iterator()
    try:
      _,acc,ce = sess.run([train_op, accuracy, cross_entropy], feed_dict={it_handle: training_handle})
#       print(sess.run(x,feed_dict={it_handle: training_handle}))

      if (i % 100 == 0):
        print("train: ",acc,ce)

#        print(sess.run([accuracy, cross_entropy], feed_dict={it_handle: validation_handle}))
        
    except tf.errors.OutOfRangeError:
      print("End of file")
      break

  y_val = sess.run(tf.nn.softmax(denseNet(tf.reshape(xin, [-1,3]),W1,b1,Wo,bo)))
  np.savetxt("y_predic.txt",y_val[:,1],delimiter=" ")
  f = open('weights.txt','w')
  f.write(str(1)+"\n")
  f.write(" ".join(map(str,[1, 2])))
  f.write("\n")

  f.write(str(Wi))+"\n")
  f.write(str(bi))+"\n")
 
  f.write(str(sess.run(W1))+"\n")
  f.write(str(sess.run(b1))+"\n")
 
  f.write(str(sess.run(Wo))+"\n")
  f.write(str(sess.run(bo))+"\n")
  
  f.close()
  
  
  

