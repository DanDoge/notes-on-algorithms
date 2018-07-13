import cv2
import numpy as np

def get_min_rect(img_path):
    img = cv2.imread(img_path)
    # 灰度图
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # 二值化
    ret, binary = cv2.threshold(gray, 250, 255, cv2.THRESH_BINARY)
    # 反色
    binary = 255 - binary
    # 最小外接矩形
    _, contours, hierarchy = cv2.findContours(binary, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)
    cnt = contours[0]
    min_rect = cv2.minAreaRect(cnt)
    # 旋转
    M = cv2.getRotationMatrix2D(min_rect[0], min_rect[2], 1.0)
    rot = cv2.warpAffine(binary, M, (img.shape[1], img.shape[0]))
    _, contours, hierarchy = cv2.findContours(rot, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)
    cnt = contours[0]
    min_rect = cv2.minAreaRect(cnt)
    box = cv2.boxPoints(min_rect)
    box = np.int0(box)
    cut = rot[box[1][1] : box[3][1], box[0][0] : box[2][0]]
    cv2.imshow("cut", cut)
    cv2.waitKey(0)
    return cut

def get_similarity_from_cut(cut_1, cut_2, size):
    inter = 0
    union = 0
    for i in range(size):
        for j in range(size):
            if cut_1[i][j] != 0:
                union += 1
                if cut_2[i][j] != 0:
                    inter += 1
    return inter / union

def get_similarity_from_img(img_path_1, img_path_2):
    min_rect_1 = get_min_rect(img_path_1)
    min_rect_2 = get_min_rect(img_path_2)
    new_size = max(min_rect_1.shape[:2] + min_rect_2.shape[:2])

    #min_rect_1 = cv2.resize(min_rect_1, (new_size, new_size), interpolation=cv2.INTER_CUBIC)
    top_padding = (new_size - min_rect_1.shape[0]) // 2
    down_padding = new_size - top_padding - min_rect_1.shape[0]
    left_padding = (new_size - min_rect_1.shape[1]) // 2
    right_padding = new_size - left_padding - min_rect_1.shape[1]
    min_rect_1 = cv2.copyMakeBorder(min_rect_1, top_padding, down_padding, left_padding, right_padding, cv2.BORDER_CONSTANT, value=(0, 0, 0))
    #min_rect_2 = cv2.resize(min_rect_2, (new_size, new_size), interpolation=cv2.INTER_CUBIC)
    top_padding = (new_size - min_rect_2.shape[0]) // 2
    down_padding = new_size - top_padding - min_rect_2.shape[0]
    left_padding = (new_size - min_rect_2.shape[1]) // 2
    right_padding = new_size - left_padding - min_rect_2.shape[1]
    min_rect_2 = cv2.copyMakeBorder(min_rect_2, top_padding, down_padding, left_padding, right_padding, cv2.BORDER_CONSTANT, value=(0, 0, 0))

    cv2.imshow("cut_now_1", min_rect_1)
    cv2.waitKey(0)
    cv2.imshow("cut_now_2", min_rect_2)
    cv2.waitKey(0)

    sim_1 = get_similarity_from_cut(min_rect_1, min_rect_2, new_size)
    min_rect_2 = np.rot90(min_rect_2)
    sim_2 = get_similarity_from_cut(min_rect_1, min_rect_2, new_size)
    min_rect_2 = np.rot90(min_rect_2)
    sim_3 = get_similarity_from_cut(min_rect_1, min_rect_2, new_size)
    min_rect_2 = np.rot90(min_rect_2)
    sim_4 = get_similarity_from_cut(min_rect_1, min_rect_2, new_size)
    print(sim_1, sim_2, sim_3, sim_4)
    return min(sim_1, sim_2, sim_3, sim_4)

# test
if __name__ == '__main__':
    print(get_similarity_from_img("./test1.png", "./test2.png"))
