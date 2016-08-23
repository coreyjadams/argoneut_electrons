from matplotlib import pyplot as plt
import numpy

# Need to read the data file downloaded from Xcom database.
#
_file = "gammaXsec.txt"

f = open(_file, 'r')
lines = f.readlines()
# print lines[0]
# print lines[1]
# print lines[2]
# print lines[3]
# print lines[4]
# print lines[5]


energy = []
compton_xsec = []
pair_prod_nuclear = []
pair_prod_electron = []
total_xsec = []

# for l in lines[50:60]:
for l in lines[3:]:
    nums = l.rstrip(' \n').split(' ')
    energy.append(float(nums[0]))
    compton_xsec.append(float(nums[1]))
    pair_prod_nuclear.append(float(nums[2]))
    pair_prod_electron.append(float(nums[3]))
    total_xsec.append(float(nums[4]))

total_xsec = numpy.asarray(total_xsec)
compton_xsec = numpy.asarray(compton_xsec)
pair_prod_nuclear = numpy.asarray(pair_prod_nuclear)
pair_prod_electron = numpy.asarray(pair_prod_electron)


argon_density = 1.3954  # [g/cm^3]
barn_per_cm2 = 1e24
u = 1.6605402e-24  # [g]
A = 40

attn_to_xsec = barn_per_cm2*u*A/argon_density

fig, ax = plt.subplots(figsize=(15, 9))


plt.loglog(energy, attn_to_xsec*total_xsec,
           linewidth=4,
           color="black",
           label=r"$\sigma_{Total}$")

plt.loglog(energy, attn_to_xsec*compton_xsec,
           linewidth=2, 
           ls="--",
           label=r"$\sigma_{Compton}$")

plt.loglog(energy, attn_to_xsec*pair_prod_nuclear,
           linewidth=2, 
           label=r"$\kappa_{Nucl.}$")

plt.loglog(energy, attn_to_xsec*pair_prod_electron,
           linewidth=2, 
           label=r"$\kappa_{Elec.}$")

plt.legend(fontsize=40,labelspacing=.05)

for tick in ax.xaxis.get_major_ticks():
    tick.label.set_fontsize(20)
for tick in ax.yaxis.get_major_ticks():
    tick.label.set_fontsize(20)

ax.set_xlim([1, 1000])
ax.set_ylim([1e-3, 1e3])

plt.ylabel("Cross Section [barns/atom]", fontsize=25)
plt.xlabel("Gamma Energy [MeV]", fontsize=25)
plt.title("Gamma Interaction Cross Section on Argon", fontsize=30)
plt.grid(True)
# plt.show()
plt.savefig("/home/cadams/Dropbox/dEdx_Paper/figures/photonCrossSection.png",format="png")

# Make another plot, the fraction of the total cross section from Comptons
# and the fraction from either pair production
fig, ax = plt.subplots(figsize=(15, 9))

compton_perct = compton_xsec / total_xsec
pair_perct = (pair_prod_electron + pair_prod_nuclear) / total_xsec

plt.semilogx(energy, compton_perct,
             label="Compton",
             linewidth=4)
plt.semilogx(energy, pair_perct,
             label="Pair Production",
             linewidth=4)

ax.set_xlim([1, 1000])

plt.legend(fontsize=25,loc=7)

for tick in ax.xaxis.get_major_ticks():
    tick.label.set_fontsize(20)
for tick in ax.yaxis.get_major_ticks():
    tick.label.set_fontsize(20)

plt.xlabel("Gamma Energy [MeV]",fontsize=25)
plt.ylabel("Fraction of Total Cross Section",fontsize=25)
plt.title("Relative Contributions to Total Gamma Cross Section",fontsize=25)
plt.grid(True, which='minor')
plt.grid(True, which='major')
plt.show()
plt.savefig("/home/cadams/Dropbox/dEdx_Paper/figures/relative_photonCrossSection.png",format="png")
