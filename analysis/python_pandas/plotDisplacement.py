import ROOT
import numpy

from matplotlib import pyplot as plt

from dataFrameHandle import dataFrameHandle

width = 0.3
histbins = numpy.arange(0,20.0,width)



# electrons_df = dataFrameHandle("../anatrees/electrons_anatree_fix.root", "electron")
# electrons_df_pre = dataFrameHandle("../anatrees/electrons_anatree.root", "electron")

# electrons_disp =  electrons_df._df["displacement"].tolist()
# electrons_disp_pre =  electrons_df_pre._df["displacement"].tolist()


# plt.hist(electrons_disp,histbins,alpha=0.5,label="Electrons With Fix",normed=True)
# plt.hist(electrons_disp_pre,histbins,alpha=0.5,label = "Electrons Without Fix",normed=True)
# plt.grid(True)
# plt.xlabel("Reconstructed Vertex Displacement [cm]")

# plt.axvline(x=2.0,color='r',ls='--',label="Previous Cut")
# plt.axvline(x=4.0,color='b',ls='--',label="New Cut")
# plt.legend()

# bins=[0,2.0,100]
# old_data, bins = numpy.histogram(electrons_disp_pre,bins)
# bins=[0,4.0,100]
# new_data, bins = numpy.histogram(electrons_disp,bins)



# old_eff = 1.0*old_data[0]/(numpy.sum(old_data))
# new_eff = 1.0*new_data[0]/(numpy.sum(new_data))

# print old_data, old_eff
# print new_data, new_eff

# plt.text(10.0,0.55,"Old Efficiency: {:.3}%".format(100.0*old_eff))
# plt.text(10.0,0.45,"New Efficiency: {:.3}%".format(100.0*new_eff))

# plt.show()

# plt.close()

photons_df = dataFrameHandle("../anatrees/photons_anatree_fix.root", "photon")
photons_df_pre = dataFrameHandle("../anatrees/photons_anatree.root", "photon")

photons_disp =  photons_df._df["displacement"].tolist()
photons_disp_pre =  photons_df_pre._df["displacement"].tolist()


bins=[0,2.0,100]
old_data, bins = numpy.histogram(photons_disp_pre,bins)
bins=[0,4.0,100]
new_data, bins = numpy.histogram(photons_disp,bins)



old_eff = 1.0*old_data[0]/(numpy.sum(old_data))
new_eff = 1.0*new_data[0]/(numpy.sum(new_data))

print old_data, old_eff
print new_data, new_eff


plt.hist(photons_disp,histbins,alpha=0.5,label="Photons With Fix",normed=True)
plt.hist(photons_disp_pre,histbins,alpha=0.5,label = "Photons Without Fix",normed=True)
plt.xlabel("Reconstructed Vertex Displacement [cm]")

plt.axvline(x=2.0,color='r',ls='--',label="Previous Cut")
plt.axvline(x=4.0,color='b',ls='--',label="New Cut")
plt.legend()
plt.grid(True)


plt.text(10.0,0.55,"Old Efficiency: {:.3}%".format(100.0*old_eff))
plt.text(10.0,0.45,"New Efficiency: {:.3}%".format(100.0*new_eff))

plt.show()