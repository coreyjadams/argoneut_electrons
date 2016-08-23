import ROOT
from ROOT import larutil
# import larlite

import showerCalo

import pickle

import math
import numpy as np
from scipy import optimize

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



def plotDistance(p_sim,p_data, binwidth):

    bins = np.arange(0.0, 50+binwidth, binwidth)
    energy_bins = np.arange(0, 2000, 50)

    showers = p_sim.getShowerCaloVector()
    geoHelper = larutil.GeometryHelper.GetME()

    distances = []
    energies = []
    weights = []
    n_1cm = 0
    for shower in showers:
        dist = (shower._true_interaction_point - shower._true_start_point).Mag()
        en = shower.mc_true_energy()
        if math.isnan(dist):
            print "This is NaN!"
        distances.append(dist)
        energies.append(en)
        weights.append(shower._weight)
        if dist < 1.0:
            n_1cm += 1

    pk = open('/data_linux/argoneut/dedx_files/photon_vertexes.pkl')
    _2d_starts = pickle.load(pk)

    data_distances = []

    geoHelper = larutil.GeometryHelper.GetME()

    for shower in p_data.getShowerCaloVector():
        # print "Shower run, event: {}, {}".format(shower._run, shower._event)
        if shower._run in _2d_starts:
            if shower._event in _2d_starts[shower._run]:
                if len(_2d_starts[shower._run][shower._event]) == 2:
                    point0 = larutil.PxPoint()
                    point1 = larutil.PxPoint()
                    point0.w = _2d_starts[shower._run][shower._event][0][0]*geoHelper.WireToCm()
                    point0.t = _2d_starts[shower._run][shower._event][0][1]*geoHelper.TimeToCm()
                    point0.plane = 0
                    point1.w = _2d_starts[shower._run][shower._event][1][0]*geoHelper.WireToCm()
                    point1.t = _2d_starts[shower._run][shower._event][1][1]*geoHelper.TimeToCm()
                    point1.plane = 1
                    xyz = np.ndarray(3)
                    # print "This shower has a match!"
                    geoHelper.GetXYZ(point0,point1,xyz)
                    _3d_point = ROOT.TVector3()
                    _3d_point.SetX(xyz[0])
                    _3d_point.SetY(xyz[1])
                    _3d_point.SetZ(xyz[2])
                    distance = (shower._3D_start_point - _3d_point).Mag()
                    data_distances.append(distance)

    # print data_distances
    # print bins

    # p_conv_dist, bin_edges = np.histogram(distances, bins, density=True)

    # p_conv_dist_data, bin_edges = np.histogram(data_distances,bins,density=True)


    # bin_centers = 0.5*(bin_edges[1:] + bin_edges[:-1])

    print "{} of {} photons converted less than 1 centimeter.".format(n_1cm, len(distances))

    f, ax = plt.subplots(figsize=(10,5))

    dist_hist, bin_edges = np.histogram(distances, bins=bins)

    dist_errs = np.sqrt(dist_hist)

    bin_centers = (bin_edges[:1] + bin_edges[1:] - 0.5*binwidth)


    print bin_edges
    print bin_centers

    plt.bar(bin_centers,dist_errs,width=binwidth,
            bottom=dist_hist-dist_errs,
            alpha=0.5,
            align='center')
    # ax.plot(bin_centers, dist_hist,
    #         color="b",
    #         # fill=True,
    #         ls="steps-mid",
    #         label="Simulated Photons")

    # ax.plot(bin_centers, p_conv_dist_data, color="black",
    #         ls="none",marker="o",label="Reconstructed Photons")


    ax.set_title("Conversion Distance of Gammas",fontsize=35)


    plt.xlabel("Conversion Distance [cm]",fontsize=30)
    plt.ylabel("Area Normalized",fontsize=30)
    plt.legend(fontsize=30)
    plt.grid(True)
    for tick in ax.xaxis.get_major_ticks():
        tick.label.set_fontsize(25) 
    for tick in ax.yaxis.get_major_ticks():
        tick.label.set_fontsize(0) 
    # Save the plot:
    # plt.savefig("/home/cadams/Dropbox/Talks/dEdx/EGammaSep/dedx_6_plots/landau/true_landau_{}cm.png".format(distance))
    # plt.close()
    # plt.grid(True)
    plt.show()

    f, ax = plt.subplots(figsize=(10,10))

    print len(distances)
    print len(energies)
    ax.hist2d(distances, energies, bins=[bins,energy_bins])
    plt.title(r"Energy Dependence of $\gamma$ Conversion Distance", fontsize=35)
    plt.xlabel("Conversion Distance [cm]",fontsize=30)
    plt.ylabel("Photon Energy [MeV]",fontsize=30)

    for tick in ax.xaxis.get_major_ticks():
        tick.label.set_fontsize(25) 
    for tick in ax.yaxis.get_major_ticks():
        tick.label.set_fontsize(25) 
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



def main():
    
    (e_data, e_sim), (p_data, p_sim) = showerCalo.full_samples()
    # (e_data, e_sim), (p_data, p_sim) = showerCalo.lite_samples()

    adjustWeightsToNCPi0_Gauss(482.6,635,5793.0,p_sim)


    binwidth = 1.25

    plotDistance(p_sim, p_data, binwidth)


if __name__ == '__main__':
    main()
