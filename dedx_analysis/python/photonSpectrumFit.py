import landau
import numpy
import math

import ROOT

from matplotlib.ticker import NullFormatter
from matplotlib import pyplot as plt
from matplotlib.colors import LogNorm, Normalize

from scipy import optimize

import showerCalo

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
nc_max = numpy.max(ncpi0_y)

ncpi0_y /= nc_max


def optimizer_gauss(params, p_sim, p_data):



  adjustWeightsToNCPi0_Gauss(params[0], params[1], params[2], p_sim)
  return qualityOfFit(p_sim, p_data)

def optimizer_exp(params,p_sim,p_data):


  adjustWeightsToNCPi0_exp(params[0], params[1], p_sim)
  return qualityOfFit(p_sim, p_data)


def qualityOfFit(p_sim, p_data):

    photon_data = p_data.getBestMedianVector()
    photon_sim = p_sim.getBestMedianVector()
    sim_weights = p_sim.getShowerCaloVector().weights()

    data_bin_width = 0.4
    sim_bin_width = 0.4

    data_bins = numpy.arange(0.1, 8.0, data_bin_width)
    sim_bins = numpy.arange(0.1, 8.0, sim_bin_width)

    photon_data_hist, bin_edges_data = numpy.histogram(
        photon_data, data_bins, density=True)

    photon_sim_hist, bin_edges_sim = numpy.histogram(
        photon_sim, sim_bins, density=True, weights=sim_weights)

    chisq = 0.0
    for sim, data in zip(photon_data_hist, photon_sim_hist):
      if data != 0:
        chisq += (sim - data)**2 / data

    return chisq


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
    energy_spectrum, bins = numpy.histogram(
        p_sim.getShowerCaloVector().mc_true_energy(),
        bins=bin_edges,
        density=True)

    # print 0.5*(bins[0] + bins[1])
    # print 0.5*(bins[1] + bins[2])
    # print 0.5*(bins[2] + bins[3])

    # print "------------"

    # exit(0)

    # scale this spectrum to peak at 1:
    sim_max = numpy.max(energy_spectrum)
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


def adjustWeightsToNCPi0_exp(lam, scale, p_sim):

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
    energy_spectrum, bins = numpy.histogram(
        p_sim.getShowerCaloVector().mc_true_energy(),
        bins=bin_edges,
        density=True)

    # print 0.5*(bins[0] + bins[1])
    # print 0.5*(bins[1] + bins[2])
    # print 0.5*(bins[2] + bins[3])

    # print "------------"

    # exit(0)

    # scale this spectrum to peak at 1:
    sim_max = numpy.max(energy_spectrum)
    energy_spectrum /= sim_max

    # Now we can make a 1-1 ratio of the sim to nc spectrum:

    # for i in xrange(len(energy_spectrum)):
    #     print "{:.2} vs {:.2}".format(ncpi0_y[i], energy_spectrum[i])

    # Set the weights as the ratio of ncpi0_y[i] to energy_spectrum[i]

    weights = ROOT.vector('double')()

    for i in xrange(len(energy_spectrum)):
        weights.push_back(1.0)
        extra = math.exp(-(ncpi0_x[i]/lam) )
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



def makePlot(p_sim, p_data, bins):

    photon_data = p_data.getBestMedianVector()
    photon_sim = p_sim.getBestMedianVector()
    sim_weights = p_sim.getShowerCaloVector().weights()

    print sim_weights[0]
    print sim_weights[10]
    print sim_weights[40]

    # Plot the dE/dx plot and the energy spectrum of the photon spectrum side
    # by side

    data_bin_width = 0.4
    sim_bin_width = 0.4

    data_bins = numpy.arange(0.1, 8.0, data_bin_width)
    sim_bins = numpy.arange(0.1, 8.0, sim_bin_width)

    photon_data_hist, bin_edges_data = numpy.histogram(
        photon_data, data_bins)

    photon_err = []
    photon_frac_err = []
    for p_val in photon_data_hist:
        photon_err.append(math.sqrt(p_val))
        if p_val != 0:
            photon_frac_err.append(math.sqrt(p_val) / p_val)
        else:
            photon_frac_err.append(0.0)

    photon_data_hist, bin_edges_data = numpy.histogram(
        photon_data, data_bins, density=True)

    photon_sim_hist, bin_edges_sim = numpy.histogram(
        photon_sim, sim_bins, density=True, weights=sim_weights)

    bin_centers_data = bin_edges_data[:-1] + 0.5*data_bin_width
    bin_centers_sim = bin_edges_sim[:-1] + 0.5*sim_bin_width

    photon_err_norm = []
    for p_val, frac_err in zip(photon_data_hist, photon_frac_err):
        photon_err_norm.append(p_val*frac_err)

    f2, axarr = plt.subplots(2, figsize=(15, 16))
    axarr[0].set_title("dE/dx of Photon Sample", fontsize=30)
    axarr[0].errorbar(bin_centers_data,
                      photon_data_hist,
                      xerr=data_bin_width*0.5,
                      yerr=photon_err_norm,
                      marker="o",
                      # fillstyle="none",
                      label="Photons, Data",
                      capsize=2,
                      color='r',
                      ls="none")
    axarr[0].set_xlabel("dE/dx [MeV/cm]", fontsize=20)
    axarr[0].set_ylabel("Unit Normalized", fontsize=20)

    axarr[0].plot(bin_centers_sim+0.5*sim_bin_width,
                  photon_sim_hist,
                  color="r",
                  ls="steps",
                  label="Simulated Photons")

    axarr[0].grid(True)
    axarr[0].legend(fontsize=20)

    # Draw a histogram with all of these:
    chisq = qualityOfFit(p_sim, p_data)
    # print "Chisq is {}".format(chisq)

    # Add the chisq to the plot:
    y_lims = axarr[0].get_ylim()
    axarr[0].text(6.0, 0.55*y_lims[1], "$\chi^2$: {:.3}".format(chisq),fontsize=20)

    # Now draw the photon simulated energy spectrum:
    energy_spectrum, bin_edges = numpy.histogram(
        p_sim.getShowerCaloVector().mc_true_energy(),
        bins=bins,
        weights=sim_weights,
        density=True)

    energy_bin_centers = 0.5*(bin_edges[:-1] + bin_edges[1:])

    # Scale the energy spectra so that both peak at 1:
    sim_max = numpy.max(energy_spectrum)

    energy_spectrum /= sim_max

    axarr[1].plot(energy_bin_centers,
                  energy_spectrum,
                  label="Photon Energy Spectrum",
                  c='r')
    axarr[1].plot(ncpi0_x, ncpi0_y, label="NC Pi0 Spectrum")
    axarr[1].set_xlabel("Photon Energy [MeV]", fontsize=20)
    axarr[1].set_ylabel("Arbitrary", fontsize=20)
    axarr[1].legend(fontsize=20)
    axarr[1].grid(True)

    plt.show()


def makePlotLandau(p_sim, p_data,e_bins):

    hits = p_data.getShowerCaloVector().all_dedx_hits_box(1)
    sim_hits = p_sim.getShowerCaloVector().all_dedx_hits_box(1)

    binwidth = 0.25
    bins = numpy.arange(0, 8.0, binwidth)
    data, bin_edges = numpy.histogram(hits, bins)

    err = []
    for point in data:
        try:
            err.append(1.0/math.sqrt(point))
        except:
            err.append(0.0)

    data, bin_edges = numpy.histogram(hits, bins, density=True)

    err *= data

    bin_centers = bin_edges[:-1] + 0.5*binwidth

    f, axarr = plt.subplots(2, figsize=(10, 14))

    axarr[0].errorbar(bin_centers, data, yerr=err, xerr=binwidth*0.5,
                label="Collection Hits", capsize=0, ls="none", marker="o", color='r')
    axarr[0].set_title("Photon dE/dx Hits")

    y_lim = axarr[0].get_ylim()
    # plt.ylim([0, 0.6])

    # Constuct the arrays of hits and weights from the simulation:
    
    sim_hits = []
    sim_weights_hits = []

    for shower in p_sim.getShowerCaloVector():
      these_hits = shower.good_dedx_hits(0)
      for hit in these_hits:
        sim_hits.append(hit)
        sim_weights_hits.append(shower._weight)

    print len(sim_hits)
    print len(sim_weights_hits)

    sim, bin_edges = numpy.histogram(sim_hits, bins,
                                  density=True,
                                  weights=sim_weights_hits)

    bin_centers = 0.5*(bin_edges[:-1] + bin_edges[1:])

    axarr[0].plot(bin_centers+0.5*binwidth, sim, color="r",
            ls="steps", label="Simulated Photons")

    # Compute the chisq between sim and data
    chisq = 0.0
    for sim_bin, data_bin in zip(sim, data):
      if data_bin != 0.0:
        chisq += (sim_bin - data_bin)**2 / data_bin


    y_lims = axarr[0].get_ylim()

    axarr[0].text(6.0, 0.55*y_lims[1], "$\chi^2$: {:.3}".format(chisq),fontsize=20)


    axarr[0].set_xlabel("dE/dx [MeV/cm]")
    axarr[0].set_ylabel("")
    axarr[0].legend()
    axarr[0].grid(True)

    # Draw a histogram with all of these:

    print "Chisq is {}".format(chisq)

    # Now draw the photon simulated energy spectrum:
    # e_bins = numpy.arange(0,3000,100)
    energy_spectrum, bin_edges = numpy.histogram(
        p_sim.getShowerCaloVector().mc_true_energy(),
        bins=e_bins,
        weights=p_sim.getShowerCaloVector().weights(),
        density=True)

    energy_bin_centers = 0.5*(bin_edges[:-1] + bin_edges[1:])

    # Scale the energy spectra so that both peak at 1:
    sim_max = numpy.max(energy_spectrum)

    energy_spectrum /= sim_max

    axarr[1].plot(energy_bin_centers,
                  energy_spectrum,
                  label="Photon Energy Spectrum",
                  c='r')
    axarr[1].plot(ncpi0_x, ncpi0_y, label="NC Pi0 Spectrum")
    axarr[1].set_xlabel("Photon Energy [MeV]", fontsize=20)
    axarr[1].set_ylabel("Arbitrary", fontsize=20)
    axarr[1].legend(fontsize=20)
    axarr[1].grid(True)

    plt.show()


def main():

    (e_data, e_sim), (p_data, p_sim) = showerCalo.full_samples()
    # (e_data, e_sim), (p_data, p_sim) = showerCalo.lite_samples()
    # (e_data, e_sim), (p_data, p_sim) = showerCalo.full_simch_samples()
    # (e_data, e_sim), (p_data, p_sim) = showerCalo.lite_simch_samples()

    weights = ROOT.vector('double')()
    bins = ROOT.vector('double')()

    e_min = 0.0
    e_max = 3000.0
    e_step = 50.0

    this_bin = e_min

    while this_bin < e_max:
        bins.push_back(this_bin)
        weights.push_back(1.0)
        this_bin += e_step
    # bins = ROOT.vector

    bins.push_back(e_max)

    # for i in xrange(weights.size()):
    #     print "{} to {}: {}".format(bins[i], bins[i+1], weights[i])

    p_sim.getShowerCaloVector().set_weights(weights, bins)


    # Assume the photon spectrum is the NC pi0 spectrum plus some gaussian
    # (because why not?)
    
    params_g=(300,500,1.0)
    bounds_g = [(500,1500),(200,1000),(5,10.0)]

    params_e = (100,500)
    bounds_e = ((1000,10000),(100,10000.0))
    res = optimize.minimize(optimizer_gauss,params_g,args=(p_sim,p_data))
    # res = optimize.minimize(optimizer_exp,params_e,args=(p_sim,p_data))

    print res

    makePlot(p_sim, p_data, bins)
    makePlotLandau(p_sim, p_data,bins)

    adjustWeightsToNCPi0_Gauss(res.x[0],res.x[1],res.x[2],p_sim)
    # adjustWeightsToNCPi0_exp(500,1.0,p_sim)

    makePlot(p_sim, p_data, bins)
    makePlotLandau(p_sim, p_data,bins)


if __name__ == '__main__':
    main()


# 26.31579       0.08068256
# 75.18797       0.1536175
# 131.57895      0.13664484
# 184.21053      0.10197051
# 229.3233       0.074624486
# 278.1955       0.05568814
# 334.58646      0.046718784
# 383.45865      0.044930615
# 424.81204      0.03486365
# 484.9624       0.02705225
# 526.3158       0.022256538
# 582.7068       0.016608829
# 635.3383       0.015973123
# 684.2105       0.015973123
# 729.3233       0.012887524
# 785.7143       0.011689519
# 834.5865       0.010397985
# 883.4586       0.009806757
# 921.0526       0.008389359
# 977.4436       0.007759442
# 1033.8346      0.007759442
# 1086.4662      0.0060208943
# 1142.8572      0.005678548
# 1187.97        0.005790444
# 1236.8422      0.0048578116
# 1270.6766      0.004763938
# 1330.827       0.0044062366
# 1379.6992      0.0041556987
# 1428.5714      0.003996639
# 1473.6842      0.004075393
# 1533.8346      0.0032245906
# 1571.4286      0.0033529243
# 1624.0602      0.0023598336
# 1676.6918      0.0015664453
# 1733.0828      0.0033529243
# 1774.436       0.0023598336
# 1830.827       0.002225654
# 1875.9398      0.001941492
# 1932.3308      0.0023598336
# 1973.6842      0.0022695106
# 2030.0752      0.0015064895
# 2075.188       0.0016608827
# 2127.8196      0.0015361749
# 2180.4512      0.0013933743
# 2225.564       0.0010811808
# 2281.9548      0.0011689519
# 2334.5864      6.902129E-4
# 2368.4211      0.0010197051
# 2421.0527      9.806757E-4
# 2473.6843      0.0014488284
# 2530.0752      7.759442E-4
# 2571.4285      0.0012887524
# 2631.5789      0.0012394253
# 2676.6917      5.904545E-4
# 2721.8044      0.0012154742
# 2778.1956      3.843667E-4
# 2823.3083      0.0011463626
# 2875.94        6.902129E-4
# 2924.812       4.9535354E-4
# 2973.6843      0.028683169
