from KMeansPlusPlus import KMeansPlusPlus
import pickle
import numpy as np
import matplotlib.pyplot as plt


dataset1 = pickle.load(open("../data/part2_dataset_1.data", "rb"))


dataset2 = pickle.load(open("../data/part2_dataset_2.data", "rb"))

k_values = [2, 3, 4, 5, 6, 7, 8, 9, 10]

k_min_loss_1 = {
    2: [],
    3: [],
    4: [],
    5: [],
    6: [],
    7: [],
    8: [],
    9: [],
    10: []
}

k_min_loss_2 = {
    2: [],
    3: [],
    4: [],
    5: [],
    6: [],
    7: [],
    8: [],
    9: [],
    10: []
}

# Same as Kmeansexperiment.py

for k in k_values:

    for i in range(10):

        min_loss_1 = 9999999
        min_loss_2 = 9999999

        for i in range(10):

            kmeans = KMeansPlusPlus(dataset1, k)

            cluster_centers1, clusters1, loss1 = kmeans.run()

            if (loss1 < min_loss_1):
                min_loss_1 = loss1

            kmeans = KMeansPlusPlus(dataset2, k)

            cluster_centers2, clusters2, loss2 = kmeans.run()

            if (loss2 < min_loss_2):
                min_loss_2 = loss2

        k_min_loss_1[k].append(min_loss_1)
        k_min_loss_2[k].append(min_loss_2)


loss1Means = []
loss2Means = []

for i in range(2, 11):
    loss1Means.append(np.mean(k_min_loss_1[i]))
    loss2Means.append(np.mean(k_min_loss_2[i]))

print("Dataset 1: ", loss1Means)
print("Dataset 2: ", loss2Means)

plt.plot(k_values, loss1Means, label="Dataset 1")
plt.xlabel("K")
plt.ylabel("Loss")
plt.title("Loss vs K")
plt.legend()
plt.savefig("KmeansPP_1.png")

plt.plot(k_values, loss2Means, label="Dataset 2")
plt.xlabel("K")
plt.ylabel("Loss")
plt.title("Loss vs K")
plt.legend()
plt.savefig("KmeansPP_1v2.png")

plt.clf()

plt.plot(k_values, loss2Means, label="Dataset 2")
plt.xlabel("K")
plt.ylabel("Loss")
plt.title("Loss vs K")
plt.legend()
plt.savefig("KmeansPP_2.png")
