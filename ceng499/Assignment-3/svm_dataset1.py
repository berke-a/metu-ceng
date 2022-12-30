import pickle
import numpy as np

from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import StandardScaler
from sklearn.svm import SVC

import matplotlib.pyplot as plt
from sklearn.inspection import DecisionBoundaryDisplay

dataset, labels = pickle.load(open("../data/part2_dataset1.data", "rb"))

kernel_values = ['linear', 'sigmoid']
c_values = [1, 5]


# Following function is taken from https://scikit-learn.org/stable/auto_examples/svm/plot_iris_svc.html
def display_decision_boundary(clf, dataset, labels, title, ax):
    # Plot the decision boundary
    X0, X1 = dataset[:, 0], dataset[:, 1]
    disp = DecisionBoundaryDisplay.from_estimator(
        clf,
        dataset,
        response_method="predict",
        cmap=plt.cm.coolwarm,
        alpha=0.8,
        ax=ax,
        xlabel=labels[0],
        ylabel=labels[1],
    )
    ax.scatter(X0, X1, c=labels, cmap=plt.cm.coolwarm, s=20, edgecolors="k")
    ax.set_xticks(())
    ax.set_yticks(())
    ax.set_title(title)


for kernel in kernel_values:
    for c in c_values:
        # Create a classifier
        clf = SVC(kernel=kernel, C=c)
        clf.fit(dataset, labels)
        print("Trying configuration => kernel: ", kernel, " C: ", c)

        # Plot the decision boundary
        display_decision_boundary(
            clf, dataset, labels, "SVM with kernel: " + kernel + " and C: " + str(c), plt.gca())
        plt.savefig("svm_dataset1_" + kernel + "_" + str(c) + ".png")
        plt.clf()
