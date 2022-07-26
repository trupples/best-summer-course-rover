import requests
import socket
import time

ROBOT_HOSTNAME = "rover3.local"

BASE_URL = "http://" + socket.gethostbyname_ex(ROBOT_HOSTNAME)[2][0]
print(f"Resolved {ROBOT_HOSTNAME} to {BASE_URL}")

def set_motors(left, right):
	requests.get(f"{BASE_URL}/command?left={float(left):.2}&right={float(right):.2}")

for i in range(5): # repeat 5 times
	set_motors(+1, +1)
	time.sleep(0.1)
	set_motors(-0.7, -0.7)
	time.sleep(0.5)
	set_motors(-0.7, +0.7)
	time.sleep(0.5)
	set_motors(+0.7, -0.7)
	time.sleep(0.5)
	set_motors( 0.0,  0.0)
