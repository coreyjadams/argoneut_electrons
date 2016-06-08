import operator

import ROOT
import numpy
import showerCalo

import math

from matplotlib import pyplot as plt


def main():

    plane = 1
    cut = 0

    # (e_data, e_sim), (p_data, p_sim) = showerCalo.full_samples()
    (e_data, e_sim), (p_data, p_sim) = showerCalo.lite_samples()

    e_data_median = e_data.getBestMedianVector()
    e_data_modmean = e_data.getBestModmeanVector()
    e_data_lma = e_data.getBestLMAVector()

    e_sim_median = e_sim.getBestMedianVector()
    e_sim_modmean = e_sim.getBestModmeanVector()
    e_sim_lma = e_sim.getBestLMAVector()

    p_sim_median = p_sim.getBestMedianVector()
    p_sim_modmean = p_sim.getBestModmeanVector()
    p_sim_lma = p_sim.getBestLMAVector()

    p_data_median = p_data.getBestMedianVector()
    p_data_modmean = p_data.getBestModmeanVector()
    p_data_lma = p_data.getBestLMAVector()

    plotdEdx(e_data_median, e_sim_median, p_data_median, p_sim_median, "")
    # plotdEdx(e_data_modmean, e_sim_modmean, p_data_modmean, p_sim_modmean, "Outlier Removed Mean")
    # plotdEdx(e_data_lma, e_sim_lma, p_data_lma, p_sim_lma, "LMA")

    # plotSigma(e_data_median,p_data_median)
    # plotSigma(e_data_modmean,p_data_modmean)
    # plotSigma(e_data_lma,p_data_lma)


def plotdEdx(e_data, e_sim, p_data, p_sim, name):

    # cut = 0
    cut = 2.9

    data_bin_width = 0.4
    sim_bin_width = 0.2

    data_bins = numpy.arange(0.1, 8.0, data_bin_width)
    sim_bins = numpy.arange(0.1, 8.0, sim_bin_width)

    electron_data_hist, bin_edges_data = numpy.histogram(
        e_data, data_bins)
    photon_data_hist, bin_edges_data = numpy.histogram(
        p_data, data_bins)

    electron_err = []
    electron_frac_err = []
    photon_err = []
    photon_frac_err = []
    for e_val, p_val in zip(electron_data_hist, photon_data_hist):
        electron_err.append(math.sqrt(e_val))
        if e_val != 0:
            electron_frac_err.append(math.sqrt(e_val) / e_val)
        else:
            electron_frac_err.append(0.0)

        photon_err.append(math.sqrt(p_val))
        if p_val != 0:
            photon_frac_err.append(math.sqrt(p_val) / p_val)
        else:
            photon_frac_err.append(0.0)


    electron_data_hist, bin_edges_data = numpy.histogram(
        e_data, data_bins, density=True)
    photon_data_hist, bin_edges_data = numpy.histogram(
        p_data, data_bins, density=True)

    electron_sim_hist, bin_edges_sim = numpy.histogram(
        e_sim, sim_bins, density=True)
    photon_sim_hist, bin_edges_sim = numpy.histogram(
        p_sim, sim_bins, density=True)

    bin_centers_data = bin_edges_data[:-1] + 0.5*data_bin_width
    bin_centers_sim = bin_edges_sim[:-1] + 0.5*sim_bin_width

    electron_err_norm = []
    for e_val, frac_err in zip(electron_data_hist, electron_frac_err):
        electron_err_norm.append(e_val*frac_err)

    photon_err_norm = []
    for p_val, frac_err in zip(photon_data_hist, photon_frac_err):
        photon_err_norm.append(p_val*frac_err)

    f2, ax2 = plt.subplots(figsize=(15, 8))
    ax2.set_title("Calorimetric Electron Photon Separation", fontsize=30)
    ax2.errorbar(bin_centers_data, electron_data_hist, xerr=data_bin_width*0.5,
                 yerr=electron_err_norm,
                 marker="o",
                 label="Electrons, Data",
                 capsize=2,
                 ls="none")
    ax2.errorbar(bin_centers_data, photon_data_hist, xerr=data_bin_width*0.5,
                 yerr=photon_err_norm,
                 marker="o",
                 # fillstyle="none",
                 label="Photons, Data",
                 capsize=2,
                 color='r',
                 ls="none")
    plt.xlabel("dE/dx [MeV/cm]", fontsize=20)
    plt.ylabel("Unit Normalized", fontsize=20)

    ax2.plot(bin_centers_sim+0.5*sim_bin_width, electron_sim_hist, color="b",
             ls="steps", label="Simulated Electrons")
    ax2.plot(bin_centers_sim+0.5*sim_bin_width, photon_sim_hist, color="r",
             ls="steps", label="Simulated Electrons")

    if cut > 0:
        data_eff_tuple = makeCut(e_data, p_data, cut)
        # data_purity, data_eff_tuple = makeCut(e_sim.getBestdEdxVector(plane), p_sim.getBestdEdxVector(plane),cut)

        # Now draw a line at the place that gives the best dEdx cut:

        ax2.axvline(cut, linewidth=2, color='g', linestyle="--",
                    label="Cut at {} MeV/cm".format(cut))
        y_lims = ax2.get_ylim()
        plt.text(4.05, y_lims[1]*0.65, "Electron Efficiency:", size=25,)
        plt.text(7.95, y_lims[1]*0.65,
                 "{:.2} +/- {:.2}".format(1.0*data_eff_tuple[0][0],
                                          1.0*data_eff_tuple[0][1]),
                 size=25, horizontalalignment='right')
        plt.text(4.05, y_lims[1]*0.55, "Photon Mis. ID:", size=25,)
        plt.text(7.95, y_lims[1]*0.55,
                 "{:.2} +/- {:.2}".format(1.0*data_eff_tuple[1][0],
                                          1.0*data_eff_tuple[1][1]),
                 size=25, horizontalalignment='right')

    # Draw a histogram with all of these:
    plt.grid(True)
    # plt.legend(fontsize=20)
    plt.show()


def makeCut(electron_dedx_v, photon_dedx_v, cut_value):
    """
    This function will make a cut at the value specified on the branch_name specified
    It calculates the percent of each sample left on each side, and returns ???
    """

    bins = [0.1, cut_value, 10]
    # Use a histogram to slice this into two sections, one above the cut value
    # and one below
    electrons, junk = numpy.histogram(electron_dedx_v, bins)
    photons, junk = numpy.histogram(photon_dedx_v, bins)

    electron_eff = (1.0*electrons[0])/(electrons[0] + electrons[1])
    photon_eff = (1.0*photons[0])/(photons[0] + photons[1])

    print "Kept {} of {} electrons.".format(electrons[0],
                                            electrons[0] + electrons[1])
    print "Kept {} of {} photons.".format(photons[0],
                                          photons[0] + photons[1])

    try:
        electron_eff_stat_err = electron_eff * \
            math.sqrt(1.0/electrons[0] + 1.0/(electrons[0] + electrons[1]))
    except:
        electron_eff_stat_err = 999

    try:
        photon_eff_stat_err = photon_eff * \
            math.sqrt(1.0/photons[0] + 1.0/(photons[0] + photons[1]))
    except:
        photon_eff_stat_err = 999

    e_eff_mean, [e_eff_lower, e_eff_upper] = FeldmanCousins(
        electrons[0], electrons[0]+electrons[1], 0.6827)

    p_eff_mean, [p_eff_lower, p_eff_upper] = FeldmanCousins(
        photons[0], photons[0]+photons[1], 0.6827)

    print "Electron efficiency: {}".format(electron_eff)
    print "Electron efficiency uncertainty:"
    print "  Statistical ------ {:.3}".format(electron_eff_stat_err)
    print "  FeldmanCousins --- +{:.3} - {:.3}".format(e_eff_upper - e_eff_mean,
                                                       e_eff_mean - e_eff_lower)

    print "Photon efficiency: {}".format(photon_eff)
    print "Photon efficiency uncertainty:"
    print "  Statistical ------ {:.3}".format(photon_eff_stat_err)
    print "  FeldmanCousins --- +{:.3} - {:.3}".format(p_eff_upper - p_eff_mean,
                                                       p_eff_mean - p_eff_lower)

    return [[electron_eff, electron_eff_stat_err],
            [photon_eff, photon_eff_stat_err]]


def FeldmanCousins(this_ctr, total_ctr, confidence_interval):

    from ROOT import TEfficiency
    k = TEfficiency()
    eff_mean = float(this_ctr) / float(total_ctr)
    eff_upper = k.FeldmanCousins(
        total_ctr, this_ctr, confidence_interval, True)
    eff_lower = k.FeldmanCousins(
        total_ctr, this_ctr, confidence_interval, False)

    return eff_mean, [eff_lower, eff_upper]


def plotSigma(e_data, p_data):
    x_points = numpy.arange(1.0, 5.0, 0.05)

    min_dedx = 0.1
    max_dedx = 8.0

    sigma = []
    bins = [min_dedx, 2.0, max_dedx]
    e_hist, junk = numpy.histogram(e_data, bins)
    print e_hist[0]
    print e_hist[1]

    for point in x_points:
        bins = [min_dedx, point, max_dedx]
        e_hist, junk = numpy.histogram(e_data, bins)
        e_acc, e_rej = e_hist[0], e_hist[1]
        e_acc /= float(e_acc + e_rej)
        p_hist, junk = numpy.histogram(p_data, bins)
        p_acc, p_rej = p_hist[0], p_hist[1]
        p_acc /= float(p_acc + p_rej)
        print "Cut at {}: {} eff., {} mis. ID".format(
            point,
            e_acc,
            p_acc)
        sigma.append(e_acc / math.sqrt(e_acc**2 + p_acc**2 + 0.001))

    index, value = max(enumerate(sigma), key=operator.itemgetter(1))

    fig, ax = plt.subplots()
    plt.plot(x_points, sigma, label=r"$\frac{Sig.}{\sqrt{Bkg.^2 + Sig^2}}$")
    ax.axvline(x_points[index], color='g', ls="--")

    plt.xlabel("dE/dx [MeV/cm]")
    plt.legend(fontsize=25)
    plt.grid(True)
    # plt.savefig("/data_linux/dedx_plots/1D/sigma_dedx_1D_"+branch_name + ".png")
    plt.show()
    # return sigma


if __name__ == '__main__':
    main()
