# import ROOT
# import larlite
import landau
import numpy as np
import scipy.optimize as opt

from matplotlib import pyplot as plt

from dataFrameHandle import dataFrameHandle

df = dataFrameHandle("anatrees/electrons_anatree.root", "electron")

df.make_quality_cuts()

measure = "charge"
recomb = "const"

title = "Electrons, All Collection Hits"

c_hits = []
i_hits = []

name = measure + "_dedx_" + recomb

# Need to dig out all the hit information from each event:
for index, row in df._df.iterrows():
    i_cand_hits = row["i_"+name]
    c_cand_hits = row["c_"+name]
    dists = row['c_dist_from_start']
    for hit, dist in zip(c_cand_hits, dists):
        if dist > 0 and dist < 3.5:
            c_hits.append(hit)
    dists = row['i_dist_from_start']
    for hit, dist in zip(i_cand_hits, dists):
        if dist > 0 and dist < 3.5:
            i_hits.append(hit)

binwidth = 0.25
bins = np.arange(0, 10, binwidth)
c_data, bin_edges = np.histogram(c_hits, bins)
i_data, bin_edges = np.histogram(i_hits, bins)
c_err = []
i_err = []
for point in c_data:
    try:
        c_err.append(1.0/math.sqrt(point))
    except:
        c_err.append(0.0)
for point in i_data:
    try: 
        i_err.append(1.0/math.sqrt(point))
    except:
        i_err.append(0.0)

c_data, bin_edges = np.histogram(c_hits, bins,density=True)
i_data, bin_edges = np.histogram(i_hits, bins,density=True)

c_err *= c_data
i_err *= i_data

x = bin_edges + 0.25
x = x[:-1]

f, ax = plt.subplots()

ax.errorbar(x, c_data, yerr=c_err, xerr=binwidth*0.5, label="Collection Hits",capsize=0)
# ax.errorbar(x, i_data, yerr=i_err, xerr=binwidth*0.5, label="Induction Hits",capsize=0)
ax.set_title(title)


# Make a fit to the hits:
popt, pcov = opt.curve_fit(landau.gauss_landau,x,c_data,bounds=(0,[10,10,10,10]) )
x_fit = np.arange(0,10, 0.5*binwidth)
y_fit = landau.gauss_landau(x_fit,popt[0],popt[1],popt[2], 1)
plt.plot(x_fit,y_fit,label="Fitted landau")

textstring =  "Landau MPV: {:.3}\n".format(popt[0])
textstring += "Landau $\sigma$: {:.3}\n".format(popt[1])
textstring += "Gauss $\sigma$: {:.3}\n".format(popt[2])
textstring += "N: {:}\n".format(len(c_hits))
textstring += "<dE/dx>: {:.3}".format(np.mean(c_hits))
props = dict(boxstyle='round', facecolor='wheat', alpha=1.0)
ax.text(5.0,0.2,textstring,bbox=props,fontsize=20)

plt.xlabel("dE/dx [MeV/cm]")
plt.ylabel("")
plt.legend()
plt.grid(True)
plt.show()


# if "Electron" in title:
#     plt.savefig("/data_linux/dedx_plots/Landau/landau_electrons_"+name+".png")
# if "Photon" in title:
#     plt.savefig("/data_linux/dedx_plots/Landau/landau_photons_"+name+".png")
# plt.close(f)