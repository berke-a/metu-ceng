import numpy as np
# In the decision tree, non-leaf nodes are going to be represented via TreeNode


class TreeNode:
    def __init__(self, attribute):
        self.attribute = attribute
        # dictionary, k: subtree, key (k) an attribute value, value is either TreeNode or TreeLeafNode
        self.subtrees = {}


# In the decision tree, leaf nodes are going to be represented via TreeLeafNode


class TreeLeafNode:
    def __init__(self, data, label):
        self.data = data
        self.labels = label


class DecisionTree:
    def __init__(self, dataset: list, labels, features, criterion="information gain"):
        """
        :param dataset: array of data instances, each data instance is represented via an Python array
        :param labels: array of the labels of the data instances
        :param features: the array that stores the name of each feature dimension
        :param criterion: depending on which criterion ("information gain" or "gain ratio") the splits are to be performed
        """
        self.dataset = dataset
        self.labels = labels
        self.features = features
        self.criterion = criterion
        # it keeps the root node of the decision tree
        self.root = None

        # further variables and functions can be added...

    def print_tree(self):
        """
        This function prints the decision tree
        """
        self.print_tree__(self.root, 0)

    def print_tree__(self, node, level):
        """
        Recursive function for printing the decision tree
        :param node: the current node
        :param level: the current level of the tree
        """
        if isinstance(node, TreeLeafNode):
            print("      " * level, "->", node.labels[0])
        else:
            for key in node.subtrees:
                print()
                print("------" * level + node.attribute, "=", key)
                self.print_tree__(node.subtrees[key], level + 1)

    def calculate_entropy__(self, dataset, labels):
        """
        :param dataset: array of the data instances
        :param labels: array of the labels of the data instances
        :return: calculated entropy value for the given dataset
        """
        entropy_value = 0.0

        """
        Entropy calculations
        """

        values, counts = np.unique(labels, return_counts=True)

        for i in range(len(values)):
            entropy_value += (-(counts[i] / len(labels))
                              * np.log2(counts[i] / len(labels)))

        return entropy_value

    def calculate_average_entropy__(self, dataset, labels, attribute):
        """
        :param dataset: array of the data instances on which an average entropy value is calculated
        :param labels: array of the labels of those data instances
        :param attribute: for which attribute an average entropy value is going to be calculated...
        :return: the calculated average entropy value for the given attribute
        """
        average_entropy = 0.0
        """
            Average entropy calculations
        """
        idx = self.features.index(attribute)
        np_d = np.array(dataset)
        # get unique values and counts
        values, counts = np.unique(np_d[:, idx], return_counts=True)

        for i in range(len(values)):
            # create sub dataset and sub labels for each unique value
            sub_dataset = []
            sub_labels = []
            for j in range(len(dataset)):
                if dataset[j][idx] == values[i]:
                    sub_dataset.append(dataset[j])
                    sub_labels.append(labels[j])
            average_entropy += (counts[i] / len(labels)
                                * self.calculate_entropy__(sub_dataset, sub_labels))

        return average_entropy

    def calculate_information_gain__(self, dataset, labels, attribute):
        """
        :param dataset: array of the data instances on which an information gain score is going to be calculated
        :param labels: array of the labels of those data instances
        :param attribute: for which attribute the information gain score is going to be calculated...
        :return: the calculated information gain score
        """
        information_gain = 0.0
        """
            Information gain calculations
        """
        # information gain = entropy - average entropy
        information_gain = self.calculate_entropy__(dataset, labels) - self.calculate_average_entropy__(
            dataset, labels, attribute
        )
        return information_gain

    def calculate_intrinsic_information__(self, dataset, labels, attribute):
        """
        :param dataset: array of data instances on which an intrinsic information score is going to be calculated
        :param labels: array of the labels of those data instances
        :param attribute: for which attribute the intrinsic information score is going to be calculated...
        :return: the calculated intrinsic information score
        """
        intrinsic_info = None
        """
            Intrinsic information calculations for a given attribute
        """

        idx = self.features.index(attribute)
        dataset = np.array(dataset)
        values, counts = np.unique(dataset[:, idx], return_counts=True)
        intrinsic_info = 0.0
        for i in range(len(values)):
            intrinsic_info += (-(counts[i] / len(labels))
                               * np.log2(counts[i] / len(labels)))

        return intrinsic_info

    def calculate_gain_ratio__(self, dataset, labels, attribute):
        """
        :param dataset: array of data instances with which a gain ratio is going to be calculated
        :param labels: array of labels of those instances
        :param attribute: for which attribute the gain ratio score is going to be calculated...
        :return: the calculated gain ratio score
        """
        """
            Your implementation
        """
        # gain ratio = information gain / intrinsic information
        return (self.calculate_information_gain__(dataset, labels, attribute)/self.calculate_intrinsic_information__(dataset, labels, attribute))

    def ID3__(self, dataset, labels, used_attributes):
        """
        Recursive function for ID3 algorithm
        :param dataset: data instances falling under the current  tree node
        :param labels: labels of those instances
        :param used_attributes: while recursively constructing the tree, already used labels should be stored in used_attributes
        :return: it returns a created non-leaf node or a created leaf node
        """
        """
            Your implementation
        """

        gain_ = float(-1)
        attribute_ = 'None'

        for i in range(len(self.features)):

            # if attribute is already used, skip
            if self.features[i] in used_attributes:
                continue

            # calculate information gain or gain ratio
            if self.criterion == "information gain":
                temp_gain = self.calculate_information_gain__(
                    dataset, labels, self.features[i])
            elif self.criterion == "gain ratio":
                temp_gain = self.calculate_gain_ratio__(
                    dataset, labels, self.features[i])

            # if temp_gain is greater than gain_, update gain_ and attribute_
            if temp_gain-gain_ > gain_:
                gain_ = temp_gain
                attribute_ = self.features[i]

        # Create a non-leaf node
        node = TreeNode(attribute_)
        used_attributes.append(attribute_)
        idx = self.features.index(attribute_)
        np_d = np.array(dataset)
        # get unique values and counts for the attribute
        values, counts = np.unique(np_d[:, idx], return_counts=True)

        for i in range(len(values)):
            # create sub dataset
            sub_dataset = []
            sub_labels = []
            for j in range(len(dataset)):
                if dataset[j][idx] == values[i]:
                    sub_dataset.append(dataset[j])
                    sub_labels.append(labels[j])

            # get unique labels and counts
            sub_values, sub_counts = np.unique(sub_labels, return_counts=True)

            if (len(sub_values) == 1):  # if all labels are same, create a leaf node
                leaf_node = TreeLeafNode(data=None, label=sub_labels)
                node.subtrees[values[i]] = leaf_node
            else:  # if not, create a non-leaf node recursively
                node.subtrees[values[i]] = self.ID3__(
                    sub_dataset, sub_labels, used_attributes)
        return node

    def predict(self, x):
        """
        :param x: a data instance, 1 dimensional Python array
        :return: predicted label of x

        If a leaf node contains multiple labels in it, the majority label should be returned as the predicted label
        """
        predicted_label = None
        """
            Your implementation
        """

        # traverse the tree
        node = self.root
        while not isinstance(node, TreeLeafNode):
            idx = self.features.index(node.attribute)
            node = node.subtrees[x[idx]]

        if len(node.labels) == 1:  # if node is a leaf node with only one label, return that label
            predicted_label = node.labels[0]
        else:  # if node is a leaf node with multiple labels, return the majority label
            counts = np.bincount(node.labels)
            majority = np.argmax(counts)
            predicted_label = majority

        return predicted_label

    def train(self):
        self.root = self.ID3__(self.dataset, self.labels, [])
        print("Training completed")
