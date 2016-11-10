import ROOT
# import larlite
import landau
import numpy as np
import scipy.optimize as opt
import math

import showerCalo

from matplotlib import pyplot as plt


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

    bins = np.arange(binwidth, 8+binwidth, binwidth)
    data, bin_edges = np.histogram(electron_data, bins)


    err = []
    for point in data:
        try:
            err.append(1.0/math.sqrt(point))
        except:
            err.append(0.0)

    data, bin_edges = np.histogram(electron_data, bins, density=True)

    err *= data

    bin_centers = 0.5*(bin_edges[1:] + bin_edges[:-1])



    x_fit = np.arange(0, 8, 0.25*binwidth)
    x_fit += 0.5*0.25*binwidth

    popt, pcov = opt.curve_fit(
        landau.gauss_landau, bin_centers, data, bounds=([0,0,0,0], [10, 10, 10, 10]))
    y_fit = landau.gauss_landau(x_fit, popt[0], popt[1], popt[2],popt[3])
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

        bins = np.arange(binwidth, 8.0+binwidth, binwidth)

        sim, bin_edges = np.histogram(electron_sim, bins,density=True)
        bin_centers = 0.5*(bin_edges[:-1] + bin_edges[1:])

        # for i in xrange(len(sim)):
        #     print "[{:.2} - {:.2}]: {:.3}".format(
        #         bin_edges[i], bin_edges[i+1], sim[i])
        ax.plot(bin_centers+0.5*binwidth, sim, color="b",
             ls="steps", label="Simulated Electrons")
        # ax.bar(bin_centers, sim,align="center",width=binwidth,color=(1.0,1.0,1.0))
        
        # ax.plot(bin_centers, sim, color="b",
             # marker="x",ls="")


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
    ax.set_title("Photon dE/dx Hits")

    plt.plot(x_fit, y_fit, label="Fitted landau",color='g')
    y_lim = ax.get_ylim()
    # plt.ylim([0, 0.6])

    # props = dict(boxstyle='round', facecolor='wheat', alpha=1.0)
    # ax.text(5, 0.45*y_lim[1], textstring, bbox=props, fontsize=20)

    # Draw the simulation if it's available:
    if photon_sim is not None:

        sim, bin_edges = np.histogram(photon_sim, bins,density=True)

        bin_centers = 0.5*(bin_edges[:-1] + bin_edges[1:])

        ax.plot(bin_centers+0.5*binwidth, sim, color="r",
             ls="steps", label="Simulated Gammas")



    plt.xlabel("dE/dx [MeV/cm]")
    plt.ylabel("Area Normalized")
    plt.legend()
    plt.grid(True)

    plt.show()

def photonDataMC(photon_data,photon_sim):

    bins = np.arange(0, 8.01+binwidth, binwidth)
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


    # Make a fit to the hits for photons
    
  
    f, ax = plt.subplots(figsize=(15, 7))



    # y_lim = ax.get_ylim()
    # plt.ylim([0, 0.6])

    # props = dict(boxstyle='round', facecolor='wheat', alpha=1.0)
    # ax.text(5, 0.45*y_lim[1], textstring, bbox=props, fontsize=20)

    # Draw the simulation if it's available:
    if photon_sim is not None:

        sim, bin_edges = np.histogram(photon_sim, bins,density=True)
        sim[0] = 0

        bin_centers = 0.5*(bin_edges[:-1] + bin_edges[1:])

        ax.plot(bin_centers, sim, color="r",
             ls="steps-mid", label="Simulated Gamma dE/dx Hits",
             linewidth=3)

    ax.errorbar(bin_centers, data, yerr=err, xerr=binwidth*0.5,
                label="Data Gamma dE/dx Hits", capsize=0,ls="none",marker="o",color='r')
    # ax.errorbar(bin_centers, i_data, yerr=i_err, xerr=binwidth*0.5, label="Induction Hits",capsize=0)
    ax.set_title("Gamma dE/dx Hits",fontsize=30)

    for tick in ax.xaxis.get_major_ticks():
        tick.label.set_fontsize(25)
    for tick in ax.yaxis.get_major_ticks():
        tick.label.set_fontsize(0)

    plt.xlabel("dE/dx [MeV/cm]",fontsize=30)
    plt.ylabel("Area Normalized",fontsize=30)
    plt.legend(fontsize=20)
    plt.grid(True)
    plt.xlim([0,8.0])

    plt.show()



# (e_data, e_sim), (p_data, p_sim) = showerCalo.full_samples()
(e_data, e_sim), (p_data, p_sim) = showerCalo.lite_samples()

# e_data.getShowerCaloVector().set_drop_first_hit(True)
# adjustWeightsToNCPi0_Gauss(298.6,743,9870,p_sim)
adjustWeightsToNCPi0_Gauss(482.6,635,5793.0,p_sim)


hits = e_data.getShowerCaloVector().best_dedx_hits_box()
sim_hits = e_sim.getShowerCaloVector().all_dedx_hits_box(1)
binwidth = 0.3

# electronFit(hits,binwidth,sim_hits)


hits = p_data.getShowerCaloVector().all_dedx_hits_box(1)
sim_hits = p_sim.getShowerCaloVector().all_dedx_hits_box(1)
binwidth = 0.25
# photonFit(hits, binwidth, sim_hits)
photonDataMC(hits, sim_hits)




# if "Electron" in title:
#     plt.savefig(
#         "/data_linux/dedx_plots/Landau/landau_electrons_with_cuts_"+name+".png")
# if "Photon" in title:
#     plt.savefig(
#         "/data_linux/dedx_plots/Landau/landau_photons_with_cuts_"+name+".png")

# plt.close(f)

