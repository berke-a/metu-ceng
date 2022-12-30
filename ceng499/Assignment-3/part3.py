import numpy as np
from DataLoader import DataLoader
from sklearn.preprocessing import MinMaxScaler
from sklearn.model_selection import RepeatedStratifiedKFold, GridSearchCV
from sklearn.neighbors import KNeighborsClassifier
from sklearn.svm import SVC
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import f1_score, make_scorer


data_path = "../data/credit.data"

dataset, labels = DataLoader.load_credit_with_onehot(data_path)

# Hyperparameter configurations for KNN, SVM, Decision Tree, and Random Forest
# 2 configurations for each algorithm
configurations = {
    'knn': {
        'n_neighbors': [1, 5],
        'weights': ['uniform', 'distance']
    },
    'svm': {
        'kernel': ['linear', 'sigmoid'],
        'C': [1, 5]
    },
    'dt': {
        'criterion': ['gini', 'entropy'],
        'max_depth': [1, 5]
    },
    'rf': {
        'n_estimators': [10, 50],
        'criterion': ['gini', 'entropy'],
        'max_depth': [1, 5]
    },
}

# Scale the dataset
scaler = MinMaxScaler()
dataset = scaler.fit_transform(dataset, labels)

# Initialize mean results dictionary for each parameter config
mean_results = {}
for k in configurations.keys():
    mean_results[k] = {
        'config': [],
        'f1': [],
    }

# Create f1 scorer
f1 = make_scorer(f1_score, average='binary')

# outer cross-validation with 3 splits, 5 repetations with shuffle
outer_cv = RepeatedStratifiedKFold(n_splits=3, n_repeats=5)
split_dataset = outer_cv.split(dataset, labels)

# For printing the progress
j = 0

for train_index, test_index in split_dataset:

    # inner cross-validation with 5 splits and 5 repetations with shuffle
    inner_cv = RepeatedStratifiedKFold(n_splits=5, n_repeats=5)

    # split the dataset into train and test
    X_train, X_test = dataset[train_index], dataset[test_index]
    y_train, y_test = labels[train_index], labels[test_index]

    # KNN
    knn_model = GridSearchCV(KNeighborsClassifier(), configurations['knn'], cv=inner_cv,
                             scoring=f1, n_jobs=-1)
    knn_model.fit(X_train, y_train)

    # svm
    svm_model = GridSearchCV(SVC(), configurations['svm'], cv=inner_cv,
                             scoring=f1, n_jobs=-1)
    svm_model.fit(X_train, y_train)

    # Decision Tree
    dt_model = GridSearchCV(DecisionTreeClassifier(), configurations['dt'], cv=inner_cv,
                            scoring=f1, n_jobs=-1)
    dt_model.fit(X_train, y_train)

    # Random Forest
    max_f1 = 0
    for i in range(5):
        # Random Forest with different random states
        rf_model = GridSearchCV(RandomForestClassifier(), configurations['rf'], cv=inner_cv,
                                scoring=f1, n_jobs=-1)
        rf_model.fit(X_train, y_train)
        # Keep the best model
        if rf_model.best_score_ >= max_f1:
            max_f1 = rf_model.best_score_
            best_rf_model = rf_model

    # Evaluate the models on the test set
    knn_f1 = f1_score(y_test, knn_model.predict(X_test))
    svm_f1 = f1_score(y_test, svm_model.predict(X_test))
    dt_f1 = f1_score(y_test, dt_model.predict(X_test))
    rf_f1 = f1_score(y_test, best_rf_model.predict(X_test))

    # Update the mean results dictionary
    mean_results['knn']['config'].append(knn_model.best_params_)
    mean_results['knn']['f1'].append(knn_f1)

    mean_results['svm']['config'].append(svm_model.best_params_)
    mean_results['svm']['f1'].append(svm_f1)

    mean_results['dt']['config'].append(dt_model.best_params_)
    mean_results['dt']['f1'].append(dt_f1)

    mean_results['rf']['config'].append(best_rf_model.best_params_)
    mean_results['rf']['f1'].append(rf_f1)

    print("Progress: %.2f" % (j / 15 * 100), "%")
    j += 1

# Print the mean results
for k in mean_results.keys():
    print(k)
    print("Mean F1: %.3f" % (np.mean(mean_results[k]['f1']) * 100))
    print("Confidence Interval: %.3f" %
          ((1.96 * np.std(mean_results[k]['f1']) / np.sqrt(15)) * 100))
    print("Best Config: ", mean_results[k]['config']
          [np.argmax(mean_results[k]['f1'])])
    print()
