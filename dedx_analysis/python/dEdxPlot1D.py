import operator

import ROOT
import numpy
import showerCalo

import math

from matplotlib import pyplot as plt

import numpy as np

ncpi0_x = [26.31579, 75.18797, 131.57895, 184.21053, 229.3233,
           278.1955, 334.58646, 383.45865, 424.81204, 484.9624,
           526.3158, 582.7068, 635.3383, 684.2105, 729.3233,
           785.7143, 834.5865, 883.4586, 921.0526, 977.4436,
           1033.8346, 1086.4662, 1142.8572, 1187.97, 1236.8422,
           1270.6766, 1330.827, 1379.6992, 1428.5714, 1473.6842,
           1533.8346, 1571.4286, 1624.0602, 1676.6918, 1733.0828,
           1774.436, 1830.827, 1875.9398, 1932.3308, 1973.6842,
           2030.0752, 2075.188, 2127.8196, 2180.4512, 2225.564,
           2281.9548, 2334.5864, 2368.4211, 2421.0527, 2473.6843,
           2530.0752, 2571.4285, 2631.5789, 2676.6917, 2721.8044,
           2778.1956, 2823.3083, 2875.94, 2924.812, 2973.6843]

ncpi0_y = [0.08068256, 0.1536175, 0.13664484, 0.10197051, 0.074624486,
           0.05568814, 0.046718784, 0.044930615, 0.03486365, 0.02705225,
           0.022256538, 0.016608829, 0.015973123, 0.015973123, 0.012887524,
           0.011689519, 0.010397985, 0.009806757, 0.008389359, 0.007759442,
           0.007759442, 0.0060208943, 0.005678548, 0.005790444,
           0.0048578116, 0.004763938, 0.0044062366, 0.0041556987,
           0.003996639, 0.004075393, 0.0032245906, 0.0033529243,
           0.0023598336, 0.0015664453, 0.0033529243, 0.0023598336,
           0.002225654, 0.001941492, 0.0023598336, 0.0022695106,
           0.0015064895, 0.0016608827, 0.0015361749, 0.0013933743,
           0.0010811808, 0.0011689519, 6.902129E-4, 0.0010197051,
           9.806757E-4, 0.0014488284, 7.759442E-4, 0.0012887524,
           0.0012394253, 5.904545E-4, 0.0012154742, 3.843667E-4,
           0.0011463626, 6.902129E-4, 4.9535354E-4, 0.028683169]
# Normalize the y range to 1:
nc_max = np.max(ncpi0_y)

ncpi0_y /= nc_max


def adjustWeightsToNCPi0_Gauss(sigma, mu, scale, p_sim):

    # Define the bins as the ncpi0 bins, though those
    # are the bin centers.  So get the bin edges:
    bin_edges = ROOT.vector('double')()
    bin_edges.push_back(0)

    # Define the edge to be halfway between the current point and the next
    # point
    for i in xrange(len(ncpi0_x) - 1):
        center = ncpi0_x[i]
        next_center = ncpi0_x[i+1]
        bin_edges.push_back(0.5*(center + next_center))

    bin_edges.push_back(3000)

    # Now get the spectrum of the sim binned into this set of energies
    energy_spectrum, bins = np.histogram(
        p_sim.getShowerCaloVector().mc_true_energy(),
        bins=bin_edges,
        density=True)

    # print 0.5*(bins[0] + bins[1])
    # print 0.5*(bins[1] + bins[2])
    # print 0.5*(bins[2] + bins[3])

    # print "------------"

    # exit(0)

    # scale this spectrum to peak at 1:
    sim_max = np.max(energy_spectrum)
    energy_spectrum /= sim_max

    # Now we can make a 1-1 ratio of the sim to nc spectrum:

    # for i in xrange(len(energy_spectrum)):
    #     print "{:.2} vs {:.2}".format(ncpi0_y[i], energy_spectrum[i])

    # Set the weights as the ratio of ncpi0_y[i] to energy_spectrum[i]

    weights = ROOT.vector('double')()

    for i in xrange(len(energy_spectrum)):
        weights.push_back(1.0)
        extra = math.exp(-(ncpi0_x[i]-mu)**2 / (2*sigma**2))
        if energy_spectrum[i] != 0:
            weights[i] = (ncpi0_y[i] + scale*extra) / energy_spectrum[i]

    # # Corrected:

    # print "\n\nCorrected:"

    # for i in xrange(len(energy_spectrum)):
    # print "{:.2} vs {:.2}".format(ncpi0_y[i], weights[i]*energy_spectrum[i])

    # # Adjust the weights above 500 MeV:
    # for i in xrange(len(ncpi0_y)):
    #   if ncpi0_x[i] > 200:
    #     weights[i] *= 2.5
    #   if ncpi0_x[i] > 500:
    #     weights[i] *= 1.5

    p_sim.getShowerCaloVector().set_weights(weights, bin_edges)


def main():

    plane = 1
    cut = 0

    (e_data, e_sim), (p_data, p_sim) = showerCalo.full_samples()
    # (e_data, e_sim), (p_data, p_sim) = showerCalo.lite_samples()
    adjustWeightsToNCPi0_Gauss(482.6, 635, 5793.0, p_sim)

    p_sim_weights = p_sim.getShowerCaloVector().weights()

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

    plotdEdx(e_data_median, e_sim_median, p_data_median,
             p_sim_median, p_sim_weights, "")
    # plotdEdx_bkgSubtracted(e_data_median, e_sim_median, p_data_median, p_sim_median, p_sim_weights, 0.15)
    # plotdEdx(e_data_modmean, e_sim_modmean, p_data_modmean, p_sim_modmean, "Outlier Removed Mean")
    # plotdEdx(e_data_lma, e_sim_lma, p_data_lma, p_sim_lma, "LMA")

    # plotSigma(e_data_median,p_data_median)
    # plotSigma(e_data_modmean,p_data_modmean)
    # plotSigma(e_data_lma,p_data_lma)


# def plotdEdx_bkgSubtracted(e_data, e_sim, p_data, p_sim, p_sim_weights,
# p_contamination):


#     cut = 0
#     cut = 2.9

#     data_bin_width = 0.3
#     sim_bin_width = 0.3

#     data_bins = numpy.arange(0.1, 8.0+data_bin_width, data_bin_width)
#     sim_bins = numpy.arange(0.1, 8.0+sim_bin_width, sim_bin_width)

#     electron_data_hist, bin_edges_data = numpy.histogram(
#         e_data, data_bins)
#     photon_data_hist, bin_edges_data = numpy.histogram(
#         p_data, data_bins)

#     electron_err = []
#     electron_frac_err = []
#     photon_err = []
#     photon_frac_err = []
#     for e_val, p_val in zip(electron_data_hist, photon_data_hist):
#         electron_err.append(math.sqrt(e_val))
#         if e_val != 0:
#             electron_frac_err.append(math.sqrt(e_val) / e_val)
#         else:
#             electron_frac_err.append(0.0)

#         photon_err.append(math.sqrt(p_val))
#         if p_val != 0:
#             photon_frac_err.append(math.sqrt(p_val) / p_val)
#         else:
#             photon_frac_err.append(0.0)


#     electron_data_hist, bin_edges_data = numpy.histogram(
#         e_data, data_bins, density=True)
#     photon_data_hist, bin_edges_data = numpy.histogram(
#         p_data, data_bins, density=True)

#     electron_sim_hist, bin_edges_sim = numpy.histogram(
#         e_sim, sim_bins, density=True)
#     photon_sim_hist, bin_edges_sim = numpy.histogram(
#         p_sim, sim_bins, density=True,weights=p_sim_weights)


#     print "Intgral is {}".format(numpy.sum(electron_data_hist))

#     # Now, subtract the photon contamination from the electron sample:
#     electron_data_hist = electron_data_hist - p_contamination*photon_data_hist
#     print "Intgral is {}".format(numpy.sum(electron_data_hist))

#     for i in xrange(len(electron_data_hist)):
#         if electron_data_hist[i] < 0:
#             electron_data_hist[i] = 0

#     print "Intgral is {}".format(numpy.sum(electron_data_hist))

#     electron_data_hist /= numpy.sum(electron_data_hist)*data_bin_width

#     print "Intgral is {}".format(numpy.sum(electron_data_hist))


#     # electron_sim_hist = (1-0.17011)*electron_sim_hist + 0.17011*photon_sim_hist

#     bin_centers_data = bin_edges_data[:-1] + 0.5*data_bin_width
#     bin_centers_sim = bin_edges_sim[:-1] + 0.5*sim_bin_width

#     electron_err_norm = []
#     for e_val, frac_err in zip(electron_data_hist, electron_frac_err):
#         electron_err_norm.append(e_val*frac_err)

#     photon_err_norm = []
#     for p_val, frac_err in zip(photon_data_hist, photon_frac_err):
#         photon_err_norm.append(p_val*frac_err)

#     f2, ax2 = plt.subplots(figsize=(15, 8))
#     ax2.set_title("Calorimetric Electron Photon Separation", fontsize=30)


#     ax2.plot(bin_centers_sim, electron_sim_hist, color="b",
#              ls="steps-mid", linewidth=3,
#              label="Simulated Electrons",alpha=0.7)
#     ax2.plot(bin_centers_sim, photon_sim_hist, color="r",
#              ls="steps-mid", linewidth=3,
#              label="Simulated Electrons",alpha=0.7)


#     ax2.errorbar(bin_centers_data, electron_data_hist, xerr=data_bin_width*0.5,
#                  yerr=electron_err_norm,
#                  marker="o",
#                  markersize=8,
#                  label="Electrons, Data",
#                  capsize=2,
#                  ls="none")
#     ax2.errorbar(bin_centers_data, photon_data_hist, xerr=data_bin_width*0.5,
#                  yerr=photon_err_norm,
#                  marker="^",
#                  # fillstyle="none",
#                  markersize=8,
#                  label="Photons, Data",
#                  capsize=2,
#                  color='r',
#                  ls="none")
#     plt.xlabel("dE/dx [MeV/cm]", fontsize=20)
#     plt.ylabel("Unit Normalized", fontsize=20)

#     if cut > 0:
#         data_eff_tuple = makeCut(e_data, p_data, cut)
#         # data_purity, data_eff_tuple = makeCut(e_sim.getBestdEdxVector(plane), p_sim.getBestdEdxVector(plane),cut)

#         # Now draw a line at the place that gives the best dEdx cut:

#         ax2.axvline(cut, linewidth=2, color='g', linestyle="--",
#                     label="Cut at {} MeV/cm".format(cut))
#         y_lims = ax2.get_ylim()
#         plt.text(4.05, y_lims[1]*0.65, "Electron Efficiency:", size=25,)
#         plt.text(7.95, y_lims[1]*0.65,
#                  "{:.2} +/- {:.2}".format(1.0*data_eff_tuple[0][0],
#                                           1.0*data_eff_tuple[0][1]),
#                  size=25, horizontalalignment='right')
#         plt.text(4.05, y_lims[1]*0.55, "Photon Mis. ID:", size=25,)
#         plt.text(7.95, y_lims[1]*0.55,
#                  "{:.2} +/- {:.2}".format(1.0*data_eff_tuple[1][0],
#                                           1.0*data_eff_tuple[1][1]),
#                  size=25, horizontalalignment='right')

#     for tick in ax2.xaxis.get_major_ticks():
#         tick.label.set_fontsize(16)
#     for tick in ax2.yaxis.get_major_ticks():
#         tick.label.set_fontsize(0)

#     # Draw a histogram with all of these:
#     plt.grid(True)
#     plt.legend(fontsize=20)
#     plt.show()

def plotdEdx(e_data, e_sim, p_data, p_sim, p_sim_weights, name):

    # cut = 0
    cut = 2.5

    data_bin_width = 0.4
    sim_bin_width = 0.4

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
        p_sim, sim_bins, density=True, weights=p_sim_weights)

    electron_sim_hist = (1-0.17011)*electron_sim_hist + 0.17011*photon_sim_hist

    bin_centers_data = bin_edges_data[:-1] + 0.25*data_bin_width
    bin_centers_sim = bin_edges_sim[:-1] + 0.25*sim_bin_width

    electron_err_norm = []
    for e_val, frac_err in zip(electron_data_hist, electron_frac_err):
        electron_err_norm.append(e_val*frac_err)

    photon_err_norm = []
    for p_val, frac_err in zip(photon_data_hist, photon_frac_err):
        photon_err_norm.append(p_val*frac_err)

    f2, ax2 = plt.subplots(figsize=(15, 8))
    ax2.set_title("Calorimetric Electron/Gamma Separation", fontsize=30)

    ax2.plot(bin_centers_sim, electron_sim_hist, color="b",
             ls="steps-mid", linewidth=3,
             label="Simulated Electron Candidates", alpha=0.7)
    ax2.plot(bin_centers_sim, photon_sim_hist, color="r",
             ls="steps-mid", linewidth=3,
             label="Simulated Gammas", alpha=0.7)

    e_line = ax2.errorbar(bin_centers_data, electron_data_hist, xerr=data_bin_width*0.5,
                          yerr=electron_err_norm,
                          marker="o",
                          markersize=8,
                          label="Electrons, Data",
                          capsize=2,
                          ls="none")
    p_line = ax2.errorbar(bin_centers_data, photon_data_hist, xerr=data_bin_width*0.5,
                          yerr=photon_err_norm,
                          marker="^",
                          # fillstyle="none",
                          markersize=8,
                          label="Photons, Data",
                          capsize=2,
                          color='r',
                          ls="none")
    plt.xlabel("dE/dx [MeV/cm]", fontsize=30)
    plt.ylabel("Area Normalized", fontsize=30)
    # plt.legend()

    if cut > 0:
        mc_eff_tuple = makeCut(e_sim, p_sim,cut)
        data_eff_tuple = makeCut(e_data, p_data, cut)
        print mc_eff_tuple
        print data_eff_tuple
        # data_purity, data_eff_tuple = makeCut(e_sim.getBestdEdxVector(plane), p_sim.getBestdEdxVector(plane),cut)

        # Now draw a line at the place that gives the best dEdx cut:

        # ax2.axvline(cut, linewidth=2, color='g', linestyle="--",
        #             label="Cut at {} MeV/cm".format(cut))
        # y_lims = ax2.get_ylim()
        # plt.text(4.05, y_lims[1]*0.65, "Electron Efficiency:", size=25,)
        # plt.text(7.95, y_lims[1]*0.65,
        #          "{:.2} +/- {:.2}".format(1.0*data_eff_tuple[0][0],
        #                                   1.0*data_eff_tuple[0][1]),
        #          size=25, horizontalalignment='right')
        # plt.text(4.05, y_lims[1]*0.55, "Photon Mis. ID:", size=25,)
        # plt.text(7.95, y_lims[1]*0.55,
        #          "{:.2} +/- {:.2}".format(1.0*data_eff_tuple[1][0],
        #                                   1.0*data_eff_tuple[1][1]),
        #          size=25, horizontalalignment='right')

    # # Now, get the sigma curve (s/sqrt(s**2 + b**2))
    # x_points, sigma = plotSigma(e_data, p_data)
    # x_points, sim_sigma = plotSigma(e_sim, p_sim)

    for tick in ax2.xaxis.get_major_ticks():
        tick.label.set_fontsize(20)

    for tick in ax2.yaxis.get_major_ticks():
        tick.label.set_fontsize(0)

    # ax1 = ax2.twinx()
    # sig_line = ax1.plot(x_points, sigma, linewidth=3, color='g', label="Data")
    # # sig_line = ax1.plot(x_points, sim_sigma, linewidth=3,ls='--',
    # #                     color='g', label="Simulation")

    # # ax1.set_ylim((0, 2.0))
    # ax1.set_ylabel(r"$\frac{S}{\sqrt{S^2 + B^2}}$", color='g',
    #                rotation='horizontal', fontsize=30, labelpad=25)

    # ax1.tick_params(axis='y', which='major', labelsize=16, colors='g')
    # # for tick in ax1.yaxis.get_major_ticks():
    # # tick.label.set_fontsize(26)
    # # tick.label.set_color('g')

    # # print e_line
    # # print p_line
    # # print sig_line

    # # lns = e_line+p_line+sig_line
    # # labs = [l.get_label() for l in lns]
    # # ax2.legend(lns, labs, loc=0)

    # # Draw a histogram with all of these:
    ax2.grid(True)
    ax2.legend(fontsize=20)
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
    x_points = numpy.arange(0.1, 8.1, 0.1)

    min_dedx = 0.1
    max_dedx = 8.0

    sigma = []

    for point in x_points:
        bins = [min_dedx, point, max_dedx]
        e_hist, junk = numpy.histogram(e_data, bins)
        e_acc, e_rej = e_hist[0], e_hist[1]
        e_acc /= float(e_acc + e_rej)
        p_hist, junk = numpy.histogram(p_data, bins)
        p_acc, p_rej = p_hist[0], p_hist[1]
        p_acc /= float(p_acc + p_rej)
        # print "Cut at {}: {} eff., {} mis. ID".format(
        #     point,
        #     e_acc,
        #     p_acc)
        # if p_acc >0 or e_acc > 0:
        sigma.append( e_acc / math.sqrt(e_acc**2 + p_acc**2 +0.001))
        # else:
            # sigma.append(0)

    index, value = max(enumerate(sigma), key=operator.itemgetter(1))

    # fig, ax = plt.subplots()
    # plt.plot(x_points, sigma, label=r"$\frac{Sig.}{\sqrt{Bkg.^2 + Sig^2}}$")
    # ax.axvline(x_points[index], color='g', ls="--")

    # plt.xlabel("dE/dx [MeV/cm]")
    # plt.legend(fontsize=25)
    # plt.grid(True)
    # plt.savefig("/data_linux/dedx_plots/1D/sigma_dedx_1D_"+branch_name + ".png")
    # plt.show()
    return x_points, sigma


if __name__ == '__main__':
    main()
