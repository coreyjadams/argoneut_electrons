import operator

import ROOT
import numpy
import showerCalo

import math

from matplotlib import pyplot as plt

import numpy as np


def main():

    # (e_data, e_sim), (p_data, p_sim) = showerCalo.full_samples()
    (e_data, e_sim), (p_data, p_sim) = showerCalo.lite_samples()

    e_mean, e_rms, centers = alldEdxHits(e_data)
    # p_mean, p_rms, centers = alldEdxHits(p_data)

    fig, ax = plt.subplots(figsize=(10, 7))

    plt.errorbar(centers, e_mean, yerr=e_rms, color="b", label="Electrons")
    # plt.plot(centers, p_mean, color="r",label="Gammas")

    plt.xlabel("Distance [cm]")
    plt.ylabel("<dE/dx> [MeV/cm]")

    ax.set_ylim([0, 5.0])

    plt.grid(True)
    plt.legend(fontsize=20)
    plt.show()

    # Also, make a 2D histogram of dE/dx vs. distance:
    e_distances = []
    e_dedx = []
    for shower in e_data.getShowerCaloVector():
        values = shower.dEdx_hits_and_distances(0)
        if values.size() > 100:
            continue
        # print values.size()
        # print len(values)
        for value in values:
            e_distances.append(value.first)
            e_dedx.append(value.second)

    # fig, ax = plt.subplots(figsize=(8, 8))

    # dedx_bins = numpy.arange(0.0, 10.01, 0.5)
    # dist_bins = numpy.arange(0.0, 15.01, 1)

    # cmap = plt.get_cmap('jet')
    # cmap.set_under('w')

    # # the scatter plot:
    # ax.hist2d(e_distances,
    #           e_dedx,
    #           bins=[dist_bins,dedx_bins],
    #           vmin=0.01,
    #           cmap=cmap)
    # # cmap="nipy_spectral_r")
    # plt.grid(True)
    # plt.show()


def alldEdxHits(_data):

    # Loop over all showers and accumulate the dE/dx values at
    # set distance steps

    # Need a set of lists, essentially, to dictate where to
    # draw boundaries.

    boundaries = numpy.arange(0.5, 10.01, 0.5)
    centers = 0.5*(boundaries[1:] + boundaries[0:-1])
    # print boundaries
    # print centers

    accumulators = []
    for i in xrange(len(centers)):
        accumulators.append([])

    for shower in _data.getShowerCaloVector():
        values = shower.dEdx_hits_and_distances(0)
        if values.size() > 100:
            continue
        # print values.size()
        # print len(values)
        for value in values:
            # Each "value" is a pair (distance, dE/dx) for a hit.
            for i in xrange(len(boundaries) - 1):
                if value.first > boundaries[i] and \
                        value.first < boundaries[i+1]:
                    accumulators[i].append(value.second)

    # Now, each distance point has a list of dE/dx values at that point.
    # Compute the mean at each point and the rms.
    mean_vals = numpy.zeros(len(accumulators))
    rms_vals = numpy.zeros(len(accumulators))

    for i in xrange(len(accumulators)):
        mean_vals[i] = numpy.mean(accumulators[i])
        rms_vals[i] = numpy.std(accumulators[i])

    return mean_vals, rms_vals, centers


if __name__ == '__main__':
    main()
