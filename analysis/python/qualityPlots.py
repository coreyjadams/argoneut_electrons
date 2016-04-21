import numpy as np
import math

from matplotlib.ticker import NullFormatter
from matplotlib import pyplot as plt

from dataFrameHandle import dataFrameHandle


def main():
    electrons_df = dataFrameHandle(
        "../anatrees/electrons_anatree.root", "electron")
    # photons_df = None
    photons_df =  dataFrameHandle("../anatrees/photons_anatree.root", "photon")
    # electrons_df.make_quality_cuts()
    # photons_df.make_quality_cuts()

    measure = "charge"
    recomb = "const"

    # plane_cut(electrons_df)
    plane_cut(photons_df)


def plane_cut(electrons_df):

    nbins = 20
    electrons_df._df.info()
    c_electons_dedx_meta = electrons_df._df["c_charge_dedx_box_meta_0"]
    i_electons_dedx_meta = electrons_df._df["i_charge_dedx_box_meta_0"]
    c_electons_dedx_meta_err = electrons_df._df["c_charge_dedx_box_meta_0_err"]
    i_electons_dedx_meta_err = electrons_df._df["i_charge_dedx_box_meta_0_err"]

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

    bins = [0,0.25,5]
    c_electron_hist, bin_edges = np.histogram(c_relative_err,
                                              bins=bins,
                                              density=False)
    i_electron_hist, bin_edges = np.histogram(i_relative_err,
                                              bins=bins,
                                              density=False)

    print c_electron_hist
    print i_electron_hist

    coll_eff = 1.0*c_electron_hist[0]/(c_electron_hist[0] + c_electron_hist[1])
    ind_eff = 1.0*i_electron_hist[0]/(i_electron_hist[0] + i_electron_hist[1])

    x = bin_edges[:-1] + 0.5*(bin_edges[1] - bin_edges[0])


    fig, axes = plt.subplots()
    # plt.plt(x, c_electron_hist,
    #          # marker="o", ls="none",
    plt.hist(c_relative_err,30,normed=True,
             label="Collection Relative Error",alpha=0.5)
    # plt.plt(x, i_electron_hist,
    #          # marker="o", ls="none",
    plt.hist(i_relative_err,30,normed=True,
             label="Induction Relative Error",alpha=0.5)

    # Put a vertical line at the cut, 0.25
    plt.axvline(0.25)

    plt.text(0.8,4,"Collection Eff: {:.3}%".format(coll_eff*100.),fontsize=20)
    plt.text(0.8,3.5,"Induction Eff: {:.3}%".format(ind_eff*100.),fontsize=20)
    plt.title("dE/dx Consistency Check, Photons")
    plt.legend()
    plt.grid()
    plt.show()

if __name__ == '__main__':
    main()
