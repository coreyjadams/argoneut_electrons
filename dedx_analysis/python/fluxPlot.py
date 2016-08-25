import root_numpy
import pandas
import numpy

from matplotlib import pyplot as plt

a = pandas.DataFrame(root_numpy.root2array("/data_linux/argoneut/bareFlux.root",
                                           start=0,
                                           stop=20000000))

nue = a.query('pdg==12')
nuebar = a.query('pdg==-12')
numu = a.query('pdg==14')
numubar = a.query('pdg==-14')

print "Average nue energy: {}".format(numpy.mean(nue['E']))
print "Average nuebar energy: {}".format(numpy.mean(nuebar['E']))
print "Average numu energy: {}".format(numpy.mean(numu['E']))
print "Average numubar energy: {}".format(numpy.mean(numubar['E']))

# Plot the relative fluxes with histograms:

fig, ax = plt.subplots(figsize=(10, 7))

bins = numpy.arange(0, 20.01, 0.5)

total_hist, bin_edges = numpy.histogram(a['E'], bins)
nue_hist, bin_edges = numpy.histogram(nue['E'], bins)
nuebar_hist, bin_edges = numpy.histogram(nuebar['E'], bins)
numu_hist, bin_edges = numpy.histogram(numu['E'], bins)
numubar_hist, bin_edges = numpy.histogram(numubar['E'], bins)

x_bins = 0.5*(bin_edges[1:] + bin_edges[:-1])

plt.semilogy(x_bins, total_hist, linewidth=3,
             ls="steps-mid", label="Total Flux")
plt.semilogy(x_bins, nue_hist, linewidth=3,
             ls="steps-mid", label=r"$\nu_{e}$ Flux")
plt.semilogy(x_bins, nuebar_hist, linewidth=3,
             ls="steps-mid", label=r"$\bar{\nu}_{e}$ Flux")
plt.semilogy(x_bins, numu_hist, linewidth=3,
             ls="steps-mid", label=r"$\nu_{\mu}$ Flux")
plt.semilogy(x_bins, numubar_hist, linewidth=3,
             ls="steps-mid", label=r"$\bar{\nu}_{\mu}$ Flux")

for tick in ax.xaxis.get_major_ticks():
    tick.label.set_fontsize(20)

for tick in ax.yaxis.get_major_ticks():
    tick.label.set_fontsize(20)

plt.ylabel(r"$\Phi$ [$\nu$/cm$^2$]",fontsize=20)
plt.xlabel("Neutrino Energy [MeV]",fontsize=20)
plt.legend(fontsize=20)
plt.grid(True)
plt.show()
