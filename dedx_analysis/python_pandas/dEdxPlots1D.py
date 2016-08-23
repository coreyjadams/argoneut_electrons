import landau
import numpy as np
import math

import operator

from matplotlib.ticker import NullFormatter
from matplotlib import pyplot as plt

from dataFrameHandle import dataFrameHandle


def makeDistPlots(mindist, maxdist, step):

    dist = mindist
    while dist < maxdist:
        electrons_df = dataFrameHandle("../anatrees/electrons_anatree_fix.root", "electron")
        photons_df =  dataFrameHandle("../anatrees/photons_anatree_fix.root", "photon")
        electrons_df._dists = [dist]
        photons_df._dists = [dist]
        electrons_df.init()
        photons_df.init()
        electrons_df.make_quality_cuts(1.5)
        photons_df.make_quality_cuts(1.5)

        print len(electrons_df._df.index)
        print len(photons_df._df.index)

        measure = "charge"
        recomb = "const"

        # for i in np.arange(0,6,0.25):
        #     accepted,rejected = electron_df.get_cut_amounts("c_charge_dedx_const_median_0",i)
        #     print i, ":{} / {} = {}".format(accepted,accepted+rejected,accepted/(accepted+rejected)) 

        # plotEff(electron_df,photon_df,"c_charge_dedx_box_LMA")

        metrics = ["median_0"]
        # metrics = ["median_0","mean_no_outliers_0","LMA","meta_0"]


        for _metric in metrics:
                # plotdEdx(electrons_df._df,photons_df._df,"c_charge_dedx_{}_{}".format(recomb,metric),-1)
                # plotdEdx(electrons_df._df,photons_df._df,"i_charge_dedx_{}_{}".format(recomb,metric),-1)
            cut = sigma(electrons_df,photons_df,"c_charge_dedx_{}_{}".format(recomb,_metric))
            plotdEdx(electrons_df._df,photons_df._df,"c_charge_dedx_{}_{}".format(recomb,_metric),cut,dist)
            cut = sigma(electrons_df,photons_df,"i_charge_dedx_{}_{}".format(recomb,_metric))
            plotdEdx(electrons_df._df,photons_df._df,"i_charge_dedx_{}_{}".format(recomb,_metric),cut,dist)
    
        dist += step

def main():

    # makeDistPlots(1.75,4.0,0.1)
    # return

    electrons_df = dataFrameHandle("../anatrees/electrons_anatree_fix.root", "electron")
    photons_df =  dataFrameHandle("../anatrees/photons_anatree_fix.root", "photon")
    electrons_df.make_quality_cuts(1.5)
    photons_df.make_quality_cuts(1.5)

    print len(electrons_df._df.index)
    print len(photons_df._df.index)

    measure = "const"
    recomb = "box"

    # for i in np.arange(0,6,0.25):
    #     accepted,rejected = electron_df.get_cut_amounts("c_charge_dedx_const_median_0",i)
    #     print i, ":{} / {} = {}".format(accepted,accepted+rejected,accepted/(accepted+rejected)) 

    # plotEff(electron_df,photon_df,"c_charge_dedx_box_LMA")

    metrics = ["median_"]
    # metrics = ["meta_"]

    dists = electrons_df._dists

    for metric in metrics:
        for i in range(len(dists)):
            dist = dists[i]
            # print metric
            _metric = metric + str(i)
            # print _metric
            cut = sigma(electrons_df,photons_df,"c_charge_dedx_{}_{}".format(recomb,_metric))
            plotdEdx(electrons_df._df,photons_df._df,"c_charge_dedx_{}_{}".format(recomb,_metric),cut,dist)
            cut = sigma(electrons_df,photons_df,"i_charge_dedx_{}_{}".format(recomb,_metric))
            plotdEdx(electrons_df._df,photons_df._df,"i_charge_dedx_{}_{}".format(recomb,_metric),cut,dist)
    


def plotEff(electron_df,photon_df,branch_name):

    # Get the efficiency as a function of dE/dx cut
    x_points = np.arange(0,10,0.1)
    electron_eff = getEfficiency(electron_df,branch_name,x_points)
    photon_eff   = getEfficiency(photon_df,branch_name,x_points)


    fig,ax = plt.subplots()
    plt.plot(x_points,electron_eff)
    plt.plot(x_points,photon_eff,color='r')
    plt.show()

def sigma(electron_df,photon_df,branch_name):
    x_points = np.arange(2.0,12,0.05)

    sigma = []

    for point in x_points:
        e_acc, e_rej = electron_df.get_cut_amounts(branch_name,point) 
        e_acc /= (e_acc + e_rej)
        p_acc, p_rej = photon_df.get_cut_amounts(branch_name,point) 
        p_acc /= (p_acc + p_rej)
        sigma.append(e_acc / math.sqrt(e_acc**2 + p_acc**2 + 0.01))


    index, value = max(enumerate(sigma), key=operator.itemgetter(1))

    # fig,ax = plt.subplots()
    # plt.plot(x_points,sigma,label=r"$\frac{Sig.}{\sqrt{Bkg.^2 + Sig^2}}$")
    # ax.axvline(x_points[index],color='g',ls="--")

    # plt.xlabel("dE/dx [MeV/cm]")
    # plt.legend(fontsize=25)
    # plt.grid(True)
    # plt.savefig("/data_linux/dedx_plots/1D/sigma_dedx_1D_"+branch_name + ".png")
    # plt.show()
    return x_points[index]

def getEfficiency(dataframe,branch_name,points):
    res = []
    for point in points:
        accepted, rejected = dataframe.get_cut_amounts(branch_name,point)
        res.append(accepted/(accepted+rejected))
    return res

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

def optimizeCut(electron_df, photon_df, branch_name):

    # Basically, loop through and find the value that optimizes the ratio of electrons to photons:
    # Setting lower and upper limits just to stop the edge effects:
    lower_limit = 1.0
    upper_limit = 10

    # Here's the plan.  take a value, x, and a step alpha.
    # Get the eff at x, x - alpha and x + alpha.
    # Set x to the one with the best eff.  If it's x, decrease alpha.
    #
    # When alpha is small, stop.
    #
    # To ensure it doesn't hit a local max, start by scanning over a few points
    # and use the max value.
    points = np.arange(lower_limit, upper_limit, 0.1)
    vals = []
    max_purity = 0.0
    max_tup = []
    max_point = 0
    for point in points:
        purity, tup = makeCut(electron_df, photon_df, branch_name, point)
        if purity > max_purity and tup[0][0] > 0.75:
            max_point = point
            max_tup = tup
            max_purity = purity

        # print "\tPurity: {}".format(purity)

    return max_point, max_purity, max_tup


def makeCut(electron_df, photon_df, branch_name, cut_value):
    """
    This function will make a cut at the value specified on the branch_name specified
    It calculates the percent of each sample left on each side, and returns ???
    """

    bins = [0, cut_value, 10]

    # First, get the arrays of data:
    electrons = electron_df[branch_name].values
    photons = photon_df[branch_name].values

    # Use a histogram to slice this into two sections, one above the cut value
    # and one below
    electrons, junk = np.histogram(electron_df[branch_name], bins)
    photons, junk = np.histogram(photon_df[branch_name], bins)

    electron_eff = (1.0*electrons[0])/(electrons[0] + electrons[1])
    photon_eff = (1.0*photons[0])/(photons[0] + photons[1])

    try:
        purity = 1.0*electrons[0] / (electrons[0] + photons[0])
    except:
        purity = 0.0

    try:
        electron_eff_stat_err = electron_eff * \
            math.sqrt(1.0/electrons[0] + 1.0/(electrons[0] + electrons[1]))
    except:
        electron_eff_stat_err = 999

    try:
        photon_eff_stat_err = photon_eff * \
            math.sqrt(1.0/photons[0] + 1.0/(photons[0] + photons[1]))
    except:
        photon_eff_stat_err = 999

    try:
        opt = electron_eff / photon_eff
    except:
        opt = 0.0

    try:
        opt_err = opt * \
            math.sqrt(
                electron_eff_stat_err/electron_eff + photon_eff_stat_err / photon_eff)
    except:
        opt_err = 999

    return purity, [[electron_eff, electron_eff_stat_err], [photon_eff, photon_eff_stat_err]]


if __name__ == '__main__':
    main()
