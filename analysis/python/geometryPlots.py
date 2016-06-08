import operator

import ROOT
import numpy
import showerCalo

import math
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import pyplot as plt


def main():

    plane = 1
    cut = 0

    # (e_data, e_sim), (p_data, p_sim) = showerCalo.full_samples()
    (e_data, e_sim), (p_data, p_sim) = showerCalo.lite_samples()

    # Plot the Y-Z, X-Y, and Z-X distributions for the data events

    e_x = []    
    e_y = []    
    e_z = []    
    for shower in e_data.getShowerCaloVector():
        e_x.append(shower._3D_start_point.X())
        e_y.append(shower._3D_start_point.Y())
        e_z.append(shower._3D_start_point.Z())

    p_x = []    
    p_y = []    
    p_z = []    
    for shower in p_data.getShowerCaloVector():
        p_x.append(shower._3D_start_point.X())
        p_y.append(shower._3D_start_point.Y())
        p_z.append(shower._3D_start_point.Z())

    
    # X-Y:
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    # ax.scatter(e_x, e_y, e_z,marker="o")
    # ax.scatter(p_x, p_y, p_z,marker="x",c='r')
    # ax.set_xlim((0,45))
    # ax.set_ylim((-20,20))
    # ax.set_zlim((0,90))
    # plt.show()


if __name__ == '__main__':
    main()
