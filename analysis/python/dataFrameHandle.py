import ROOT
import larlite
import math

# Needed for reading in calibrations
import pickle

# Always need numpy
import numpy as np

from ROOT import larlite, larutil
# from ROOT import TTree

# Do the calculations with pandas, interface to root file is root2array
import pandas
from root_numpy import root2array


class dataFrameHandle(object):

    """docstring for dataFrameHandle"""

    def __init__(self, root_file, signal_type):
        super(dataFrameHandle, self).__init__()
        self._goodEventList = dict()
        self._fileList = []
    
        self._type = signal_type
        self._root_file = root_file
    
        self._dataFrame = None
    
        self._planes = ['c', 'i']
        self._measures = ["charge"]
        self._recombs = ["box","const"]
        self._dists = [3.0]
    
        # Now initialize this stuff
        self.init()



    def init(self):
        if "electron" in self._type:
            self._fileList = ["text/nu_electrons.txt",
                              "text/anu_electrons.txt",
                              "text/misc_electrons.txt"]
        if "photon" in self._type:
            self._fileList = ["text/nu_photons.txt",
                              "text/anu_pi0.txt",
                              "text/nu_pi0.txt",
                              "text/misc_photons.txt"]

        self.readWireCalibrations()

        self._df = pandas.DataFrame(root2array(self._root_file))
  

        self._df = self._df.query("displacement < 2.0")
        self._df = self.calcdEdx(self._df)

    def make_quality_cuts(self):
        self._df = self._df.query("c_charge_dedx_const_meta_0_err/c_charge_dedx_const_meta_0 < 0.5")
        self._df = self._df.query("i_charge_dedx_const_meta_0_err/i_charge_dedx_const_meta_0 < 0.5")
        self._df = self._df.query("abs(c_charge_dedx_const_meta_0 - i_charge_dedx_const_meta_0) < 1.5")

    def get_row(self, row):
        return self._df.iloc[[row]].squeeze()

    def n_events(self):
        return len(self._df.index)

    def readEventListFile(self):

        # photons = dict()
        electronFiles = ["text/nu_electrons.txt",
                         "text/anu_electrons.txt",
                         "text/misc_electrons.txt"]
        for _f in self._fileList:
            with open(_f, "r") as f:
                for line in f:
                    vals = line.split()
                    run = int(vals[0])
                    event = int(vals[1])
                    if run in self._goodEventList:
                        if event in self._goodEventList[run]:
                            pass
                        else:
                            self._goodEventList[run].append(event)
                    else:
                        self._goodEventList.update({run: [event]})



    def readWireCalibrations(self):
        pf = open('wireByWireCorrections_gapsFilled.pkl','rb')
        self._wire_calibrations = pickle.load(pf)



    def calcdEdx(self, df):

        # df.info()

        for plane in self._planes:
            for measure in self._measures:
                df["{}_hit{}s".format(
                    plane, measure)] *= df["{}_lifetime_corr".format(plane)]

            df["{}_startwire".format(plane)] = df[
                "{}_starthit".format(plane)].apply(self.selectWire)
            if plane is "c":
                df['{}_wire_corrections'.format(plane)] = df[
                    '{}_hitwires'.format(plane)].apply(self.wireCorrC)
            if plane is 'i':
                df['{}_wire_corrections'.format(plane)] = df[
                    '{}_hitwires'.format(plane)].apply(self.wireCorrI)


            df['{}_dist_from_start'.format(plane)] = (df['{}_hitwires'.format(
                plane)] - df['{}_startwire'.format(plane)])*df['{}_pitch'.format(plane)]

            for measure in self._measures:

                df['{}_hit{}s_dq'.format(plane, measure)] = df['{}_hit{}s'.format(plane, measure)]*df['{}_wire_corrections'.format(plane)]
                # df['{}_hit{}s_q'.format(plane,measure)] =  df['{}_hit{}s_q'.format(plane,measure)]* Q_AREA
                # df['{}_hit{}s_dq'.format(plane, measure)] = df[
                    # '{}_hit{}s'.format(plane, measure)]*correction[plane][measure]
                df['{}_{}_dqdx'.format(plane, measure)] = df[
                    '{}_hit{}s_dq'.format(plane, measure)] / df['{}_pitch'.format(plane)]

        df = df.apply(self.dqdx_to_dedx_all, 1)

        df = df.apply(self.dEdxFunctions, 1)

        df = df.apply(self.dEdx_meta,1)

        return df


    def selectWire(self, hit):
        return hit[0]

    def wireCorrC(self, wires):
        vals = []
        for wire in wires:
            vals.append(self._wire_calibrations[1][wire])
        return vals


    def wireCorrI(self, wires):
        vals = []
        for wire in wires:
            vals.append(self._wire_calibrations[0][wire])
        return vals



    def dqdx_to_dedx_all(self, s):

        for plane in self._planes:
            for measure in self._measures:
                s['{}_{}_dedx_box'.format(plane, measure)] = self.boxCorrection(
                    s['{}_{}_dqdx'.format(plane, measure)])
                s['{}_{}_dedx_const'.format(plane, measure)] = self.constCorrection(
                    s['{}_{}_dqdx'.format(plane, measure)])

        return s

    def boxCorrection(self, dqdx_v):
        result = []
        for x in dqdx_v:
            result.append(larutil.LArProperties.GetME().ModBoxCorrection(x))
        return result

    def constCorrection(self, dqdx_v):
        result = []
        for x in dqdx_v:
            result.append(3.40e-05 * x)
        return result

    def birksCorrection(self, dqdx_v):
        result = []
        for x in dqdx_v:
            result.append(larutil.LArProperties.GetME().BirksCorrection(x))
        return result



    def dEdxFunctions(self, s):

            # First, cut out any hits that are at negative distance
            # print s

        for plane in self._planes:
            for measure in self._measures:
                for rec in self._recombs:
                    # s['{}_{}_dedx_{}_mean'.format(plane, measure, rec)] = self.dEdx_mean(
                        # s['{}_{}_dedx_{}'.format(plane, measure, rec)])

                    s['{}_{}_dedx_{}_LMA'.format(plane, measure, rec)] = self.dEdx_lowest_moving_average(
                        s['{}_{}_dedx_{}'.format(plane, measure, rec)])

                    for i in xrange(len(self._dists)):
                        dist = self._dists[i]
                        s['{}_{}_dedx_{}_mean_no_outliers_{}'.format(
                            plane, measure, rec, i)] = self.dEdx_mean_no_outliers(
                                s['{}_{}_dedx_{}'.format(plane, measure, rec)],
                                s['{}_dist_from_start'.format(plane)], dist)
                        s['{}_{}_dedx_{}_median_{}'.format(
                            plane, measure, rec, i)] = self.dEdx_median(
                                s['{}_{}_dedx_{}'.format(plane, measure, rec)],
                                s['{}_dist_from_start'.format(plane)], dist)

        return s

    def dEdx_meta(self, s):
        # This function looks at the induction and collection plane measures of the dE/dx and calculates 
        for plane in self._planes:
            for measure in self._measures:
                for rec in self._recombs:
                    for i in xrange(len(self._dists)):
                        dist = self._dists[i] 
                        dedx_v = []
                        dedx_v.append(s["{}_{}_dedx_{}_mean_no_outliers_{}".format(plane, measure, rec,i)])
                        dedx_v.append(s["{}_{}_dedx_{}_median_{}".format(plane, measure, rec,i)])
                        dedx_v.append(s["{}_{}_dedx_{}_LMA".format(plane, measure, rec)])
                        s["{}_{}_dedx_{}_meta_{}".format(plane, measure, rec,i)] = np.mean(dedx_v)
                        s["{}_{}_dedx_{}_meta_{}_err".format(plane, measure, rec,i)] = np.std(dedx_v)



        return s

    def dEdx_mean_no_outliers(self, dedx_v, dist_v, dist_cut=2.4, startindex=1):

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


    def dEdx_lowest_moving_average(self, dedx_v, N=3, startindex=1):
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


    def dEdx_median(self, dedx_v, dist_v, dist_cut=2.4, startindex=1):

        # Pick out values with dist >= 0, < dist_cut
        points_within_dist = []
        for val, d in zip(dedx_v[startindex:], dist_v[startindex:]):
            if d > 0 and d < dist_cut:
                points_within_dist.append(val)

        if len(points_within_dist) == 0:
            return 0.0

        return np.median(points_within_dist)
