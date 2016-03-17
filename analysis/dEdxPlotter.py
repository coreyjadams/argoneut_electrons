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

def calcdEdx(df):

    df['c_hitcharges'] *= df["c_lifetime_corr"]
    df['c_hitpeaks'] *= df["c_lifetime_corr"]
    df['i_hitcharges'] *= df["i_lifetime_corr"]
    df['i_hitpeaks'] *= df["i_lifetime_corr"]

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

    df['c_charge_dedx_box'] = df['c_charge_dqdx'].apply(boxCorrection)
    df['i_charge_dedx_box'] = df['i_charge_dqdx'].apply(boxCorrection)
    df['c_charge_dedx_birks'] = df['c_charge_dqdx'].apply(birksCorrection)
    df['i_charge_dedx_birks'] = df['i_charge_dqdx'].apply(birksCorrection)

    df['c_charge_dedx_box_mean'] = df['c_charge_dedx_box'].apply(dEdx_mean)
    df['c_charge_dedx_birks_mean'] = df['c_charge_dedx_birks'].apply(dEdx_mean)
    df['i_charge_dedx_box_mean'] = df['i_charge_dedx_box'].apply(dEdx_mean)
    df['i_charge_dedx_birks_mean'] = df['i_charge_dedx_birks'].apply(dEdx_mean)

    df['c_charge_dedx_box_mean_no_outliers'] = df['c_charge_dedx_box'].apply(dEdx_mean_no_outliers)
    df['c_charge_dedx_birks_mean_no_outliers'] = df['c_charge_dedx_birks'].apply(dEdx_mean_no_outliers)
    df['i_charge_dedx_box_mean_no_outliers'] = df['i_charge_dedx_box'].apply(dEdx_mean_no_outliers)
    df['i_charge_dedx_birks_mean_no_outliers'] = df['i_charge_dedx_birks'].apply(dEdx_mean_no_outliers)

    df['c_charge_dedx_box_median'] = df['c_charge_dedx_box'].apply(dEdx_median)
    df['c_charge_dedx_birks_median'] = df['c_charge_dedx_birks'].apply(dEdx_median)
    df['i_charge_dedx_box_median'] = df['i_charge_dedx_box'].apply(dEdx_median)
    df['i_charge_dedx_birks_median'] = df['i_charge_dedx_birks'].apply(dEdx_median)

    df['c_charge_dedx_box_LMA'] = df['c_charge_dedx_box'].apply(dEdx_lowest_moving_average)
    df['c_charge_dedx_birks_LMA'] = df['c_charge_dedx_birks'].apply(dEdx_lowest_moving_average)
    df['i_charge_dedx_box_LMA'] = df['i_charge_dedx_box'].apply(dEdx_lowest_moving_average)
    df['i_charge_dedx_birks_LMA'] = df['i_charge_dedx_birks'].apply(dEdx_lowest_moving_average)

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

def dEdx_mean(dedx_v):
  # Just compute the mean of these points and return that
  return np.mean(dedx_v)

def dEdx_mean_no_outliers(dedx_v):
  mean = np.mean(dedx_v)
  dev = np.std(dedx_v)
  val = 0.0
  n = 0
  for dedx in dedx_v:
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

def dEdx_median(dedx_v):
  return np.median(dedx_v)


def loopThroughEvents(df):
    for index, row in df.iterrows():
        # Generate horizontal lines for electron and photon expected rates
        electrons=[2.3]*len(row['c_hitwires'])
        photons=[4.6]*len(row['c_hitwires'])
        plt.close('all')
        # Two subplots, unpack the axes array immediately
        f, (ax1, ax2) = plt.subplots(1, 2, sharey=True,figsize=[20,10])
        f.suptitle("dE/dx, Run {r}, Event {e}".format(r=row.run, e=row.event))
        ax1.set_title('Collection Plane')
        ax1.plot(row['c_hitwires'],row['c_charge_dedx_box'],marker = '+',markersize=20)
        ax1.plot(row['c_hitwires'],row['c_charge_dedx_birks'],marker = 'x',markersize=20)
        ax1.plot(row['c_hitwires'],electrons,linestyle='--')
        ax1.plot(row['c_hitwires'],photons,linestyle='--')
        plt.ylabel("dE/dx [MeV/cm]")
        f.text(0.01,0.7,"1: {}".format(row.c_charge_dedx_box_mean))
        f.text(0.01,0.65,"2: {}".format(row.c_charge_dedx_box_mean_no_outliers))
        f.text(0.01,0.6,"3: {}".format(row.c_charge_dedx_box_median))
        f.text(0.01,0.55,"4: {}".format(row.c_charge_dedx_box_LMA))

        ax1.grid(True)
        # ax2.scatter(x, y)
        ax2.set_title('Induction Plane')
        ax2.plot(row['i_hitwires'],row['i_charge_dedx_box'],marker = '+',markersize=20)
        ax2.plot(row['i_hitwires'],row['i_charge_dedx_birks'],marker = 'x',markersize=20)
        ax2.plot(row['i_hitwires'],electrons,linestyle='--')
        ax2.plot(row['i_hitwires'],photons,linestyle='--')
        ax2.grid(True)

        f.text(0.91,0.7,"1: {}".format(row.i_charge_dedx_box_mean))
        f.text(0.91,0.65,"2: {}".format(row.i_charge_dedx_box_mean_no_outliers))
        f.text(0.91,0.6,"3: {}".format(row.i_charge_dedx_box_median))
        f.text(0.91,0.55,"3: {}".format(row.i_charge_dedx_box_LMA))
        # Add some text to the plot to display the dE/dx calculated:

        x1,x2,y1,y2 = plt.axis()
        plt.axis((x1,x2,0,10))
        plt.show()
      

def plotdEdx(electrons_df,photons_df,branch_name):

  electron_data = electrons_df[branch_name]
  photon_data = electrons_df[branch_name]

  f,ax = plt.subplots()
  ax.hist(electron_data,bins=np.linspace(0,10,20),alpha=0.5)
  # ax.hist(photon_data,bins=np.linspace(0,10,20),alpha=0.5)

  plt.show()

if __name__ == '__main__':
  print "Making dE/dx plots."

  larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)


  # Need the files to work on, which are basic root ttrees
  # 
  files = ["electrons_anatree.root","photons_anatree.root"]
  # files = ["from_test_ana_you_can_remove_me.root"]

  dataframes =[]

  for f in files:
    mydf= pd.DataFrame( root2array( f ) )
    calcdEdx(mydf)
    # mydf.info()
    # mydf.hist('i_charge_dedx_mean_no_outliers',bins=np.linspace(0,10,10))
    # plt.show()
    
    # This allows to reject events where the uncertainty on the pitch is high:
    mydf = mydf.query('i_pitch_err < 0.1 and c_pitch_err < 0.1')
    

    dataframes.append(mydf)


  # This function will loop through the events and draw dEdx vs wire:
  # loopThroughEvents(dataframes[0])

  # This function will make the dE/dx plot for electrons and photons:
  plotdEdx(dataframes[0],dataframes[1],"c_charge_dedx_box_mean")


      
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
# Once dE/dx is calculated, need to work on a cut to separate electrons from photons