import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import numpy as np
import os

path = "./models"

files = os.listdir(path)

az = ["0", "60", "120", "180", "240", "300"]
el = ["-60", "-20", "20", "60"]

for file in files:
    if not os.path.isdir(file):
        input_data = []
        for a in az:
            for e in el:
                this_file_name = file + "_" + a + "_" + e + ".jpg"
                this_channel = mpimg.imread(this_file_name)
                input_data.append(this_channel)
        print(np.shape(input_data)) # 24 * w * h, 之后用vgg-cifar10的转置手段就可以了
        exit()
