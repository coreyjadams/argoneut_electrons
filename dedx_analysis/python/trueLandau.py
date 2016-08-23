# import ROOT
# import larlite
import numpy as np
import math

import showerCalo

from matplotlib import pyplot as plt


def drawLandaus(e_sim, p_sim, binwidth):

    # This will loop over a distance soon:
    distances = [0.5,1,1.5,2,2.5]
    distances = np.arange(0.5,50,0.5)

    bins = np.arange(binwidth, 12+binwidth, binwidth)


    for distance in distances:
        print "Starting distance {}".format(distance)
        e_true_hits = e_sim.getShowerCaloVector().true_dedx_hits(distance)
        p_true_hits = p_sim.getShowerCaloVector().true_dedx_hits(distance)



        e_data, bin_edges = np.histogram(e_true_hits, bins,density=True)
        p_data, bin_edges = np.histogram(p_true_hits, bins,density=True)



        bin_centers = 0.5*(bin_edges[1:] + bin_edges[:-1])


        f, ax = plt.subplots(figsize=(10, 7))


        ax.plot(bin_centers, e_data, color="b",
             ls="steps", label="Simulated Electron Hits")
        ax.plot(bin_centers, p_data, color="r",
             ls="steps", label="Simulated Photon Hits")

        ax.set_title("True dE/dx Hits")

        y_lim = ax.get_ylim()
        plt.ylim([0, 1.3])

        plt.text(7.0, 0.75,"Distance = {} cm".format(distance),fontsize=20)



        # for i in xrange(len(sim)):
        #     print "[{:.2} - {:.2}]: {:.3}".format(
        #         bin_edges[i], bin_edges[i+1], sim[i])

        # ax.bar(bin_centers, sim,align="center",width=binwidth,color=(1.0,1.0,1.0))
        
        # ax.plot(bin_centers, sim, color="b",
             # marker="x",ls="")


        plt.xlabel("dE/dx [MeV/cm]")
        plt.ylabel("")
        plt.legend()
        plt.grid(True)

        # Save the plot:
        plt.savefig("/home/cadams/Dropbox/Talks/dEdx/EGammaSep/dedx_6_plots/landau/true_landau_{}cm.png".format(distance))
        # plt.close()
        plt.show()


(e_data, e_sim), (p_data, p_sim) = showerCalo.full_simch_samples()
# (e_data, e_sim), (p_data, p_sim) = showerCalo.lite_simch_samples()

binwidth = 0.2


drawLandaus(e_sim, p_sim, binwidth)
