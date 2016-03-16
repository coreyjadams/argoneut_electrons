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


Q_AREA =  [0.8152236862020928, 0.8078780681936417]
Q_AMP =   [9.207991314407435, 18.562195730195935]
E_AREA =  [185.00584273133333, 44.90267652367623]
E_AMP =   [184.9556285310639, 45.1669419567248]


def boxCorrection(dqdx_v):
    result = []
    for x in dqdx_v:
        result.append(larutil.LArProperties.GetME().ModBoxCorrection(x))
    return result

def birksCorrection(dqdx):
    result = []
    for x in dqdx_v:
        result.append(larutil.LArProperties.GetME().BirksCorrection(x))
    return result


if __name__ == '__main__':
  print "Making dE/dx plots."

  larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)


  # Need the files to work on, which are basic root ttrees
  # 
  files = ["nu_electrons_anatree.root"]


  for f in files:
    mydf= pd.DataFrame( root2array( f ) )

    mydf['c_hitcharges'] *= mydf["c_lifetime_corr"]
    mydf['c_hitpeaks'] *= mydf["c_lifetime_corr"]
    mydf['i_hitcharges'] *= mydf["i_lifetime_corr"]
    mydf['i_hitpeaks'] *= mydf["i_lifetime_corr"]

    mydf['c_hitcharges_q'] = mydf['c_hitcharges'] * Q_AREA[1]
    mydf['c_hitpeaks_q'] = mydf['c_hitpeaks'] * Q_AMP[1]
    mydf['i_hitcharges_q'] = mydf['i_hitcharges'] * Q_AREA[0]
    mydf['i_hitpeaks_q'] = mydf['i_hitpeaks'] * Q_AMP[0]
    mydf['c_hitcharges_dq'] = mydf['c_hitcharges_q'] * E_AREA[1]
    mydf['c_hitpeaks_dq'] = mydf['c_hitpeaks_q'] * E_AMP[1]
    mydf['i_hitcharges_dq'] = mydf['i_hitcharges_q'] * E_AREA[0]
    mydf['i_hitpeaks_dq'] = mydf['i_hitpeaks_q'] * E_AMP[0]
    mydf['c_charge_dqdx'] = mydf['c_hitcharges_dq'] / mydf['c_pitch']
    mydf['c_peak_dqdx'] = mydf['c_hitpeaks_dq'] / mydf['c_pitch']
    mydf['i_charge_dqdx'] = mydf['i_hitcharges_dq'] / mydf['i_pitch']
    mydf['i_peak_dqdx'] = mydf['i_hitpeaks_dq'] / mydf['c_pitch']

    mydf['c_charge_dedx'] = mydf['c_charge_dqdx'].apply(boxCorrection)

    for index, row in mydf.iterrows():
        plt.plot( row['c_hitwires'], row['c_charge_dedx'])
        plt.show()
    # plt.show()
    # a = mydf.loc[0]
    # print type(a)
    # print a
    # a['q_cal_collection_hitcharges'] = a.collection_hitcharges * Q_AREA[1]
    # print a