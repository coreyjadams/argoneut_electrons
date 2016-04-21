import numpy
from matplotlib import pyplot as plt
import ROOT
import larlite

from ROOT import larutil, larlite

larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

util = larutil.LArProperties.GetME()

a = numpy.arange(0.5,3.0,0.01)

box = []
birks = []

for _b in a:
  box.append(util.ModBoxInverse(_b))
  birks.append(util.BirksInverse(_b))
  correction = _b / util.ModBoxInverse(_b)
  print correction



plt.plot(a,box,label="Box")
plt.plot(a,birks,label="Birks")
plt.xlabel("dE/dx [Mev/cm]")
plt.ylabel("dQ/dx")
plt.grid(True)
plt.legend()
plt.show()