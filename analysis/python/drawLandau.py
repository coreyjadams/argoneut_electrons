# import ROOT
# import larlite
import landau
import numpy as np
import scipy.optimize as opt
import math

import showerCalo

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


def electronFit(electron_data, binwidth, electron_sim=None):

    bins = np.arange(0, 8.0+binwidth, binwidth)
    data, bin_edges = np.histogram(electron_data, bins)

    err = []
    for point in data:
        try:
            err.append(1.0/math.sqrt(point))
        except:
            err.append(0.0)

    data, bin_edges = np.histogram(electron_data, bins, density=True)

    err *= data

    bin_centers = bins[:-1] + 0.5*binwidth

    x_fit = np.arange(0, 8, 0.25*binwidth)


    popt, pcov = opt.curve_fit(
        landau.gauss_landau, bin_centers, data, bounds=([0,0,0.1,0], [10, 10, 10, 10]))
    y_fit = landau.gauss_landau(x_fit, popt[0], popt[1], popt[2], 1)
    textstring = "Landau MPV: {:.3}\n".format(popt[0])
    textstring += "Landau $\sigma$: {:.3}\n".format(popt[1])
    textstring += "Gauss $\sigma$: {:.3}\n".format(popt[2])
    textstring += "N: {:}\n".format(len(electron_data))
    textstring += "<dE/dx>: {:.3}".format(np.mean(electron_data))

    f, ax = plt.subplots(figsize=(10, 7))


    ax.errorbar(bin_centers, data, yerr=err, xerr=binwidth*0.5,
                label="Collection Hits", capsize=0,ls="none",marker="o",color='b')
    # ax.errorbar(x, i_data, yerr=i_err, xerr=binwidth*0.5, label="Induction Hits",capsize=0)
    ax.set_title("Electron dE/dx Hits")

    plt.plot(x_fit, y_fit, label="Fitted landau",color='g')
    y_lim = ax.get_ylim()
    # plt.ylim([0, 0.6])



    props = dict(boxstyle='round', facecolor='wheat', alpha=1.0)
    ax.text(5, 0.45*y_lim[1], textstring, bbox=props, fontsize=20)



    # Draw the simulation if it's available:
    if electron_sim is not None:

        sim, bin_edges = np.histogram(electron_sim, bins,density=True)

        # for i in xrange(len(sim)):
        #     print "[{:.2} - {:.2}]: {:.3}".format(
        #         bin_edges[i], bin_edges[i+1], sim[i])
        ax.plot(bin_edges[1:], sim, color="b",
             ls="steps", label="Simulated Electrons")


    plt.xlabel("dE/dx [MeV/cm]")
    plt.ylabel("")
    plt.legend()
    plt.grid(True)

    plt.show()


def photonFit(photon_data, binwidth,photon_sim=None):

    bins = np.arange(0, 8.0, binwidth)
    data, bin_edges = np.histogram(photon_data, bins)

    err = []
    for point in data:
        try:
            err.append(1.0/math.sqrt(point))
        except:
            err.append(0.0)

    data, bin_edges = np.histogram(photon_data, bins, density=True)

    err *= data

    bin_centers = bin_edges[:-1] + 0.5*binwidth

    x_fit = np.arange(0, 8, 0.25*binwidth)

    # Make a fit to the hits for photons
    
    # This path does independant landaus:
    if False:
        popt, pcov = opt.curve_fit(
            double_gauss_landau,
            bin_centers,
            data,
            bounds=([0, 0, 0.1, 0, 0, 0, 0.1, 0],
                    [10, 10, 1, 10, 10, 10, 1, 10]))
        y_fit = double_gauss_landau(x_fit, popt[0], popt[1], popt[2], popt[3],
                                    popt[4], popt[5], popt[6], popt[7])
        textstring = "Landau MPV: {:.3}   {:.3}\n".format(popt[0], popt[4])
        textstring += "Landau $\sigma$: {:.3}   {:.3}\n".format(popt[1], popt[5])
        textstring += "Gauss $\sigma$: {:.3}   {:.3}\n".format(popt[2], popt[6])
        textstring += "N: {:}\n".format(len(photon_data))
        textstring += "<dE/dx>: {:.3}".format(np.mean(photon_data))
    # This path does landaus that are connected
    if True:
        popt, pcov = opt.curve_fit(
            photon_gauss_landau,
            bin_centers,
            data,
            bounds=([1.5, 0.1, 0.05, 0, 0.1, 0],
                    [2.1, 0.5, 0.3, 20, 0.5, 20]))
        y_fit = photon_gauss_landau(x_fit, popt[0], popt[1], popt[2], popt[3],
                                    popt[4], popt[5])
        textstring = "Landau MPV: {:.3}   {:.3}\n".format(popt[0], 2*popt[0])
        textstring += "Landau $\sigma$: {:.3}   {:.3}\n".format(popt[1], popt[4])
        textstring += "Gauss $\sigma$: {:.3}   {:.3}\n".format(popt[2], math.sqrt(2)*popt[2])
        textstring += "gamma/e:  {:.3}\n".format(popt[5]/popt[3])
        textstring += "N: {:}\n".format(len(hits))
        textstring += "<dE/dx>: {:.3}".format(np.mean(hits))

    f, ax = plt.subplots(figsize=(10, 7))


    ax.errorbar(bin_centers, data, yerr=err, xerr=binwidth*0.5,
                label="Collection Hits", capsize=0,ls="none",marker="o",color='r')
    # ax.errorbar(bin_centers, i_data, yerr=i_err, xerr=binwidth*0.5, label="Induction Hits",capsize=0)
    ax.set_title("Electron dE/dx Hits")

    plt.plot(x_fit, y_fit, label="Fitted landau",color='g')
    y_lim = ax.get_ylim()
    # plt.ylim([0, 0.6])

    props = dict(boxstyle='round', facecolor='wheat', alpha=1.0)
    ax.text(5, 0.45*y_lim[1], textstring, bbox=props, fontsize=20)

    # Draw the simulation if it's available:
    if photon_sim is not None:

        sim, bin_edges = np.histogram(photon_sim, bins,density=True)

        # for i in xrange(len(sim)):
        #     print "[{:.2} - {:.2}]: {:.3}".format(
        #         bin_edges[i], bin_edges[i+1], sim[i])
        ax.plot(bin_edges[1:], sim, color="r",
             ls="steps", label="Simulated Photons")



    plt.xlabel("dE/dx [MeV/cm]")
    plt.ylabel("")
    plt.legend()
    plt.grid(True)

    plt.show()



(e_data, e_sim), (p_data, p_sim) = showerCalo.full_samples()
# (e_data, e_sim), (p_data, p_sim) = showerCalo.lite_samples()

e_data.getShowerCaloVector().set_drop_first_hit(True)

hits = e_data.getShowerCaloVector().all_dedx_hits_box(1)
sim_hits = e_sim.getShowerCaloVector().all_dedx_hits_box(1)
binwidth = 0.2

electronFit(hits,binwidth,sim_hits)


hits = p_data.getShowerCaloVector().all_dedx_hits_box(1)
sim_hits = p_sim.getShowerCaloVector().all_dedx_hits_box(1)
binwidth = 0.2
photonFit(hits, binwidth, sim_hits)





# if "Electron" in title:
#     plt.savefig(
#         "/data_linux/dedx_plots/Landau/landau_electrons_with_cuts_"+name+".png")
# if "Photon" in title:
#     plt.savefig(
#         "/data_linux/dedx_plots/Landau/landau_photons_with_cuts_"+name+".png")

# plt.close(f)

