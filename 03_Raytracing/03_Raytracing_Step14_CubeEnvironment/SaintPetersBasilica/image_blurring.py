import numpy as np
import cv2


original = cv2.imread("posy.jpg")
x = 40
img = cv2.GaussianBlur(original, (x * 2 + 1, x * 2 + 1), 0)

cv2.imwrite("posy_blurred.jpg", img)
