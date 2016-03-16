import ROOT
import larlite

from ROOT import argoutils

a = argoutils.BGCFileReader()

fileName="/home/cadams/larlite/UserDev/argoneut_electrons/ScanFilejasaadi_antineutrino.root"

a.setInputFile(fileName)
a.readData()

# Play around with the data:

bgcdata = a.getData()



textFiles = ["anumode_AMM_pass2_filt.txt",  "showerswgapanu.txt",
             "anumode_AMM_pass3.txt",       "showerswgapanu2.txt",
             "anumode_AMM_pass_filt.txt",   "showerswgapanu3.txt",
             "numode_AMM_pass_filt.txt",    "showerswgapnu.txt"]

outputDirs = ["anu_electrons", "anu_photons",
              "anu_electrons", "anu_photons",
              "anu_electrons", "anu_photons",
              "nu_electrons", "nu_photons"]

outputFile = "/argoneut/data/users/cadams/"

electronRunList = dict()
photonRunList = dict()

leftoverPhotons = []

# for txtF,outD in zip(textFiles,outputDirs):
#     # print "File ", txtF, " going to ", outputDir+outD

#     with open(txtF) as f:
#         for line in iter(f):
#             # print line
#             vals = line.rstrip('\n').split(' ')
#             try:
#               run = int(vals[0])
#               event = int(vals[2])
#             except Exception, e:
#               continue
#             # print "Run is {r}, event is {e}".format(r=run, e=event)
#             if "electron" in outD:
#                 if run not in electronRunList:
#                     electronRunList.update({run : []})
#                 electronRunList[run].append(event)
#             if "photon" in outD:
#                 if run not in photonRunList:
#                     photonRunList.update({run : []})
#                 photonRunList[run].append(event)

for run in bgcdata:
    for event in run.second:
        if run.first in electronRunList:
            if event.first in electronRunList[run.first]:
                continue
        if run.first in photonRunList:
            if event.first in photonRunList[run.first]:
                continue
        # At this point, the event in bgcluster is not in the electron or photon list
        print run.first, event.first, " is not in a txt file."
        leftoverPhotons.append("{r} 1 {e}\n".format(r=run.first,e=event.first))


f = open("bgcluster_jasaadi_antineutrino.txt",'w')
print f

for line in leftoverPhotons:
    f.write(line)



