import numpy as np


class KMeans:
    def __init__(self, dataset, K=2):
        """
        :param dataset: 2D numpy array, the whole dataset to be clustered
        :param K: integer, the number of clusters to form
        """
        self.K = K
        self.dataset = dataset
        # each cluster is represented with an integer index
        # self.clusters stores the data points of each cluster in a dictionary
        self.clusters = {i: [] for i in range(K)}
        # self.cluster_centers stores the cluster mean vectors for each cluster in a dictionary
        self.cluster_centers = {i: None for i in range(K)}
        # you are free to add further variables and functions to the class

    def euclidianDistance(self, x, y):
        """Euclidian distance implementation"""
        return np.linalg.norm(x-y)

    def assignCluster(self):
        """Assign each data point to the closest cluster center"""
        N = self.dataset.shape[0]
        self.clusters = {i: [] for i in range(self.K)}
        for i in range(N):
            distances = []
            for j in range(self.K):
                distances.append(self.euclidianDistance(
                    self.dataset[i], self.cluster_centers[j]))
            self.clusters[np.argmin(distances)].append(self.dataset[i])

    def updateClusterCenters(self):
        """Update the cluster centers"""
        for i in range(self.K):
            if len(self.clusters[i]) != 0:
                self.cluster_centers[i] = np.mean(self.clusters[i], axis=0)

    def calculateLoss(self):
        """Loss function implementation of Equation 1"""
        loss = 0
        for k in range(self.K):

            if len(self.clusters[k]) == 0:
                continue
            for i in range(len(self.clusters[k])):
                loss += self.euclidianDistance(self.clusters[k]
                                               [i], self.cluster_centers[k])**2

        return loss

    def compareMeans(self, means):
        """Compare the cluster centers with the given means"""
        for i in range(self.K):
            if not np.array_equal(self.cluster_centers[i], means[i]):
                return False
        return True

    def assignInitialClusterCenters(self):
        """Assign initial cluster centers"""
        N = self.dataset.shape[0]
        for i in range(self.K):
            self.cluster_centers[i] = self.dataset[np.random.randint(
                0, N)].tolist()

    def run(self):
        """Kmeans algorithm implementation"""

        self.assignInitialClusterCenters()

        last_loss = 0
        last_cluster_centers = {i: None for i in range(self.K)}

        while True:
            self.assignCluster()  # Step E
            self.updateClusterCenters()  # Step M

            if self.compareMeans(last_cluster_centers):
                break  # If the cluster centers do not change, stop the algorithm

            last_cluster_centers = self.cluster_centers.copy()

        return self.cluster_centers, self.clusters, self.calculateLoss()
