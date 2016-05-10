import numpy
import glob

path = "/data_linux/dedx_plots/1D/dist/"

fullList = ""

for i in numpy.arange(0.5,4.0,0.05):
    a = glob.glob(path+"dedx*_{}_*_c_*.png".format(i))
    # print a[0]
    fullList += a[0] + " "

print fullList
