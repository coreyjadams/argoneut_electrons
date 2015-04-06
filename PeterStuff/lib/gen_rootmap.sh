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
# PeterStuff
rootlibmap libargoneut_electrons_PeterStuff.rootmap libargoneut_electrons_PeterStuff.so $LARLITE_USERDEVDIR/argoneut_electrons/PeterStuff/LinkDef.h \
    libLArLite_Analysis.so \

