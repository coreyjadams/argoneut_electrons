import ROOT
import larlite
from ROOT import larlite, larutil
import sys
import numpy

import landau 
import scipy.optimize as opt

from matplotlib import pyplot as plt

import pickle

def modBoxCorrection(dqdx):
    return larutil.LArProperties.GetME().ModBoxCorrection(dqdx)    

def runCaloMuons(amp_q_constants, area_q_constants, amp_e_constants, area_e_constants, f):

    larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

    # Create ana_processor instance
    my_proc = larlite.ana_processor()

    # Set input root file
    my_proc.add_input_file(f)

    # Specify IO mode
    my_proc.set_io_mode(larlite.storage_manager.kREAD)

    # my_proc.set_output_file("out.root")

    # Specify output root file name
    my_proc.set_ana_output_file("from_test_ana_you_can_remove_me.root")

    # Attach an analysis unit ... here we use a base class which does nothing.
    # Replace with your analysis unit if you wish.
    caloAlg = larlite.ArgoCaloXingMuons()

    caloAlg.setAreaQConstant(area_q_constants[0], 0)
    caloAlg.setAreaQConstant(area_q_constants[1], 1)
    caloAlg.setAmpQConstant(amp_q_constants[0], 0)
    caloAlg.setAmpQConstant(amp_q_constants[1], 1)

    caloAlg.setAreaEConstant(area_e_constants[0], 0)
    caloAlg.setAreaEConstant(area_e_constants[1], 1)
    caloAlg.setAmpEConstant(amp_e_constants[0], 0)
    caloAlg.setAmpEConstant(amp_e_constants[1], 1)

    # Need to set the lifetime corrections here
    lifetimes = caloAlg.getLifetimes();
    # This is for data:
    # pf = open('/data_linux/argoneut/calibration_files/lifetimes_data.pkl', 'rb')

    # lifetime_calibrations = pickle.load(pf)

    # for run in lifetime_calibrations:
    #     lifetimes[run] = lifetime_calibrations[run]

    # This is for mc:
    lifetimes[1] = 750

    caloAlg.setLifetimes(lifetimes)

    my_proc.add_process(caloAlg)

    # Let's run it.
    my_proc.run(0)

    return caloAlg


def fitWireData(caloAlg, plane, wire):

    # Get the data:
    wireCaloInfo = caloAlg.getWireCaloInfo(plane,wire)
    if wireCaloInfo.dQdx_area_e.size() < 500:
        return -1

    dqdx_to_dedx = numpy.vectorize(modBoxCorrection)


    binwidth = 0.1
    bins = numpy.arange(0,10,binwidth)


    converged = False
    scale = 1.0
    i = 0
    while not converged:
        # Stretch or compress this data along dQdx to get the dE/dx right
        this_data = dqdx_to_dedx(scale*numpy.asarray(wireCaloInfo.dQdx_area_e))

        y_data, bin_edges = numpy.histogram(this_data,bins=bins,normed=True)
        bin_centers = bin_edges[:-1] + 0.5 * binwidth
        popt, pcov = opt.curve_fit(landau.gauss_landau,bin_centers,y_data,bounds=(0,[10,10,10,10]) )
        if (abs(popt[0] - 1.73) < 0.005 ):
            converged = True
        else:
            scale -= 0.5*(popt[0] - 1.73) / 1.73
        i += 1
        if i > 5:
            break

    # scale the y axis to fit nicely:
    y_data *= 1.0/popt[3]

    x_fit = numpy.arange(0,10, 0.5*binwidth)
    y_fit = landau.gauss_landau(x_fit,popt[0],popt[1],popt[2], 1)

    fig,axes = plt.subplots(figsize=(10,5))
    plt.plot(bin_centers,y_data,ls="None",marker="x",label="Wire Data")
    plt.plot(x_fit,y_fit,label="Fitted landau")
    plt.title("Plane {}, Wire {}".format(plane, wire))
    textstring =  "Landau MPV: {:.3}\n".format(popt[0])
    textstring += "Landau $\sigma$: {:.3}\n".format(popt[1])
    textstring += "Gauss $\sigma$: {:.3}\n".format(popt[2])
    textstring += "N: {:}\n".format(this_data.size)
    textstring += "<dE/dx>: {:.3}".format(numpy.mean(this_data))
    # plt.text(7,0.7,"Landau MPV: {:.3}".format(popt[0]),fontsize=20)
    # plt.text(7,0.6,"Landau $\sigma$: {:.3}".format(popt[1]),fontsize=20)
    # plt.text(7,0.5,"Gauss $\sigma$: {:.3}".format(popt[2]),fontsize=20)
    # plt.text(7,0.4,"Scale: {:.3}".format(popt[3]),fontsize=20)
    # plt.text(7,0.3,"<dE/dx>: {:.3}".format(numpy.mean(this_data)),fontsize=20)
    props = dict(boxstyle='round', facecolor='wheat', alpha=1.0)
    axes.text(5.0,0.5,textstring,bbox=props,fontsize=20)
    plt.ylim([0,1.5])
    plt.legend()
    plt.grid()
    plt.xlabel("dE/dx [MeV/cm]")
    plt.ylabel("Normalized")
    # plt.show()

    file_name = "xing_muons_dedx_fit_plane{}_wire{}.png".format(plane,wire)
    file_path = "/data_linux/argoneut/dedx_plots/xing_muon_calibration_data/wires/"
    plt.savefig(file_path+file_name)
    plt.close(fig)
    return scale

if __name__ == '__main__':

    larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

    if len(sys.argv) < 2:
        msg = '\n'
        msg += "Usage 1: %s $INPUT_ROOT_FILE(s)\n" % sys.argv[0]
        msg += '\n'
        sys.stderr.write(msg)
        sys.exit(1)

    f = sys.argv[-1]

    # Initial constants:

    target_error = 0.001


    area_q_constants = [0.8152236610976646, 0.8079060827478044]
    amp_q_constants  = [9.207990852976915, 18.562009816939195]

    area_e_constants = [185.00584273133333, 44.90267652367623]
    amp_e_constants  = [184.9556285310639, 45.1669419567248]

    # This runs the initial calculation, which gets ~close
    caloAlg = runCaloMuons(amp_q_constants, area_q_constants, amp_e_constants, area_e_constants, f)

    wireCorrections = dict()
    wireCorrections.update({0:dict()})
    wireCorrections.update({1:dict()})

    # Let's look at just one of the wires.
    # plane = 1
    # wire = 101
    for plane in [0,1]:
        # for wire in [30,65]:
        for wire in xrange(240):
            print "On plane {}, wire {}".format(plane,wire)
            scale = fitWireData(caloAlg,plane,wire)
            if scale > 0:
                total_correction = area_q_constants[plane]*area_e_constants[plane]*scale
            else:
                total_correction = 0.
            # print total_correction
            wireCorrections[plane].update({wire:total_correction})



    print wireCorrections
    output = open("wireByWireCorrections_sim.pkl",'wb')
    pickle.dump(wireCorrections,output)

