import landau
import numpy as np
import math

from matplotlib.ticker import NullFormatter
from matplotlib import pyplot as plt

from dataFrameHandle import dataFrameHandle

from dEdxPlots1D import makeCut


def main():
    electron_df = dataFrameHandle(
        "../anatrees/electrons_anatree_fix.root", "electron")
    photon_df = dataFrameHandle("../anatrees/photons_anatree_fix.root", "photon")
    electron_df.make_quality_cuts(1.5)
    photon_df.make_quality_cuts(1.5)

    starting_electrons = len(electron_df._df.index)
    starting_photons = len(photon_df._df.index)

    print starting_electrons
    print starting_photons

    measure = "charge"
    recomb = "const"

    # Cut 1:
    # for i in reversed(np.arange(0.125, 0.725, 0.05)):
    # Cut 2:
    # for i in reversed(np.arange(0.05, 1.0, 0.02)):
    # electron_df.make_quality_cuts(i)
    # photon_df.make_quality_cuts(i)
    dEdxCorrelation(electron_df._df,
                    photon_df._df,
                    "c_charge_dedx_const_meta_0",
                    "i_charge_dedx_const_meta_0",
                    1.5,
                    starting_electrons,
                    starting_photons)

    # dEdxCorrelation(electron_df._df,photon_df._df,"c_charge_dedx_const_median_0","i_charge_dedx_const_median_0")
    # dEdxCorrelation(electron_df._df,photon_df._df,"c_charge_dedx_const_mean_no_outliers_0","i_charge_dedx_const_mean_no_outliers_0")
    # dEdxCorrelation(electron_df._df,photon_df._df,"c_charge_dedx_const_LMA","i_charge_dedx_const_LMA")


def dEdxCorrelation(electrons_df, photons_df, branch_name_x, branch_name_y, coll_ind_cut,starting_electrons,starting_photons):

    # temp_e_df = electrons_df
    # temp_p_df = photons_df

    # temp_e_df.make_quality_cuts(1.5)
    # temp_p_df.make_quality_cuts(1.5)

    # current_electrons = len(temp_e_df.index)
    # current_photons = len(temp_p_df.index)

    collection_cut = 3.0
    induction_cut = 2.5

    binwidth = 0.25
    bins = np.arange(0, 10 + binwidth, binwidth)
    electron_data_x, bin_edges = np.histogram(
        electrons_df[branch_name_x], bins)
    electron_data_y, bin_edges = np.histogram(
        electrons_df[branch_name_y], bins)
    photon_data_x, bin_edges = np.histogram(photons_df[branch_name_x], bins)
    photon_data_y, bin_edges = np.histogram(photons_df[branch_name_y], bins)

    coll_purity, coll_eff_tup = makeCut(
        electrons_df, photons_df, branch_name_x, collection_cut)
    ind_purity, ind_eff_tup = makeCut(
        electrons_df, photons_df, branch_name_y, induction_cut)

    electron_frac_err_x = []
    electron_frac_err_y = []
    photon_frac_err_x = []
    photon_frac_err_y = []
    for e_val, p_val in zip(electron_data_x, photon_data_x):
        if e_val != 0:
            electron_frac_err_x.append(math.sqrt(e_val) / e_val)
        else:
            electron_frac_err_x.append(0.0)
        if p_val != 0:
            photon_frac_err_x.append(math.sqrt(p_val) / p_val)
        else:
            photon_frac_err_x.append(0.0)
    for e_val, p_val in zip(electron_data_y, photon_data_y):
        if e_val != 0:
            electron_frac_err_y.append(math.sqrt(e_val) / e_val)
        else:
            electron_frac_err_y.append(0.0)
        if p_val != 0:
            photon_frac_err_y.append(math.sqrt(p_val) / p_val)
        else:
            photon_frac_err_y.append(0.0)

    x = bin_edges + 0.25

    electron_data_norm_x, bin_edges = np.histogram(
        electrons_df[branch_name_x], bins, density=True)
    electron_data_norm_y, bin_edges = np.histogram(
        electrons_df[branch_name_y], bins, density=True)
    photon_data_norm_x, bin_edges = np.histogram(
        photons_df[branch_name_x], bins, density=True)
    photon_data_norm_y, bin_edges = np.histogram(
        photons_df[branch_name_y], bins, density=True)

    electron_err_norm_x = []
    electron_err_norm_y = []
    for e_val, frac_err in zip(electron_data_norm_x, electron_frac_err_x):
        electron_err_norm_x.append(e_val*frac_err)
    for e_val, frac_err in zip(electron_data_norm_y, electron_frac_err_y):
        electron_err_norm_y.append(e_val*frac_err)

    photon_err_norm_x = []
    photon_err_norm_y = []
    for p_val, frac_err in zip(photon_data_norm_x, photon_frac_err_x):
        photon_err_norm_x.append(p_val*frac_err)
    for p_val, frac_err in zip(photon_data_norm_y, photon_frac_err_y):
        photon_err_norm_y.append(p_val*frac_err)

    # If cut_val is not -1, it means a cut on abs(ind - col) was made
    # Display this with a box on the plots.

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
    plt.xlabel("dE/dx [MeV/cm], {l}".format(l="Collection Plane"))
    # plt.ylabel("dE/dx [MeV/cm], {l}".format(l=branch_name_y))
    plt.ylabel("dE/dx [MeV/cm], {l}".format(l="Induction Plane"))

    # axScatter.text(2,8,"El cut eff: {:.2}\nPh. cut eff: {:.2}\nCut: {:.2}".format(
    #     1.0*current_electrons/starting_electrons,
    #     1.0*current_photons/starting_photons,
    #     coll_ind_cut))

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

    # the scatter plot:
    axScatter.scatter(electrons_df[branch_name_x],
                      electrons_df[branch_name_y],
                      marker="o",
                      label="Electrons", c="blue")

    axScatter.scatter(photons_df[branch_name_x],
                      photons_df[branch_name_y],
                      marker="x",
                      label="Photons", c="red")
    axScatter.legend()

    # # Cut 1:
    # # Draw lines with slope = 1 at a x and y displacement
    # val = (1 - coll_ind_cut) / (coll_ind_cut + 1)
    # axScatter.plot((0, 10), (0, 10*val), "g", ls="-")
    # axScatter.plot((0, 10*val), (0, 10), "g")

    # Cut 2:
    # # Draw lines with slope = 1 at a x and y displacement
    axScatter.plot((coll_ind_cut, 10), (0, 10-coll_ind_cut), "g", ls="-")
    axScatter.plot((0, 10-coll_ind_cut), (coll_ind_cut, 10), "g")


    # axScatter.plot(y,x)

    # Set limits for dE/dx values
    axScatter.set_xlim((0, 10))
    axScatter.set_ylim((0, 10))
    axScatter.grid(True)

    axHistx.set_xlim(axScatter.get_xlim())
    axHisty.set_ylim(axScatter.get_ylim())

    axHistx.grid(True)

    # axHistx.set_title("Electron Photon separation, normalized")
    axHistx.errorbar(x[:-1], electron_data_norm_x, xerr=binwidth*0.5,
                     yerr=electron_err_norm_x, label="Electrons", capsize=0,marker="o")
    axHistx.errorbar(x[:-1], photon_data_norm_x, xerr=binwidth*0.5,
                     yerr=photon_err_norm_x, label="Photons", capsize=0, color='r',marker="D")
    axHistx.axvline(collection_cut, color="g", ls="--")
    axHistx.text(5, 0.5, "Electron Eff: {:.2} +/- {:.2}\nPhoton MisID: {:.2} +/- {:.2}".format(
        coll_eff_tup[0][0],
        coll_eff_tup[0][1],
        coll_eff_tup[1][0],
        coll_eff_tup[1][1]))

    axHisty.grid(True)
    # axHisty.set_title("Electron Photon separation, normalized")
    axHisty.errorbar(electron_data_norm_y, x[:-1], yerr=binwidth*0.5,
                     xerr=electron_err_norm_y, label="Electrons", capsize=0,marker="o")
    axHisty.errorbar(photon_data_norm_y, x[:-1], yerr=binwidth*0.5,
                     xerr=photon_err_norm_y, label="Photons", capsize=0, color='r',marker="D")
    axHisty.axhline(induction_cut, color="g", ls="--")
    axHisty.text(0.6, 9, "Electron Eff: {:.2} +/- {:.2}\nPhoton MisID: {:.2} +/- {:.2}".format(
        ind_eff_tup[0][0],
        ind_eff_tup[0][1],
        ind_eff_tup[1][0],
        ind_eff_tup[1][1]), rotation=-90)
    # plt.legend()
    plt.grid(True)

    # plt.savefig("/data_linux/dedx_plots/ScatterPlots/LineCut_1/" +
                # "scatter_dedx_{}.png".format(coll_ind_cut))
    # plt.close()
    plt.show()


if __name__ == '__main__':
    main()
