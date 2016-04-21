# import ROOT
# import larlite
import landau
import numpy as np
import scipy.optimize as opt
import math

from matplotlib import pyplot as plt


def photon_gauss_landau(x_points,
                        landau_mpv, landau_sigma_1, gauss_sigma, scale_1,
                        landau_sigma_2, scale_2):
    l1 = landau.gauss_landau(
        x_points, landau_mpv, landau_sigma_1, gauss_sigma, scale_1)
    l2 = landau.gauss_landau(
        x_points, 2*landau_mpv, landau_sigma_2, math.sqrt(2)*gauss_sigma, scale_2)
    return l1 + l2


def double_gauss_landau(x_points,
                        landau_mpv_1, landau_sigma_1, gauss_sigma_1, scale_1,
                        landau_mpv_2, landau_sigma_2, gauss_sigma_2, scale_2):
    l1 = landau.gauss_landau(
        x_points, landau_mpv_1, landau_sigma_1, gauss_sigma_1, scale_1)
    l2 = landau.gauss_landau(
        x_points, landau_mpv_2, landau_sigma_2, gauss_sigma_2, scale_2)
    return l1 + l2


from dataFrameHandle import dataFrameHandle

df = dataFrameHandle("../anatrees/electrons_anatree_fix.root", "electron")
# df = dataFrameHandle("../anatrees/photons_anatree_fix.root", "photon")

df.make_quality_cuts(1.5)

measure = "charge"
recomb = "box"

mode = "electrons"
# mode = "photons"

binwidth = 0.25


for plane in ["collection", "induction"]:

    title = "{}, All {} Hits".format(mode, plane).title()

    hits = []

    name = plane[0] + "_"+  measure + "_dedx_" + recomb

    # Need to dig out all the hit information from each event:
    for index, row in df._df.iterrows():
        cand_hits = row[name]
        dists = row['{}_dist_from_start'.format(plane[0])]
        for hit, dist in zip(cand_hits, dists):
            if dist > 0 and dist < 3.5:
                hits.append(hit)

    bins = np.arange(0, 12, binwidth)
    data, bin_edges = np.histogram(hits, bins)
    err = []
    for point in data:
        try:
            err.append(1.0/math.sqrt(point))
        except:
            err.append(0.0)

    data, bin_edges = np.histogram(hits, bins, density=True)

    err *= data

    x = bin_edges + 0.25
    x = x[:-1]

    f, ax = plt.subplots(figsize=(10, 7))


    x_fit = np.arange(0, 12, 0.25*binwidth)

    if "electron" in mode:
        popt, pcov = opt.curve_fit(
            landau.gauss_landau, x, data, bounds=([0,0,0.1,0], [10, 10, 10, 10]))
        y_fit = landau.gauss_landau(x_fit, popt[0], popt[1], popt[2], 1)
        textstring = "Landau MPV: {:.3}\n".format(popt[0])
        textstring += "Landau $\sigma$: {:.3}\n".format(popt[1])
        textstring += "Gauss $\sigma$: {:.3}\n".format(popt[2])
        textstring += "N: {:}\n".format(len(hits))
        textstring += "<dE/dx>: {:.3}".format(np.mean(hits))
        c='b'

    else:
        # Make a fit to the hits for photons
        c='r'
        
        # This path does independant landaus:
        if False:
            popt, pcov = opt.curve_fit(
                double_gauss_landau,
                x,
                data,
                bounds=([0, 0, 0.1, 0, 0, 0, 0.1, 0],
                        [10, 10, 1, 10, 10, 10, 1, 10]))
            y_fit = double_gauss_landau(x_fit, popt[0], popt[1], popt[2], popt[3],
                                        popt[4], popt[5], popt[6], popt[7])
            textstring = "Landau MPV: {:.3}   {:.3}\n".format(popt[0], popt[4])
            textstring += "Landau $\sigma$: {:.3}   {:.3}\n".format(popt[1], popt[5])
            textstring += "Gauss $\sigma$: {:.3}   {:.3}\n".format(popt[2], popt[6])
            textstring += "N: {:}\n".format(len(hits))
            textstring += "<dE/dx>: {:.3}".format(np.mean(hits))
        # This path does landaus that are connected
        if True:
            popt, pcov = opt.curve_fit(
                photon_gauss_landau,
                x,
                data,
                bounds=([1, 0, 0.1, 0, 0, 0],
                        [3, 10, 1, 10, 10, 10]))
            y_fit = photon_gauss_landau(x_fit, popt[0], popt[1], popt[2], popt[3],
                                        popt[4], popt[5])
            textstring = "Landau MPV: {:.3}   {:.3}\n".format(popt[0], 2*popt[0])
            textstring += "Landau $\sigma$: {:.3}   {:.3}\n".format(popt[1], popt[4])
            textstring += "Gauss $\sigma$: {:.3}   {:.3}\n".format(popt[2], math.sqrt(2)*popt[2])
            textstring += "gamma/e:  {:.3}\n".format(popt[5]/popt[3])
            textstring += "N: {:}\n".format(len(hits))
            textstring += "<dE/dx>: {:.3}".format(np.mean(hits))



    ax.errorbar(x, data, yerr=err, xerr=binwidth*0.5,
                label="{} Hits".format(plane).title(), capsize=0,ls="none",marker="o",color=c)
    # ax.errorbar(x, i_data, yerr=i_err, xerr=binwidth*0.5, label="Induction Hits",capsize=0)
    ax.set_title(title)

    plt.plot(x_fit, y_fit, label="Fitted landau",color='g')
    y_lim = ax.get_ylim()
    # plt.ylim([0, 0.6])

    props = dict(boxstyle='round', facecolor='wheat', alpha=1.0)
    ax.text(6.5, 0.45*y_lim[1], textstring, bbox=props, fontsize=20)

    plt.xlabel("dE/dx [MeV/cm]")
    plt.ylabel("")
    plt.legend()
    plt.grid(True)

    plt.show()

    # if "Electron" in title:
    #     plt.savefig(
    #         "/data_linux/dedx_plots/Landau/landau_electrons_with_cuts_"+name+".png")
    # if "Photon" in title:
    #     plt.savefig(
    #         "/data_linux/dedx_plots/Landau/landau_photons_with_cuts_"+name+".png")
    
    plt.close(f)

