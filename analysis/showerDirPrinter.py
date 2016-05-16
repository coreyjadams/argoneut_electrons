import ROOT
import larlite
from ROOT import larlite

_mgr = larlite.storage_manager()
_mgr.reset()
# _mgr.add_in_filename("electron_shower_reco.root")
_mgr.add_in_filename("photon_shower_reco.root")
_mgr.set_io_mode(larlite.storage_manager.kREAD)
_mgr.open()

while _mgr.next_event():
  showers = _mgr.get_data(larlite.data.kShower,"bootlegMatched")
  print "This event has {} showers".format(showers.size())
  for shower in showers:
    print shower.Direction().X()
    print shower.Direction().Y()
    print shower.Direction().Z()