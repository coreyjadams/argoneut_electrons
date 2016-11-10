import ROOT
import showerCalo

import scipy.stats

import numpy


from matplotlib import pyplot as plt


def main():

    # threshold = 5.0

    # load the data:
    # (e_data, e_sim), (p_data, p_sim) = showerCalo.lite_simch_samples()
    (e_data, e_sim) = showerCalo.full_simch_electrons()

    sim_reco_dep_e_i = e_sim.getShowerCaloVector().reco_deposited_energy(0)
    sim_reco_dep_e_c = e_sim.getShowerCaloVector().reco_deposited_energy(1)
    sim_true_dep_e = e_sim.getShowerCaloVector().mc_deposited_energy()

    fig, axScatter = plt.subplots(figsize=(8, 8))

    # axScatter = plt.axes(rect_scatter)
    plt.ylabel("Reco Deposited Energy [MeV]", fontsize=20)
    plt.xlabel("True Deposited Energy [MeV]", fontsize=20)

    bins = numpy.arange(0, 2000, 20)

    cmap = plt.get_cmap('jet')
    cmap.set_under('w')

    # Fit a straight line to this set of data:
    res = scipy.stats.linregress(sim_true_dep_e, sim_reco_dep_e_i)

    # the scatter plot:
    axScatter.hist2d(sim_true_dep_e,
                     sim_reco_dep_e_i,
                     bins=[bins, bins],
                     vmin=0.01,
                     cmap=cmap)
    # cmap="nipy_spectral_r")

    for tick in axScatter.xaxis.get_major_ticks():
        tick.label.set_fontsize(20)

    for tick in axScatter.yaxis.get_major_ticks():
        tick.label.set_fontsize(20)

    slope = res.slope
    intc = res.intercept

    axScatter.plot([0, 2000], [intc, intc + slope*2000],
                   ls="--",
                   color='black',
                   linewidth=2,
                   label=r"Linear Fit, $\alpha={:.2},\beta={:.3}$".format(intc,slope))


    # # Set limits for dE/dx values
    # axScatter.set_xlim((x_bins[0], x_bins[-1]))
    # axScatter.set_ylim((y_bins[0], y_bins[-1]))
    axScatter.grid(True)

    axScatter.grid(True)
    # plt.legend()

    plt.show()

    fig, axScatter = plt.subplots(figsize=(8, 8))

    # axScatter = plt.axes(rect_scatter)
    plt.ylabel("Reco Deposited Energy [MeV]", fontsize=20)
    plt.xlabel("True Deposited Energy [MeV]", fontsize=20)

    bins = numpy.arange(0, 2000, 20)

    cmap = plt.get_cmap('jet')
    cmap.set_under('w')

    # the scatter plot:
    axScatter.hist2d(sim_true_dep_e,
                     sim_reco_dep_e_c,
                     bins=[bins, bins],
                     vmin=0.01,
                     cmap=cmap)
    # cmap="nipy_spectral_r")

    for tick in axScatter.xaxis.get_major_ticks():
        tick.label.set_fontsize(20)

    for tick in axScatter.yaxis.get_major_ticks():
        tick.label.set_fontsize(20)

    # Fit a straight line to this set of data:
    res = scipy.stats.linregress(sim_true_dep_e, sim_reco_dep_e_c)
    slope = res.slope
    intc = res.intercept

    axScatter.plot([0, 2000], [intc, intc + slope*2000],
                   ls="--",
                   color='black',
                   linewidth=2,
                   label=r"Linear Fit, $\alpha={:.2},\beta={:.3}$".format(intc, slope))

    plt.text(50,1500,"All Depositions > {} MeV ".format(threshold),fontsize=20)
    plt.legend(fontsize=20)

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
