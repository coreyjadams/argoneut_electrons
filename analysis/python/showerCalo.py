import ROOT
import larlite
from ROOT import argoana, larutil, larlite

import pickle


class showerCalo(object):

    """interface to run a c++ script to analyse showers"""

    def __init__(self, _file, calibrations,lifetimes, mc, producer="showerreco",select=False):
        super(showerCalo, self).__init__()
        self._file = _file
        self._calibrations_file = calibrations

        self._is_mc = mc

        self._process = argoana.dEdxShowerCaloMaker()

        self._process.setProducer(producer)

        self._process.isMC(mc)

        if select:
            self._process.set_select_events(True)

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


        # Need to set the lifetime corrections here
        if lifetimes is not None:
            pf = open(lifetimes, 'rb')
            lifetimes_map = self._process.getLifetimes();

            lifetime_calibrations = pickle.load(pf)

            for run in lifetime_calibrations:
                lifetimes_map[run] = lifetime_calibrations[run]

            self._process.setLifetimes(lifetimes_map)
        else:
            lifetimes_map = self._process.getLifetimes();
            lifetimes_map[1] = 750
            self._process.setLifetimes(lifetimes_map)

    def run(self, n_events=-1, n_skip=-1):
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
            if n_skip == -1:
                my_proc.run(0, n_events)
            else:
                my_proc.run(n_skip, n_events)

    def getBestMedianVector(self):
        return self._process.getShowerCalos().dEdx_best_median()

    def getBestModmeanVector(self):
        return self._process.getShowerCalos().dEdx_best_modmean()

    def getBestLMAVector(self):
        return self._process.getShowerCalos().dEdx_best_LMA()

    def getShowerCaloVector(self):
        return self._process.getShowerCalos()

    def getMedianVector(self, plane):
        return self._process.getShowerCalos().dEdx_median(plane)

    def getLMAVector(self, plane):
        return self._process.getShowerCalos().dEdx_LMA(plane)

    def getModMeanVector(self, plane):
        return self._process.getShowerCalos().dEdx_modmean(plane)

    def getMetaVector(self, plane):
        return self._process.getShowerCalos().dEdx_meta(plane)

    def getMetaErrVector(self, plane):
        return self._process.getShowerCalos().dEdx_meta_err(plane)


    def getAllGoodDedxHits(self,plane):
        return self._process.getShowerCalos().all_dedx_hits(plane)

    def getNGoodHitsVector(self,plane):
        return self._process.getShowerCalos().n_good_hits(plane)

    def getPercentGoodHitVector(self,plane):
        goodHits = self._process.getShowerCalos().n_good_hits(plane)
        allHits = self._process.getShowerCalos().n_hits(plane)
        _vector = []
        for _n1, _n2 in zip(goodHits, allHits):
            if _n2 != 0:
                _vector.append(1.0*_n1 / _n2)
            else:
                _vector.append(0.0)
        return _vector

    def getBestdEdxVector(self,plane):
        return self._process.getShowerCalos().best_dedx()

    def getJointdEdxVector(self,plane):
        return self._process.getShowerCalos().joint_dedx()

    def getDistVector(self,plane):
        return self._process.getShowerCalos().distance(plane)

    def size(self):
        return self._process.getShowerCalos().size()

def lite_samples():
    electron_data_file = "/data_linux/argoneut/dedx_files/electron_shower_reco.root"
    photon_data_file = "/data_linux/argoneut/dedx_files/photon_shower_reco.root"

    data_calibration_file = "/data_linux/argoneut/calibration_files/wireByWireCorrections_data_gapsFilled.pkl"
    lifetime_file = '/data_linux/argoneut/calibration_files/lifetimes_data.pkl'

    electron_data_module = showerCalo(
        electron_data_file, data_calibration_file, lifetime_file, mc=False, producer="bootlegMatched",select=True)
    photon_data_module = showerCalo(
        photon_data_file, data_calibration_file, lifetime_file, mc=False, producer="bootlegMatched")




    electron_data_module.run()
    photon_data_module.run()

    # For MC:
    electron_sim_file = "/data_linux/argoneut/argoneut_single_electron/electron_sample_larlite_lowerE_mergeall_endpoint2d_match_shower.root"
    photon_sim_file = "/data_linux/argoneut/argoneut_single_photon/photon_sample_larlite_lowerE_mergeall_endpoint2d_match_shower.root"

    sim_calibration_file = "/data_linux/argoneut/calibration_files/wireByWireCorrections_sim.pkl"

    electron_sim_module = showerCalo(
        electron_sim_file, sim_calibration_file,None, mc=True, producer="showerreco")
    photon_sim_module = showerCalo(
        photon_sim_file, sim_calibration_file,None, mc=True, producer="showerreco")

    electron_sim_module.run(1000)
    photon_sim_module.run(1000)

    return ((electron_data_module, electron_sim_module),
            (photon_data_module, photon_sim_module))


def full_samples():
    electron_data_file = "/data_linux/argoneut/dedx_files/electron_shower_reco.root"
    photon_data_file = "/data_linux/argoneut/dedx_files/photon_shower_reco.root"

    data_calibration_file = "/data_linux/argoneut/calibration_files/wireByWireCorrections_data_gapsFilled.pkl"
    lifetime_file = '/data_linux/argoneut/calibration_files/lifetimes_data.pkl'

    electron_data_module = showerCalo(
        electron_data_file, data_calibration_file,lifetime_file, mc=False, producer="bootlegMatched",select=True)
    photon_data_module = showerCalo(
        photon_data_file, data_calibration_file,lifetime_file, mc=False, producer="bootlegMatched")

    electron_data_module.run()
    photon_data_module.run()

    # For MC:
    electron_sim_file = "/data_linux/argoneut/argoneut_single_electron/electron_sample_larlite_lowerE_mergeall_endpoint2d_match_shower.root"
    photon_sim_file = "/data_linux/argoneut/argoneut_single_photon/photon_sample_larlite_lowerE_mergeall_endpoint2d_match_shower.root"

    sim_calibration_file = "/data_linux/argoneut/calibration_files/wireByWireCorrections_sim.pkl"

    electron_sim_module = showerCalo(
        electron_sim_file, sim_calibration_file,None, mc=True, producer="showerreco")
    photon_sim_module = showerCalo(
        photon_sim_file, sim_calibration_file,None, mc=True, producer="showerreco")

    electron_sim_module.run()
    photon_sim_module.run()

    return ((electron_data_module, electron_sim_module),
            (photon_data_module, photon_sim_module))
