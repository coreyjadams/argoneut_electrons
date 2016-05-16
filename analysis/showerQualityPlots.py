
from matplotlib import pyplot as plt
import numpy
import ROOT
import larlite
from ROOT import larlite, larutil, protoshower

import math


def getTruthInfo(storage):

    # Get the mctruth info:
    truth = storage.get_data(larlite.data.kMCTruth, "generator")
    true_shower = truth.front().GetParticles().front()
    trueVertex = true_shower.Position().Vect()
    trueDirection = true_shower.Momentum().Vect()

    return trueVertex, trueDirection


def main(f):

    larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

    # This whole class is meant to just make shower quality plots.
    # So, we need a storage manager:

    manager = larlite.storage_manager()
    manager.reset()
    manager.add_in_filename(f)
    manager.set_io_mode(larlite.storage_manager.kREAD)
    manager.open()

    # need a protoshower alg
    ps_helper = protoshower.ProtoShowerHelper()
    ps_alg = protoshower.ProtoShowerAlgArgoNeuT()
    ps_helper.setProtoShowerAlg(ps_alg)

    # Define some containers to hold the info:
    _x_resolution = ROOT.vector(float)()
    _y_resolution = ROOT.vector(float)()
    _z_resolution = ROOT.vector(float)()
    _3d_resolution = ROOT.vector(float)()

    _3d_angle_res = ROOT.vector(float)()

    _collection_slope_diff_reco = ROOT.vector(float)()
    _induction_slope_diff_reco = ROOT.vector(float)()
    _collection_slope_diff_true = ROOT.vector(float)()
    _induction_slope_diff_true = ROOT.vector(float)()

    counter = 0
    while manager.next_event():

        if counter % 100 == 0:
            print "On event {}".format(counter)

        if counter > 500:
          break

        # Get the reco showers:
        showers = manager.get_data(larlite.data.kShower, "showerreco")

        if showers.size():
            shower = showers.front()
            # Get the pfparticle so proto showers can happen:
            pfpart = manager.get_data(
                larlite.data.kPFParticle, "mergeallMatched")
            # Need a vector of protoshowers:
            ps_vec = ROOT.vector(protoshower.ProtoShower)()

            ps_helper.GenerateProtoShowers(manager, "mergeallMatched", ps_vec)

            # Only consider clusters that have at least 50 hits in them:
            ps = ps_vec.front()
            skip = False
            for params in ps.params():
                if params.N_Hits < 50:
                    skip = True

            if skip:
                continue

            # Need the true start point and direction
            true_start, true_dir = getTruthInfo(manager)

            # Now we can compare the shower direction to the true direction
            # and also the shower direction to the directions in each plane
            _x_resolution.push_back(true_start.X() - shower.ShowerStart().X())
            _y_resolution.push_back(true_start.Y() - shower.ShowerStart().Y())
            _z_resolution.push_back(true_start.Z() - shower.ShowerStart().Z())

            _3d_res = (true_start - shower.ShowerStart()).Mag()

            _3d_resolution.push_back(math.sqrt(_3d_res))

            angle = (180.0/numpy.pi)*numpy.arccos(
                true_dir.Dot(shower.Direction()) / (true_dir.Mag() * shower.Direction().Mag()))
            if not math.isnan(angle):
                _3d_angle_res.push_back(angle)

            # For the clusters in each plane, get the start dir from the params
            for param in ps.params():
                start_dir = param.start_dir
                if start_dir[0] == 0:
                    continue

                start_slope = start_dir[1]/start_dir[0]
                reco_slope = larutil.GeometryHelper.GetME().Slope_3Dto2D(
                    shower.Direction(), param.plane_id.Plane)
                true_slope = larutil.GeometryHelper.GetME().Slope_3Dto2D(
                    true_dir, param.plane_id.Plane)

                theta_reco = abs(start_slope - reco_slope) / \
                    (1.0 + start_slope*reco_slope)
                theta_reco = (180.0/numpy.pi) * numpy.arctan(theta_reco)

                theta_true = abs(start_slope - true_slope) / \
                    (1.0 + start_slope*true_slope)
                theta_true = (180.0/numpy.pi) * numpy.arctan(theta_true)

                if param.plane_id.Plane == 0:
                    _induction_slope_diff_reco.push_back(theta_reco)
                    _induction_slope_diff_true.push_back(theta_true)

                if param.plane_id.Plane == 1:
                    _collection_slope_diff_reco.push_back(theta_reco)
                    _collection_slope_diff_true.push_back(theta_true)

        counter += 1

    # Now make some plots

    # ########################
    # # 1D Resolutions
    # ########################
    # bin_width=0.15
    # resolution_bins = numpy.arange(-5 + 0.5*bin_width ,5 + 0.5*bin_width, bin_width)

    # plt.hist(_x_resolution,bins=resolution_bins,alpha=0.5,label="X Resolution, Electrons")
    # plt.hist(_y_resolution,bins=resolution_bins,alpha=0.5,label="Y Resolution, Electrons")
    # plt.hist(_z_resolution,bins=resolution_bins,alpha=0.5,label="Z Resolution, Electrons")
    # plt.grid(True)
    # plt.xlabel("Vertex Resolution [cm]")
    # plt.legend()
    # plt.show()

    # ########################
    # # 3D Vertex Resolution
    # ########################
    # _3d_res_bins = numpy.arange(0,6,0.1)
    # plt.hist(_3d_resolution,bins=_3d_res_bins, label="Single Isotropic Electrons")
    # plt.grid(True)
    # plt.xlabel("Distance of Reco. Start to True Vertex [cm]")
    # plt.title("3D Vertex Resolution")
    # plt.legend()
    # plt.show()

    # ########################
    # # 3D Angle Resolution
    # ########################
    # print _3d_angle_res
    # _3d_angle_bins = numpy.arange(0, 10, 0.1)
    # plt.hist(_3d_angle_res, bins=_3d_angle_bins,
    #          color="g", label="Single Isotropic Electrons")
    # plt.grid(True)
    # plt.xlabel("Angle between True and Reco Direction [Deg.]")
    # plt.title("3D Angle Resolution")
    # plt.legend()
    # plt.show()

    ########################
    # 2D Angle Resolution
    ########################
    _2d_angle_bins = numpy.arange(0, 5, .1)
    plt.hist(_collection_slope_diff_reco, bins=_2d_angle_bins, alpha=0.5,
             color="g", label="Angle between 3D Reco. slope and 2D Reco. slope")
    plt.hist(_collection_slope_diff_true, bins=_2d_angle_bins, alpha=0.5,
             color="b", label="Angle between 3D Reco. slope and 2D True slope")
    plt.grid(True)
    plt.xlabel("Angle between True and Reco Direction [Deg.]")
    plt.title("Collection 2D Angle Resolution")
    plt.legend()
    plt.show()

    plt.scatter(_3d_resolution,
                _collection_slope_diff_true, marker="+")
    plt.xlabel("Reco 3D - Reco 2D")
    plt.ylabel("True 3D - Reco 2D")
    plt.ylim((0,10))
    plt.xlim((0,10))

    plt.grid(True)
    plt.show()

    # plt.hist(_induction_slope_diff_reco, bins=_2d_angle_bins, alpha=0.5,
    #          color="g", label="Angle between 3D Reco. slope and 2D Reco. slope")
    # plt.hist(_induction_slope_diff_true, bins=_2d_angle_bins, alpha=0.5,
    #          color="b", label="Angle between 3D Reco. slope and 2D True slope")
    # plt.grid(True)
    # plt.xlabel("Angle between True and Reco Direction [Deg.]")
    # plt.title("Induction 2D Angle Resolution")
    # plt.legend()
    # plt.show()


if __name__ == '__main__':
    main(
        "/data_linux/argoneut/argoneut_single_photon/single_photons_larlite_mergeall_endpoint2d_match_shower.root")
