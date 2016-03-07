import ROOT
import larlite

from ROOT import larlite

# use matplotlib to make these plots

import matplotlib.pyplot as plt
import numpy as np



# Loop over the events and read out the showers.

_mgr = larlite.storage_manager()

# Here are the input files
files = ["/data_linux/dedx_files/anu_electrons_reconstructed.root",
         "/data_linux/dedx_files/anu_photons_reconstructed.root",
         "/data_linux/dedx_files/nu_electrons_reconstructed.root",
         "/data_linux/dedx_files/nu_photons_reconstructed.root"]



electron_data = []
photon_data = []

electron_data_q = []
photon_data_q = []

electrons = False
photons = False

for f in files:
  _mgr.reset()
  _mgr.set_io_mode(larlite.storage_manager.kREAD)
  _mgr.add_in_filename(f)
  _mgr.open()

  if "electrons" in f:
    electrons = True
    photons = False
  if "photons" in f:
    electrons = False
    photons = True

  if _mgr.is_open():
    while _mgr.next_event():
      showers = _mgr.get_data(larlite.data.kShower, "showerreco")
      for shower in showers:

        # Cut events that are crazy
        if shower.OpeningAngle() > 1:
          continue

        if shower.Length() < 10:
          continue

        # Get the dE/dx vector:
        dedx_v = shower.dEdx_v()
        dqdx_v = shower.dQdx_v()

        if dedx_v[1] > 10:
          continue


        if electrons:
          electron_data.append(dedx_v[1])
          electron_data_q.append(dqdx_v[1])
        if photons:
          photon_data.append(dedx_v[1])
          photon_data_q.append(dqdx_v[1])

print len(electron_data)
print len(photon_data)


# bins = np.linspace(0,7,25)

fig, ax = plt.subplots()
ax.hist(electron_data,alpha=0.5)
ax.hist(photon_data,alpha=0.5)

plt.title("dE/dx Separation of electrons and photons")
plt.xlabel("dE/dx [MeV/cm]")

fig, ax = plt.subplots()

ax.hist(electron_data_q,alpha=0.5)
ax.hist(photon_data_q,alpha=0.5)

plt.title("dQ/dx Separation of electrons and photons")
plt.xlabel("dQ/dx [???/cm]")


plt.show()