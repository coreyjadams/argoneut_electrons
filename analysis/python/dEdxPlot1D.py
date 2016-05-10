
import ROOT
import numpy
import showerCalo

import math

from matplotlib import pyplot as plt

def main():

    plane = 0

    # (e_data, e_sim), (p_data, p_sim) = showerCalo.full_samples()
    (e_data, e_sim), (p_data, p_sim) = showerCalo.lite_samples()

    print e_data.size()
    print e_sim.size()
    print p_data.size()
    print p_sim.size()

    # for shower in e_data.getShowerCaloVector():
        # print "{}, {:.3}".format(shower.n_good_hits(1), shower.dEdx_median(1))

    data_bin_width = 0.3
    sim_bin_width = 0.1

    data_bins = numpy.arange(data_bin_width, 8.0, data_bin_width)
    sim_bins = numpy.arange(sim_bin_width, 8.0, sim_bin_width)

    electron_data_hist, bin_edges_data = numpy.histogram(e_data.getMedianVector(plane), data_bins)
    photon_data_hist, bin_edges_data = numpy.histogram(p_sim.getMedianVector(plane), data_bins)

    electron_err = []
    electron_frac_err = []
    photon_err = []
    photon_frac_err = []
    for e_val, p_val in zip(electron_data_hist, photon_data_hist):
        electron_err.append(math.sqrt(e_val))
        if e_val != 0:
            electron_frac_err.append(math.sqrt(e_val) / e_val)
        else:
            electron_frac_err.append(0.0)

        photon_err.append(math.sqrt(p_val))
        if p_val != 0:
            photon_frac_err.append(math.sqrt(p_val) / p_val)
        else:
            photon_frac_err.append(0.0)

    electron_data_hist, bin_edges_data = numpy.histogram(e_data.getMedianVector(plane), data_bins,density=True)
    photon_data_hist, bin_edges_data = numpy.histogram(p_data.getMedianVector(plane), data_bins,density=True)


    electron_sim_hist, bin_edges_sim = numpy.histogram(e_sim.getMedianVector(plane), sim_bins,density=True)
    photon_sim_hist, bin_edges_sim = numpy.histogram(p_sim.getMedianVector(plane), sim_bins,density=True)


    bin_centers_data = bin_edges_data[:-1] + 0.5*data_bin_width
    bin_centers_sim = bin_edges_sim[:-1] + 0.5*sim_bin_width

    electron_err_norm = []
    for e_val, frac_err in zip(electron_data_hist, electron_frac_err):
        electron_err_norm.append(e_val*frac_err)

    photon_err_norm = []
    for p_val, frac_err in zip(photon_data_hist, photon_frac_err):
        photon_err_norm.append(p_val*frac_err)

    f2, ax2 = plt.subplots(figsize=(15, 8))
    ax2.set_title("Electron Photon Separation",fontsize=30)
    ax2.errorbar(bin_centers_data, electron_data_hist, xerr=data_bin_width*0.5,
                 yerr=electron_err_norm,marker="o", label="Electrons, Data",capsize=0,ls="none")
    ax2.errorbar(bin_centers_data, photon_data_hist, xerr=data_bin_width*0.5,
                 yerr=photon_err_norm,marker="o", label="Photons, Data",capsize=0,color='r',ls="none")
    plt.xlabel("dE/dx [MeV/cm]",fontsize=20)
    plt.ylabel("Unit Normalized",fontsize=20)

    ax2.plot(bin_centers_sim,electron_sim_hist,color="b",ls="steps",label="Simulated Electrons")
    ax2.plot(bin_centers_sim,photon_sim_hist,color="r",ls="steps",label="Simulated Electrons")

    plt.grid(True)
    plt.legend()

    # Draw a histogram with all of these:

    plt.show()


def plotdEdx(electrons_df, photons_df,branch_name,cut=-1.0,dist=-1.0):

    plane = ""
    if branch_name[0] == 'c':
        plane = "Collection"
    elif branch_name[0] == 'i':
        plane = "Induction"
    else:
        plane = "Combined"

    binwidth = 0.4
    bins = np.arange(0, 12, binwidth)
    electron_data, bin_edges = np.histogram(electrons_df[branch_name], bins)
    photon_data, bin_edges = np.histogram(photons_df[branch_name], bins)

    # print bin_edges

    electron_err = []
    electron_frac_err = []
    photon_err = []
    photon_frac_err = []
    for e_val, p_val in zip(electron_data, photon_data):
        electron_err.append(math.sqrt(e_val))
        if e_val != 0:
            electron_frac_err.append(math.sqrt(e_val) / e_val)
        else:
            electron_frac_err.append(0.0)

        photon_err.append(math.sqrt(p_val))
        if p_val != 0:
            photon_frac_err.append(math.sqrt(p_val) / p_val)
        else:
            photon_frac_err.append(0.0)

    x = bin_edges + 0.5*binwidth

    electron_data_norm, bin_edges = np.histogram(
        electrons_df[branch_name], bins, density=True)
    photon_data_norm, bin_edges = np.histogram(
        photons_df[branch_name], bins, density=True)

    photon_data_norm *= binwidth
    electron_data_norm *= binwidth

    # print electron_data
    # # print photon_data
    # print "N electrons: ", np.sum(electron_data)
    # print "N photons: ", np.sum(photon_data)
    # print electron_data_norm    

    electron_err_norm = []
    for e_val, frac_err in zip(electron_data_norm, electron_frac_err):
        electron_err_norm.append(e_val*frac_err)

    photon_err_norm = []
    for p_val, frac_err in zip(photon_data_norm, photon_frac_err):
        photon_err_norm.append(p_val*frac_err)

    f2, ax2 = plt.subplots(figsize=(15, 8))
    ax2.set_title("Electron Photon Separation, {}".format(plane),fontsize=30)
    ax2.errorbar(x[:-1], electron_data_norm, xerr=binwidth*0.5,
                 yerr=electron_err_norm,marker="o", label="Electrons",capsize=0,ls="none")
    ax2.errorbar(x[:-1], photon_data_norm, xerr=binwidth*0.5,
                 yerr=photon_err_norm,marker="o", label="Photons",capsize=0,color='r',ls="none")
    plt.xlabel("dE/dx [MeV/cm]",fontsize=20)
    plt.ylabel("Unit Normalized",fontsize=20)

    plt.ylim([0,0.75])
    
    for tick in ax2.xaxis.get_major_ticks():
        tick.label.set_fontsize(20)
    for tick in ax2.yaxis.get_major_ticks():
        tick.label.set_fontsize(0)

    if cut > 0:
        purity, eff_tuple = makeCut(electrons_df, photons_df,branch_name,cut)

        # Now draw a line at the place that gives the best dEdx cut:

        ax2.axvline(cut,linewidth=2,color='g',linestyle="--",label="Cut at {} MeV/cm".format(cut))
        y_lims = ax2.get_ylim()
        plt.text(6.05,y_lims[1]*0.6,"Electron Efficiency:",size=25,)
        plt.text(11.95,y_lims[1]*0.6,"{:.2} +/- {:.2}".format(1.0*eff_tuple[0][0],1.0*eff_tuple[0][1]),size=25,horizontalalignment='right')
        plt.text(6.05,y_lims[1]*0.5,"Photon Mis. ID:",size=25,)
        plt.text(11.95,y_lims[1]*0.5,"{:.2} +/- {:.2}".format(1.0*eff_tuple[1][0],1.0*eff_tuple[1][1]),size=25,horizontalalignment='right')
        if dist != -1.0:
            plt.text(6.05,y_lims[1]*0.4,"Distance:",size=25)
            plt.text(11.95,y_lims[1]*0.4,"{:.3}".format(dist),size=25,horizontalalignment='right')
    # if info is not None:
    #     plt.text(6.05,0.405,info,size=18)

    plt.legend(fontsize=25)
    plt.grid(True)
    # # plt.show()
    path="/data_linux/dedx_plots/1D/"
    name="dedx_1D_"
    if dist != -1.0:
        name += "dist_study_{}_".format(dist)
    if cut != -1.0:
        name += "cut_{}_".format(cut)
    plt.savefig(path + name + branch_name + ".png")
    plt.close(f2)
    plt.show()

if __name__ == '__main__':
  main()