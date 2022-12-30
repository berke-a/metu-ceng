import pickle
from Distance import Distance
from Knn import KNN
import numpy as np
from sklearn.model_selection import StratifiedKFold
from sklearn.metrics import accuracy_score
from tabulate import tabulate

dataset, labels = pickle.load(open("../data/part1_dataset.data", "rb"))


def calculateConfidence(accuracy):
    error = 1 - (accuracy/100)
    confidence_interval = 100*(1.96 *
                               (error * (1 - error) / label_len)**0.5)
    return confidence_interval


# Hyperparameters
K = [5, 10, 15]
simFunctions = [Distance.calculateCosineDistance,
                Distance.calculateMinkowskiDistance, Distance.calculateMahalanobisDistance]

# Stratified K-Fold
fold = StratifiedKFold(n_splits=10, shuffle=True)


label_len = len(labels)*0.1

max_accuracy = 0
best_configuration = [None, None]  # [similarity_function, K]

table = []
table_index = 0

for k in K:

    for funcs in simFunctions:

        # Initialize Table for each configuration
        if (funcs == Distance.calculateMahalanobisDistance):
            table.append(
                ["K = " + str(k) + ", similarity_function = mahalanobis"])

        if (funcs == Distance.calculateMinkowskiDistance):
            table.append(
                ["K = " + str(k) + ", similarity_function = minkowski"])

        if (funcs == Distance.calculateCosineDistance):
            table.append(
                ["K = " + str(k) + ", similarity_function = cosine"])

        # Additional parameters for Mahalanobis Distance
        if (funcs == Distance.calculateMahalanobisDistance):
            simFuncParams = np.linalg.inv(np.cov(dataset, rowvar=False))
        else:
            simFuncParams = None

        config_accuracy = 0

        for i in range(1, 6):

            train_accuracy = 0

            # Split the dataset into 10 folds
            for train_indices, test_indices in fold.split(dataset, labels):

                current_train = dataset[train_indices]
                current_train_label = labels[train_indices]

                knn = KNN(current_train, current_train_label,
                          funcs, simFuncParams, k)
                current_test = dataset[test_indices]
                current_test_labels = labels[test_indices]

                # Predict
                predicted = []
                for i in range(len(current_test)):
                    predicted.append(knn.predict(current_test[i]))

                # Calculate accuracy
                accuracy = accuracy_score(
                    current_test_labels, predicted)*100

                train_accuracy += accuracy

            config_accuracy += train_accuracy/10  # 10 fold cross validation

        config_accuracy = config_accuracy/5  # 5 iteration
        confidence_interval = calculateConfidence(config_accuracy)

        # Add to table
        table[table_index].append(
            str(config_accuracy) + " ± " + str(confidence_interval))
        table_index += 1

        if (config_accuracy > max_accuracy):
            max_accuracy = config_accuracy
            best_configuration = [funcs, k]


# Print the best configuration
print("Best configuration : ", best_configuration,
      "accuracy : ", max_accuracy, " - confidence interval : ", confidence_interval)

# Saves accuracy table with different parameters + confidence interval
with open('accuracy.txt', 'w') as f:
    f.write(tabulate(table, headers=[
        'K and Similarity Function', 'Accuracy and Confidence Interval'], tablefmt="rounded_grid"))
