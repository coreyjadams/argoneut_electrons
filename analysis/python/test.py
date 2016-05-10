
import ROOT
import larlite
from ROOT import argoana

from showerCalo import showerCalo

import numpy

from matplotlib import pyplot as plt


def main():
    # For data:
    electron_data_file = "/data_linux/argoneut/dedx_files/electron_shower_reco.root"
    photon_data_file = "/data_linux/argoneut/dedx_files/photon_shower_reco.root"

    data_calibration_file = "/data_linux/argoneut/calibration_files/wireByWireCorrections_data_gapsFilled.pkl"

    electron_data_module = showerCalo(
        electron_data_file, data_calibration_file, mc=False, producer="bootlegMatched")
    photon_data_module = showerCalo(
        photon_data_file, data_calibration_file,mc=False, producer="bootlegMatched")

    electron_data_module.run()
    photon_data_module.run()

    # For MC:
    electron_sim_file = "/data_linux/argoneut/argoneut_single_electron/single_electrons_sim_larlite_mergeall_endpoint2d_match_shower.root"
    photon_sim_file = "/data_linux/argoneut/argoneut_single_photon/single_photons_larlite_mergeall_endpoint2d_match_shower.root"

    sim_calibration_file = "/data_linux/argoneut/calibration_files/wireByWireCorrections_sim.pkl"

    electron_sim_module = showerCalo(
        electron_sim_file, sim_calibration_file, mc=True, producer="showerreco")
    photon_sim_module = showerCalo(
        photon_sim_file, sim_calibration_file,mc=True, producer="showerreco")

    # electron_sim_module.run()
    # photon_sim_module.run()

    for shower in electron_data_module.getShowerCaloVector():
        dists = shower._collection_dist_3D
        for dist in dists:
            print dist
        plot_dEdx_vs_distance(shower)
        print "Electron dE/dx n_good_hits: {} / {}".format(
        shower.n_good_hits(1),shower.n_hits(1))

    # for shower in photon_module.getShowerCaloVector():
    #   plot_dEdx_vs_distance(shower)
    #   # print "Photon dE/dx median: {}".format( shower.n_good_hits(1))


    # Plot the histograms of electrons:
    bin_width = 0.15
    bins = numpy.arange(0,8.0+bin_width,bin_width)

    e_sim_dedx_hist, bin_edges = numpy.histogram(electron_sim_module.getMedianVector(1),bins=bins,normed=True)
    e_data_dedx_hist, bin_edges = numpy.histogram(electron_data_module.getMedianVector(1),bins=bins,normed=True)


    g_sim_dedx_hist, bin_edges = numpy.histogram(photon_sim_module.getMedianVector(1),bins=bins,normed=True)
    g_data_dedx_hist, bin_edges = numpy.histogram(photon_data_module.getMedianVector(1),bins=bins,normed=True)


    bin_centers = bin_edges[:-1] + 0.5*bin_width


    plt.plot(bin_centers,e_sim_dedx_hist,color="b",label="Simulated Electrons")
    plt.plot(bin_centers,g_sim_dedx_hist,color="r",label="Simulated Photons")
    plt.plot(bin_centers,e_data_dedx_hist,color="b",marker="x",ls="",label="Data Electrons")
    plt.plot(bin_centers,g_data_dedx_hist,color="r",marker="x",ls="",label="Data Photons")
    plt.grid(True)
    plt.legend()
    plt.show()

def plot_dEdx_vs_distance(shower):

    c_dedx = shower._collection_dedx
    c_dist = shower._collection_dist_3D
    i_dedx = shower._induction_dedx
    i_dist = shower._induction_dist_3D

    # print "Collection start: ({}, {})".format(shower._collection_start.w, shower._collection_start.t)
    # print "Induction start: ({}, {})".format(shower._induction_start.w,
    # shower._induction_start.t)

    # for hit, dist, dedx in zip(shower._collection_hits,
    #                            shower._collection_dist_3D,
    #                            shower._collection_dedx):
    #     print "Selected hit at ({:.3},{:.3}), dist {:.3}, dE/dx {:.3}".format(hit.w, hit.t,dist,dedx)

    print "Start X: {}".format(shower._3D_start_point.X())
    print "Other X: {}".format(shower._other_start_point.X())

    print "Vertex error: {}".format(shower.vertex_error())
    print "Slope error: {}".format(shower.slope_error())

    plt.plot(c_dist, c_dedx, marker="+", ls="", label="Collection Plane")
    plt.plot(i_dist, i_dedx, marker="+", ls="", label="Induction Plane")
    plt.axhline(y=shower.dEdx_median(
        1), ls="--", color="b", label="Collection dE/dx")
    plt.axhline(
        y=shower.dEdx_median(0), ls="--", color="g", label="Induction dE/dx")
    plt.grid(True)
    plt.ylim((0, 8))
    plt.xlabel("3D Distance [cm]")
    plt.ylabel("dE/dx [MeV/cm]")
    plt.legend()
    plt.title("Run {}, Event {}".format(shower.run, shower.event))
    plt.show()

if __name__ == '__main__':
    main()
