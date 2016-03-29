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

    df['c_hitcharges'] *= df["c_lifetime_corr"]
    df['c_hitpeaks'] *= df["c_lifetime_corr"]
    df['i_hitcharges'] *= df["i_lifetime_corr"]
    df['i_hitpeaks'] *= df["i_lifetime_corr"]

    df["c_startwire"] = df["c_starthit"].apply(selectWire)
    df["i_startwire"] = df["i_starthit"].apply(selectWire)

    df['c_wire_corrections'] = df['c_hitwires'].apply(wireCorrC)
    df['i_wire_corrections'] = df['i_hitwires'].apply(wireCorrI)

    df['c_dist_from_start'] = (
        df['c_hitwires'] - df['c_startwire'])*df['c_pitch']
    df['i_dist_from_start'] = (
        df['i_hitwires'] - df['i_startwire'])*df['i_pitch']

    df['c_hitcharges'] *= df['c_wire_corrections']
    df['i_hitcharges'] *= df['i_wire_corrections']

    df['c_hitcharges_q'] = df['c_hitcharges'] * Q_AREA[1]
    df['c_hitpeaks_q'] = df['c_hitpeaks'] * Q_AMP[1]
    df['i_hitcharges_q'] = df['i_hitcharges'] * Q_AREA[0]
    df['i_hitpeaks_q'] = df['i_hitpeaks'] * Q_AMP[0]
    df['c_hitcharges_dq'] = df['c_hitcharges_q'] * E_AREA[1]
    df['c_hitpeaks_dq'] = df['c_hitpeaks_q'] * E_AMP[1]
    df['i_hitcharges_dq'] = df['i_hitcharges_q'] * E_AREA[0]
    df['i_hitpeaks_dq'] = df['i_hitpeaks_q'] * E_AMP[0]
    df['c_charge_dqdx'] = df['c_hitcharges_dq'] / df['c_pitch']
    df['c_peak_dqdx'] = df['c_hitpeaks_dq'] / df['c_pitch']
    df['i_charge_dqdx'] = df['i_hitcharges_dq'] / df['i_pitch']
    df['i_peak_dqdx'] = df['i_hitpeaks_dq'] / df['i_pitch']

    # df['c_charge_dedx_box'] = df['c_charge_dqdx'].apply(boxCorrection)
    # df['i_charge_dedx_box'] = df['i_charge_dqdx'].apply(boxCorrection)
    # df['c_charge_dedx_birks'] = df['c_charge_dqdx'].apply(birksCorrection)
    # df['i_charge_dedx_birks'] = df['i_charge_dqdx'].apply(birksCorrection)

    df = df.apply(dqdx_to_dedx_all, 1)

    df = df.apply(dEdxFunctions, 1)

    return df


def dqdx_to_dedx_all(s):

    s['c_charge_dedx_box'] = boxCorrection(s['c_charge_dqdx'])
    s['i_charge_dedx_box'] = boxCorrection(s['i_charge_dqdx'])
    s['c_charge_dedx_birks'] = birksCorrection(s['c_charge_dqdx'])
    s['i_charge_dedx_birks'] = birksCorrection(s['i_charge_dqdx'])

    return s


def boxCorrection(dqdx_v):
    result = []
    for x in dqdx_v:
        result.append(larutil.LArProperties.GetME().ModBoxCorrection(x))
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

    s['c_charge_dedx_box_mean'] = dEdx_mean(s['c_charge_dedx_box'])
    s['c_charge_dedx_birks_mean'] = dEdx_mean(s['c_charge_dedx_birks'])
    s['i_charge_dedx_box_mean'] = dEdx_mean(s['i_charge_dedx_box'])
    s['i_charge_dedx_birks_mean'] = dEdx_mean(s['i_charge_dedx_birks'])

    cut=1.6
    s['c_charge_dedx_box_mean_no_outliers'] = dEdx_mean_no_outliers(
        s['c_charge_dedx_box'], s['c_dist_from_start'],cut)
    s['c_charge_dedx_birks_mean_no_outliers'] = dEdx_mean_no_outliers(
        s['c_charge_dedx_birks'], s['c_dist_from_start'],cut)
    s['i_charge_dedx_box_mean_no_outliers'] = dEdx_mean_no_outliers(
        s['i_charge_dedx_box'], s['i_dist_from_start'],cut)
    s['i_charge_dedx_birks_mean_no_outliers'] = dEdx_mean_no_outliers(
        s['i_charge_dedx_birks'], s['i_dist_from_start'],cut)

    s['c_charge_dedx_box_median'] = dEdx_median(
        s['c_charge_dedx_box'], s['c_dist_from_start'],cut)
    s['c_charge_dedx_birks_median'] = dEdx_median(
        s['c_charge_dedx_birks'], s['c_dist_from_start'],cut)
    s['i_charge_dedx_box_median'] = dEdx_median(
        s['i_charge_dedx_box'], s['i_dist_from_start'],cut)
    s['i_charge_dedx_birks_median'] = dEdx_median(
        s['i_charge_dedx_birks'], s['i_dist_from_start'],cut)

    s['c_charge_dedx_box_LMA'] = dEdx_lowest_moving_average(
        s['c_charge_dedx_box'])
    s['c_charge_dedx_birks_LMA'] = dEdx_lowest_moving_average(
        s['c_charge_dedx_birks'])
    s['i_charge_dedx_box_LMA'] = dEdx_lowest_moving_average(
        s['i_charge_dedx_box'])
    s['i_charge_dedx_birks_LMA'] = dEdx_lowest_moving_average(
        s['i_charge_dedx_birks'])

    return s


def dEdx_mean_no_outliers(dedx_v, dist_v, dist_cut=2.4):

    # Pick out values with dist >= 0, < dist_cut
    points_within_dist = []
    for val, d in zip(dedx_v, dist_v):
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


def dEdx_lowest_moving_average(dedx_v, N=5):
    vals = []
    for i in xrange(len(dedx_v) - N):
        vals.append(0.0)
        for v in xrange(N):
            vals[-1] += dedx_v[i+v]
        vals[-1] /= N
    if len(vals) > 0:
        return min(vals)
    else:
        return 0


def dEdx_median(dedx_v, dist_v, dist_cut=2.4):

    # Pick out values with dist >= 0, < dist_cut
    points_within_dist = []
    for val, d in zip(dedx_v, dist_v):
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


def plotdEdx(electrons_df, photons_df, branch_name):

    bins = np.linspace(0, 10, 41)
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

    x = bin_edges + 0.25

    f, ax = plt.subplots()
    ax.set_title("Electron Photon separation, {s}".format(s=branch_name))
    # ax.hist(electron_data,bins=np.linspace(0,10,20),alpha=0.5)
    ax.errorbar(x[:-1], electron_data, xerr=0.25*0.5,
                yerr=electron_err, label="Electrons")
    ax.errorbar(
        x[:-1], photon_data, xerr=0.25*0.5, yerr=photon_err, label="Photons")
    plt.xlabel("dE/dx [MeV/cm]")
    plt.ylabel("Number of Events")
    plt.legend()
    plt.grid(True)

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

    f2, ax2 = plt.subplots()
    ax2.set_title("Electron Photon separation, unit normalized")
    ax2.errorbar(x[:-1], electron_data_norm, xerr=0.25*0.5,
                 yerr=electron_err_norm, label="Electrons")
    ax2.errorbar(x[:-1], photon_data_norm, xerr=0.25*0.5,
                 yerr=photon_err_norm, label="Photons")
    plt.xlabel("dE/dx [MeV/cm]")
    plt.ylabel("Arbitrary Units")
    plt.legend()
    plt.grid(True)

    plt.show()


def dEdxCorrelation(electrons_df, photons_df, branch_name_x, branch_name_y,cut_dist=-1):


    binwidth = 0.5
    bins = np.arange(0, 10 + binwidth, binwidth)
    electron_data_x, bin_edges = np.histogram(electrons_df[branch_name_x], bins)
    electron_data_y, bin_edges = np.histogram(electrons_df[branch_name_y], bins)
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

#############


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
                      label="Electrons",c="blue")

    axScatter.scatter(photons_df[branch_name_x],
                      photons_df[branch_name_y],
                      marker="x",
                      label="Photons",c="green")


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
    axHisty.errorbar(electron_data_norm_y,x[:-1], yerr=binwidth*0.5,
                 xerr=electron_err_norm_y, label="Electrons")
    axHisty.errorbar(photon_data_norm_y,x[:-1], yerr=binwidth*0.5,
                 xerr=photon_err_norm_y, label="Photons")
    # plt.legend()
    plt.grid(True)


    plt.show()


def makeLandau(df):

    hits = []

    # Need to dig out all the hit information from each event:
    for index, row in df.iterrows():
        cand_hits = row["c_charge_dedx_box"]
        dists = row['c_dist_from_start']
        for hit, dist in zip(cand_hits, dists):
            if dist > 0 and dist < 2.4:
                hits.append(hit)

    bins = np.linspace(0, 10, 21)
    electron_data, bin_edges = np.histogram(hits, bins)

    x = bin_edges + 0.25

    f, ax = plt.subplots()

    ax.plot(x[:-1], electron_data, label="Electron Hits")
    ax.set_title("Electron Hits")

    plt.xlabel("dE/dx [MeV/cm]")
    plt.ylabel("All Hits")
    plt.legend()
    plt.grid(True)
    plt.show()


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

def optimizeCut(electron_df,photon_df,branch_name):

  # Basically, loop through and find the value that optimizes the ratio of electrons to photons:
  # Setting lower and upper limits just to stop the edge effects:
  lower_limit = 0.0
  upper_limit = 10

  # Here's the plan.  take a value, x, and a step alpha.
  # Get the eff at x, x - alpha and x + alpha.
  # Set x to the one with the best eff.  If it's x, decrease alpha.
  # 
  # When alpha is small, stop.
  # 
  # To ensure it doesn't hit a local max, start by scanning over a few points
  # and use the max value.
  points = np.arange(lower_limit,upper_limit,0.25)
  vals = []
  for point in points:
    purity, tup = makeCut(electron_df,photon_df,branch_name,point)
    # vals.append(makeCut(electron_df,photon_df,branch_name,point))
    print "{}: \n\tElectrons: {} +/- {}\n\tPhotons: {} +/- {}".format(
      point,
      tup[0][0],tup[0][1],
      tup[1][0],tup[1][1])
    print "\tPurity: {}".format(purity)




def makeCut(electron_df, photon_df, branch_name, cut_value):
  """
  This function will make a cut at the value specified on the branch_name specified
  It calculates the percent of each sample left on each side, and returns ???
  """

  bins = [0, cut_value, 10]

  # First, get the arrays of data:
  electrons = electron_df[branch_name].values
  photons = photon_df[branch_name].values

  # Use a histogram to slice this into two sections, one above the cut value and one below
  electrons, junk = np.histogram(electron_df[branch_name], bins)
  photons, junk = np.histogram(photon_df[branch_name], bins)

  electron_eff = (1.0*electrons[0])/(electrons[0] + electrons[1])
  photon_eff = (1.0*photons[0])/(photons[0] + photons[1])

  try:
    purity = 1.0*electrons[0] / (electrons[0] + photons[0])
  except:
    purity = 0.0

  try:
    electron_eff_stat_err = electron_eff * math.sqrt(1.0/electrons[0] + 1.0/(electrons[0] + electrons[1]))
  except:
    electron_eff_stat_err = 999

  try:
    photon_eff_stat_err = photon_eff * math.sqrt(1.0/photons[0] + 1.0/(photons[0] + photons[1]))
  except:
    photon_eff_stat_err = 999


  try:
    opt = electron_eff / photon_eff
  except:
    opt = 0.0

  try:
    opt_err = opt * math.sqrt( electron_eff_stat_err/electron_eff + photon_eff_stat_err/ photon_eff)
  except:
    opt_err = 999

  return purity, [[electron_eff, electron_eff_stat_err],[photon_eff, photon_eff_stat_err]]  

  # electron_stat_err = 


if __name__ == '__main__':
    print "Making dE/dx plots."

    wire_calibrations = readWireCalibrations()
    readElectrons()
    readPhotons()

    larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

    # Need the files to work on, which are basic root ttrees
    #

    electronFiles = ["anatrees/anu_electrons_anatree.root",
                     "anatrees/nu_electrons_anatree.root",
                     "anatrees/misc_bootleg_anatree.root"]
    photonFiles = ["anatrees/misc_bootleg_anatree.root",
                   # "anatrees/nu_pi0_anatree.root",
                   # "anatrees/anu_pi0_anatree.root",
                   "anatrees/nu_photons_anatree.root"
                   ]
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

    print "dEdx calculated."

    electronDataFrame = electronDataFrame.query(
        'i_pitch_err < 0.1 or c_pitch_err < 0.1')
    photonDataFrame = photonDataFrame.query(
        'i_pitch_err < 0.1 or c_pitch_err < 0.1')

    # electronDataFrameCut = electronDataFrame.query("abs(c_charge_dedx_box_LMA - i_charge_dedx_box_LMA) > 1")
    # photonDataFrameCut = photonDataFrame.query("abs(c_charge_dedx_box_LMA - i_charge_dedx_box_LMA) > 1")



    # electronDataFrame = electronDataFrame.query("abs(c_charge_dedx_box_LMA - i_charge_dedx_box_LMA) < 1")
    # photonDataFrame = photonDataFrame.query("abs(c_charge_dedx_box_LMA - i_charge_dedx_box_LMA) < 1")


    # electronDataFrame = electronDataFrame.query("abs(c_charge_dedx_box_mean_no_outliers - i_charge_dedx_box_mean_no_outliers) < 1")
    # photonDataFrame = photonDataFrame.query("abs(c_charge_dedx_box_mean_no_outliers - i_charge_dedx_box_mean_no_outliers) < 1")

    print "Analysis finished, making plots."

    # This function will loop through the events and draw dEdx vs wire:

    # loopThroughEvents(electronDataFrameCut,20)
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

    # makeLandau(electronDataFrame)
    # makeLandau(photonDataFrame)

    # print optimizeCut(electronDataFrame,photonDataFrame,"c_charge_dedx_box_LMA")
    # print optimizeCut(electronDataFrame,photonDataFrame,"i_charge_dedx_box_LMA")
    # print optimizeCut(electronDataFrame,photonDataFrame,"c_charge_dedx_box_median")
    # print optimizeCut(electronDataFrame,photonDataFrame,"i_charge_dedx_box_median")
    # print optimizeCut(electronDataFrame,photonDataFrame,"c_charge_dedx_box_mean_no_outliers")
    # print optimizeCut(electronDataFrame,photonDataFrame,"i_charge_dedx_box_mean_no_outliers")
    # dEdxCorrelation(electronDataFrameCut,
    #                 photonDataFrameCut,
    #                 "c_charge_dedx_box_LMA",
    #                 "c_charge_dedx_box_mean_no_outliers")

    # dEdxCorrelation(electronDataFrameCut,
    #                 photonDataFrameCut,
    #                 "c_charge_dedx_box_median",
    #                 "c_charge_dedx_box_mean_no_outliers")

    # dEdxCorrelation(electronDataFrameCut,
    #                 photonDataFrameCut,
    #                 "c_charge_dedx_box_median",
    #                 "c_charge_dedx_box_LMA")

    # dEdxCorrelation(electronDataFrameCut,
    #                 photonDataFrameCut,
    #                 "i_charge_dedx_box_LMA",
    #                 "i_charge_dedx_box_mean_no_outliers")

    # dEdxCorrelation(electronDataFrameCut,
    #                 photonDataFrameCut,
    #                 "i_charge_dedx_box_median",
    #                 "i_charge_dedx_box_mean_no_outliers")

    # dEdxCorrelation(electronDataFrameCut,
    #                 photonDataFrameCut,
    #                 "i_charge_dedx_box_median",
    #                 "i_charge_dedx_box_LMA")

    dEdxCorrelation(electronDataFrame,
                    photonDataFrame,
                    "c_charge_dedx_box_median",
                    "i_charge_dedx_box_median")

    dEdxCorrelation(electronDataFrame,
                    photonDataFrame,
                    "c_charge_dedx_box_mean_no_outliers",
                    "i_charge_dedx_box_mean_no_outliers")

    dEdxCorrelation(electronDataFrame,
                    photonDataFrame,
                    "c_charge_dedx_box_LMA",
                    "i_charge_dedx_box_LMA")

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
