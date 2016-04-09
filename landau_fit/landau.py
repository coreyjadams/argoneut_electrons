from ROOT import TFile
import scipy.optimize as opt
import scipy.signal as sig
import numpy

import _landau

from matplotlib import pyplot as plt

def getNdarrayFromHist(hist):
  _x = numpy.zeros(hist.GetNbinsX())
  _y = numpy.zeros(hist.GetNbinsX())

  for i_bin in xrange(hist.GetNbinsX()):
    _x[i_bin] = hist.GetBinCenter(i_bin+1)
    _y[i_bin] = hist.GetBinContent(i_bin+1)
  return _x, _y

if __name__ == '__main__':


    _fname="/home/cadams/larlite/UserDev/argoneut_electrons/calo/from_test_ana_you_can_remove_me.root"
    f = TFile(_fname)
    hist = f.Get("_collection_caloAlg_dEdx_area")
    print hist
    x_axis,y_axis = getNdarrayFromHist(hist)

    y_axis /= numpy.sum(y_axis)
    print numpy.sum(y_axis)

    mu = 1.7
    sigma_landau = 0.09
    sigma_gauss = 0.2

    _min = x_axis[0]
    _max = x_axis[x_axis.size-1]
    stepsize = (_max - _min) / 100.0
    x = numpy.arange(_min,_max, stepsize)
    y = _landau.convlangau(x,mu,sigma_landau,sigma_gauss)
    y /= numpy.sum(y)

    # Try to fit to the y_data:

    bounds = ((0,0,0,0),(10,10,10,100))
    # bounds.append((0,10)) # MPV
    # bounds.append((0,10)) # sigma Landau
    # bounds.append((0,10)) # sigma Gauss
    # bounds.append((0,100)) # scale
    # popt, pcov = opt.curve_fit(convolvedLandau_arr,x_axis,y_axis,bounds=(0,[10,10,10,100]))

    # print popt
    # convLandau = v_convLandau(x, popt[0],popt[1],popt[2],popt[3])
    # convLandau /= numpy.sum(convLandau)


    plt.plot(x_axis, y_axis, label="Actual Data",ls="none",marker="x")
    # plt.plot(x, y, label="Landau")
    plt.plot(x, y, label="Convolved Landau")
    plt.legend()
    plt.show()
