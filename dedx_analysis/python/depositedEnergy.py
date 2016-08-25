import ROOT
import numpy
import showerCalo

import math

from matplotlib import pyplot as plt

import numpy


def main():

    # load the data:
    (e_data, e_sim), (p_data, p_sim) = showerCalo.lite_samples()
    # (e_data, e_sim), (p_data, p_sim) = showerCalo.full_samples()


    sim_reco_dep_e_i = e_sim.getShowerCaloVector().reco_deposited_energy(0)
    sim_reco_dep_e_c = e_sim.getShowerCaloVector().reco_deposited_energy(1)
    sim_true_dep_e = e_sim.getShowerCaloVector().mc_deposited_energy()

    fig, axScatter = plt.subplots(figsize=(8, 8))

    # axScatter = plt.axes(rect_scatter)
    plt.xlabel("Reco Deposited Energy [MeV]",fontsize=20)
    plt.ylabel("True Deposited Energy [MeV]",fontsize=20)

    bins = numpy.arange(0,1000,20)

    cmap = plt.get_cmap('jet')
    cmap.set_under('w')

    # the scatter plot:
    axScatter.hist2d(sim_reco_dep_e_i,
                     sim_true_dep_e,
                     bins=[bins,bins],
                     vmin=0.01,
                     cmap=cmap)
                     # cmap="nipy_spectral_r")

    axScatter.plot([0,10],[0,10],ls="--",color='black',linewidth=2)


    # # Set limits for dE/dx values
    # axScatter.set_xlim((x_bins[0], x_bins[-1]))
    # axScatter.set_ylim((y_bins[0], y_bins[-1]))
    axScatter.grid(True)

    axScatter.grid(True)
    # plt.legend()

    plt.show()


    fig, axScatter = plt.subplots(figsize=(8, 8))

    # axScatter = plt.axes(rect_scatter)
    plt.xlabel("Reco Deposited Energy [MeV]",fontsize=20)
    plt.ylabel("True Deposited Energy [MeV]",fontsize=20)

    bins = numpy.arange(0,1000,20)

    cmap = plt.get_cmap('jet')
    cmap.set_under('w')

    # the scatter plot:
    axScatter.hist2d(sim_reco_dep_e_c,
                     sim_true_dep_e,
                     bins=[bins,bins],
                     vmin=0.01,
                     cmap=cmap)
                     # cmap="nipy_spectral_r")

    axScatter.plot([0,10],[0,10],ls="--",color='black',linewidth=2)


    # # Set limits for dE/dx values
    # axScatter.set_xlim((x_bins[0], x_bins[-1]))
    # axScatter.set_ylim((y_bins[0], y_bins[-1]))
    axScatter.grid(True)

    axScatter.grid(True)
    # plt.legend()

    plt.show()

    # Compare the 




if __name__ == "__main__":
    main()
