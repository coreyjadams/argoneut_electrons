import ROOT
import larlite
import math

from ROOT import larlite, larutil
from ROOT import TTree

# use matplotlib to make these plots

import matplotlib.pyplot as plt
from matplotlib.ticker import NullFormatter
import numpy as np

import pandas as pd
from root_numpy import root2array

photons = dict()
electrons = dict()


Q_AREA = [0.8152236862020928, 0.8078780681936417]
Q_AMP = [9.207991314407435, 18.562195730195935]
E_AREA = [185.00584273133333, 44.90267652367623]
E_AMP = [184.9556285310639, 45.1669419567248]


correction = dict()
correction.update({'c': dict()})
correction.update({'i': dict()})
correction['c'].update({'charge': Q_AREA[1]*E_AREA[1]})
correction['i'].update({'charge': Q_AREA[0]*E_AREA[0]})
correction['c'].update({'peak': Q_AMP[1]*E_AMP[1]})
correction['i'].update({'peak': Q_AMP[0]*E_AMP[0]})


planes = ["c","i"]
measures = ["charge","peak"]
recombs = ["const","birks","box"]
dists = [2.75]
# dists = np.arange(0.71,3.51,0.02)
# dists = [2.0]

# planes = ["c",]
# measures = [ "peak"]
# recombs = ["birks"]
# dists = [1.0]

def readElectrons():

    # photons = dict()
    electronFiles = ["text/nu_electrons.txt",
                     "text/anu_electrons.txt",
                     "text/misc_electrons.txt"]
    for ef in electronFiles:
        with open(ef, "r") as f:
            for line in f:
                vals = line.split()
                run = int(vals[0])
                event = int(vals[1])
                if run in electrons:
                    if event in electrons[run]:
                        pass
                    else:
                        electrons[run].append(event)
                else:
                    electrons.update({run: [event]})


def readPhotons():
    # photons = dict()
    photonFiles = ["text/nu_photons.txt",
                   "text/anu_pi0.txt",
                   "text/nu_pi0.txt",
                   "text/misc_photons.txt"]
    for pf in photonFiles:
        with open(pf, "r") as f:
            for line in f:
                vals = line.split()
                run = int(vals[0])
                event = int(vals[1])
                if run in photons:
                    if event in photons[run]:
                        pass
                    else:
                        photons[run].append(event)
                else:
                    photons.update({run: [event]})


def readWireCalibrations():
    cals = dict()
    cals.update({0: dict()})
    cals.update({1: dict()})
    with open("text/wire_calibrations.txt", "r") as f:
        for line in f:
            vals = line.split()
            # print vals
            cals[int(vals[0])].update({int(vals[1]): float(vals[2])})
    return cals


def selectWire(hit):
    return hit[0]


def calcdEdx(df):

        # df.info()

    for plane in planes:
        for measure in measures:
            df["{}_hit{}s".format(
                plane, measure)] *= df["{}_lifetime_corr".format(plane)]

        df["{}_startwire".format(plane)] = df[
            "{}_starthit".format(plane)].apply(selectWire)

        df['{}_wire_corrections'.format(plane)] = df[
            '{}_hitwires'.format(plane)].apply(wireCorrC)

        df['{}_dist_from_start'.format(plane)] = (df['{}_hitwires'.format(
            plane)] - df['{}_startwire'.format(plane)])*df['{}_pitch'.format(plane)]

        for measure in measures:

            df['{}_hit{}s'.format(
                plane, measure)] *= df['{}_wire_corrections'.format(plane)]
            # df['{}_hit{}s_q'.format(plane,measure)] =  df['{}_hit{}s_q'.format(plane,measure)]* Q_AREA
            df['{}_hit{}s_dq'.format(plane, measure)] = df[
                '{}_hit{}s'.format(plane, measure)]*correction[plane][measure]
            df['{}_{}_dqdx'.format(plane, measure)] = df[
                '{}_hit{}s_dq'.format(plane, measure)] / df['{}_pitch'.format(plane)]

    df = df.apply(dqdx_to_dedx_all, 1)

    df = df.apply(dEdxFunctions, 1)

    return df


def dqdx_to_dedx_all(s):

    for plane in planes:
        for measure in measures:
            s['{}_{}_dedx_box'.format(plane, measure)] = boxCorrection(
                s['{}_{}_dqdx'.format(plane, measure)])
            s['{}_{}_dedx_birks'.format(plane, measure)] = birksCorrection(
                s['{}_{}_dqdx'.format(plane, measure)])
            s['{}_{}_dedx_const'.format(plane, measure)] = constCorrection(
                s['{}_{}_dqdx'.format(plane, measure)])

    return s


def boxCorrection(dqdx_v):
    result = []
    for x in dqdx_v:
        result.append(larutil.LArProperties.GetME().ModBoxCorrection(x))
    return result

def constCorrection(dqdx_v):
    result = []
    for x in dqdx_v:
        result.append(3.40e-05 * x)
    return result

def birksCorrection(dqdx_v):
    result = []
    for x in dqdx_v:
        result.append(larutil.LArProperties.GetME().BirksCorrection(x))
    return result


def wireCorrC(wires):
    vals = []
    for wire in wires:
        vals.append(wire_calibrations[1][wire])
    return vals


def wireCorrI(wires):
    vals = []
    for wire in wires:
        vals.append(wire_calibrations[0][wire])
    return vals


def dEdx_mean(dedx_v):
    # Just compute the mean of these points and return that
    return np.mean(dedx_v)


def dedx_dist(input_s):
    # print input_s

    return


def dEdxFunctions(s):

        # First, cut out any hits that are at negative distance
        # print s

    for plane in planes:
        for measure in measures:
            for rec in recombs:
                s['{}_{}_dedx_{}_mean'.format(plane, measure, rec)] = dEdx_mean(
                    s['{}_{}_dedx_{}'.format(plane, measure, rec)])

                s['{}_{}_dedx_{}_LMA'.format(plane, measure, rec)] = dEdx_lowest_moving_average(
                    s['{}_{}_dedx_{}'.format(plane, measure, rec)])

                for dist in dists:
                    s['{}_{}_dedx_{}_mean_no_outliers_{}'.format(
                        plane, measure, rec, dist)] = dEdx_mean_no_outliers(
                            s['{}_{}_dedx_{}'.format(plane, measure, rec)],
                            s['{}_dist_from_start'.format(plane)], dist)
                    s['{}_{}_dedx_{}_median_{}'.format(
                        plane, measure, rec, dist)] = dEdx_median(
                            s['{}_{}_dedx_{}'.format(plane, measure, rec)],
                            s['{}_dist_from_start'.format(plane)], dist)

    return s


def dEdx_mean_no_outliers(dedx_v, dist_v, dist_cut=2.4, startindex=1):

    # Pick out values with dist >= 0, < dist_cut
    points_within_dist = []
    for val, d in zip(dedx_v[startindex:], dist_v[startindex:]):
        if d > 0 and d < dist_cut:
            points_within_dist.append(val)

    if len(points_within_dist) == 0:
        return 0.0

    mean = np.mean(points_within_dist)
    dev = np.std(points_within_dist)
    val = 0.0
    n = 0
    if dev == 0.0:
        return mean
    for dedx in points_within_dist:
        if abs(mean-dedx) < 2.0*dev and dedx < 10:
            val += dedx
            n += 1
    if n > 0:
        return val/n
    else:
        return 0.0


def dEdx_lowest_moving_average(dedx_v, N=3, startindex=1):
    vals = []
    for i in xrange(len(dedx_v) - N):
        if i < startindex:
            continue
        vals.append(0.0)
        for v in xrange(N):
            vals[-1] += dedx_v[i+v]
        vals[-1] /= N
    if len(vals) > 0:
        return min(vals)
    else:
        return 0


def dEdx_median(dedx_v, dist_v, dist_cut=2.4, startindex=1):

    # Pick out values with dist >= 0, < dist_cut
    points_within_dist = []
    for val, d in zip(dedx_v[startindex:], dist_v[startindex:]):
        if d > 0 and d < dist_cut:
            points_within_dist.append(val)

    if len(points_within_dist) == 0:
        return 0.0

    return np.median(points_within_dist)


def loopThroughEvents(df, n=1):
    it = 0
    for index, row in df.iterrows():
        # Generate horizontal lines for electron and photon expected rates
        electrons = [2.3]*len(row['c_dist_from_start'])
        photons = [4.6]*len(row['c_dist_from_start'])
        plt.close('all')
        # Two subplots, unpack the axes array immediately
        f, (ax1, ax2) = plt.subplots(1, 2, sharey=True, figsize=[15, 7.5])
        f.suptitle(
            "dE/dx, Run {r}, Event {e}".format(r=row['run'], e=row.event))
        ax1.set_title('Collection Plane')
        ax1.plot(row['c_dist_from_start'], row[
                 'c_charge_dedx_box'], marker='+', markersize=20)
        ax1.plot(row['c_dist_from_start'], row[
                 'c_charge_dedx_birks'], marker='x', markersize=20)
        ax1.plot(row['c_dist_from_start'], electrons, linestyle='--')
        ax1.plot(row['c_dist_from_start'], photons, linestyle='--')
        plt.ylabel("dE/dx [MeV/cm]")
        f.text(0.01, 0.7, "mean: {}".format(row['c_charge_dedx_box_mean']))
        f.text(0.01, 0.65, "modmean: {}".format(
            row['c_charge_dedx_box_mean_no_outliers']))
        f.text(0.01, 0.6, "Med: {}".format(row['c_charge_dedx_box_median']))
        f.text(0.01, 0.55, "LMA: {}".format(row['c_charge_dedx_box_LMA']))

        ax1.grid(True)
        # ax2.scatter(x, y)
        ax2.set_title('Induction Plane')
        ax2.plot(row['i_dist_from_start'], row[
                 'i_charge_dedx_box'], marker='+', markersize=20)
        ax2.plot(row['i_dist_from_start'], row[
                 'i_charge_dedx_birks'], marker='x', markersize=20)
        electrons = [2.3]*len(row['i_dist_from_start'])
        photons = [4.6]*len(row['i_dist_from_start'])
        ax2.plot(row['i_dist_from_start'], electrons, linestyle='--')
        ax2.plot(row['i_dist_from_start'], photons, linestyle='--')
        ax2.grid(True)

        f.text(0.91, 0.7, "1: {}".format(row['i_charge_dedx_box_mean']))
        f.text(0.91, 0.65, "2: {}".format(
            row['i_charge_dedx_box_mean_no_outliers']))
        f.text(0.91, 0.6, "3: {}".format(row['i_charge_dedx_box_median']))
        f.text(0.91, 0.55, "4: {}".format(row['i_charge_dedx_box_LMA']))
        # Add some text to the plot to display the dE/dx calculated:

        x1, x2, y1, y2 = plt.axis()
        plt.axis((x1, x2, 0, 10))
        plt.show()
        it += 1
        if it >= n:
            return


def plotdEdx(electrons_df, photons_df,branch_name,info=None):


    binwidth = 0.4
    bins = np.arange(0, 10 + binwidth, binwidth)
    electron_data, bin_edges = np.histogram(electrons_df[branch_name], bins)
    photon_data, bin_edges = np.histogram(photons_df[branch_name], bins)

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

    x = bin_edges + binwidth*0.5

    # f, ax = plt.subplots(figsize=(8, 4))
    # ax.set_title("Electron Photon separation, {s}".format(s=branch_name))
    # # ax.hist(electron_data,bins=np.linspace(0,10,20),alpha=0.5)
    # ax.errorbar(x[:-1], electron_data, xerr=0.25*0.5,
    #             yerr=electron_err, label="Electrons")
    # ax.errorbar(
    #     x[:-1], photon_data, xerr=0.25*0.5, yerr=photon_err, label="Photons")
    # plt.xlabel("dE/dx [MeV/cm]")
    # plt.ylabel("Number of Events")
    # plt.legend()
    # plt.grid(True)

    electron_data_norm, bin_edges = np.histogram(
        electrons_df[branch_name], bins, density=True)
    photon_data_norm, bin_edges = np.histogram(
        photons_df[branch_name], bins, density=True)

    electron_err_norm = []
    for e_val, frac_err in zip(electron_data_norm, electron_frac_err):
        electron_err_norm.append(e_val*frac_err)

    photon_err_norm = []
    for p_val, frac_err in zip(photon_data_norm, photon_frac_err):
        photon_err_norm.append(p_val*frac_err)

    f2, ax2 = plt.subplots(figsize=(15, 8))
    ax2.set_title("Electron Photon Separation",fontsize=30)
    ax2.errorbar(x[:-1], electron_data_norm, xerr=0.25*0.5,
                 yerr=electron_err_norm, label="Electrons")
    ax2.errorbar(x[:-1], photon_data_norm, xerr=0.25*0.5,
                 yerr=photon_err_norm, label="Photons")
    plt.xlabel("dE/dx [MeV/cm]",fontsize=20)
    plt.ylabel("Unit Normalized",fontsize=20)
    
    for tick in ax2.xaxis.get_major_ticks():
        tick.label.set_fontsize(20)
    for tick in ax2.yaxis.get_major_ticks():
        tick.label.set_fontsize(20)

    # Now draw a line at the place that gives the best dEdx cut:

    point, purity, tup = optimizeCut(
        electronDataFrame, photonDataFrame, branch_name)
    # print "c_charge_dedx_box_LMA @ {}: \n\tElectrons: {} +/- {}\n\tPhotons: {} +/- {}\n\t{}".format(
    #   point,
    #   tup[0][0],tup[0][1],
    #   tup[1][0],tup[1][1],
    #   purity)

    dEdxLine_y = np.linspace(0, 1, 10)
    dEdxLine_x = [point]*10
    plt.plot(dEdxLine_x, dEdxLine_y, linewidth=2,linestyle="--",label="Cut at {} MeV/cm".format(point))
    plt.text(6.05,0.67,"Electron Efficiency:",size=20,)
    plt.text(9.95,0.67,"{:.2} +/- {:.2}".format(tup[0][0],tup[0][1]),size=20,horizontalalignment='right')
    plt.text(6.05,0.61,"Photon Mis. ID:",size=20,)
    plt.text(9.95,0.61,"{:.2} +/- {:.2}".format(tup[1][0],tup[1][1]),size=20,horizontalalignment='right')
    if info is not None:
        plt.text(6.05,0.405,info,size=18)

    plt.legend()
    plt.grid(True)
    plt.show()
    # plt.savefig("/data_linux/dedx_plots/1D/dedx_1D_"+branch_name+".png")
    plt.close(f2)

def dEdxCorrelation(electrons_df, photons_df, branch_name_x, branch_name_y, cut_dist=-1):

    binwidth = 0.5
    bins = np.arange(0, 10 + binwidth, binwidth)
    electron_data_x, bin_edges = np.histogram(
        electrons_df[branch_name_x], bins)
    electron_data_y, bin_edges = np.histogram(
        electrons_df[branch_name_y], bins)
    photon_data_x, bin_edges = np.histogram(photons_df[branch_name_x], bins)
    photon_data_y, bin_edges = np.histogram(photons_df[branch_name_y], bins)

    electron_frac_err_x = []
    electron_frac_err_y = []
    photon_frac_err_x = []
    photon_frac_err_y = []
    for e_val, p_val in zip(electron_data_x, photon_data_x):
        if e_val != 0:
            electron_frac_err_x.append(math.sqrt(e_val) / e_val)
        else:
            electron_frac_err_x.append(0.0)
        if p_val != 0:
            photon_frac_err_x.append(math.sqrt(p_val) / p_val)
        else:
            photon_frac_err_x.append(0.0)
    for e_val, p_val in zip(electron_data_y, photon_data_y):
        if e_val != 0:
            electron_frac_err_y.append(math.sqrt(e_val) / e_val)
        else:
            electron_frac_err_y.append(0.0)
        if p_val != 0:
            photon_frac_err_y.append(math.sqrt(p_val) / p_val)
        else:
            photon_frac_err_y.append(0.0)

    x = bin_edges + 0.25

    electron_data_norm_x, bin_edges = np.histogram(
        electrons_df[branch_name_x], bins, density=True)
    electron_data_norm_y, bin_edges = np.histogram(
        electrons_df[branch_name_y], bins, density=True)
    photon_data_norm_x, bin_edges = np.histogram(
        photons_df[branch_name_x], bins, density=True)
    photon_data_norm_y, bin_edges = np.histogram(
        photons_df[branch_name_y], bins, density=True)

    electron_err_norm_x = []
    electron_err_norm_y = []
    for e_val, frac_err in zip(electron_data_norm_x, electron_frac_err_x):
        electron_err_norm_x.append(e_val*frac_err)
    for e_val, frac_err in zip(electron_data_norm_y, electron_frac_err_y):
        electron_err_norm_y.append(e_val*frac_err)

    photon_err_norm_x = []
    photon_err_norm_y = []
    for p_val, frac_err in zip(photon_data_norm_x, photon_frac_err_x):
        photon_err_norm_x.append(p_val*frac_err)
    for p_val, frac_err in zip(photon_data_norm_y, photon_frac_err_y):
        photon_err_norm_y.append(p_val*frac_err)

    # If cut_val is not -1, it means a cut on abs(ind - col) was made
    # Display this with a box on the plots.

    # definitions for the axes
    left, width = 0.1, 0.65
    bottom, height = 0.1, 0.65
    bottom_h = left_h = left + width + 0.02

    rect_scatter = [left, bottom, width, height]
    rect_histx = [left, bottom_h, width, 0.2]
    rect_histy = [left_h, bottom, 0.2, height]

    # start with a rectangular Figure
    plt.figure(1, figsize=(8, 8))

    axScatter = plt.axes(rect_scatter)
    plt.xlabel("dE/dx [MeV/cm], {l}".format(l=branch_name_x))
    plt.ylabel("dE/dx [MeV/cm], {l}".format(l=branch_name_y))

    axHistx = plt.axes(rect_histx)
    axHisty = plt.axes(rect_histy)

    nullfmt = NullFormatter()         # no labels

    # no labels
    axHistx.xaxis.set_major_formatter(nullfmt)
    axHistx.yaxis.set_major_formatter(nullfmt)
    axHisty.xaxis.set_major_formatter(nullfmt)
    axHisty.yaxis.set_major_formatter(nullfmt)

    # the scatter plot:
    axScatter.scatter(electrons_df[branch_name_x],
                      electrons_df[branch_name_y],
                      marker="o",
                      label="Electrons", c="blue")

    axScatter.scatter(photons_df[branch_name_x],
                      photons_df[branch_name_y],
                      marker="x",
                      label="Photons", c="green")

    # Set limits for dE/dx values
    axScatter.set_xlim((0, 10))
    axScatter.set_ylim((0, 10))

    axHistx.set_xlim(axScatter.get_xlim())
    axHisty.set_ylim(axScatter.get_ylim())

    axHistx.grid(True)

    # axHistx.set_title("Electron Photon separation, normalized")
    axHistx.errorbar(x[:-1], electron_data_norm_x, xerr=binwidth*0.5,
                     yerr=electron_err_norm_x, label="Electrons")
    axHistx.errorbar(x[:-1], photon_data_norm_x, xerr=binwidth*0.5,
                     yerr=photon_err_norm_x, label="Photons")

    axHisty.grid(True)
    # axHisty.set_title("Electron Photon separation, normalized")
    axHisty.errorbar(electron_data_norm_y, x[:-1], yerr=binwidth*0.5,
                     xerr=electron_err_norm_y, label="Electrons")
    axHisty.errorbar(photon_data_norm_y, x[:-1], yerr=binwidth*0.5,
                     xerr=photon_err_norm_y, label="Photons")
    # plt.legend()
    plt.grid(True)

    plt.show()


def makeLandau(df,measure,recomb,title):

    c_hits = []
    i_hits = []

    name = measure + "_dedx_" + recomb

    # Need to dig out all the hit information from each event:
    for index, row in df.iterrows():
        i_cand_hits = row["i_"+name]
        c_cand_hits = row["c_"+name]
        dists = row['c_dist_from_start']
        for hit, dist in zip(c_cand_hits, dists):
            if dist > 0 and dist < 3.5:
                c_hits.append(hit)
        dists = row['i_dist_from_start']
        for hit, dist in zip(i_cand_hits, dists):
            if dist > 0 and dist < 3.5:
                i_hits.append(hit)

    bins = np.linspace(0, 10, 21)
    c_data, bin_edges = np.histogram(c_hits, bins)
    i_data, bin_edges = np.histogram(i_hits, bins)
    c_err = []
    i_err = []
    for point in c_data:
        try:
            c_err.append(1.0/math.sqrt(point))
        except:
            c_err.append(0.0)
    for point in i_data:
        try: 
            i_err.append(1.0/math.sqrt(point))
        except:
            i_err.append(0.0)

    c_data, bin_edges = np.histogram(c_hits, bins,density=True)
    i_data, bin_edges = np.histogram(i_hits, bins,density=True)

    c_err *= c_data
    i_err *= i_data

    x = bin_edges + 0.25

    f, ax = plt.subplots()

    ax.errorbar(x[:-1], c_data, yerr=c_err, xerr=0.25, label="Collection Hits")
    ax.errorbar(x[:-1], i_data, yerr=i_err, xerr=0.25, label="Induction Hits")
    ax.set_title(title)

    plt.xlabel("dE/dx [MeV/cm]")
    plt.ylabel("")
    plt.legend()
    plt.grid(True)
    # plt.show()
    if "Electron" in title:
        plt.savefig("/data_linux/dedx_plots/Landau/landau_electrons_"+name+".png")
    if "Photon" in title:
        plt.savefig("/data_linux/dedx_plots/Landau/landau_photons_"+name+".png")
    plt.close(f)

def selectElectronEvents(df):

    dropIndexes = []
    for index, row in df.iterrows():
        _pass = False
        if row['run'] in electrons:
            if row['event'] in electrons[row['run']]:
                _pass = True
        # Pop out this row if false
        if _pass is False:
            # print "Dropping {r},{e}, index{i}".format(
            #       i=index,r=row['run'],e=row['event'])
            dropIndexes.append(index)

    df.drop(df.index[dropIndexes], inplace=True)


def selectPhotonEvents(df):

    dropIndexes = []
    for index, row in df.iterrows():
        _pass = False
        if row['run'] in photons:
            if row['event'] in photons[row['run']]:
                _pass = True
        # Pop out this row if false
        if _pass is False:
            # print "Dropping {r},{e}, index{i}".format(
                  # i=index,r=row['run'],e=row['event'])
            dropIndexes.append(index)

    df.drop(df.index[dropIndexes], inplace=True)


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
        if purity > max_purity and tup[0][0] > 0.5:
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

    # electron_stat_err =


if __name__ == '__main__':
    print "Making dE/dx plots."

    wire_calibrations = readWireCalibrations()
    readElectrons()
    readPhotons()

    larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

    # Need the files to work on, which are basic root ttrees

    # electronFiles = ["anatrees/anu_electrons_anatree.root",
    #                  "anatrees/nu_electrons_anatree.root",
    #                  "anatrees/misc_bootleg_anatree.root"]
    # photonFiles = ["anatrees/misc_bootleg_anatree.root",
    #                # "anatrees/nu_pi0_anatree.root",
    #                # "anatrees/anu_pi0_anatree.root",
    #                "anatrees/nu_photons_anatree.root"
    #                ]
    electronFiles = ["anatrees/electrons_anatree.root"]
    photonFiles = ["anatrees/photons_anatree.root"]
    # photonFiles = ["anatrees/nu_pi0_anatree.root",
    #                "anatrees/anu_pi0_anatree.root"]

    electronDataFrame = None
    for ef in electronFiles:
        if electronDataFrame is None:
            electronDataFrame = pd.DataFrame(root2array(ef))
        else:
            electronDataFrame = electronDataFrame.append(
                pd.DataFrame(root2array(ef)), ignore_index=True)

    selectElectronEvents(electronDataFrame)
    # electronDataFrame.info()

    photonDataFrame = None
    for pf in photonFiles:
        if photonDataFrame is None:
            photonDataFrame = pd.DataFrame(root2array(pf))
        else:
            photonDataFrame = photonDataFrame.append(
                pd.DataFrame(root2array(pf)), ignore_index=True)

    selectPhotonEvents(photonDataFrame)

    # This function does dE/dx calculations:
    electronDataFrame = calcdEdx(electronDataFrame)
    photonDataFrame = calcdEdx(photonDataFrame)

    print "Analysis finished, making plots."

    # This function will loop through the events and draw dEdx vs wire:

    # loopThroughEvents(electronDataFrame,20)
    # loopThroughEvents(photonDataFrame,20)

    # # This function will make the dE/dx plot for electrons and photons:
    # plotdEdx(electronDataFrame, photonDataFrame, "i_charge_dedx_box_LMA")
    # plotdEdx(electronDataFrame, photonDataFrame, "i_charge_dedx_box_median")
    # plotdEdx(electronDataFrame, photonDataFrame,
    #          "i_charge_dedx_box_mean_no_outliers")
    # plotdEdx(electronDataFrame, photonDataFrame, "c_charge_dedx_box_LMA")
    # plotdEdx(electronDataFrame, photonDataFrame, "c_charge_dedx_box_median")
    # plotdEdx(electronDataFrame, photonDataFrame,
    #          "c_charge_dedx_box_mean_no_outliers")

    # Make the 1D plots
    
    # electronDataFrame.info()

    # print electronDataFrame.info()
    # print photonDataFrame.info()

    if False:
        for plane in planes:
            for measure in measures:
                for recomb in recombs:
                    info=""
                    if plane is "c":
                        info = "Collection Plane\n"
                    else:
                        info = "Induction Plane\n"
                    if measure is "charge":
                        info += "Area, "
                    else:
                        info += "Amp., "
                    if recomb is "birks":
                        info += "Birks"
                    elif recomb is "box":
                        info += "Box"
                    else:
                        info += "Const. Recomb."


                    base = "{}_{}_dedx_{}_".format(plane,measure,recomb)
                    # LMA doesn't get a dist:
                    plotdEdx(electronDataFrame,photonDataFrame,base+"LMA",info+"\nL.M.A.")
                    for dist in dists:
                        plotdEdx(electronDataFrame,
                                photonDataFrame,
                                base+"median_{}".format(dist),
                                info+"\nMedian, {}cm".format(dist))
                        # plotdEdx(electronDataFrame,
                        #         photonDataFrame,
                        #         base+"mean_no_outliers_{}".format(dist),
                        #         info+"\nMod. Mean, {}cm".format(dist))



    if True:
        for measure in measures:
            for recomb in recombs:
             
                info=""
                if measure is "charge":
                    info += "Area, "
                else:
                    info += "Amp., "
                if recomb is "birks":
                    info += "Birks"
                elif recomb is "box":
                    info += "Box"
                else:
                    info += "Const. Recomb."

                title = "All Electron Hits, " + info
                makeLandau(electronDataFrame,measure,recomb,title)
                title = "All Photon Hits, " + info
                makeLandau(photonDataFrame,measure,recomb,title)

    # point, purity, tup = optimizeCut(electronDataFrame,photonDataFrame,"c_charge_dedx_box_LMA")
    # print "c_charge_dedx_box_LMA @ {}: \n\tElectrons: {} +/- {}\n\tPhotons: {} +/- {}\n\t{}".format(
    #   point,
    #   tup[0][0],tup[0][1],
    #   tup[1][0],tup[1][1],
    #   purity)
    # point, purity, tup = optimizeCut(electronDataFrame,photonDataFrame,"i_charge_dedx_box_LMA")
    # print "i_charge_dedx_box_LMA @ {}: \n\tElectrons: {} +/- {}\n\tPhotons: {} +/- {}\n\t{}".format(
    #   point,
    #   tup[0][0],tup[0][1],
    #   tup[1][0],tup[1][1],
    #   purity)
    # point, purity, tup = optimizeCut(electronDataFrame,photonDataFrame,"c_charge_dedx_box_median")
    # print "c_charge_dedx_box_median @ {}: \n\tElectrons: {} +/- {}\n\tPhotons: {} +/- {}\n\t{}".format(
    #   point,
    #   tup[0][0],tup[0][1],
    #   tup[1][0],tup[1][1],
    #   purity)
    # point, purity, tup = optimizeCut(electronDataFrame,photonDataFrame,"i_charge_dedx_box_median")
    # print "i_charge_dedx_box_median @ {}: \n\tElectrons: {} +/- {}\n\tPhotons: {} +/- {}\n\t{}".format(
    #   point,
    #   tup[0][0],tup[0][1],
    #   tup[1][0],tup[1][1],
    #   purity)
    # point, purity, tup = optimizeCut(electronDataFrame,photonDataFrame,"c_charge_dedx_box_mean_no_outliers")
    # print "c_charge_dedx_box_mean_no_outliers @ {}: \n\tElectrons: {} +/- {}\n\tPhotons: {} +/- {}\n\t{}".format(
    #   point,
    #   tup[0][0],tup[0][1],
    #   tup[1][0],tup[1][1],
    #   purity)
    # point, purity, tup = optimizeCut(electronDataFrame,photonDataFrame,"i_charge_dedx_box_mean_no_outliers")
    # print "i_charge_dedx_box_mean_no_outliers @ {}: \n\tElectrons: {} +/- {}\n\tPhotons: {} +/- {}\n\t{}".format(
    #   point,
    #   tup[0][0],tup[0][1],
    #   tup[1][0],tup[1][1],
    #   purity)

    # dEdxCorrelation(electronDataFrame,
    #                 photonDataFrame,
    #                 "c_charge_dedx_box_LMA",
    #                 "c_charge_dedx_box_mean_no_outliers")

    # dEdxCorrelation(electronDataFrame,
    #                 photonDataFrame,
    #                 "c_charge_dedx_box_median",
    #                 "c_charge_dedx_box_mean_no_outliers")

    # dEdxCorrelation(electronDataFrame,
    #                 photonDataFrame,
    #                 "c_charge_dedx_box_median",
    #                 "c_charge_dedx_box_LMA")

    # dEdxCorrelation(electronDataFrame,
    #                 photonDataFrame,
    #                 "i_charge_dedx_box_LMA",
    #                 "i_charge_dedx_box_mean_no_outliers")

    # dEdxCorrelation(electronDataFrame,
    #                 photonDataFrame,
    #                 "i_charge_dedx_box_median",
    #                 "i_charge_dedx_box_mean_no_outliers")

    # dEdxCorrelation(electronDataFrame,
    #                 photonDataFrame,
    #                 "i_charge_dedx_box_median",
    #                 "i_charge_dedx_box_LMA")

    # dEdxCorrelation(electronDataFrame,
    #                 photonDataFrame,
    #                 "c_charge_dedx_box_median",
    #                 "i_charge_dedx_box_median")

    # dEdxCorrelation(electronDataFrame,
    #                 photonDataFrame,
    #                 "c_charge_dedx_box_mean_no_outliers",
    #                 "i_charge_dedx_box_mean_no_outliers")

    # dEdxCorrelation(electronDataFrame,
    #                 photonDataFrame,
    #                 "c_charge_dedx_box_LMA",
    #                 "i_charge_dedx_box_LMA")

    # for index,row in electronDataFrameCut.iterrows():
    #   print "{},{}".format(row.run,row.event)

# i=np.linspace(1,12,12)
# pitch = dataframes[0]["c_pitch"]
# pitch_err = dataframes[0]["c_pitch_err"]
# plt.errorbar(i,pitch,yerr=pitch_err)
# plt.show()
# Here is the list of plots to make:
#
# Landau curves of dE/dx and dQ/dx (for each hit), for each plane, using both area and amp. (8 plots total, all events in one plot)
#
# Different metrics to calculate dE/dx (always implied to do this 4 time: both planes, both amp/area):
#
#   <dE/dx> - mean of all dE/dx
#   <dE/dx>_no_outliers - mean of all dE/dx with outlier hits removed
#   median(dE/dx) - median of dE/dx, using some number of hits
#   moving average - take a 3 or 4 point average of the hits (minus any crazy hits) and use the lowest moving average as calc'd dE/dx
#     -> this is Minerva's method I think.
#   Others?
#
# Also compute all of the above for dQ/dx
#
# All dE/dx plots should also be made using the Birks correction and the Box correction
#
# Once dE/dx is calculated, need to work on a cut to separate electrons
# from photons
