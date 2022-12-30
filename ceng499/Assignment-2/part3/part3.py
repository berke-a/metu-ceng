import pickle
from sklearn.cluster import AgglomerativeClustering
from sklearn.metrics import silhouette_samples, silhouette_score
from scipy.cluster.hierarchy import dendrogram
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import numpy as np


dataset = pickle.load(open("../data/part3_dataset.data", "rb"))


# A function to plot the dendrogram taken from the scikit-learn documentation
def plot_dendrogram(model,  K, affinity, linkage, **kwargs):
    # Create linkage matrix and then plot the dendrogram

    # create the counts of samples under each node
    counts = np.zeros(model.children_.shape[0])
    n_samples = len(model.labels_)
    for i, merge in enumerate(model.children_):
        current_count = 0
        for child_idx in merge:
            if child_idx < n_samples:
                current_count += 1  # leaf node
            else:
                current_count += counts[child_idx - n_samples]
        counts[i] = current_count

    linkage_matrix = np.column_stack(
        [model.children_, model.distances_, counts]
    ).astype(float)

    # Plot the corresponding dendrogram
    dendrogram(linkage_matrix, **kwargs)
    plt.savefig("part3_dendrogram_{}_{}_{}.png".format(K, affinity, linkage))
    plt.clf()


# Majority of the code below taken from the scikit-learn documentation
def plot_silhouette(cluster_labels, n_clusters, affinity, linkage):
    fig, ax1 = plt.subplots(1, 1)
    fig.set_size_inches(18, 7)

    # The silhouette coefficient can range from -1, 1
    ax1.set_xlim([-1, 1])
    # The (n_clusters+1)*10 is for inserting blank space between silhouette
    # plots of individual clusters, to demarcate them clearly.
    ax1.set_ylim([0, len(dataset) + (n_clusters + 1) * 10])

    silhouette_avg = silhouette_score(dataset, clusters.labels_)
    print(
        "For the following config = [K = {}, affinity = {}, linkage = {}], the average silhouette_score is : {}".format(
            n_clusters, affinity, linkage, silhouette_avg
        )
    )

    # Compute the silhouette scores for each sample
    sample_silhouette_values = silhouette_samples(dataset, cluster_labels)

    y_lower = 10
    for i in range(n_clusters):
        # Aggregate the silhouette scores for samples belonging to
        # cluster i, and sort them
        ith_cluster_silhouette_values = sample_silhouette_values[cluster_labels == i]

        ith_cluster_silhouette_values.sort()

        size_cluster_i = ith_cluster_silhouette_values.shape[0]
        y_upper = y_lower + size_cluster_i

        color = cm.nipy_spectral(float(i) / n_clusters)
        ax1.fill_betweenx(
            np.arange(y_lower, y_upper),
            0,
            ith_cluster_silhouette_values,
            facecolor=color,
            edgecolor=color,
            alpha=0.7,
        )

        # Label the silhouette plots with their cluster numbers at the middle
        ax1.text(-0.05, y_lower + 0.5 * size_cluster_i, str(i))

        # Compute the new y_lower for next plot
        y_lower = y_upper + 10  # 10 for the 0 samples

    ax1.set_title("The silhouette plot for the various clusters.")
    ax1.set_xlabel("The silhouette coefficient values")
    ax1.set_ylabel("Cluster label")

    # The vertical line for average silhouette score of all the values
    ax1.axvline(x=silhouette_avg, color="red", linestyle="--")

    ax1.set_yticks([])  # Clear the yaxis labels / ticks
    ax1.set_xticks([-0.1, 0, 0.2, 0.4, 0.6, 0.8, 1])

    plt.savefig("part3_{}_{}_{}.png".format(n_clusters, affinity, linkage))
    plt.clf()


# Maximum silhouette averages for each configuration
max_silhouette = [-1, -1, -1, -1]

# K values with maximum silhouette averages for each configuration
optimum_K = [0, 0, 0, 0]

for K in range(2, 6):

    # Creating the models with different configurations of affinity and linkage for each K

    """Euclidean Distance, Single Linkage"""
    model = AgglomerativeClustering(
        n_clusters=K, affinity='euclidean', linkage='single', compute_distances=True)
    clusters = model.fit(dataset)
    silhouette_avg = silhouette_score(dataset, clusters.labels_)
    # Finding the maximum silhouette average and the corresponding K value for each configuration
    if silhouette_avg > max_silhouette[0]:
        max_silhouette[0] = silhouette_avg
        optimum_K[0] = K
    # Plot the dendrogram and silhouette plots
    plot_silhouette(clusters.labels_,
                    K, 'euclidean', 'single')
    plot_dendrogram(clusters, K=K, affinity='euclidean',
                    linkage='single', truncate_mode='level', p=3)

    # Follwing segment is not commented as it is the same as above

    """Cosine Distance, Single Linkage"""
    model = AgglomerativeClustering(
        n_clusters=K, affinity='cosine', linkage='single', compute_distances=True)
    clusters = model.fit(dataset)
    silhouette_avg = silhouette_score(dataset, clusters.labels_)
    if silhouette_avg > max_silhouette[1]:
        max_silhouette[1] = silhouette_avg
        optimum_K[1] = K
    plot_silhouette(clusters.labels_,
                    K, 'cosine', 'single')
    plot_dendrogram(clusters, K=K, affinity='cosine',
                    linkage='single', truncate_mode='level', p=3)

    """Euclidean Distance, Complete Linkage"""
    model = AgglomerativeClustering(
        n_clusters=K, affinity='euclidean', linkage='complete', compute_distances=True)
    clusters = model.fit(dataset)
    silhouette_avg = silhouette_score(dataset, clusters.labels_)
    if silhouette_avg > max_silhouette[2]:
        max_silhouette[2] = silhouette_avg
        optimum_K[2] = K
    plot_silhouette(clusters.labels_,
                    K,  'euclidean', 'complete')
    plot_dendrogram(clusters, K=K, affinity='euclidean',
                    linkage='complete', truncate_mode='level', p=3)

    """Cosine Distance, Complete Linkage"""
    model = AgglomerativeClustering(
        n_clusters=K, affinity='cosine', linkage='complete', compute_distances=True)
    clusters = model.fit(dataset)
    silhouette_avg = silhouette_score(dataset, clusters.labels_)
    if silhouette_avg > max_silhouette[3]:
        max_silhouette[3] = silhouette_avg
        optimum_K[3] = K
    plot_silhouette(clusters.labels_,
                    K,  'cosine', 'complete')
    plot_dendrogram(clusters, K=K, affinity='cosine',
                    linkage='complete', truncate_mode='level', p=3)


# Printing the maximum silhouette averages and the corresponding K values for each configuration
print("Optimum K values for each configuration:" + str(optimum_K))
print("Maximum silhouette averages for each configuration:" + str(max_silhouette))
