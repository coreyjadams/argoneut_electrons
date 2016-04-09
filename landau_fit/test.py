import _landau
import numpy

sigma_landau = 0.5
sigma_gauss = 0.5
mean = 0
mu = 1.7
step_size = 0.11

x = numpy.arange(-1,10,step_size)
# x = numpy.arange(mu - 4*sigma,mean + 4*sigma,step_size)
y = _landau.convlangau(x,mu,sigma_landau,sigma_gauss)
y2 = _landau.landau(x,mu,sigma_landau)
# print _landau.gauss(1.0,1.0,1.0)

print numpy.sum(y)
print numpy.sum(y2)
print 1.0/step_size;
print step_size*numpy.sum(y)

from matplotlib import pyplot as plt
plt.plot(x,y)
plt.plot(x,y2)
plt.grid(True)
plt.show()
