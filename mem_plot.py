import numpy as np
import pandas as pd
import matplotlib as plt
from matplotlib import pyplot as pllt
import math
from scipy.special import gamma


def main():
    data = pd.read_csv('wyniki.csv')
    data = np.array(data)
    n = np.array([data[i][1].count(' ') for i in range(len(data))])
    data = np.column_stack((data, n))
    x = [data[i][5] for i in range(len(data))]
    y = [data[i][3]/1000000 for i in range(len(data))]
    pllt.scatter(x, y, label='TSP Held-Karp',c='green')
    x_e = np.linspace(min([data[i][5] for i in range(len(data))]), max(
        [data[i][5] for i in range(len(data))]), 100)
    y_e = [i*math.pow(2,i) for i in x_e]
    param = max(y)/max(y_e)
    y_e = [i*param for i in y_e]
    x_e = np.linspace(min([data[i][5] for i in range(len(data))]), max(
        [data[i][5] for i in range(len(data))])+max(
        [data[i][5] for i in range(len(data))])/5, 120)
    for i in range(20):
        y_e.append(0)
    for i in range(119,20,-1):
        y_e[i] = y_e[i-20]
    for i in range(20):
        y_e[i] = 0
        
    pllt.plot(x_e, y_e, label='n*2^n', linestyle='--', linewidth=2.0)
    pllt.xlabel('Liczba wierzchołków w grafie')
    pllt.ylabel('Maksymalne zużycie pamięci[GB]')
    pllt.legend()
    pllt.show()


if __name__ == "__main__":
    main()
