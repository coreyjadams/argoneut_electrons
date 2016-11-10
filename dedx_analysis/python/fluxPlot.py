import root_numpy
import pandas
import numpy

from matplotlib import pyplot as plt

a = pandas.DataFrame(root_numpy.root2array("/data_linux/argoneut/bareFlux.root",
                                           start=0,
                                           stop=int(0.2*231515021)))
                                           # stop=231515021))

# a = pandas.DataFrame(root_numpy.root2array("/data_linux/argoneut/bareFlux.root"))

nue = a.query('pdg==12')
nuebar = a.query('pdg==-12')
numu = a.query('pdg==14')
numubar = a.query('pdg==-14')

print "Average nue energy: {}".format(numpy.mean(nue['E']))
print "Average nuebar energy: {}".format(numpy.mean(nuebar['E']))
print "Average numu energy: {}".format(numpy.mean(numu['E']))
print "Average numubar energy: {}".format(numpy.mean(numubar['E']))

bin_width=0.5

bins = numpy.arange(0, 20.01, bin_width)

target_width=1.0
tot_pot = (0.2*90)*5.5e8
target_pot = 1.e8
sim_win = 915.4*1328.0  # (cm^2)
argoneut_window = 40.*47.
# Plot the relative fluxes with histograms:

# Scale everything to neutrinos through argoneut per X protons
scale = (argoneut_window / sim_win)*(target_pot/tot_pot)*(target_width/bin_width)

fig, ax = plt.subplots(figsize=(10, 7))


total_hist, bin_edges = numpy.histogram(a['E'], bins)
nue_hist, bin_edges = numpy.histogram(nue['E'], bins)
nuebar_hist, bin_edges = numpy.histogram(nuebar['E'], bins)
numu_hist, bin_edges = numpy.histogram(numu['E'], bins)
numubar_hist, bin_edges = numpy.histogram(numubar['E'], bins)

x_bins = 0.5*(bin_edges[1:] + bin_edges[:-1])

plt.semilogy(x_bins, scale*total_hist, linewidth=3,
             ls="steps-mid", label="Total Flux")
plt.semilogy(x_bins, scale*nue_hist, linewidth=3,
             ls="steps-mid", label=r"$\nu_{e}$ Flux")
plt.semilogy(x_bins, scale*nuebar_hist, linewidth=3,
             ls="steps-mid", label=r"$\bar{\nu}_{e}$ Flux")
plt.semilogy(x_bins, scale*numu_hist, linewidth=3,
             ls="steps-mid", label=r"$\nu_{\mu}$ Flux")
plt.semilogy(x_bins, scale*numubar_hist, linewidth=3,
             ls="steps-mid", label=r"$\bar{\nu}_{\mu}$ Flux")

for tick in ax.xaxis.get_major_ticks():
    tick.label.set_fontsize(20)

for tick in ax.yaxis.get_major_ticks():
    tick.label.set_fontsize(20)

plt.ylabel(r"$\Phi (E)$ [$\nu$ / GeV / cm$^2$/ 1e8 POT]", fontsize=20)
plt.xlabel("Neutrino Energy [GeV]", fontsize=20)
plt.legend(fontsize=20)
plt.grid(True)
plt.show()

total_nue = nue_hist + nuebar_hist


fig, ax = plt.subplots(figsize=(10, 7))

plt.semilogy(x_bins, scale*total_nue, linewidth=3,
             ls="steps-mid", label=r"$\nu_{e}$ + $\bar{\nu}_{e}$ Flux")
plt.semilogy(x_bins, scale*nue_hist, linewidth=3,
             ls="steps-mid", label=r"$\nu_{e}$ Flux")
plt.semilogy(x_bins, scale*nuebar_hist, linewidth=3,
             ls="steps-mid", label=r"$\bar{\nu}_{e}$ Flux")

for tick in ax.xaxis.get_major_ticks():
    tick.label.set_fontsize(20)

for tick in ax.yaxis.get_major_ticks():
    tick.label.set_fontsize(20)

plt.ylabel(r"$\Phi_{\nu_e + \bar{\nu}_e} (E)$ [$\nu$ / GeV / cm$^2$/ 1e8 POT]",
           fontsize=20)
plt.xlabel("Neutrino Energy [GeV]", fontsize=20)
plt.legend(fontsize=20)
plt.grid(True)
plt.show()
