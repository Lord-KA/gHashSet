import matplotlib.pyplot as plt
size = 250
students_id = [i for i in range(size)]
data = [0] * size

for i in range(size):
    data[i] = int(input())

plt.bar(students_id, data)
plt.show()
