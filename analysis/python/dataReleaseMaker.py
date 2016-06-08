
import ROOT
import larlite
from ROOT import evd, larlite, larutil
from evdmanager import geometry

# Needed for the process to read data and filter noise:

# Needed for array manipulation and color mapping:
from matplotlib.colors import LinearSegmentedColormap
from matplotlib import cm

import sys
import time

import numpy
import showerCalo



import math


import Image
import ImageFont
import ImageDraw
import matplotlib
from matplotlib import pyplot as plt

# This defines the official argoneut color map
# in a way usable by matplotlib colormaps
collection_cdict = {'red':
                    ((0.0, 030./255, 030./255),
                     (0.4, 000./255, 000./255),
                     (0.8, 000./255, 000./255),
                     (1.0, 255./255, 255./255)),
                    'green':
                    ((0.0, 030./255, 030./255),
                     (0.4, 255./255, 255./255),
                     (0.8, 255./255, 255./255),
                     (1.0, 000./255, 000./255)),
                    'blue':
                    ((0.0, 255./255, 255./255),
                     (0.4, 255./255, 255./255),
                     (0.8, 000./255, 000./255),
                     (1.0, 000./255, 000./255))
                    }

induction_cdict = {'red':
                    ((0.0, 030./255, 030./255),
                     (0.32, 000./255, 000./255),
                     (0.8, 000./255, 000./255),
                     (1.0, 255./255, 255./255)),
                    'green':
                    ((0.0, 030./255, 030./255),
                     (0.32, 255./255, 255./255),
                     (0.8, 255./255, 255./255),
                     (1.0, 000./255, 000./255)),
                    'blue':
                    ((0.0, 255./255, 255./255),
                     (0.32, 255./255, 255./255),
                     (0.8, 000./255, 000./255),
                     (1.0, 000./255, 000./255))
                    }


# Read in the argoneut logo:
argoneut_logo = Image.open('/data_linux/argoneut/ANT_LOGO_candidate_T.jpg')
new_size_x = int(0.5*argoneut_logo.size[0])
new_size_y = int(0.5*argoneut_logo.size[1])
argoneut_logo = argoneut_logo.resize((new_size_x, new_size_y),Image.ANTIALIAS)

print argoneut_logo.size

# This takes the data and maps it on to an RGBalpha array
# Create a color map using the official color scheme
coll_map = LinearSegmentedColormap('collection_map', collection_cdict)
ind_map = LinearSegmentedColormap('induction_map', induction_cdict)
# register the color map:
plt.register_cmap(cmap=coll_map)
plt.register_cmap(cmap=ind_map)

# this is a utility that maps arrays into rgb arrays
collectionMapper = cm.ScalarMappable()
collectionMapper.set_cmap('collection_map')
# Set the levels of the color mapping:
collectionMapper.set_clim(vmin=-25, vmax=100)

inductionMapper = cm.ScalarMappable()
inductionMapper.set_cmap('induction_map')
# Set the levels of the color mapping:
inductionMapper.set_clim(vmin=-25, vmax=100)

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'




def drawEvent(_file, shower,output_folder):

    print ROOT.gROOT.GetListOfFiles().RemoveAll()


    if shower.dEdx_best_median() < 0.1:
        return

    print bcolors.OKBLUE, "Initializing data ...", bcolors.ENDC

    ana_processor = larlite.ana_processor()
    # ana_processor.enable_event_alignment(False)
    ana_processor.set_io_mode(larlite.storage_manager.kREAD)
    image_process = evd.DrawWire()

    # Set up the noise filter and initialize
    image_process.initialize()

    ana_processor.add_process(image_process)
    ana_processor.add_input_file(_file)
    ana_processor.process_event(shower.fileIndex() - 1)

    # # Fetch the data from the reader and transpose it to get the axes correct
    _orig_coll_array = image_process.getArrayByPlane(1)
    _orig_ind_array = image_process.getArrayByPlane(0)
    _orig_coll_array = numpy.flipud(numpy.transpose(_orig_coll_array))
    _orig_ind_array = numpy.flipud(numpy.transpose(_orig_ind_array))

    _scaled_coll_array = numpy.repeat(_orig_coll_array, 13, axis=1)
    _scaled_ind_array = numpy.repeat(_orig_ind_array, 13, axis=1)

    # This function maps the data onto the color scheme and makes and RGB array
    coll_array = collectionMapper.to_rgba(_scaled_coll_array)
    coll_array = numpy.uint8(coll_array*255)

    ind_array = inductionMapper.to_rgba(_scaled_ind_array)
    ind_array = numpy.uint8(ind_array*255)

    run = shower.run()
    event = shower.event()

    x_extent = 240*13
    y_extent = 2048

    # logo_top = 

    run_info = "Run {}, Event {}".format(run, event)
    calo_info = "Best Plane: {}\ndE/dx: {:.3}".format(shower.best_plane(),shower.dEdx_best_median())

    collfileName = "R{}_E{}_collection.png".format(run,event)
    indfileName = "R{}_E{}_induction.png".format(run,event)

    coll_image = Image.fromarray(coll_array)
    draw = ImageDraw.Draw(coll_image)
    fontsize = 60
    font = ImageFont.truetype("arialbd.ttf",fontsize)
    draw.text((100, y_extent - fontsize - 100),run_info,(255,255,255),font)
    # draw.text((100, y_extent - fontsize*3 - 100),calo_info,(255,255,255),font)

    coll_image.paste(argoneut_logo,(1,1))

    coll_image.save(output_folder + collfileName)

    ind_image = Image.fromarray(ind_array)
    draw = ImageDraw.Draw(ind_image)
    draw.text((100, y_extent - fontsize - 100),run_info,(255,255,255),font)
    # draw.text((100, y_extent - fontsize*3 - 100),calo_info,(255,255,255),font)
    ind_image.paste(argoneut_logo,(1,1))

    ind_image.save(output_folder + indfileName)
    print bcolors.OKBLUE, "Finished " + run_info, bcolors.ENDC



def main():

    larutil.LArUtilManager.Reconfigure(larlite.geo.kArgoNeuT)

    (e_data, e_sim), (p_data, p_sim) = showerCalo.lite_samples()

    folder = "/data_linux/argoneut/data_release/electrons/"
    _file = "/data_linux/argoneut/dedx_files/electron_shower_reco.root"

    i = 0
    for e_shower in e_data.getShowerCaloVector():
        drawEvent(_file, e_shower, folder)

    # folder = "/data_linux/argoneut/data_release/photons/"
    # _file = "/data_linux/argoneut/dedx_files/photon_shower_reco.root"
   
    # for p_shower in p_data.getShowerCaloVector():
    #     drawEvent(_file, p_shower, folder)
   



if __name__ == '__main__':

    main()