#!/bin/bash

temp=make_rootmap.$$.temp

rootlibmap() {
 ROOTMAP=$1
 SOFILE=$2
 LINKDEF=$3
 shift 3
 DEPS=$*
 if [[ -e $SOFILE && -e $LINKDEF ]]; then
     rlibmap -f -o $ROOTMAP -l $SOFILE -d $DEPS -c $LINKDEF 2>> $temp
     rm -f $temp
 fi
}

######################################################
# minos
rootlibmap libargoneut_electrons_minos.rootmap libargoneut_electrons_minos.so $LARLITE_USERDEVDIR/argoneut_electrons/minos/LinkDef.h \
    libLArLite_Analysis.so #\


