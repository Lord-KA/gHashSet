import matplotlib.pyplot as plt
size = 250
students_id = [i for i in range(size)]
data = [i for i in range(size)]

for i in range(size):
    data[i] = int(input())

plt.scatter(students_id, data)
plt.show()

