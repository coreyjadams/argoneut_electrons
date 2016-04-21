import ROOT
import larlite
import sys


from ROOT import larlite, larutil, argoana


def processFile(input_file, output_file):

    # if len(sys.argv) < 2:
    #     msg  = '\n'
    #     msg += "Usage 1: %s $INPUT_ROOT_FILE(s)\n" % sys.argv[0]
    #     msg += '\n'
    #     sys.stderr.write(msg)
    #     sys.exit(1)

    # Create ana_processor instance
    my_proc = larlite.ana_processor()
    my_proc.add_input_file(input_file)
# # Set input root file
# for x in xrange(len(sys.argv)-1):
#     my_proc.add_input_file(sys.argv[x+1])

    # Specify IO mode
    my_proc.set_io_mode(larlite.storage_manager.kREAD)

    larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

    # Specify output root file name
    my_proc.set_ana_output_file(output_file)

    # Attach an analysis unit ... here we use a base class which does nothing.
    # Replace with your analysis unit if you wish.
    my_proc.add_process(argoana.dEdxPfPartAna())

    # Let's run it.
    my_proc.run(0)


if __name__ == '__main__':

    # input_files = [
    #     "/data_linux/dedx_files/anu_electrons_bootleg_matched.root",
    #     "/data_linux/dedx_files/jasaadi_photons_antineutrino_bootleg_matched.root",
    #     "/data_linux/dedx_files/jasaadi_photons_neutrino_bootleg_matched.root",
    #     "/data_linux/dedx_files/misc_events_bootleg_matched.root",
    #     "/data_linux/dedx_files/nu_electrons_bootleg_matched.root",
    #     "/data_linux/dedx_files/nu_photons_bootleg_matched.root"
    # ]
    # output_files = [
    #     "anatrees/anu_electrons_anatree.root",
    #     "anatrees/anu_pi0_anatree.root",
    #     "anatrees/nu_pi0_anatree.root",
    #     "anatrees/misc_bootleg_anatree.root",
    #     "anatrees/nu_electrons_anatree.root",
    #     "anatrees/nu_photons_anatree.root"
    # ]

    input_files = [
        # "/data_linux/dedx_files/electron_sample_bootleg_matched_3D.root",
        # "/data_linux/dedx_files/photon_sample_bootleg_matched_3D.root",
        "electron_shower_reco.root",
        "photon_shower_reco.root"
    ]
    output_files = [
        "anatrees/electrons_anatree_fix.root",
        "anatrees/photons_anatree_fix.root"
    ]


    for _in, _out in zip (input_files, output_files):
      processFile(_in,_out)