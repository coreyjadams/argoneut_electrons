import ROOT
from ROOT import larlite, larutil, argoana

import sys

samples = dict()
samples.update({'nue' : "/data/argoneut/larsoft/nue_cc_sim/nue_cc_stage0_larlite.root"})
samples.update({'numu' : "/data/argoneut/larsoft/numu_cc_sim/numu_stage0_larlite.root"})
samples.update({'xing' : "/data/argoneut/larsoft/xing_muons/xing_muons_stage0_larlite.root"})
samples.update({'anu_sim' : "/data/argoneut/larsoft/anu_sim/anu_sim_stage0_larlite.root"})


def process_sample(_type, _file):

    # Create ana_processor instance
    my_proc = larlite.ana_processor()

    # Set input root file
    # for _f in f:
        # my_proc.add_input_file(_f)

    my_proc.add_input_file(_file)

    # Specify IO mode
    my_proc.set_io_mode(larlite.storage_manager.kREAD)

    # Specify output root file name
    my_proc.set_ana_output_file(_file.replace('.root', '') + "_ana.root")
    # my_proc.set_output_file("")

    filterModule = argoana.Stage1Efficiency()
    filterModule.setTrackProducer("pmtrack")
    filterModule.setClusterProducer("trajcluster")
    filterModule.setMinTrackLength(15)

    # Attach an analysis unit ... here we use a base class which do
    my_proc.add_process(filterModule)

    my_proc.run(0)

def main():

    larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

    for _type in samples:
        process_sample(_type, samples[_type])


if __name__ == '__main__':
  main()