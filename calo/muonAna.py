import ROOT
import larlite
from ROOT import larlite, larutil
import sys
import numpy

import landau
import scipy.optimize as opt

from matplotlib import pyplot as plt
from matplotlib import gridspec
from matplotlib.colors import LogNorm, Normalize
import pickle


def modBoxCorrection(dqdx):
    return larutil.LArProperties.GetME().ModBoxCorrection(dqdx)


def runCaloMuons(wire_calibrations, f):

    larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

    # Create ana_processor instance
    my_proc = larlite.ana_processor()

    # Set input root file
    my_proc.add_input_file(f)

    # Specify IO mode
    my_proc.set_io_mode(larlite.storage_manager.kREAD)

    # my_proc.set_output_file("out.root")

    # Specify output root file name
    my_proc.set_ana_output_file("from_test_ana_you_can_remove_me.root")

    # Attach an analysis unit ... here we use a base class which does nothing.
    # Replace with your analysis unit if you wish.
    caloAlg = larlite.ArgoXingMuons2D()

    # Convert the corrections to a vector<vector<double> > in order to pass
    # them:
    _vector_corrections = caloAlg.getWireCorrections()
    _vector_corrections.resize(2)
    _vector_corrections[0].resize(240)
    _vector_corrections[1].resize(240)

    for plane in wire_calibrations:
        for wire in wire_calibrations[plane]:
            # print "({},{}): {:.2f}".format(plane,wire,
            # wire_calibrations[plane][wire])
            _vector_corrections[plane][wire] = 1.025*wire_calibrations[plane][wire]

    caloAlg.setWireCorrections(_vector_corrections)

    my_proc.add_process(caloAlg)

    # Let's run it.
    my_proc.run(0)

    return caloAlg


def makeCorrelationPlots(caloAlg,sim=False):

    e_min = 1.25
    e_max = 2.5
    e_min_2 = 1.25
    e_max_2 = 4.0

    n_steps = 50
    n_steps_2 = 100

    e_bins_2 = numpy.arange(e_min_2, e_max_2, (e_max_2-e_min_2) / n_steps)
    e_bins = numpy.arange(e_min, e_max, (e_max-e_min) / n_steps)

    coll_dedx_mean = caloAlg._collection_muons_average_dedx
    ind_dedx_mean = caloAlg._induction_muons_average_dedx

    coll_dedx_median = caloAlg._collection_muons_median_dedx
    ind_dedx_median = caloAlg._induction_muons_median_dedx

    coll_dedx_mode = caloAlg._collection_muons_mode_dedx
    ind_dedx_mode = caloAlg._induction_muons_mode_dedx


    plt.plot([e_min, e_max], [e_min, e_max],
             color="black", ls="--", linewidth=1)
    plt.hist2d(
        coll_dedx_median, ind_dedx_median, bins=e_bins, norm=LogNorm(), cmap="rainbow")
    plt.colorbar()
    plt.xlabel("Collection dE/dx [MeV/cm]")
    plt.ylabel("Induction dE/dx [MeV/cm]")
    if sim:
        plt.text(2.0,1.4,"SIMULATION",color="r", fontsize=20)
    plt.title("ArgoNeuT Induction vs. Collection Median dE/dx, Crossing Muons")
    plt.grid(True)

    plt.show()

    plt.plot([e_min, e_max], [e_min, e_max],
             color="black", ls="--", linewidth=1)
    plt.hist2d(
        coll_dedx_mode, ind_dedx_mode, bins=e_bins, norm=LogNorm(), cmap="rainbow")
    plt.colorbar()
    plt.xlabel("Collection dE/dx [MeV/cm]")
    plt.ylabel("Induction dE/dx [MeV/cm]")
    if sim:
        plt.text(2.0,1.4,"SIMULATION",color="r", fontsize=20)
    plt.title("ArgoNeuT Induction vs. Collection M.P.V. dE/dx, Crossing Muons")
    plt.grid(True)

    plt.show()


    plt.plot([e_min_2, e_max_2], [e_min_2, e_max_2],
             color="black", ls="--", linewidth=1)
    plt.hist2d(
        coll_dedx_mean, ind_dedx_mean, bins=e_bins_2, norm=LogNorm(), cmap="rainbow")
    plt.colorbar()
    plt.xlabel("Collection dE/dx [MeV/cm]")
    plt.ylabel("Induction dE/dx [MeV/cm]")
    if sim:
        plt.text(1.5, 3.5,"SIMULATION",color="r", fontsize=20)
    plt.title("ArgoNeuT Induction vs. Collection Mean dE/dx, Crossing Muons")
    plt.grid(True)

    # plt.set_aspect("equal")

    plt.tight_layout()

    plt.show()


def muonPlot2D(caloAlg):

    # Making a 2D histogram of the dE/dx landau for each wire

    # Induction Plane:
    landau_bins = numpy.arange(0, 8.1, 0.1)

    # set up a 2D matrix to hold the histogram:
    n_wires = 240
    n_bins = len(landau_bins)

    ind_matrix = numpy.zeros((n_wires, n_bins))
    coll_matrix = numpy.zeros((n_wires, n_bins))

    ind_total = numpy.zeros(n_bins)
    coll_total = numpy.zeros(n_bins)

    for wire in range(n_wires):
        wireCaloInfo = caloAlg.getWireCaloInfo(0, wire)
        # Make a histogram of this wire:
        y_data, bin_edges = numpy.histogram(
            wireCaloInfo.dEdx_area, bins=landau_bins, normed=True)
        # print bin_edges
        # print landau_bins
        for _bin in xrange(len(landau_bins) - 1):
            ind_matrix[wire][_bin] = y_data[_bin]
            ind_total[_bin] += y_data[_bin]
        wireCaloInfo = caloAlg.getWireCaloInfo(1, wire)
        # Make a histogram of this wire:
        y_data, bin_edges = numpy.histogram(
            wireCaloInfo.dEdx_area, bins=landau_bins, normed=True)
        # print bin_edges
        # print landau_bins
        for _bin in xrange(len(landau_bins) - 1):
            coll_matrix[wire][_bin] = y_data[_bin]
            coll_total[_bin] += y_data[_bin]

    ind_matrix = numpy.flipud(numpy.transpose(ind_matrix))
    coll_matrix = numpy.flipud(numpy.transpose(coll_matrix))


    fig = plt.figure()

    ax_ind_mat = plt.subplot2grid((2, 3), (0, 0), colspan=2)
    ax_coll_mat = plt.subplot2grid((2, 3), (1, 0), colspan=2,sharey=ax_ind_mat)


    ax_ind_mat.imshow(ind_matrix)

    ax_coll_mat.imshow(coll_matrix)

    plt.xlabel("Wire Number")
    plt.show()

def makeTotalLandaus(caloAlg):

    binwidth = 0.1
    landau_bins = numpy.arange(0, 8.1, binwidth)
    n_wires = 240
    n_bins = len(landau_bins)

    n_coll = 0.0
    n_ind = 0.0

    coll_sum = 0.0
    ind_sum  = 0.0


    ind_total = numpy.zeros(n_bins)
    coll_total = numpy.zeros(n_bins)

    for wire in range(n_wires):
        wireCaloInfo = caloAlg.getWireCaloInfo(0, wire)
        if wireCaloInfo.dEdx_area.size() > 500:

            for val in wireCaloInfo.dEdx_area:
                if val < 8.0:
                    ind_sum += val
                    n_ind += 1

            # Make a histogram of this wire:
            y_data, bin_edges = numpy.histogram(
                wireCaloInfo.dEdx_area, bins=landau_bins, normed=False)


            for _bin in xrange(len(landau_bins) - 1):
                ind_total[_bin] += y_data[_bin]
        else:
            print "excluding plane 0 wire ", wire

        wireCaloInfo = caloAlg.getWireCaloInfo(1, wire)        
        if wireCaloInfo.dEdx_area.size() > 500:

            for val in wireCaloInfo.dEdx_area:
                if val < 8.0:
                    coll_sum += val
                    n_coll += 1

            # Make a histogram of this wire:
            y_data, bin_edges = numpy.histogram(
                wireCaloInfo.dEdx_area, bins=landau_bins, normed=False)

            for _bin in xrange(len(landau_bins) - 1):
                coll_total[_bin] += y_data[_bin]
        else:
            print "excluding plane 1 wire ", wire

    # Normalize the two samples to have the max amplitude at 1:
    coll_total *= 1.0/numpy.max(coll_total)
    ind_total *= 1.0/numpy.max(ind_total)


    bin_centers = landau_bins + 0.5*binwidth
    x_fit = numpy.arange(0,8, 0.5*binwidth)

    popt, pcov = opt.curve_fit(landau.gauss_landau,bin_centers,coll_total,bounds=(0,[10,10,10,10]) )

    coll_fit = landau.gauss_landau(x_fit,popt[0],popt[1],popt[2], popt[3])




    fig, axes = plt.subplots(figsize=(8,5))
    plt.plot(bin_centers, coll_total,marker="+",ls="",label="Reconstructed Muons")
    plt.plot(x_fit,coll_fit,label="Fitted landau")
    plt.xlabel("dE/dx [MeV/cm]")
    plt.ylabel("Arbitrary")
    plt.title("Collection Plane Muons, Data")
    textstring =  "Landau MPV: {:.3}\n".format(popt[0])
    textstring += "Landau $\sigma$: {:.3}\n".format(popt[1])
    textstring += "Gauss $\sigma$: {:.3}\n".format(popt[2])
    textstring += "N: {:}\n".format(n_coll)
    textstring += "<dE/dx>: {:.3}".format(coll_sum / n_coll)
    props = dict(boxstyle='round', facecolor='wheat', alpha=1.0)
    axes.text(5.0,0.5,textstring,bbox=props,fontsize=20)
    plt.ylim([0,1.25])
    plt.legend()
    plt.grid(True)
    plt.show()


    popt, pcov = opt.curve_fit(landau.gauss_landau,bin_centers,ind_total,bounds=(0,[10,10,10,10]) )
    ind_fit = landau.gauss_landau(x_fit,popt[0],popt[1],popt[2], popt[3])

    fig, axes = plt.subplots(figsize=(8,5))
    plt.plot(bin_centers, ind_total,marker="+",ls="",label="Reconstructed Muons")
    plt.plot(x_fit,ind_fit,label="Fitted landau")
    plt.xlabel("dE/dx [MeV/cm]")
    plt.ylabel("Arbitrary")
    plt.title("Induction Plane Muons, Data")
    textstring =  "Landau MPV: {:.3}\n".format(popt[0])
    textstring += "Landau $\sigma$: {:.3}\n".format(popt[1])
    textstring += "Gauss $\sigma$: {:.3}\n".format(popt[2])
    textstring += "N: {:}\n".format(n_ind)
    textstring += "<dE/dx>: {:.3}".format(ind_sum / n_ind)
    props = dict(boxstyle='round', facecolor='wheat', alpha=1.0)
    axes.text(5.0,0.5,textstring,bbox=props,fontsize=20)
    plt.ylim([0,1.25])
    plt.legend()
    plt.grid(True)
    plt.show()


def main():

    larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

    if len(sys.argv) < 2:
        msg = '\n'
        msg += "Usage 1: %s $INPUT_ROOT_FILE(s)\n" % sys.argv[0]
        msg += '\n'
        sys.stderr.write(msg)
        sys.exit(1)

    f = sys.argv[-1]

    if "sim" in f:
        sim = True
        pf = open('/data_linux/argoneut/calibration_files/wireByWireCorrections_sim.pkl', 'rb')
    else:
        sim = False
        pf = open('/data_linux/argoneut/calibration_files/wireByWireCorrections_data_gapsFilled.pkl', 'rb')

    wire_calibrations = pickle.load(pf)

    # This runs the initial calculation, which gets ~close
    caloAlg = runCaloMuons(wire_calibrations, f)

    # makeCorrelationPlots(caloAlg,sim)
    # muonPlot2D(caloAlg)

    makeTotalLandaus(caloAlg)

    # The wire by wire correction is already done.  So read in the muons
    # and then we can make plots.

    # First, make a landau plot of all the wires from the two planes


if __name__ == '__main__':
    main()
