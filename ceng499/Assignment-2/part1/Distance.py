from numpy import dot
from numpy.linalg import norm


class Distance:
    @staticmethod
    def calculateCosineDistance(x, y):
        return dot(x, y)/(norm(x)*norm(y))

    @staticmethod
    def calculateMinkowskiDistance(x, y, p=2):
        if (p == None):
            p = 2
        sum = 0
        for a, b in zip(x, y):
            sum += (abs(a-b)**p)
        return sum**(1/p)

    @staticmethod
    def calculateMahalanobisDistance(x, y, S_minus_1):
        x_y = x - y
        dot1 = dot(x_y.T, S_minus_1)
        dot2 = dot(dot1, x_y)
        dot2 = dot2 ** 0.5
        return dot2
