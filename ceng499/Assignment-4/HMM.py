import numpy as np


class HMM:
    def __init__(self, A, B, Pi):
        self.A = A
        self.B = B
        self.Pi = Pi

    def forward_log(self, O: list):
        """
        :param O: is the sequence (an array of) discrete (integer) observations, i.e. [0, 2,1 ,3, 4]
        :return: ln P(O|λ) score for the given observation, ln: natural logarithm
        """

        # Initialization
        T = len(O)
        N = self.A.shape[0]
        alpha = np.zeros((T, N))
        alpha[0] = self.Pi * self.B[:, O[0]]

        c = np.zeros(T)
        c[0] = 1 / np.sum(alpha[0])
        alpha[0] = alpha[0] * c[0]
        c[0] = np.log(c[0])

        for t in range(1, T):
            for j in range(N):
                # Compute alpha[t, j]
                alpha[t, j] = np.sum(
                    alpha[t - 1] * self.A[:, j]) * self.B[j, O[t]]

            # Normalize alpha[t] and compute c[t]
            c[t] = 1 / np.sum(alpha[t])
            alpha[t] = alpha[t] * c[t]
            c[t] = np.log(c[t])

        # Compute ln P(O|λ)
        return -np.sum(c)

    def viterbi_log(self, O: list):
        """
        :param O: is an array of discrete (integer) observations, i.e. [0, 2,1 ,3, 4]
        :return: the tuple (Q*, ln P(Q*|O,λ)), Q* is the most probable state sequence for the given O
        """

        # Initialization
        T = len(O)
        N = self.A.shape[0]
        delta = np.zeros((T, N))
        delta[0] = np.log(self.Pi) + np.log(self.B[:, O[0]])
        psi = np.zeros((T, N), dtype=int)
        psi[0] = 0

        for t in range(1, T):
            for j in range(N):
                # Compute delta[t, j] and psi[t, j]
                delta[t, j] = np.max(
                    delta[t - 1] + np.log(self.A[:, j])) + np.log(self.B[j, O[t]])
                psi[t, j] = np.argmax(delta[t - 1] + np.log(self.A[:, j]))

        # Backtracking Q_star
        Q_star = np.zeros(T, dtype=int)
        Q_star[T - 1] = np.argmax(delta[T - 1])
        for t in range(T - 2, -1, -1):
            Q_star[t] = psi[t + 1, Q_star[t + 1]]

        # I think there is a typo either in the homework pdf or in the function description
        # Nevertheless, I am returning the tuple of (ln P(Q*|O,λ), Q*) as described in the homework pdf

        # Compute ln P(Q*|O,λ)
        return np.max(delta[T - 1]), Q_star
