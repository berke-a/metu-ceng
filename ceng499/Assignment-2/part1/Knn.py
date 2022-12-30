import numpy as np
from Distance import Distance


class KNN:
    def __init__(self, dataset, data_label, similarity_function, similarity_function_parameters=None, K=1):
        """
        :param dataset: dataset on which KNN is executed, 2D numpy array
        :param data_label: class labels for each data sample, 1D numpy array
        :param similarity_function: similarity/distance function, Python function
        :param similarity_function_parameters: auxiliary parameter or parameter array for distance metrics
        :param K: how many neighbors to consider, integer
        """
        self.K = K
        self.dataset = dataset
        self.dataset_label = data_label
        self.similarity_function = similarity_function
        self.similarity_function_parameters = similarity_function_parameters

    def predict(self, instance):
        ''' Predict class label for instance '''
        distances = self.calc_distances(instance)
        sorted_indices = np.argsort(distances)
        sorted_labels = self.dataset_label[sorted_indices]
        return np.bincount(sorted_labels[:self.K]).argmax()  # Majority voting

    def calc_distances(self, instance):
        ''' Calculate distances between instance and all data samples in the dataset '''
        tmp = np.shape(self.dataset)[0]
        distances = []
        if (self.similarity_function == Distance.calculateCosineDistance):
            for i in range(tmp):
                distances.append(-1 * self.similarity_function(
                    self.dataset[i], instance))  # Cosine distance is negative of cosine similarity
        else:
            for i in range(tmp):
                distances.append(self.similarity_function(
                    instance, self.dataset[i], self.similarity_function_parameters))
        return distances
