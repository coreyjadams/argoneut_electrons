
import ROOT
import larlite
from ROOT import argoana

from showerCalo import showerCalo

import numpy

from matplotlib import pyplot as plt


def main():
    # For data:
    electron_data_file = "/data_linux/argoneut/dedx_files/electron_shower_reco.root"
    photon_data_file = "/data_linux/argoneut/dedx_files/photon_shower_reco.root"

    lifetime_file = '/data_linux/argoneut/calibration_files/lifetimes_data.pkl'
    data_calibration_file = "/data_linux/argoneut/calibration_files/wireByWireCorrections_data_gapsFilled.pkl"

    electron_data_module = showerCalo(
        electron_data_file, data_calibration_file, lifetime_file, mc=False, producer="bootlegMatched",select=True)
    photon_data_module = showerCalo(
        photon_data_file, data_calibration_file, lifetime_file, mc=False, producer="bootlegMatched")

    electron_data_module.run()
    photon_data_module.run()



    i = 0
    for shower in electron_data_module.getShowerCaloVector():

        print i
        i += 1
        
        print "Run {}, Event {}".format(
            shower.run(),
            shower.event())

        print "  dE/dx value: {}".format(shower.best_dedx())
        print "  Best  Plane: {}".format(shower.best_plane())
        # try:
        #     print "  Collection dE/dx (average): {:.3} +/- {:.3}".format(
        #         shower.dEdx_meta(1),
        #         shower.dEdx_meta_err(1)/shower.dEdx_meta(1))
        # except Exception, e:
        #     print "  Collection dE/dx (average): {:.3} +/- undef".format(
        #         shower.dEdx_meta(1))
        # try:
        #     print "  Induction dE/dx (average): {:.3} +/- {:.3}".format(
        #         shower.dEdx_meta(0),
        #         shower.dEdx_meta_err(0)/shower.dEdx_meta(0))
        # except Exception, e:
        #     print "  Induction dE/dx (average): {:.3} +/- undef".format(
        #         shower.dEdx_meta(0))

        c_dists = shower._collection_dist_3D
        c_dedx  = shower._collection_dedx
        i_dists = shower._induction_dist_3D
        i_dedx  = shower._induction_dedx

        for i in xrange(max(len(c_dists), len(i_dists))):
            string = "  {:2} | ".format(i)
            c_string = "               |  "
            i_string = "               "
            if i < len(c_dists):
                c_string = "{:+4.2f}: {:+4.2f}  |  ".format(c_dists[i],c_dedx[i])
            if i < len(i_dists):
                i_string = "{:+4.2f}: {:+4.2f}".format(i_dists[i],i_dedx[i])

            print string + c_string +i_string

        plot_dEdx_vs_distance(shower)

        # raw_input("Press enter for next event")

        # print "Electron dE/dx induction n_good_hits: {} / {}".format(
        #     shower.n_good_hits(0), shower.n_hits(0))
        # print "Electron dE/dx collection n_good_hits: {} / {}".format(
        #     shower.n_good_hits(1), shower.n_hits(1))
        # c_dists = shower._collection_dist_3D
        # c_dedx  = shower._collection_dedx
        # i_dists = shower._induction_dist_3D
        # i_dedx  = shower._induction_dedx

        # for i in xrange(max(len(c_dists), len(i_dists))):
        #     string = " {:2} | ".format(i)
        #     c_string = "              |  "
        #     i_string = "               "
        #     if i < len(c_dists):
        #         c_string = "{:+4.2f}: {:+4.2f}  |  ".format(c_dists[i],c_dedx[i])
        #     if i < len(i_dists):
        #         i_string = "{:+4.2f}: {:+4.2f}".format(i_dists[i],i_dedx[i])

        #     print string + c_string +i_string

        # print "{:.3}, {:.3}, {:.3} = {:.3} +/- {:.3} ({:.3}%)".format(
        #     shower.dEdx_median(1),
        #     shower.dEdx_modmean(1),
        #     shower.dEdx_LMA(1),
        #     shower.dEdx_meta(1),
        #     shower.dEdx_meta_err(1),
        #     100.*shower.dEdx_meta_err(1) / shower.dEdx_meta(1)
        #     )

        # print "{:.3}, {:.3}, {:.3} = {:.3} +/- {:.3} ({:.3}%)".format(
        #     shower.dEdx_median(0),
        #     shower.dEdx_modmean(0),
        #     shower.dEdx_LMA(0),
        #     shower.dEdx_meta(0),
        #     shower.dEdx_meta_err(0),
        #     100.*shower.dEdx_meta_err(0) / shower.dEdx_meta(0)
        #     )

        # print "Best Plane: {}".format(shower.best_plane())
        # print "Best dE/dx: {}".format(shower.best_dedx())
        # print "Joint: {}".format(shower.joint_dEdx())


def plot_dEdx_vs_distance(shower):

    c_dedx = shower._collection_dedx
    c_dist = shower._collection_dist_3D
    i_dedx = shower._induction_dedx
    i_dist = shower._induction_dist_3D

    plt.plot(c_dist, c_dedx, marker="+", markersize=10,
             ls="", label="Collection Plane")
    plt.plot(i_dist, i_dedx, marker="+", markersize=10,
             ls="", label="Induction Plane")
    plt.axhline(y=shower.dEdx_meta(1),
                ls="--", color="b", label="Collection dE/dx")
    plt.axhline(
        y=shower.dEdx_meta(0), ls="--", color="g", label="Induction dE/dx")
    # plt.axhline(
    #     y=shower.joint_dEdx(), ls="--", color="r", label="Combined dE/dx")

    plt.grid(True)
    plt.ylim((0, 8))
    plt.xlabel("3D Distance [cm]")
    plt.ylabel("dE/dx [MeV/cm]")
    plt.legend()
    plt.title("Run {}, Event {}".format(shower.run(), shower.event()))
    plt.show()

if __name__ == '__main__':
    main()
