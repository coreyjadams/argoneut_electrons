from matplotlib.ticker import NullFormatter
from matplotlib import pyplot as plt

import ROOT
import numpy
import showerCalo

import math

from matplotlib import pyplot as plt


def main():

    # (e_data, e_sim), (p_data, p_sim) = showerCalo.lite_samples()
    (e_data, e_sim), (p_data, p_sim) = showerCalo.full_samples()

    print e_data.size()
    print e_sim.size()
    print p_data.size()
    print p_sim.size()

    # VertexQuality(e_sim, p_sim)
    AngleQuality(e_sim, p_sim)


def VertexQuality(electrons_mc, photons_mc):

    # Get the vector of reconstructed start points and the one of mc start
    # points

    electron_x_res = electrons_mc.getShowerCaloVector().mc_x_resolution()
    electron_y_res = electrons_mc.getShowerCaloVector().mc_y_resolution()
    electron_z_res = electrons_mc.getShowerCaloVector().mc_z_resolution()
    photon_x_res = photons_mc.getShowerCaloVector().mc_x_resolution()
    photon_y_res = photons_mc.getShowerCaloVector().mc_y_resolution()
    photon_z_res = photons_mc.getShowerCaloVector().mc_z_resolution()

    electron_reco_starts = electrons_mc.getShowerCaloVector(
        ).mc_3D_vertex_resolution()
    photon_reco_starts = photons_mc.getShowerCaloVector(
        ).mc_3D_vertex_resolution()

    binwidth = 0.1
    _3d_bins = numpy.arange(0.0, 5.0, binwidth)
    _1d_bins = numpy.arange(-2.5, 4.5, binwidth)

    electron_hist, _3d_bin_edges = numpy.histogram(
        electron_reco_starts, bins=_3d_bins, density=True)
    photon_hist, _3d_bin_edges = numpy.histogram(
        photon_reco_starts, bins=_3d_bins, density=True)

    electron_hist_x, _1d_bin_edges = numpy.histogram(
        electron_x_res, bins=_1d_bins, density=True)
    photon_hist_x, _1d_bin_edges = numpy.histogram(
        photon_x_res, bins=_1d_bins, density=True)
    electron_hist_y, _1d_bin_edges = numpy.histogram(
        electron_y_res, bins=_1d_bins, density=True)
    photon_hist_y, _1d_bin_edges = numpy.histogram(
        photon_y_res, bins=_1d_bins, density=True)
    electron_hist_z, _1d_bin_edges = numpy.histogram(
        electron_z_res, bins=_1d_bins, density=True)
    photon_hist_z, _1d_bin_edges = numpy.histogram(
        photon_z_res, bins=_1d_bins, density=True)

    _3d_bin_centers = _3d_bin_edges[:-1] + 0.5*binwidth

    _1d_bin_centers = _1d_bin_edges[:-1] + 0.5*binwidth


    plt.plot(_1d_bin_centers,electron_hist_x,ls="steps",label="MC Single Electrons X")
    plt.plot(_1d_bin_centers,photon_hist_x,ls="steps",label="MC Single Photons X")
    
    plt.legend()
    plt.title("X Resolution of Single Showers")
    plt.xlabel("True - Reco Vertex [cm]")
    plt.ylabel("Area Normalized")
    plt.grid(True)

    plt.show()

    plt.plot(_1d_bin_centers,electron_hist_y,ls="steps",label="MC Single Electrons Y")
    plt.plot(_1d_bin_centers,photon_hist_y,ls="steps",label="MC Single Photons Y")
    
    plt.legend()
    plt.title("Y Resolution of Single Showers")
    plt.xlabel("True - Reco Vertex [cm]")
    plt.ylabel("Area Normalized")
    plt.grid(True)

    plt.show()

    plt.plot(_1d_bin_centers,electron_hist_z,ls="steps",label="MC Single Electrons Z")
    plt.plot(_1d_bin_centers,photon_hist_z,ls="steps",label="MC Single Photons Z")
    
    plt.legend()
    plt.title("Z Resolution of Single Showers")
    plt.xlabel("True - Reco Vertex [cm]")
    plt.ylabel("Area Normalized")
    plt.grid(True)

    plt.show()


    plt.plot(_3d_bin_centers,electron_hist,ls="steps",label="MC Single Electrons")
    plt.plot(_3d_bin_centers,photon_hist,ls="steps",label="MC Single Photons")
    
    plt.legend()
    plt.title("3D Vertex Resolution of Single Showers")
    plt.xlabel("True - Reco Vertex [cm]")
    plt.ylabel("Area Normalized")
    plt.grid(True)

    plt.show()


def AngleQuality(electrons_mc, photons_mc):

    # Get the vector of reconstructed start points and the one of mc start points
    
    electron_reco_angles = electrons_mc.getShowerCaloVector().mc_3D_angle_resolution()
    photon_reco_angles = photons_mc.getShowerCaloVector().mc_3D_angle_resolution()

    for p_angle in photon_reco_angles:
        if math.isnan(p_angle):
            print "p ", p_angle

    for p_angle in electron_reco_angles:
        if math.isnan(p_angle):
            print "e ", p_angle

    binwidth = 0.25
    bins = numpy.arange(0.0, 20.0, binwidth)

    electron_hist, bin_edges = numpy.histogram(electron_reco_angles, bins=bins,density=True)
    photon_hist, bin_edges = numpy.histogram(photon_reco_angles, bins=bins,density=True)

    bin_centers = bin_edges[:-1] + 0.5*binwidth
    print bin_centers

    plt.plot(bin_centers,electron_hist,ls="steps",label="MC Single Electrons")
    plt.plot(bin_centers,photon_hist,ls="steps",label="MC Single Photons")
    
    plt.legend()
    plt.title("3D Angular Resolution of Single Showers")
    plt.xlabel("True - Reco Direction [degree]")
    plt.ylabel("Area Normalized")
    plt.grid(True)

    plt.show()

if __name__ == '__main__':
    main()
