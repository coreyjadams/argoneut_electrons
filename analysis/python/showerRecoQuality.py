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

    VertexQuality(e_sim, p_sim)
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

    fig, ax = plt.subplots(figsize=(10,6))
    plt.plot(_1d_bin_centers,electron_hist_x,ls="steps-mid",label="MC Single Electrons X",linewidth=3,c='b')
    plt.plot(_1d_bin_centers,photon_hist_x,ls="steps-mid",label="MC Single Photons X",linewidth=3,c='r')
    
    

    plt.legend(fontsize=20)
    plt.title("X Resolution of Single Showers",fontsize=25)
    plt.xlabel("|True - Reco Vertex [cm]|",fontsize=20)
    plt.ylabel("Area Normalized",fontsize=20)
    plt.grid(True)
    for tick in ax.xaxis.get_major_ticks():
        tick.label.set_fontsize(16)
    for tick in ax.yaxis.get_major_ticks():
        tick.label.set_fontsize(0)
    plt.show()

    fig, ax = plt.subplots(figsize=(10,6))
    plt.plot(_1d_bin_centers,electron_hist_y,ls="steps-mid",label="MC Single Electrons Y",linewidth=3,c='b')
    plt.plot(_1d_bin_centers,photon_hist_y,ls="steps-mid",label="MC Single Photons Y",linewidth=3,c='r')
    
    
    plt.legend(fontsize=20)
    plt.title("Y Resolution of Single Showers",fontsize=25)
    plt.xlabel("|True - Reco Vertex [cm]|",fontsize=20)
    plt.ylabel("Area Normalized",fontsize=20)
    plt.grid(True)
    for tick in ax.xaxis.get_major_ticks():
        tick.label.set_fontsize(16)
    for tick in ax.yaxis.get_major_ticks():
        tick.label.set_fontsize(0)
    plt.show()

    fig, ax = plt.subplots(figsize=(10,6))
    plt.plot(_1d_bin_centers,electron_hist_z,ls="steps-mid",label="MC Single Electrons Z",linewidth=3,c='b')
    plt.plot(_1d_bin_centers,photon_hist_z,ls="steps-mid",label="MC Single Photons Z",linewidth=3,c='r')
    
    
    plt.legend(fontsize=20)
    plt.title("Z Resolution of Single Showers",fontsize=25)
    plt.xlabel("|True - Reco Vertex [cm]|",fontsize=20)
    plt.ylabel("Area Normalized",fontsize=20)
    plt.grid(True)
    for tick in ax.xaxis.get_major_ticks():
        tick.label.set_fontsize(16)
    for tick in ax.yaxis.get_major_ticks():
        tick.label.set_fontsize(0)
    plt.show()


    fig, ax = plt.subplots(figsize=(10,6))
    plt.plot(_3d_bin_centers,electron_hist,ls="steps-mid",label="MC Single Electrons",linewidth=3,c='b')
    plt.plot(_3d_bin_centers,photon_hist,ls="steps-mid",label="MC Single Photons",linewidth=3,c='r')
    
    
    plt.legend(fontsize=20)
    plt.title("3D Vertex Resolution of Single Showers",fontsize=25)
    plt.xlabel("|True - Reco Vertex [cm]|",fontsize=20)
    plt.ylabel("Area Normalized",fontsize=20)
    plt.grid(True)
    for tick in ax.xaxis.get_major_ticks():
        tick.label.set_fontsize(16)
    for tick in ax.yaxis.get_major_ticks():
        tick.label.set_fontsize(0)
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

    binwidth = 0.4
    bins = numpy.arange(0.0, 20.0, binwidth)

    electron_hist, bin_edges = numpy.histogram(electron_reco_angles, bins=bins,density=True)
    photon_hist, bin_edges = numpy.histogram(photon_reco_angles, bins=bins,density=True)

    bin_centers = bin_edges[:-1] + 0.5*binwidth
    print bin_centers

    fig, ax = plt.subplots(figsize=(10,6))
    plt.plot(bin_centers,electron_hist,ls="steps-mid",label="MC Single Electrons",linewidth=3,c='b')
    plt.plot(bin_centers,photon_hist,ls="steps-mid",label="MC Single Photons",linewidth=3,c='r')
    
    
    plt.legend(fontsize=20)
    plt.title("3D Angular Resolution of Single Showers",fontsize=25)
    plt.xlabel("|True - Reco Direction [degree]|",fontsize=20)
    plt.ylabel("Area Normalized",fontsize=20)
    plt.grid(True)
    for tick in ax.xaxis.get_major_ticks():
        tick.label.set_fontsize(16)
    for tick in ax.yaxis.get_major_ticks():
        tick.label.set_fontsize(0)
    plt.show()

if __name__ == '__main__':
    main()
