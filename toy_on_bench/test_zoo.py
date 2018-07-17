'''
import numpy as np


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

per = [permutation_none, permutation_rot_60, permutation_rot_180, permutation_rot_120, permutation_rot_240, permutation_rot_300, permutation_rot_60_ud, permutation_rot_120_ud, permutation_rot_180_ud, permutation_rot_240_ud, permutation_rot_300_ud]

for permut in per:
    print(permut, np.where(permut == 11)[0][0])
'''

import numpy as np

x = np.zeros((64, 64))

y = np.expand_dims(x, axis=0)

print(y.shape)

z = np.concatenate((y, np.expand_dims(x, axis=0)), axis=0)

print(z.shape)

w = np.transpose(np.reshape(z, (1, 2, 64, 64)), (0, 2, 3, 1)).astype(np.float32)

print(w.shape)

print()
