import os
import math
import numpy as np

def load_file(file_path):
    f = open(file_path)
    vertices = []
    faces = []

    for line in f:
        if line[:3] == "v  ":
            idx1 = line.find(" ") + 1
            idx2 = line.find(" ", idx1 + 1)
            idx3 = line.find(" ", idx2 + 1)

            vertex = (float(line[idx1: idx2]), float(line[idx2: idx3]), float(line[idx3: -1]))
            vertices.append(vertex)
        elif line[0] == "f":
            str = line.replace("//", "/")
            idx1 = str.find(" ") + 1
            idx2 = str.find(" ", idx1 + 1)
            idx3 = str.find(" ", idx2 + 1)

            face = (int(str[idx1: idx2]), int(str[idx2: idx3]), int(str[idx3: -1]))
            faces.append(face)
    f.close()
    return vertices, faces

def get_rotation_matrix(theta_x, theta_y, theta_z):
    mat_x = [
    [1, 0, 0],
    [0, math.cos(theta_x), -math.sin(theta_x)],
    [0, math.sin(theta_x), math.cos(theta_x)]
    ]
    mat_y = [
    [math.cos(theta_y), 0, math.sin(theta_y)],
    [0, 1, 0],
    [-math.sin(theta_y), 0, math.cos(theta_y)]
    ]
    mat_z = [
    [math.cos(theta_z), -math.sin(theta_z), 0],
    [math.sin(theta_z), math.cos(theta_z), 0],
    [0, 0, 1]
    ]

    matrot = np.matmul(np.matmul(mat_x, mat_y), mat_z)
    return matrot

def get_rot_vertices(vertices, rot_mat):
    print(np.shape(vertices), np.shape(rot_mat))
    return np.matmul(vertices, rot_mat)

def write_to_file(file_name, rot_vertices, faces, theta_x, theta_y, theta_z):
    this_name = file_name + "_" + str(theta_x) + "_" + str(theta_y) + "_" + str(theta_z) + ".obj"
    #os.mknod(this_name)
    f = open(this_name, "w")
    for [x, y, z] in rot_vertices:
        f.write("v  " + str(x) + " " + str(y) + " " + str(z) + "\n")
    for [x, y, z] in faces:
        f.write("f " + str(x) + " " + str(y) + " " + str(z) + "\n")

    f.close()

if __name__ == '__main__':
    file_name = "./models/chair2"
    vertices, faces = load_file(file_name + ".obj")
    for theta_x in range(0, 6):
        for theta_y in range(0, 6):
            for theta_z in range(0, 6):
                rot_mat = get_rotation_matrix(theta_x * math.pi / 6, theta_y * math.pi / 6, theta_z * math.pi / 6)
                rot_vertices = get_rot_vertices(vertices, rot_mat)
                write_to_file(file_name, rot_vertices, faces, theta_x, theta_y, theta_z)
