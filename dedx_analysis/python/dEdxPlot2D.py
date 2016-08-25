import landau
import numpy
import math
import ROOT

from matplotlib.ticker import NullFormatter
from matplotlib import pyplot as plt
from matplotlib.colors import LogNorm, Normalize

import showerCalo

# from dEdxPlots1D import makeCut

from collections import namedtuple


dataSet = namedtuple("dataSet",
                     ["values_x",
                      "values_y",
                      "x_errs",
                      "y_errs",
                      "label",
                      "marker",
                      "color"])


def main():

    # (e_data, e_sim), (p_data, p_sim) = showerCalo.full_samples()
    (e_data, e_sim), (p_data, p_sim) = showerCalo.lite_samples()
    # (e_data, e_sim), (p_data, p_sim) = showerCalo.full_simch_samples()
    # (e_data, e_sim), (p_data, p_sim) = showerCalo.lite_simch_samples()
    adjustWeightsToNCPi0_Gauss(482.6,635,5793.0,p_sim)

    # e_data.getShowerCaloVector().set_drop_first_hit(True)

    # e_sim_energy_vs_dedx(e_sim)
    # p_sim_energy_vs_dedx(p_sim)

    # Check the dE/dx

    # showers = e_sim.getShowerCaloVector()

    # i = 0
    # for shower in showers:

    #     # for pair in shower._distance_and_E:
    #     #   print "  {:.3}: {:.2}".format(pair.first, pair.second)
    #     print "{} {:.2} {:.2}".format(i,shower.dEdx_best_median(),
    #                                 shower.true_dEdx_median(0))
    #     print "  dedx vec:"

    #     i += 1
    #     if i > 10:
    #       break

    e_true_vs_reco_dedx(e_sim)
    p_true_vs_reco_dedx(p_sim)

    # data_collection_vs_induction(e_data,p_data)
    # sim_collection_vs_induction(e_sim,p_sim)

    # pitch_vs_angular_resolution(e_sim)
    # pitch_vs_angular_resolution(p_sim)


def e_true_vs_reco_dedx(e_sim):

    # Build the electron data set:
    electronDataSet = dataSet(e_sim.getBestMedianVector(),
                              e_sim.getShowerCaloVector().true_dedx_median(1),
                              None,
                              None,
                              "Electron Simulation",
                              marker="o",
                              color="b")

    dedx_bins = numpy.arange(0.1, 4.0, 0.05)

    x_label = "Reco. dE/dx [MeV/cm]"
    y_label = "True dE/dx [MeV/cm]"

    dEdxCorrelationHist(electronDataSet,
                        dedx_bins,
                        dedx_bins,
                        x_label,
                        y_label)

def p_true_vs_reco_dedx(p_sim):

    # Build the electron data set:
    electronDataSet = dataSet(p_sim.getBestMedianVector(),
                              p_sim.getShowerCaloVector().true_dedx_median(1),
                              None,
                              None,
                              "Electron Simulation",
                              marker="o",
                              color="b")

    dedx_bins = numpy.arange(1.0, 7.0, 0.1)

    x_label = "Reco. dE/dx [MeV/cm]"
    y_label = "True dE/dx [MeV/cm]"

    dEdxCorrelationHist(electronDataSet,
                        dedx_bins,
                        dedx_bins,
                        x_label,
                        y_label)


def pitch_vs_angular_resolution(e_sim):

    # Build the electron data set:
    electronDataSet = dataSet(e_sim.getShowerCaloVector().pitch(1),
                              e_sim.getShowerCaloVector(
    ).mc_3D_angle_resolution(),
        None,
        None,
        "Electron Simulation",
        marker="o",
        color="b")

    pitch_bins = numpy.arange(0.1, 2.0, 0.025)
    angle_bins = numpy.arange(0.0, 20.0, 0.25)

    x_label = "Collection pitch [cm]"
    y_label = "Angular Resolution [deg.]"

    dEdxCorrelationHist(electronDataSet,
                        pitch_bins,
                        angle_bins,
                        x_label,
                        y_label)


def dist_vs_dedx(e_data, p_data):

    # Build the electron data set:
    electronDataSet = dataSet(e_data.getShowerCaloVector().distance(0),
                              e_data.getMetaVector(0),
                              None,
                              None,
                              "Electron Data",
                              marker="o",
                              color="b")

    photonDataSet = dataSet(p_data.getShowerCaloVector().distance(0),
                            p_data.getMetaVector(0),
                            None,
                            None,
                            "Photon Data",
                            marker="x",
                            color="r")

    dedx_bins = numpy.arange(0.1, 8.0, 0.2)
    distance_bins = numpy.arange(0.0, 6.0, 0.25)
    x_label = "Induction Distance [cm]"
    y_label = "Induction dE/dx [MeV/cm]"

    dEdxCorrelation([photonDataSet, electronDataSet],
                    distance_bins,
                    dedx_bins,
                    x_label,
                    y_label)


def dedx_vs_angular_resolution(e_sim):

    # Build the electron data set:
    electronDataSet = dataSet(e_sim.getMetaVector(1),
                              e_sim.getShowerCaloVector(
    ).mc_3D_angle_resolution(),
        None,
        None,
        "Electron Simulation",
        marker="o",
        color="b")

    dedx_bins = numpy.arange(0.1, 8.0, 0.2)
    angle_bins = numpy.arange(0.0, 20.0, 0.25)

    x_label = "Collection dE/dx [MeV/cm]"
    y_label = "Angular Resolution"

    dEdxCorrelationHist(electronDataSet,
                        dedx_bins,
                        angle_bins,
                        x_label,
                        y_label)


def e_sim_energy_vs_dedx(e_sim):

    # Build the electron data set:
    electronDataSet = dataSet(e_sim.getMedianVector(1),
                              e_sim.getShowerCaloVector().mc_true_energy(),
                              None,
                              None,
                              "Electron Simulation",
                              marker="o",
                              color="b")

    dedx_bins = numpy.arange(0.1, 8.0, 0.2)
    energy_bins = numpy.arange(0, 5000, 250)

    x_label = "Collection dE/dx [MeV/cm]"
    y_label = "Simulated Energy [MeV]"

    dEdxCorrelationHist(electronDataSet,
                        dedx_bins,
                        energy_bins,
                        x_label,
                        y_label)


def p_sim_energy_vs_dedx(p_sim):

    # Build the electron data set:
    photonDataSet = dataSet(p_sim.getMedianVector(1),
                            p_sim.getShowerCaloVector().mc_true_energy(),
                            None,
                            None,
                            "Photon Simulation",
                            marker="o",
                            color="b")

    dedx_bins = numpy.arange(0.1, 8.0, 0.2)
    energy_bins = numpy.arange(0, 2000, 100)

    x_label = "Collection dE/dx [MeV/cm]"
    y_label = "Simulated Energy [MeV]"

    dEdxCorrelationHist(photonDataSet,
                        dedx_bins,
                        energy_bins,
                        x_label,
                        y_label)


def data_collection_vs_induction(e_data, p_data):

    # Build the electron data set:
    electronDataSet = dataSet(e_data.getMetaVector(1),
                              e_data.getMetaVector(0),
                              None,
                              None,
                              "Electron Data",
                              marker="o",
                              color="b")

    photonDataSet = dataSet(p_data.getMedianVector(1),
                            p_data.getMedianVector(0),
                            None,
                            None,
                            "Photon Data",
                            marker="x",
                            color="r")

    dedx_bins = numpy.arange(0.1, 8.0, 0.2)

    x_label = "Collection dE/dx [MeV/cm]"
    y_label = "Induction dE/dx [MeV/cm]"

    dEdxCorrelation([photonDataSet, electronDataSet],
                    dedx_bins,
                    dedx_bins,
                    x_label,
                    y_label)


def sim_collection_vs_induction(e_sim, p_sim):

    # Build the electron data set:
    electronSimSet = dataSet(e_sim.getMedianVector(1),
                             e_sim.getMedianVector(0),
                             None,
                             None,
                             "Electron Sim",
                             marker="o",
                             color="b")

    photonSimSet = dataSet(p_sim.getMedianVector(1),
                           p_sim.getMedianVector(0),
                           None,
                           None,
                           "Photon Sim",
                           marker="x",
                           color="r")

    dedx_bins = numpy.arange(0.1, 8.0, 0.2)

    x_label = "Collection dE/dx [MeV/cm]"
    y_label = "Induction dE/dx [MeV/cm]"

    dEdxCorrelation([electronSimSet, photonSimSet],
                    dedx_bins,
                    dedx_bins,
                    x_label,
                    y_label)


def dEdxCorrelation(data_set_list, x_bins, y_bins, x_label, y_label):

    # Set up the plots:
    # definitions for the axes
    left, width = 0.1, 0.65
    bottom, height = 0.1, 0.65
    bottom_h = left_h = left + width + 0.02

    rect_scatter = [left, bottom, width, height]
    rect_histx = [left, bottom_h, width, 0.2]
    rect_histy = [left_h, bottom, 0.2, height]

    # start with a rectangular Figure
    plt.figure(1, figsize=(8, 8))

    axScatter = plt.axes(rect_scatter)
    # plt.xlabel("dE/dx [MeV/cm], {l}".format(l=branch_name_x))
    plt.xlabel(x_label)
    # plt.ylabel("dE/dx [MeV/cm], {l}".format(l=branch_name_y))
    plt.ylabel(y_label)

    axHistx = plt.axes(rect_histx)
    axHisty = plt.axes(rect_histy)

    nullfmt = NullFormatter()         # no labels

    # no labels
    axHistx.xaxis.set_major_formatter(nullfmt)
    axHistx.yaxis.set_major_formatter(nullfmt)
    axHisty.xaxis.set_major_formatter(nullfmt)
    axHisty.yaxis.set_major_formatter(nullfmt)

    axHistx.set_ylim([0, 1.0])
    axHisty.set_xlim([0, 1.0])

    # Unpack the tuples and draw them:

    for _set in data_set_list:

        hist_data_x, x_bin_edges = numpy.histogram(
            _set.values_x, x_bins, density=True)
        hist_data_y, y_bin_edges = numpy.histogram(
            _set.values_y, y_bins, density=True)

        max_val = numpy.max((numpy.max(hist_data_x), numpy.max(hist_data_y)))

        hist_data_x *= 0.75/max_val
        hist_data_y *= 0.75/max_val

        x_centers = []
        for i in xrange(len(x_bin_edges) - 1):
            x_centers.append(0.5*(x_bin_edges[i] + x_bin_edges[i+1]))
        y_centers = []
        for i in xrange(len(y_bin_edges) - 1):
            y_centers.append(0.5*(y_bin_edges[i] + y_bin_edges[i+1]))

        # the scatter plot:
        axScatter.errorbar(_set.values_x,
                           _set.values_y,
                           xerr=_set.x_errs,
                           yerr=_set.y_errs,
                           ls="",
                           marker=_set.marker,
                           label=_set.label, c=_set.color)

        axHistx.errorbar(x_centers, hist_data_x,
                         # xerr=binwidth*0.5,
                         # yerr=electron_err_norm_x,
                         label=_set.label,
                         capsize=0,
                         marker=_set.marker,
                         color=_set.color)

        axHisty.errorbar(hist_data_y, y_centers,
                         # xerr=binwidth*0.5,
                         # yerr=electron_err_norm_x,
                         label=_set.label,
                         capsize=0,
                         marker=_set.marker,
                         color=_set.color)

    axScatter.legend()

    # # Set limits for dE/dx values
    axScatter.set_xlim((x_bins[0], x_bins[-1]))
    axScatter.set_ylim((y_bins[0], y_bins[-1]))
    # axScatter.grid(True)

    axHistx.set_xlim(axScatter.get_xlim())
    axHisty.set_ylim(axScatter.get_ylim())

    axHistx.grid(True)
    axHisty.grid(True)
    axScatter.grid(True)
    # plt.legend()

    plt.show()


def dEdxCorrelationHist(data_set, x_bins, y_bins, x_label, y_label):

    # Set up the plots:
    # definitions for the axes
    left, width = 0.1, 0.65
    bottom, height = 0.1, 0.65
    bottom_h = left_h = left + width + 0.02

    rect_scatter = [left, bottom, width, height]
    rect_histx = [left, bottom_h, width, 0.2]
    rect_histy = [left_h, bottom, 0.2, height]

    # start with a rectangular Figure
    plt.figure(1, figsize=(8, 8))

    axScatter = plt.axes(rect_scatter)
    # plt.xlabel("dE/dx [MeV/cm], {l}".format(l=branch_name_x))
    plt.xlabel(x_label,fontsize=20)
    # plt.ylabel("dE/dx [MeV/cm], {l}".format(l=branch_name_y))
    plt.ylabel(y_label,fontsize=20)

    axHistx = plt.axes(rect_histx)
    axHisty = plt.axes(rect_histy)

    nullfmt = NullFormatter()         # no labels

    # no labels
    axHistx.xaxis.set_major_formatter(nullfmt)
    axHistx.yaxis.set_major_formatter(nullfmt)
    axHisty.xaxis.set_major_formatter(nullfmt)
    axHisty.yaxis.set_major_formatter(nullfmt)

    # axHistx.set_ylim([0, 1.0])
    # axHisty.set_xlim([0, 1.0])

    hist_data_x, x_bin_edges = numpy.histogram(
        data_set.values_x, x_bins, density=True)
    hist_data_y, y_bin_edges = numpy.histogram(
        data_set.values_y, y_bins, density=True)

    hist_data_x *= 0.75/numpy.max(hist_data_x)
    hist_data_y *= 0.75/numpy.max(hist_data_y)

    x_centers = []
    for i in xrange(len(x_bin_edges) - 1):
        x_centers.append(0.5*(x_bin_edges[i] + x_bin_edges[i+1]))
    y_centers = []
    for i in xrange(len(y_bin_edges) - 1):
        y_centers.append(0.5*(y_bin_edges[i] + y_bin_edges[i+1]))

    cmap = plt.get_cmap('jet')
    cmap.set_under('w')

    # the scatter plot:
    axScatter.hist2d(data_set.values_x,
                     data_set.values_y,
                     bins=[x_bin_edges, y_bin_edges],
                     label=data_set.label,
                     vmin=0.01,
                     cmap=cmap)
                     # cmap="nipy_spectral_r")

    axScatter.plot([0,10],[0,10],ls="--",color='black',linewidth=2)

    axHistx.errorbar(x_centers, hist_data_x,
                     # xerr=binwidth*0.5,
                     # yerr=electron_err_norm_x,
                     label=data_set.label,
                     capsize=0,
                     marker=data_set.marker,
                     color=data_set.color)

    axHisty.errorbar(hist_data_y, y_centers,
                     # xerr=binwidth*0.5,
                     # yerr=electron_err_norm_x,
                     label=data_set.label,
                     capsize=0,
                     marker=data_set.marker,
                     color=data_set.color)

    for tick in axScatter.xaxis.get_major_ticks():
        tick.label.set_fontsize(20)
    for tick in axScatter.yaxis.get_major_ticks():
        tick.label.set_fontsize(20)

    axScatter.legend()

    # # Set limits for dE/dx values
    axScatter.set_xlim((x_bins[0], x_bins[-1]))
    axScatter.set_ylim((y_bins[0], y_bins[-1]))
    # axScatter.grid(True)

    axHistx.set_xlim(axScatter.get_xlim())
    axHisty.set_ylim(axScatter.get_ylim())

    axHistx.grid(True)
    axHisty.grid(True)
    axScatter.grid(True)
    # plt.legend()

    plt.show()




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
nc_max = numpy.max(ncpi0_y)

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
    energy_spectrum, bins = numpy.histogram(
        p_sim.getShowerCaloVector().mc_true_energy(),
        bins=bin_edges,
        density=True)

    # print 0.5*(bins[0] + bins[1])
    # print 0.5*(bins[1] + bins[2])
    # print 0.5*(bins[2] + bins[3])

    # print "------------"

    # exit(0)

    # scale this spectrum to peak at 1:
    sim_max = numpy.max(energy_spectrum)
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
            weights[i] = (ncpi0_y[i] + scale*extra  ) / energy_spectrum[i]

    # # Corrected:

    # print "\n\nCorrected:"

    # for i in xrange(len(energy_spectrum)):
    #     print "{:.2} vs {:.2}".format(ncpi0_y[i], weights[i]*energy_spectrum[i])

    # # Adjust the weights above 500 MeV:
    # for i in xrange(len(ncpi0_y)):
    #   if ncpi0_x[i] > 200:
    #     weights[i] *= 2.5
    #   if ncpi0_x[i] > 500:
    #     weights[i] *= 1.5

    p_sim.getShowerCaloVector().set_weights(weights, bin_edges)


if __name__ == '__main__':
    main()
