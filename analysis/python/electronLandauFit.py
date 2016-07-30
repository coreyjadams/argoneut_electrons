import ROOT
import larlite

import showerCalo
from matplotlib import pyplot as plt

import numpy as np
import math
from scipy import optimize
import landau



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


def chisq(target_dist, fitted_dist):
    cs = 0.0
    for t, f in zip(target_dist, fitted_dist):
        if t > 0.01:
            cs += (t-f)**2 / t
    return cs


def bestFit(alpha, e_data, e_sim, p_sim):
    joint_dist = (alpha[0])*e_sim + alpha[1] * p_sim
    return chisq(e_data, joint_dist)


def drawMCLandaus(e_sim, p_sim, binwidth):

    bins = np.arange(binwidth, 8+binwidth, binwidth)

    e_mc_hits = e_sim.getShowerCaloVector().all_dedx_hits_box(1)
    p_mc_hits = p_sim.getShowerCaloVector().all_dedx_hits_box(1)

    e_mc_dist, bin_edges = np.histogram(e_mc_hits, bins, density=True)
    p_mc_dist, bin_edges = np.histogram(p_mc_hits, bins, density=True)

    bin_centers = 0.5*(bin_edges[1:] + bin_edges[:-1])

    f, ax = plt.subplots(figsize=(10, 7))

    ax.plot(bin_centers, e_mc_dist, color="b",
            ls="steps-mid", label="Simulated Electron Hits",
            linewidth=3)
    ax.plot(bin_centers, p_mc_dist, color="r",
            ls="steps-mid", label="Simulated Photon Hits",
            linewidth=3)

    ax.set_title("True dE/dx Hits", fontsize=25)

    y_lim = ax.get_ylim()
    # plt.ylim([0, 1.3])

    # plt.text(7.0, 0.75,"Distance = {} cm".format(distance),fontsize=20)

    # for i in xrange(len(sim)):
    #     print "[{:.2} - {:.2}]: {:.3}".format(
    #         bin_edges[i], bin_edges[i+1], sim[i])

    # ax.bar(bin_centers, sim,align="center",width=binwidth,color=(1.0,1.0,1.0))

    # ax.plot(bin_centers, sim, color="b",
    # marker="x",ls="")

    plt.xlabel("dE/dx [MeV/cm]", fontsize=20)
    plt.ylabel("Normalized", fontsize=20)
    plt.legend()
    for tick in ax.xaxis.get_major_ticks():
        tick.label.set_fontsize(16)
    for tick in ax.yaxis.get_major_ticks():
        tick.label.set_fontsize(0)
    plt.grid(True)

    # Save the plot:
    # plt.savefig("/home/cadams/Dropbox/Talks/dEdx/EGammaSep/dedx_6_plots/landau/true_landau_{}cm.png".format(distance))
    # plt.close()
    plt.show()


def drawLandaus(e_data, e_sim, p_sim, binwidth):

    bins = np.arange(binwidth, 8+binwidth, binwidth)

    e_data_hits = e_data.getShowerCaloVector().best_dedx_hits_box()
    e_mc_hits = e_sim.getShowerCaloVector().all_dedx_hits_box(1)
    p_mc_hits = p_sim.getShowerCaloVector().all_dedx_hits_box(1)

    e_data_dist, bin_edges = np.histogram(e_data_hits, bins)
    e_mc_dist, bin_edges = np.histogram(e_mc_hits, bins, density=True)
    p_mc_dist, bin_edges = np.histogram(p_mc_hits, bins, density=True)

    err = []
    for point in e_data_dist:
        try:
            err.append(1.0/math.sqrt(point))
        except:
            err.append(0.0)

    e_data_dist, bin_edges = np.histogram(e_data_hits, bins, density=True)

    err *= e_data_dist

    bin_centers = 0.5*(bin_edges[1:] + bin_edges[:-1])

    # Combined distribution:
    res = optimize.minimize(
        bestFit, x0=(1.0, 0.2), args=(e_data_dist, e_mc_dist, p_mc_dist))

    print res

    alpha = res.x
    combo_dist = alpha[0]*e_mc_dist + alpha[1]*p_mc_dist

    # Calculate the chi-sq for the electron only simulation as well as
    # the joint electron + photon fit:

    chisq_e_only = chisq(e_data_dist, e_mc_dist)
    chisq_joint = chisq(e_data_dist, combo_dist)

    print "Chisq went from {} to {}".format(chisq_e_only, chisq_joint)

    # Figure out the contamination from photons.
    integral = 0.0;
    for e, p in zip(e_mc_dist, p_mc_dist):
        integral += alpha[0]*e + alpha[1]*p

    print integral*binwidth
    print np.sum(alpha)

    print "Photon contamination is {:.3}%".format(alpha[1]*100/(integral*binwidth))

    f, ax = plt.subplots(figsize=(10, 7))

    ax.bar(bin_centers-0.5*binwidth, alpha[0]*e_mc_dist,
           width=binwidth, color="b",
           label="Simulated Electron Hits",
           alpha=0.7)
    ax.bar(bin_centers-0.5*binwidth, alpha[1]*p_mc_dist,
           width=binwidth, color="red",
           bottom=alpha[0]*e_mc_dist,
           label="Simulated Photon Hits",
           alpha=0.7)
    # ax.plot(bin_centers, alpha[1]*p_mc_dist, color="r",
    # ls="steps-mid", label="Simulated Photon Hits")
    # ax.plot(bin_centers, combo_dist, color="g",
    # ls="steps-mid", label="Simulated Electrons + Photons",linewidth = 2)
    ax.errorbar(bin_centers, e_data_dist, yerr=err, xerr=binwidth*0.5,
                label="Data Hits", capsize=0,
                ls="none", marker="o", color='black')

    # plt.text(6.0, 0.65, "$\chi^2$: {:.2}".format(
    #     chisq_e_only), fontsize=20, color='b')
    # plt.text(6.0, 0.55, "$\chi^2$: {:.2}".format(
    #     chisq_joint), fontsize=20, color='g')

    ax.set_title("True dE/dx Hits",fontsize=25)
    for tick in ax.xaxis.get_major_ticks():
        tick.label.set_fontsize(16)
    for tick in ax.yaxis.get_major_ticks():
        tick.label.set_fontsize(0)
    
    # y_lim = ax.get_ylim()
    # plt.ylim([0, 1.3])

    # plt.text(7.0, 0.75,"Distance = {} cm".format(distance),fontsize=20)

    # for i in xrange(len(sim)):
    #     print "[{:.2} - {:.2}]: {:.3}".format(
    #         bin_edges[i], bin_edges[i+1], sim[i])

    # ax.bar(bin_centers, sim,align="center",width=binwidth,color=(1.0,1.0,1.0))

    # ax.plot(bin_centers, sim, color="b",
    # marker="x",ls="")

    plt.xlabel("dE/dx [MeV/cm]",fontsize=20)
    plt.ylabel("Normalized",fontsize=20)
    plt.legend()
    plt.grid(True)

    # Save the plot:
    # plt.savefig("/home/cadams/Dropbox/Talks/dEdx/EGammaSep/dedx_6_plots/landau/true_landau_{}cm.png".format(distance))
    # plt.close()
    plt.show()

    # Now, subtract the photon contamination from the electron data sample and fit a landau to it.
    e_data_corrected = e_data_dist - p_mc_dist*alpha[1]/(integral*binwidth)

    for i in xrange(len(e_data_corrected)):
        if e_data_corrected[i] < 0:
            e_data_corrected[i] = 0

    e_data_corrected /= np.sum(e_data_corrected)*binwidth

    print e_data_corrected


    x_fit = np.arange(0, 8, 0.25*binwidth)
    x_fit += 0.5*0.25*binwidth

    popt, pcov = optimize.curve_fit(
        landau.gauss_landau, bin_centers, e_data_corrected, bounds=([0,0,0,0], [10, 10, 10, 10]))
    y_fit = landau.gauss_landau(x_fit, popt[0], popt[1], popt[2],popt[3])
    textstring = "Landau MPV: {:.3}\n".format(popt[0])
    textstring += "Landau $\sigma$: {:.3}\n".format(popt[1])
    textstring += "Gauss $\sigma$: {:.3}".format(popt[2])


    f, ax = plt.subplots(figsize=(10, 7))
    ax.errorbar(bin_centers, e_data_corrected, yerr=err, xerr=binwidth*0.5,
                label="Corrected Electron Hits", capsize=0,
                ls="none", marker="o", color='black')

    # ax.errorbar(x, i_data, yerr=i_err, xerr=binwidth*0.5, label="Induction Hits",capsize=0)
    ax.set_title("Corrected Electron dE/dx Hits",fontsize=25)

    plt.plot(x_fit, y_fit, label="Fitted landau",color='g')
    y_lim = ax.get_ylim()
    # plt.ylim([0, 0.6])



    props = dict(boxstyle='round', facecolor='wheat', alpha=1.0)
    ax.text(5, 0.45*y_lim[1], textstring, bbox=props, fontsize=20)

    plt.xlabel("dE/dx [MeV/cm]",fontsize=20)
    plt.ylabel("Normalized",fontsize=20)
    plt.legend()
    plt.grid(True)
    # ax.set_title("True dE/dx Hits",fontsize=25)
    for tick in ax.xaxis.get_major_ticks():
        tick.label.set_fontsize(16)
    for tick in ax.yaxis.get_major_ticks():
        tick.label.set_fontsize(0)
    
    plt.show()


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

    (e_data, e_sim), (p_data, p_sim) = showerCalo.full_samples()
    # (e_data, e_sim), (p_data, p_sim) = showerCalo.lite_samples()

    adjustWeightsToNCPi0_Gauss(482.6, 635, 5793.0, p_sim)

    binwidth = 0.25

    drawMCLandaus(e_sim, p_sim, binwidth)
    drawLandaus(e_data, e_sim, p_sim, binwidth)


if __name__ == '__main__':
    main()
