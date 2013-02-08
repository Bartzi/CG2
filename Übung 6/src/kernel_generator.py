import random
import math

samples = []
noise = []

for i in range(0, 16):
	x = random.uniform(-1, 1)
	y = random.uniform(-1, 1)
	z = random.random()

	a = math.sqrt(math.pow(x, 2) + math.pow(y, 2) + math.pow(z, 2))

	samples.append([x / a, y / a , z / a])
	
	x = random.uniform(-1, 1)
	y = random.uniform(-1, 1)
	noise.append([x / a, y / a, 0])

for i in range(0, 16):
	print("vec3({0}, {1}, {2}),".format(samples[i][0], samples[i][1], samples[i][2]))

for i in range(0, 16):
	print("vec3({0}, {1}, {2}),".format(noise[i][0], noise[i][1], noise[i][2]))
	