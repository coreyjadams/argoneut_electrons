
from ROOT import *

fileName = "default_event_output.root"

names = ['ccNoVert', 'ccclusterWithSingles', 'ccMergedSmall0', 'ccMergedSmall1', 'ccMergedSmall2', 'ccMergedSmall3', 'ccMergedExtendBlobNoBig', 'ccMergedInline', 'ccMergedSDnoBig0', 'ccMergedSDnoBig1', 'ccMergedSDnoBig2', 'ccMergedStartTrack', 'close', 'ccMergedExtendBlob', 'ccMergedExtendBlob2', 'ccMergedFinal']


f = TFile(fileName)

nevents = f.larlite_id_tree.GetEntries()

nClustersByName = d = {name: 0.0 for name in names}
nClustersByName.update({'final': 0.0})

print ""
print "Running cluster merging analysis on", nevents, "events."
print ""
for event in xrange(nevents):
  tree = f.cluster_cccluster_tree
  tree.GetEntry(event)
  initNum = tree.cluster_cccluster_branch.size()
  prevNum = initNum
  for name in names:
    thisName = "cluster_" + name + "_tree"
    tree = getattr(f,thisName)
    tree.GetEntry(event)
    branchName = "cluster_" + name + "_branch"
    branch = getattr(tree,branchName)
    thisNum = branch.size()
    precentChange = 100.0*(thisNum - prevNum) / prevNum
    nClustersByName[name] += precentChange
    prevNum = thisNum
    # print type(branch)
    # print branch.size()
  tree = f.cluster_ccMergedFinal_tree
  tree.GetEntry(event)
  finalNum = tree.cluster_ccMergedFinal_branch.size()
  precentChange = 100.0*(finalNum - initNum) / initNum
  nClustersByName['final'] += precentChange 

for name in names:
  print name,":\t", nClustersByName[name]/nevents


print "final :\t", nClustersByName['final']/nevents
