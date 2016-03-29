import glob

photons = dict()
electrons = dict()


def readElectrons():

    # photons = dict()
    electronFiles = ["text/nu_electrons.txt",
                     "text/anu_electrons.txt",
                     "text/misc_electrons.txt"]
    for ef in electronFiles:
        with open(ef, "r") as f:
            for line in f:
                vals = line.split()
                run = int(vals[0])
                event = int(vals[1])
                if run in electrons:
                    if event in electrons[run]:
                        pass
                    else:
                        electrons[run].append(event)
                else:
                    electrons.update({run: [event]})


def readPhotons():
    # photons = dict()
    photonFiles = ["text/nu_photons.txt",
                   "text/anu_pi0.txt",
                   "text/nu_pi0.txt",
                   "text/misc_photons.txt"]
    for pf in photonFiles:
        with open(pf, "r") as f:
            for line in f:
                vals = line.split()
                run = int(vals[0])
                event = int(vals[1])
                if run in photons:
                    if event in photons[run]:
                        pass
                    else:
                        photons[run].append(event)
                else:
                    photons.update({run: [event]})


def getListOfFiles():

    # Electrons and photons are just dictated by their run and event number
    # There is a list of locations to look for files:
    baseDir = "/argoneut/data/users/cadams/"
    electronDirs = ["nu_electrons/", "anu_electrons/", "bgcluster_photons/"]
    photonDirs = ["nu_photons/", "anu_photons/", "bgcluster_photons/","jasaadi_neutrino/","jasaadi_antineutrino/"]

    # Loop over the events in the dict and find the files.
    
    electronFilesList = []
    for run in electrons:
        for event in electrons[run]:
            # Need to find this run and event number.
            # Look through the directories until it's found
            for _dir in electronDirs:
                result = glob.glob(baseDir + _dir + "*{r}*{e}*larlite*".format(r=run,e=event))
                print result