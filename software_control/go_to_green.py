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
	print(len(img_data))
	if len(img_data) < 1000:
		print(img_data)
	return cv2.imdecode(np.asarray(bytearray(img_data), dtype='uint8'), cv2.IMREAD_COLOR)

def get_sensor():
	return requests.get(BASE_URL + "/sensors").json().get("ultrasonic")

def set_motors(left, right):
	requests.get(f"{BASE_URL}/command?left={left:.2}&right={right:.2}")

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

if __name__ == '__main__':
	while True:
		im = get_image()
		h, w, _ = im.shape

		# image preprocessing: blur
		im = cv2.GaussianBlur(im, (3, 3), 3)

		# convert RGB -> HSV
		hsv = cv2.cvtColor(im, cv2.COLOR_BGR2HSV)

		# extract green pixels
		green_parts = cv2.inRange(hsv, (40, 80, 80), (100, 255, 200)) 

		# merge nearby objects with a blur + threshold
		green_parts = cv2.GaussianBlur(green_parts, (31, 31), 31)
		ret, green_parts = cv2.threshold(green_parts, 1, 255, cv2.THRESH_BINARY)

		# find the largest object
		left, top, right, bottom, n = find_largest_object(green_parts.copy())

		# mark it on the image
		cv2.rectangle(im, (left, top), (right, bottom), (255, 255, 255), 2)

		# display the images
		cv2.imshow("Camera feed", im)
		cv2.imshow("Green parts", green_parts)

		# Program will terminate when 'q' key is pressed 
		if cv2.waitKey(1) & 0xFF == ord('q'): 
			break

		# Driving logic!
		print(n)
		if n > 3000:
			if n > 50000:
				print("At target :D")
				continue

			x = (left + right) / 2
			if x < w / 3:
				print("LEFT!")
				set_motors(-0.8, 0.5)
				time.sleep(0.1)
				set_motors(0., 0.)
				time.sleep(0.1)
			elif x > 2 * w / 3:
				print("RIGHT!")
				set_motors(0.5, -0.8)
				time.sleep(0.1)
				set_motors(0., 0.)
				time.sleep(0.1)
			else:
				print("FORWARD!")
				set_motors(0.8, 0.8)
				time.sleep(0.1)
				set_motors(0., 0.)
				time.sleep(0.1)

		time.sleep(0.01)

	cv2.destroyAllWindows()