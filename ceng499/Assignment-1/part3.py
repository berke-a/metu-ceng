import torch
import torch.nn as nn
import numpy as np
import pickle
from tabulate import tabulate
import matplotlib.pyplot as plt

# MLP Model


class MlPModel(nn.Module):
    def __init__(self, layerNum, layerSize, activator):
        super(MlPModel, self).__init__()
        self.layerNum = layerNum
        self.activation_function = activator
        if layerNum == 2:
            self.layer1 = nn.Linear(784, 10)
        elif layerNum == 3:
            self.layer1 = nn.Linear(784, layerSize)
            self.layer2 = nn.Linear(layerSize, 10)
        elif layerNum == 4:
            self.layer1 = nn.Linear(784, layerSize)
            self.layer2 = nn.Linear(layerSize, layerSize)
            self.layer3 = nn.Linear(layerSize, 10)

    def forward(self, x):
        if self.layerNum == 2:
            output = self.layer1(x)
        elif self.layerNum == 3:
            hidden1 = self.layer1(x)
            output = self.layer2(self.activation_function(hidden1))
        elif self.layerNum == 4:
            hidden1 = self.layer1(x)
            hidden2 = self.layer2(self.activation_function(hidden1))
            output = self.layer3(self.activation_function(hidden2))
        return torch.softmax(output, dim=1)


# Model trainer for hyperparameter tuning
def tuner(model, optimizer, epochNumber, final_train=False):

    validation_loss_array = []
    validation_accuracy_array = []
    train_loss_array = []
    train_accuracy_array = []

    # Arrays for plotting the final training loss
    validation_loss_array_plot = []
    training_loss_array_plot = []

    for i in range(10):

        for epoch in range(epochNumber):

            model.train()

            optimizer.zero_grad()

            loss = nn.CrossEntropyLoss()

            train_loss = loss(model(x_train), y_train)
            training_loss_array_plot.append(train_loss.item())

            train_loss.backward()
            optimizer.step()

            validation_loss = loss(model(x_validation), y_validation)
            validation_loss_array_plot.append(validation_loss.item())

        # Calculate the accuracy of the model
        train_accuracy = sum(torch.argmax(
            model(x_train), dim=1) == y_train) / len(y_train)
        validation_accuracy = sum(torch.argmax(
            model(x_validation), dim=1) == y_validation) / len(y_validation)

        # In the final training, we don't need to train the model for 10 times. So we can just return the final training loss and accuracy
        if final_train:
            with torch.no_grad():
                return model, [validation_loss, validation_accuracy, train_loss, train_accuracy], validation_loss_array_plot, training_loss_array_plot

        # Append the loss and accuracy to the array
        train_loss_array.append(train_loss)
        validation_loss_array.append(validation_loss)

        train_accuracy_array.append(train_accuracy)
        validation_accuracy_array.append(validation_accuracy)

    with torch.no_grad():
        return model, [np.average(validation_loss_array), np.average(validation_accuracy_array), np.average(train_loss_array), np.average(train_accuracy_array)]


# we load all the datasets of Part 3
x_train, y_train = pickle.load(open("data/mnist_train.data", "rb"))
x_validation, y_validation = pickle.load(
    open("data/mnist_validation.data", "rb"))
x_test, y_test = pickle.load(open("data/mnist_test.data", "rb"))

x_train = x_train/255.0
x_train = x_train.astype(np.float32)

x_test = x_test / 255.0
x_test = x_test.astype(np.float32)

x_validation = x_validation/255.0
x_validation = x_validation.astype(np.float32)

# and converting them into Pytorch tensors in order to be able to work with Pytorch
x_train = torch.from_numpy(x_train)
y_train = torch.from_numpy(y_train).to(torch.long)

x_validation = torch.from_numpy(x_validation)
y_validation = torch.from_numpy(y_validation).to(torch.long)

x_test = torch.from_numpy(x_test)
y_test = torch.from_numpy(y_test).to(torch.long)


def main():
    device = torch.device('cpu')

    # Hyperparameters
    epochs = [40, 80]
    learning_rates = [0.1, 0.001]
    layer_nums = [2, 3]
    layer_sizes = [8, 32]
    activation_functions = [nn.ReLU(), nn.Sigmoid()]

    hyper_parameters, general_loss = (
        float('-inf'), float('-inf'), float('-inf'), float('-inf'), None), float('inf')
    # hyperParameters = (epochs, learningRate, layerNum, layerSize, activator)

    # Initializing the tabel data for the report
    data = []
    for layerNum in layer_nums:
        for epoch in epochs:
            if layerNum == 2:
                data.append(
                    [str(layerNum)+" / "+str(epoch)+" / No Hidden Layer"])
            else:
                for layersize in layer_sizes:
                    for activator in activation_functions:
                        data.append([str(layerNum)+" / "+str(epoch) +
                                    " / "+str(layersize)+" / "+str(activator)])

    # Dictionaries for the additional Q/A part
    learning_rate_results = {
        0.1: [],
        0.001: [],
    }
    activation_function_results = {
        str(nn.ReLU()): [],
        str(nn.Sigmoid()): []
    }

    # Hyperparameter tuning
    for learningRate in learning_rates:

        i = 0

        for layerNum in layer_nums:

            print(tabulate(data, headers=[
                'Layers/Epochs/LayerSizes/Activator']+learning_rates, tablefmt="fancy_grid"))

            for epoch in epochs:

                if layerNum == 2:

                    # Initializing the model
                    model = MlPModel(layerNum, None, None)
                    model.to(device)
                    optimizer = torch.optim.Adam(
                        model.parameters(), lr=learningRate)
                    # Training the model
                    model, averageValues = tuner(model, optimizer, epoch)

                    # Calculating confidence interval
                    error = 1 - averageValues[1]
                    confidence_interval = 100*(1.96 *
                                               (error * (1 - error) / len(y_validation))**0.5)

                    # Adding validation accuracy to my table
                    data[i].append(str("%.2f" % (averageValues[1].item()*100)) +
                                   "% +- " + str("%.2f" % confidence_interval) + "%")
                    i += 1

                    learning_rate_results[learningRate].append(
                        averageValues[1].item())

                    # If the validation loss is better than the minimum, we update the optimum hyperparameters
                    if averageValues[0] < general_loss:
                        general_loss = averageValues[0]
                        hyper_parameters = (
                            epoch, learningRate, layerNum, float('-inf'), None)
                        print("Best parameters so far - loss:", general_loss,
                              "/ hyper parameters :", hyper_parameters)

                else:

                    for layerSize in layer_sizes:

                        for activator in activation_functions:

                            # Initializing the model
                            model = MlPModel(
                                layerNum, layerSize, activator)
                            model.to(device)
                            optimizer = torch.optim.Adam(
                                model.parameters(), lr=learningRate)
                            # Training the model
                            model, averageValues = tuner(
                                model, optimizer, epoch)

                            # Calculating confidence interval
                            error = 1 - averageValues[1]
                            confidence_interval = 100*(1.96 *
                                                       (error * (1 - error) / len(y_validation))**0.5)

                            # Adding validation accuracy to my table
                            data[i].append(str("%.2f" % (averageValues[1].item()*100)) +
                                           "% +- " + str("%.2f" % confidence_interval) + "%")
                            i += 1

                            activation_function_results[str(activator)].append(
                                averageValues[1].item())

                            learning_rate_results[learningRate].append(
                                averageValues[1].item())

                            # If the validation loss is better than the minimum, we update the optimum hyperparameters
                            if averageValues[0] < general_loss:
                                general_loss = averageValues[0]
                                hyper_parameters = (
                                    epoch, learningRate, layerNum, layerSize, activator)
                                print("Best parameters so far - loss:", general_loss,
                                      "/ hyper parameters :", hyper_parameters)

    # Saves accuracy table with different parameters + confidence interval
    with open('validation_accuracy_table.txt', 'w') as f:
        f.write(tabulate(data, headers=[
            'Layers/Epochs/LayerSizes/Activator']+learning_rates, tablefmt="fancy_grid"))

    print("hyper parameters :", hyper_parameters)

    model = MlPModel(
        hyper_parameters[2], hyper_parameters[3], hyper_parameters[4])
    model.to(device)
    optimizer = torch.optim.Adam(
        model.parameters(), lr=hyper_parameters[1])

    # Final training with best parameters
    model, averageValues, validation_loss, training_loss = tuner(
        model, optimizer, hyper_parameters[0], True)

    print("validation loss: ", averageValues[0], " validation accuracy: ", averageValues[1],
          " train loss: ", averageValues[2], " train accuracy: ", averageValues[3])

    # Plotting the loss figure
    figure = plt.figure(figsize=(10, 8))
    plt.plot(training_loss, label='Training Loss')
    plt.plot(validation_loss, label='Validation Loss')
    plt.ylabel('Loss')
    plt.xlabel('Epoch Number')
    plt.legend()
    plt.tight_layout()
    # Saving the figure
    figure.savefig('final_loss_plot.png', bbox_inches='tight')

    # Obtaining accuracy and loss for test set
    test_accuracy = sum(torch.argmax(model(x_test), dim=1)
                        == y_test) / len(y_test)
    loss = nn.CrossEntropyLoss()
    test_loss = loss(model(x_test), y_test)

    print("test loss: ", test_loss.item(),
          " test accuracy: ", test_accuracy.item())

    # Saving the model
    torch.save(model.state_dict(), 'model.pth')

    # Saving the hyperparameters
    with open('hyperparameters.txt', 'w') as f:
        f.write(str(hyper_parameters))

    # Find the average accuracy of each activation function and each learning rate
    for key in learning_rate_results.keys():
        learning_rate_results[key] = np.average(learning_rate_results[key])
    for key in activation_function_results.keys():
        activation_function_results[key] = np.average(
            activation_function_results[key])
    print(learning_rate_results)
    print(activation_function_results)


if __name__ == '__main__':
    main()
