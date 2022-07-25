import requests
import cv2
import socket
import time
import sys
import numpy as np

ROBOT_HOSTNAME = sys.argv[1]

# Cache the IP address of the robot so we don't do a lenghty mDNS resolution for every single request. That turns out to be extremely slow (1 second resolve time-ish)
BASE_URL = "http://" + socket.gethostbyname_ex(ROBOT_HOSTNAME)[2][0]
print(f"Resolved {ROBOT_HOSTNAME} to {BASE_URL}")

def get_image():
	img_data = requests.get(f"{BASE_URL}/capture.jpg").content
	return cv2.imdecode(np.asarray(bytearray(img_data), dtype='uint8'), cv2.IMREAD_COLOR)

# Image reprojection calibration
## World region of interest in millimeters
world_roi_x_min = -20
world_roi_x_max = +20
world_roi_y_min = 0
world_roi_y_max = 75

# 10 pixels per millimeter resolution:
proj_w = (world_roi_x_max - world_roi_x_min) * 10
proj_h = (world_roi_y_max - world_roi_y_min) * 10

# Reference points in world-space
proj_points = [[-10, 20],
[-5, 20],
[0, 20],
[5, 20],
[10, 20],
[-10, 15],
[-5, 15],
[0, 15],
[5, 15],
[10, 15],
[-5, 10],
[0, 10],
[5, 10]]

# Map them to image-space
for i in range(13):
	proj_points[i] = (
		(proj_points[i][0] - world_roi_x_min) / (world_roi_x_max - world_roi_x_min) * proj_w,
		proj_h - (proj_points[i][1] - world_roi_y_min) / (world_roi_y_max - world_roi_y_min) * proj_h)

# Reference points in the horizon-cropped image:
horizon = 180
cam_points = [[68, 288-horizon],
[190, 290-horizon],
[305, 287-horizon],
[422, 284-horizon],
[537, 284-horizon],
[17, 329-horizon],
[161, 327-horizon],
[304, 324-horizon],
[447, 324-horizon],
[591, 322-horizon],
[116, 394-horizon],
[305, 388-horizon],
[492, 385-horizon]]

hom = cv2.findHomography(np.array(cam_points), np.array(proj_points))[0]

def find_largest_object(im):
	h, w = im.shape
	best_num = 0
	left, top, right, bottom = 0, 0, 0, 0

	for i in range(h):
		for j in range(w):
			current_left = w
			current_right = 0
			current_bottom = h
			current_top = 0
			num = 0

			q = [(i, j)]
			while q:
				ii, jj = q.pop(0)

				if ii < 0 or ii >= h or jj < 0 or jj >= w or im[ii, jj] == 0:
					continue

				im[ii, jj] = 0
				q.append((ii+1, jj))
				q.append((ii-1, jj))
				q.append((ii, jj+1))
				q.append((ii, jj-1))
				num += 1

				if jj < current_left:
					current_left = jj
				if jj > current_right:
					current_right = jj
				if ii < current_bottom:
					current_bottom = ii
				if ii > current_top:
					current_top = ii

			if num > best_num:
				best_num = num
				left = current_left
				right = current_right
				top = current_top
				bottom = current_bottom

	return left, top, right, bottom, best_num

while True:
	im = get_image()
	cv2.imshow("Rover camera", im)
	horizon = 180
	sky = im[:horizon, :]
	ground = im[horizon:, :]

	sky = cv2.GaussianBlur(sky, (3, 3), 3)

	hsv = cv2.cvtColor(sky, cv2.COLOR_BGR2HSV)
	green_low = (60, 64, 100)
	green_high = (80, 200, 255)
	green_mask = cv2.inRange(hsv, green_low, green_high)
	cv2.imshow("Green pixels", green_mask)

	left, top, right, bottom, n = find_largest_object(green_mask.copy())
	cv2.rectangle(sky, (left, top), (right, bottom), (255, 255, 255), 2)
	# find_largest_object is one of our functions, not a builtin

	cv2.imshow("Sky", sky)

	ground = cv2.warpPerspective(ground, hom, (proj_w, proj_h))

	cv2.imshow("Ground", ground)

	# time.sleep(0.05)

	if cv2.waitKey(1) & 0xFF == ord('q'): 
		break
