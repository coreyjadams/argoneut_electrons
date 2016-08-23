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
    e_theta = []
    e_phi = []
    e_c_pitch = []
    for shower in e_data.getShowerCaloVector():
        e_x.append(shower._3D_start_point.X())
        e_y.append(shower._3D_start_point.Y())
        e_z.append(shower._3D_start_point.Z())
        e_theta.append((180. / math.pi)*shower._3D_direction.Theta())
        e_phi.append((180. / math.pi)*shower._3D_direction.Phi())
        e_c_pitch.append(shower._collection_pitch)

    p_x = []
    p_y = []
    p_z = []
    p_theta = []
    p_phi = []
    p_c_pitch = []
    for shower in p_data.getShowerCaloVector():
        p_x.append(shower._3D_start_point.X())
        p_y.append(shower._3D_start_point.Y())
        p_z.append(shower._3D_start_point.Z())
        p_theta.append((180. / math.pi)*shower._3D_direction.Theta())
        p_phi.append((180. / math.pi)*shower._3D_direction.Phi())
        p_c_pitch.append(shower._collection_pitch)


    # # Plot the X-Y, Y-Z and the X-Z projections of electrons and photons
    # # Plot the angles theta and phi for electrons and photons

    # # X-Y Projection:
    # f, ax = plt.subplots(figsize=((47./40)*10,10))

    # ax.scatter(p_x,p_y, marker="+", s=200,color='r', label = "Gammas",linewidth=5)
    # ax.scatter(e_x,e_y, marker="o", s=100,color='b', label = "Electrons",linewidth=5)
    # plt.xlabel("X (Drift) Direction [cm]",fontsize=30)
    # plt.ylabel("Y (Vertical) Direction [cm]",fontsize=30)
    # plt.title("Cross Section Projection of TPC",fontsize=40)
    # ax.set_xlim((0,47))
    # ax.set_ylim((-20,20))
    # plt.grid(True)
    # for tick in ax.xaxis.get_major_ticks():
    #     tick.label.set_fontsize(25)
    # for tick in ax.yaxis.get_major_ticks():
    #     tick.label.set_fontsize(25)
    # plt.show()

    # # Y-Z Projection:
    # f, ax = plt.subplots(figsize=((90./40)*10,10))

    # ax.scatter(p_z,p_y, marker="+", s=200,color='r', label = "Gammas",linewidth=5)
    # ax.scatter(e_z,e_y, marker="o", s=100,color='b', label = "Electrons",linewidth=5)
    # plt.xlabel("Z (Beam) Direction [cm]",fontsize=30)
    # plt.ylabel("Y (Vertical) Direction [cm]",fontsize=30)
    # plt.title("Side Projection of TPC",fontsize=40)
    # ax.set_xlim((0,90))
    # ax.set_ylim((-20,20))
    # plt.grid(True)
    # for tick in ax.xaxis.get_major_ticks():
    #     tick.label.set_fontsize(25)
    # for tick in ax.yaxis.get_major_ticks():
    #     tick.label.set_fontsize(25)
    # plt.show()

    # # X-Z Projection:
    # f, ax = plt.subplots(figsize=((90./45)*10,10))

    # ax.scatter(p_z,p_x, marker="+", s=200,color='r', label = "Gammas",linewidth=5)
    # ax.scatter(e_z,e_x, marker="o", s=100,color='b', label = "Electrons",linewidth=5)
    # plt.ylabel("X (Drift) Direction [cm]",fontsize=30)
    # plt.xlabel("Z (Beam) Direction [cm]",fontsize=30)
    # plt.title("Top Down Projection of TPC",fontsize=40)
    # ax.set_ylim((0,45))
    # ax.set_xlim((0,90))
    # plt.grid(True)
    # for tick in ax.xaxis.get_major_ticks():
    #     tick.label.set_fontsize(25)
    # for tick in ax.yaxis.get_major_ticks():
    #     tick.label.set_fontsize(25)
    # plt.show()

    # # Theta angular distribution:
    # f, ax = plt.subplots(figsize=(10, 7))

    # theta_bins = numpy.arange(0, 180, 9)

    # ax.hist(e_theta, theta_bins, label="Electrons",
    #         normed=True, histtype='step',
    #         linewidth=5, color='b')
    # ax.hist(p_theta, theta_bins, label="Gammas",
    #         normed=True, histtype='step',
    #         linewidth=5, color='r')
    # plt.ylabel("Area Normalized", fontsize=30)
    # plt.xlabel("Polar Angle [Deg.]", fontsize=30)
    # plt.title(r"$\theta$ (Polar Angle) of Showers to Beam Direction", fontsize=40)
    # # ax.set_ylim((0,45))
    # # ax.set_xlim((0,90))
    # plt.legend(fontsize=40)
    # plt.grid(True)
    # for tick in ax.xaxis.get_major_ticks():
    #     tick.label.set_fontsize(25)
    # for tick in ax.yaxis.get_major_ticks():
    #     tick.label.set_fontsize(0)
    # plt.show()

    # # Phi angular distribution:
    # f, ax = plt.subplots(figsize=(10, 7))

    # phi_bins = numpy.arange(-180, 180, 50)

    # ax.hist(e_phi, phi_bins, label="Electrons",
    #         normed=True, histtype='step',
    #         linewidth=5, color='b')
    # ax.hist(p_phi, phi_bins, label="Gammas",
    #         normed=True, histtype='step',
    #         linewidth=5, color='r')
    # plt.ylabel("Normalized", fontsize=20)
    # plt.xlabel("Azimuthal Angle [Deg.]", fontsize=20)
    # plt.title("$\phi$ (Azimuthal angle) of Showers to Beam Direction", fontsize=20)
    # # ax.set_ylim((0,45))
    # # ax.set_xlim((0,90))
    # plt.legend(fontsize=20)
    # plt.grid(True)
    # for tick in ax.xaxis.get_major_ticks():
    #     tick.label.set_fontsize(16)
    # for tick in ax.yaxis.get_major_ticks():
    #     tick.label.set_fontsize(0)
    # plt.show()

    # Phi angular distribution:
    f, ax = plt.subplots(figsize=(10, 7))

    pitch_bins = numpy.arange(0.4, 2.0, 0.04)

    e_hist, junk = numpy.histogram(e_c_pitch,pitch_bins,normed=True)
    p_hist, junk = numpy.histogram(p_c_pitch,pitch_bins,normed=True)

    pitch_bins -= 0.02

    e_hist = numpy.insert(e_hist,0,0)
    p_hist = numpy.insert(p_hist,0,0)

    print len(e_hist)
    print len(pitch_bins)

    print p_hist
    print pitch_bins

    ax.step(pitch_bins, e_hist, label="Electrons",
            # ls='',
            where='mid',
            linewidth=3, color='b')
    ax.step(pitch_bins, p_hist, label="Gammas",
            # ls='step',
            where='mid',
            linewidth=3, color='r')
    plt.ylabel("Area Normalized", fontsize=25)
    plt.xlabel("Effective Pitch [cm]", fontsize=25)
    plt.title("Effective Pitch of Showers", fontsize=30)
    # ax.set_ylim((0,45))
    # ax.set_xlim((0,90))
    plt.legend(fontsize=25)
    plt.grid(True)
    for tick in ax.xaxis.get_major_ticks():
        tick.label.set_fontsize(20)
    for tick in ax.yaxis.get_major_ticks():
        tick.label.set_fontsize(0)
    ax.set_xlim([0.4,1.4])
    plt.show()

if __name__ == '__main__':
    main()
