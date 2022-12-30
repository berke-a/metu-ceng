from KMeans import KMeans
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

for k in k_values:

    for i in range(10):

        # Calculating the loss for 10 different initializations of the cluster centers for dataset 1 and dataset 2

        # Initializing the minimum loss to a very large value
        min_loss_1 = 9999999
        min_loss_2 = 9999999

        for i in range(10):

            kmeans = KMeans(dataset1, k)

            cluster_centers, clusters, loss = kmeans.run()

            if (loss < min_loss_1):
                min_loss_1 = loss
                #print("Dataset 1: k = ", k, "loss = ", min_loss_1)

            kmeans = KMeans(dataset2, k)

            cluster_centers, clusters, loss = kmeans.run()

            if (loss < min_loss_2):
                min_loss_2 = loss
                #print("Dataset 2: k = ", k, "loss = ", min_loss_2)

        k_min_loss_1[k].append(min_loss_1)
        k_min_loss_2[k].append(min_loss_2)

loss1Means = []
loss2Means = []

# Calculating the mean loss for each k value for dataset 1 and dataset 2
for i in range(2, 11):
    loss1Means.append(np.mean(k_min_loss_1[i]))
    loss2Means.append(np.mean(k_min_loss_2[i]))

print("Mean Loss for Dataset 1: ", loss1Means)
print("Mean Loss for Dataset 2: ", loss2Means)

# Plotting the mean loss for each k value for dataset 1 and dataset 2
plt.plot(k_values, loss1Means, label="Dataset 1")
plt.xlabel("K")
plt.ylabel("Loss")
plt.title("Loss vs K")
plt.legend()
plt.savefig("Kmeans_1.png")

plt.plot(k_values, loss2Means, label="Dataset 2")
plt.xlabel("K")
plt.ylabel("Loss")
plt.title("Loss vs K")
plt.legend()
plt.savefig("Kmeans_1v2.png")

plt.clf()

plt.plot(k_values, loss2Means, label="Dataset 2")
plt.xlabel("K")
plt.ylabel("Loss")
plt.title("Loss vs K")
plt.legend()
plt.savefig("Kmeans_2.png")
