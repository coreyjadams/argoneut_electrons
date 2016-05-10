import ROOT
import larlite
from ROOT import argoana, larutil, larlite

import pickle


class showerCalo(object):

    """interface to run a c++ script to analyse showers"""

    def __init__(self, _file, calibrations, mc, producer="showerreco"):
        super(showerCalo, self).__init__()
        self._file = _file
        self._calibrations_file = calibrations

        self._is_mc = mc

        self._process = argoana.dEdxShowerCaloMaker()

        self._process.setProducer(producer)

        self._process.isMC(mc)

        # read the calibrations file from pickle
        pf = open(self._calibrations_file, 'rb')

        wire_calibrations = pickle.load(pf)

        # Get the right format for the calibrations from the c++ object
        _vector_corrections = self._process.getWireCorrections()
        _vector_corrections.resize(2)
        _vector_corrections[0].resize(240)
        _vector_corrections[1].resize(240)

        for plane in wire_calibrations:
            for wire in wire_calibrations[plane]:
                # print "({},{}): {:.2f}".format(plane,wire,
                # wire_calibrations[plane][wire])
                _vector_corrections[plane][
                    wire] = wire_calibrations[plane][wire]

        self._process.setWireCorrections(_vector_corrections)

    def run(self, n_events=-1):
        larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

        # Create ana_processor instance
        my_proc = larlite.ana_processor()

        # Set input root file
        my_proc.add_input_file(self._file)

        # Specify IO mode
        my_proc.set_io_mode(larlite.storage_manager.kREAD)

        # Specify output root file name
        # my_proc.set_ana_output_file()

        # Attach an analysis unit ... here we use a base class which do
        my_proc.add_process(self._process)

        # Let's run it.
        if n_events == -1:
            my_proc.run(0)
        else:
            my_proc.run(0, n_events)

    def getShowerCaloVector(self):
        return self._process.getShowerCalos()

    def getMedianVector(self, plane):
        return self._process.getShowerCalos().dEdx_median(plane)

    def size(self):
        return self._process.getShowerCalos().size()

def lite_samples():
    electron_data_file = "/data_linux/argoneut/dedx_files/electron_shower_reco.root"
    photon_data_file = "/data_linux/argoneut/dedx_files/photon_shower_reco.root"

    data_calibration_file = "/data_linux/argoneut/calibration_files/wireByWireCorrections_data_gapsFilled.pkl"

    electron_data_module = showerCalo(
        electron_data_file, data_calibration_file, mc=False, producer="bootlegMatched")
    photon_data_module = showerCalo(
        photon_data_file, data_calibration_file, mc=False, producer="bootlegMatched")

    electron_data_module.run()
    photon_data_module.run()

    # For MC:
    electron_sim_file = "/data_linux/argoneut/argoneut_single_electron/single_electrons_sim_larlite_mergeall_endpoint2d_match_shower.root"
    photon_sim_file = "/data_linux/argoneut/argoneut_single_photon/single_photons_larlite_mergeall_endpoint2d_match_shower.root"

    sim_calibration_file = "/data_linux/argoneut/calibration_files/wireByWireCorrections_sim.pkl"

    electron_sim_module = showerCalo(
        electron_sim_file, sim_calibration_file, mc=True, producer="showerreco")
    photon_sim_module = showerCalo(
        photon_sim_file, sim_calibration_file, mc=True, producer="showerreco")

    electron_sim_module.run(1000)
    photon_sim_module.run(1000)

    return ((electron_data_module, electron_sim_module),
            (photon_data_module, photon_sim_module))


def full_samples():
    electron_data_file = "/data_linux/argoneut/dedx_files/electron_shower_reco.root"
    photon_data_file = "/data_linux/argoneut/dedx_files/photon_shower_reco.root"

    data_calibration_file = "/data_linux/argoneut/calibration_files/wireByWireCorrections_data_gapsFilled.pkl"

    electron_data_module = showerCalo(
        electron_data_file, data_calibration_file, mc=False, producer="bootlegMatched")
    photon_data_module = showerCalo(
        photon_data_file, data_calibration_file, mc=False, producer="bootlegMatched")

    electron_data_module.run()
    photon_data_module.run()

    # For MC:
    electron_sim_file = "/data_linux/argoneut/argoneut_single_electron/single_electrons_sim_larlite_mergeall_endpoint2d_match_shower.root"
    photon_sim_file = "/data_linux/argoneut/argoneut_single_photon/single_photons_larlite_mergeall_endpoint2d_match_shower.root"

    sim_calibration_file = "/data_linux/argoneut/calibration_files/wireByWireCorrections_sim.pkl"

    electron_sim_module = showerCalo(
        electron_sim_file, sim_calibration_file, mc=True, producer="showerreco")
    photon_sim_module = showerCalo(
        photon_sim_file, sim_calibration_file, mc=True, producer="showerreco")

    electron_sim_module.run()
    photon_sim_module.run()

    return ((electron_data_module, electron_sim_module),
            (photon_data_module, photon_sim_module))
