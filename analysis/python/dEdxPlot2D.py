import landau
import numpy
import math

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

    # e_data.getShowerCaloVector().set_drop_first_hit(True)

    # e_sim_energy_vs_dedx(e_sim)
    # p_sim_energy_vs_dedx(p_sim)

    data_collection_vs_induction(e_data,p_data)
    # sim_collection_vs_induction(e_sim,p_sim)

    # pitch_vs_angular_resolution(e_sim)
    # pitch_vs_angular_resolution(p_sim)


def pitch_vs_angular_resolution(e_sim):

    # Build the electron data set:
    electronDataSet = \
      dataSet(e_sim.getShowerCaloVector().pitch(1),
              e_sim.getShowerCaloVector().mc_3D_angle_resolution(),
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
    electronDataSet = dataSet(e_sim.getMetaVector(1),
                              e_sim.getShowerCaloVector().mc_true_energy(),
                              None,
                              None,
                              "Electron Simulation",
                              marker="o",
                              color="b")

    dedx_bins = numpy.arange(0.1, 8.0, 0.2)
    energy_bins = numpy.arange(0, 5000, 50)

    x_label = "Collection dE/dx [MeV/cm]"
    y_label = "Simulated Energy [MeV]"

    dEdxCorrelationHist(electronDataSet,
                        dedx_bins,
                        energy_bins,
                        x_label,
                        y_label)


def p_sim_energy_vs_dedx(p_sim):

    # Build the electron data set:
    photonDataSet = dataSet(p_sim.getMetaVector(1),
                            p_sim.getShowerCaloVector().mc_true_energy(),
                            None,
                            None,
                            "Photon Simulation",
                            marker="o",
                            color="b")

    dedx_bins = numpy.arange(0.1, 8.0, 0.2)
    energy_bins = numpy.arange(0, 2000, 50)

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

    # the scatter plot:
    axScatter.hist2d(data_set.values_x,
                     data_set.values_y,
                     bins=[x_bin_edges, y_bin_edges],
                     label=data_set.label,
                     norm=LogNorm(),
                     cmap="rainbow")

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


if __name__ == '__main__':
    main()
