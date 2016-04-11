import numpy
from  matplotlib import pyplot as plt
import pickle

# Get the data:
pk = open('wireByWireCorrections.pkl')

wire_corr = pickle.load(pk)

collection_mean = 0;
induction_mean = 0;

N=0
for val in wire_corr[1].values():
  if val != 0:
    collection_mean += val
    N += 1.0
collection_mean/= N

N=0
for val in wire_corr[0].values():
  if val != 0:
    induction_mean += val
    N += 1.0
induction_mean /= N

# Correct for zero values and save an patched version of corrections:
for key in wire_corr[1].keys():
  if wire_corr[1][key] == 0:
    wire_corr[1][key] = collection_mean 

for key in wire_corr[0].keys():
  if wire_corr[0][key] == 0:
    wire_corr[0][key] = induction_mean 


print "Induction mean and RMS is {:.4}, {:.4}".format(induction_mean, numpy.std(wire_corr[0].values()))
print "Collection mean and RMS is {:.4}, {:.4}".format(collection_mean, numpy.std(wire_corr[1].values()))

# fig,axes = plt.subplots(figsize=(10,5))
# axes.plot(wire_corr[0].keys(),wire_corr[0].values(),label="Corrected Calibration Factors")
# # axes.plot(wire_corr[0].keys(),[induction_mean]*240,label="Best Fit")
# plt.xlabel("Wire Number")
# plt.legend()
# plt.title("Induction Plane Wire Calibrations")
# plt.ylim([0,250])
# plt.grid(True)

# fig,axes = plt.subplots(figsize=(10,5))
# axes.plot(wire_corr[1].keys(),wire_corr[1].values(),label="Corrected Calibration Factors")
# # axes.plot(wire_corr[1].keys(),[collection_mean]*240,label="Best Fit")
# plt.xlabel("Wire Number")
# plt.title("Collection Plane Wire Calibrations")
# plt.ylim([0,70])
# plt.legend()
# plt.grid(True)

# plt.show()

# # Repickle the corrected version:
# output = open("wireByWireCorrections_gapsFilled.pkl",'wb')
# pickle.dump(wire_corr,output)
