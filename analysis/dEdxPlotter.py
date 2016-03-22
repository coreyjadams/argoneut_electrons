import ROOT
import larlite
import math

from ROOT import larlite, larutil
from ROOT import TTree

# use matplotlib to make these plots

import matplotlib.pyplot as plt
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

    s['c_charge_dedx_box_mean_no_outliers'] = dEdx_mean_no_outliers(
        s['c_charge_dedx_box'], s['c_dist_from_start'])
    s['c_charge_dedx_birks_mean_no_outliers'] = dEdx_mean_no_outliers(
        s['c_charge_dedx_birks'], s['c_dist_from_start'])
    s['i_charge_dedx_box_mean_no_outliers'] = dEdx_mean_no_outliers(
        s['i_charge_dedx_box'], s['i_dist_from_start'])
    s['i_charge_dedx_birks_mean_no_outliers'] = dEdx_mean_no_outliers(
        s['i_charge_dedx_birks'], s['i_dist_from_start'])

    s['c_charge_dedx_box_median'] = dEdx_median(
        s['c_charge_dedx_box'], s['c_dist_from_start'])
    s['c_charge_dedx_birks_median'] = dEdx_median(
        s['c_charge_dedx_birks'], s['c_dist_from_start'])
    s['i_charge_dedx_box_median'] = dEdx_median(
        s['i_charge_dedx_box'], s['i_dist_from_start'])
    s['i_charge_dedx_birks_median'] = dEdx_median(
        s['i_charge_dedx_birks'], s['i_dist_from_start'])

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
        if d >= 0 and d < dist_cut:
            points_within_dist.append(val)

    if len(points_within_dist) == 0:
        return 0.0

    mean = np.mean(points_within_dist)
    dev = np.std(points_within_dist)
    val = 0.0
    n = 0
    for dedx in points_within_dist:
        if abs(mean-dedx) < dev and dedx < 10:
            val += dedx
            n += 1
    if n > 0:
        return val/n
    else:
        return 0.0


def dEdx_lowest_moving_average(dedx_v):
    N = 3
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
        if d >= 0 and d < dist_cut:
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
        f.text(0.01, 0.7, "1: {}".format(row['c_charge_dedx_box_mean']))
        f.text(0.01, 0.65, "2: {}".format(
            row['c_charge_dedx_box_mean_no_outliers']))
        f.text(0.01, 0.6, "3: {}".format(row['c_charge_dedx_box_median']))
        f.text(0.01, 0.55, "4: {}".format(row['c_charge_dedx_box_LMA']))

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
        f.text(0.91, 0.55, "3: {}".format(row['i_charge_dedx_box_LMA']))
        # Add some text to the plot to display the dE/dx calculated:

        x1, x2, y1, y2 = plt.axis()
        plt.axis((x1, x2, 0, 10))
        plt.show()
        it += 1
        if it >= n:
            return


def plotdEdx(electrons_df, photons_df, branch_name):

    bins = np.linspace(0, 10, 21)
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
    ax.errorbar(x[:-1], electron_data, xerr=0.25,
                yerr=electron_err, label="Electrons")
    ax.errorbar(
        x[:-1], photon_data, xerr=0.25, yerr=photon_err, label="Photons")
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
    ax2.errorbar(x[:-1], electron_data_norm, xerr=0.25,
                 yerr=electron_err_norm, label="Electrons")
    ax2.errorbar(x[:-1], photon_data_norm, xerr=0.25,
                 yerr=photon_err_norm, label="Photons")
    plt.xlabel("dE/dx [MeV/cm]")
    plt.ylabel("Arbitrary Units")
    plt.legend()
    plt.grid(True)

    plt.show()


def makeLandau(df):

    hits = []

    # Need to dig out all the hit information from each event:
    for index, row in df.iterrows():
        cand_hits = row["i_charge_dedx_box"]
        dists = row['i_dist_from_start']
        for hit, dist in zip(cand_hits, dists):
            if dist >= 0 and dist < 2.4:
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
                   "anatrees/nu_photons_anatree.root"]

    electronDataFrame = None
    for ef in electronFiles:
        if electronDataFrame is None:
          electronDataFrame = pd.DataFrame(root2array(ef))
        else:
          electronDataFrame = electronDataFrame.append(pd.DataFrame(root2array(ef)),ignore_index=True)
    
    selectElectronEvents(electronDataFrame)
    # electronDataFrame.info()


    photonDataFrame = None
    for pf in photonFiles:
        if photonDataFrame is None:
          photonDataFrame = pd.DataFrame(root2array(pf))
        else:
          photonDataFrame = photonDataFrame.append(pd.DataFrame(root2array(pf)),ignore_index=True)

    selectPhotonEvents(photonDataFrame)

    # This function does dE/dx calculations:
    electronDataFrame = calcdEdx(electronDataFrame)
    photonDataFrame = calcdEdx(photonDataFrame)

    print "dEdx calculated."

    electronDataFrame = electronDataFrame.query('i_pitch_err < 0.1 and c_pitch_err < 0.1')
    photonDataFrame = photonDataFrame.query('i_pitch_err < 0.1 and c_pitch_err < 0.1')

    print "Analysis finished, making plots."

    # This function will loop through the events and draw dEdx vs wire:

    # loopThroughEvents(electronDataFrame,3)

    # This function will make the dE/dx plot for electrons and photons:
    plotdEdx(electronDataFrame, photonDataFrame, "i_charge_dedx_box_LMA")
    plotdEdx(electronDataFrame,photonDataFrame,"i_charge_dedx_box_median")
    # plotdEdx(electronDataFrame,photonDataFrame,"i_charge_dedx_box_mean_no_outliers")
    plotdEdx(electronDataFrame, photonDataFrame, "c_charge_dedx_box_LMA")
    plotdEdx(electronDataFrame,photonDataFrame,"c_charge_dedx_box_median")
    # plotdEdx(electronDataFrame,photonDataFrame,"c_charge_dedx_box_mean_no_outliers")

    # makeLandau(dataframes[0])

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
