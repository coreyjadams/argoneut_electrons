from ROOT import TFile, TH1F
import landau 
import scipy.optimize as opt
import numpy

from matplotlib import pyplot as plt


def getNdarrayFromHist(hist):
  _x = numpy.zeros(hist.GetNbinsX())
  _y = numpy.zeros(hist.GetNbinsX())

  for i_bin in xrange(hist.GetNbinsX()):
    _x[i_bin] = hist.GetBinCenter(i_bin+1)
    _y[i_bin] = hist.GetBinContent(i_bin+1)
  return _x, _y


def main():
  # Get the histograms from the file:
  _fname = "/home/cadams/larlite/UserDev/argoneut_electrons/calo/xing_muons_ana.root"
  f = TFile(_fname)
  
  for plane in ["collection", "induction"]:
    for measure in ["area", "amp"]:
      name = "_{}_caloAlg_dEdx_{}".format(plane, measure)
      x_data, y_data = getNdarrayFromHist(f.Get(name))

      y_data /= numpy.sum(y_data)

      popt, pcov = opt.curve_fit(landau.gauss_landau,x_data,y_data,bounds=(0,[10,10,10,200000]) )

      # Sort out the result:
      landau_mpv = popt[0]
      landau_sigma = popt[1]
      gauss_sigma = popt[2]
      scale = popt[3]


      _min = x_data[0]
      _max = x_data[x_data.size-1]
      stepsize = (_max - _min) / 200.0
      x_fit = numpy.arange(_min,_max, stepsize)

      y_fit = landau.gauss_landau(x_fit,popt[0],popt[1],popt[2], popt[3])

      top = popt[3]

      # Plot the fit:
      plt.plot(x_data, y_data, label="{} {}".format(plane,measure).title(),ls="none",marker="x")
      plt.plot(x_fit, y_fit, label="Convolved Landau")
      plt.text(5,0.7*top,"Landau MPV: {:.3}".format(popt[0]),fontsize=20)
      plt.text(5,0.6*top,"Landau $\sigma$: {:.3}".format(popt[1]),fontsize=20)
      plt.text(5,0.5*top,"Gauss \sigma: {:.3}".format(popt[2]),fontsize=20)
      plt.text(5,0.4*top,"Scale: {:.3}".format(popt[3]),fontsize=20)
      plt.legend()
      plt.grid(True)
      plt.show()

if __name__ == '__main__':
  main()
