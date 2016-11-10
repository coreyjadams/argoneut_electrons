import ROOT
import numpy
import showerCalo

from ROOT import larlite, larutil

import math

from matplotlib import pyplot as plt

import numpy


# Make an instance of a larlite processor to process the mc file and get
# the true
def processMC(f):

    larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

    # Make an instance of the larlite processing framework:
    my_proc = larlite.ana_processor()

    # Set input root file
    my_proc.add_input_file(f)

    # Specify IO mode
    my_proc.set_io_mode(larlite.storage_manager.kREAD)

    proc = larlite.MCElectronEstimate()

    # Attach an analysis unit ... here we use a base class which do
    my_proc.add_process(proc)

    # Let's run it.
    my_proc.run()

    return proc


def main():

    f = "/data_linux/argoneut/nue_beam_larlite_all.root"

    mc_est = processMC(f)

    # Get the data too:
    (e_data, e_sim), (p_data, p_sim) = showerCalo.lite_samples()

    e_theta = []
    e_phi = []
    for shower in e_data.getShowerCaloVector():
        e_theta.append(math.cos(shower._3D_direction.Theta()))
        e_phi.append((180. / math.pi)*shower._3D_direction.Phi())

    mcTheta = numpy.cos(numpy.asarray(mc_est._theta))


    theta_bins = numpy.arange(0.75, 1.001, 0.0125)

    # This section creats the mc distribution
    # area normalized with statistical error:



    mc_hist, bins = numpy.histogram(mcTheta, theta_bins, normed=False)
    mc_err = []

    for i in mc_hist:
      if i > 0:
        mc_err.append(1/math.sqrt(i))
      else:
        mc_err.append(0)


    mc_hist, bins = numpy.histogram(mcTheta, theta_bins, normed=True)

    mc_err = mc_hist * mc_err


    # This section makes the area normalize 
    # data distribution and statistical error:

    data_hist, bins = numpy.histogram(e_theta, theta_bins, normed=False)

    data_err = []
    for i in data_hist:
      if i > 0:
        data_err.append(1/math.sqrt(i))
      else:
        data_err.append(0)


    data_hist, bins = numpy.histogram(e_theta, theta_bins, normed=True)

    data_err = data_hist * data_err

    x_bins = 0.5*(bins[0:-1] + bins[1:])



    fig, ax = plt.subplots(figsize=(10, 7))

    print mc_err
    print mc_hist



    ax.bar(x_bins,mc_hist,width=0.0125, align='center',
            label="Electron Candidates",
            alpha=0.65,
            color="b")
    ax.errorbar(x_bins, data_hist, label="MC Electrons",
                ls="none",
                marker="o",
                capsize=6,
                markersize=10,
                yerr=data_err, color='black')

    plt.ylabel("Area Normalized", fontsize=20)
    plt.xlabel(r"$cos(\theta)$", fontsize=20)
    # plt.title(
        # r"$\theta$ (Polar Angle) of Showers to Beam Direction", fontsize=30)
    # ax.set_ylim((0,45))
    # ax.set_xlim((0,90))
    plt.legend(fontsize=20,loc=2)
    plt.grid(True)
    for tick in ax.xaxis.get_major_ticks():
        tick.label.set_fontsize(15)
    for tick in ax.yaxis.get_major_ticks():
        tick.label.set_fontsize(0)
    plt.show()

    plt.show()

    pass

if __name__ == '__main__':
    main()
