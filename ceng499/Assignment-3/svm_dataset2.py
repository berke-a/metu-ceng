import pickle
import numpy as np

from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import StratifiedKFold, GridSearchCV
from sklearn.svm import SVC

dataset, labels = pickle.load(open("../data/part2_dataset2.data", "rb"))

# 10-fold cross validation
fold = StratifiedKFold(n_splits=10, shuffle=True)

# Parameters to be tested
parameters = {'kernel': ['linear', 'sigmoid'], 'C': [1, 5]}


# Initialize mean results dictionary for each parameter config
mean_results = {}
for k in parameters['kernel']:
    for c in parameters['C']:
        mean_results[k + "_" + str(c)] = []


for i in range(5):

    # Scale the dataset
    scaler = StandardScaler()
    dataset = scaler.fit_transform(dataset, labels)

    # Perform grid search
    clf = GridSearchCV(SVC(), parameters, cv=fold,
                       scoring='accuracy', n_jobs=-1)
    clf.fit(dataset, labels)

    # Store the mean results for each parameter config
    for j in range(4):
        kernel = clf.cv_results_['params'][j]['kernel']
        c = clf.cv_results_['params'][j]['C']
        mean_results[kernel + "_" + str(c)].append(
            clf.cv_results_['mean_test_score'][j])

# Print the mean results and confidence intervals for each parameter config
for k in parameters['kernel']:
    for c in parameters['C']:
        mean = np.mean(mean_results[k + "_" + str(c)])
        std = np.std(mean_results[k + "_" + str(c)])
        confidence = 1.96 * std / np.sqrt(len(mean_results[k + "_" + str(c)]))
        print("Mean accuracy for kernel: ", k, " C: ", c, " is: %.3f" % (mean*100), "%",
              " with confidence interval: %.3f" % (confidence*100), "%")
