from matplotlib.ticker import NullFormatter
from matplotlib import pyplot as plt

import ROOT
import numpy
import showerCalo

import math

from matplotlib import pyplot as plt


def main():

    (e_data, e_sim), (p_data, p_sim) = showerCalo.lite_samples()

    print e_data.size()
    print e_sim.size()
    print p_data.size()
    print p_sim.size()

    measure = "charge"
    recomb = "const"

    plane_cut(e_data, 0.3)
    plane_cut(e_sim, 0.3)


def plane_cut(dataset, cut=0.25):

    # electrons_df._df.info()
    c_electons_dedx_meta = dataset.getShowerCaloVector().dEdx_meta(1)
    i_electons_dedx_meta = dataset.getShowerCaloVector().dEdx_meta(0)
    c_electons_dedx_meta_err = dataset.getShowerCaloVector().dEdx_meta_err(1)
    i_electons_dedx_meta_err = dataset.getShowerCaloVector().dEdx_meta_err(0)

    # # Want to print out the dedx values and their mean, rms:
    # c_dedx_modmean = dataset.getShowerCaloVector().dEdx_modmean(1)
    # c_dedx_median = dataset.getShowerCaloVector().dEdx_median(1)
    # c_dedx_LMA = dataset.getShowerCaloVector().dEdx_LMA(1)

    # for (modmean, median, lma, meta, err) in zip(c_dedx_modmean,
    #                                              c_dedx_median,
    #                                              c_dedx_LMA,
    #                                              c_electons_dedx_meta,
    #                                              c_electons_dedx_meta_err):
    #     print "{:.3}  {:.3}  {:.3}  {:.3}  {:.3}  {:.3}".format(
    #         modmean, median, lma, meta, err, err/meta)

    c_relative_err = []
    i_relative_err = []
    for val, err in zip(c_electons_dedx_meta, c_electons_dedx_meta_err):
        if val != 0:
            c_relative_err.append(err/val)
        else:
            c_relative_err.append(0.0)

    for val, err in zip(i_electons_dedx_meta, i_electons_dedx_meta_err):
        if val != 0:
            i_relative_err.append(err/val)
        else:
            i_relative_err.append(0.0)

    bins = [0, cut, 5]
    c_electron_hist, bin_edges = numpy.histogram(c_relative_err,
                                                 bins=bins,
                                                 density=False)
    i_electron_hist, bin_edges = numpy.histogram(i_relative_err,
                                                 bins=bins,
                                                 density=False)

    print c_electron_hist
    print i_electron_hist

    coll_eff = 1.0*c_electron_hist[0]/(c_electron_hist[0] + c_electron_hist[1])
    ind_eff = 1.0*i_electron_hist[0]/(i_electron_hist[0] + i_electron_hist[1])

    x = bin_edges[:-1] + 0.5*(bin_edges[1] - bin_edges[0])

    bins = numpy.arange(0, 0.8, 0.025)

    fig, axes = plt.subplots()
    # plt.plt(x, c_electron_hist,
    #          # marker="o", ls="none",
    plt.hist(c_relative_err, bins, normed=True,
             label="Collection Relative Error", alpha=0.5)
    # plt.plt(x, i_electron_hist,
    #          # marker="o", ls="none",
    plt.hist(i_relative_err, bins, normed=True,
             label="Induction Relative Error", alpha=0.5)

    # Put a vertical line at the cut, 0.25
    plt.axvline(cut)

    plt.text(0.4, 4, "Collection Eff: {:.3}%".format(
        coll_eff*100.), fontsize=20)
    plt.text(0.4, 3.5, "Induction Eff: {:.3}%".format(
        ind_eff*100.), fontsize=20)
    plt.title("dE/dx Consistency Check, Electrons")
    plt.legend()
    plt.grid()
    plt.show()

if __name__ == '__main__':
    main()
