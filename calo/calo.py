import ROOT
import larlite
from ROOT import larlite, larutil
import sys


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

    my_proc.add_process(caloAlg)

    # Let's run it.
    my_proc.run(0)

    # Target for dE/dx is fix at 2.3 MeV/cm

    # print the results:
    print "Induction Q area mean: {m}\ttarget: {t}".format(m=caloAlg.induction_q_area_mean(),
                                                           t=caloAlg.induction_q_target_mean())
    print "Induction Q amp mean: {m}\ttarget: {t}".format(m=caloAlg.induction_q_amp_mean(),
                                                          t=caloAlg.induction_q_target_mean())
    print "Collection Q area mean: {m}\ttarget: {t}".format(m=caloAlg.collection_q_area_mean(),
                                                            t=caloAlg.collection_q_target_mean())
    print "Collection Q amp mean: {m}\ttarget: {t}".format(m=caloAlg.collection_q_amp_mean(),
                                                           t=caloAlg.collection_q_target_mean())
    print "Induction E area mean: {m}\ttarget: {t}".format(m=caloAlg.induction_e_area_mean(),
                                                           #t=caloAlg.induction_e_target_mean())
                                                           t=2.3)
    print "Induction E amp mean: {m}\ttarget: {t}".format(m=caloAlg.induction_e_amp_mean(),
                                                          #t=caloAlg.induction_e_target_mean())
                                                          t=2.3)
    print "Collection E area mean: {m}\ttarget: {t}".format(m=caloAlg.collection_e_area_mean(),
                                                            #t=caloAlg.collection_e_target_mean())
                                                            t=2.3)
    print "Collection E amp mean: {m}\ttarget: {t}".format(m=caloAlg.collection_e_amp_mean(),
                                                           #t=caloAlg.collection_e_target_mean())
                                                           t=2.3)

    # return the correction factors:
    area_q_corrections = [caloAlg.induction_q_area_mean()/caloAlg.induction_q_target_mean(),
                          caloAlg.collection_q_area_mean()/caloAlg.collection_q_target_mean()]
    amp_q_corrections = [caloAlg.induction_q_amp_mean()/caloAlg.induction_q_target_mean(),
                         caloAlg.collection_q_amp_mean()/caloAlg.collection_q_target_mean()]
    # area_e_corrections = [caloAlg.induction_e_area_mean()/caloAlg.induction_e_target_mean(),
    #                       caloAlg.collection_e_area_mean()/caloAlg.collection_e_target_mean()]
    # amp_e_corrections = [caloAlg.induction_e_amp_mean()/caloAlg.induction_e_target_mean(),
    #                      caloAlg.collection_e_amp_mean()/caloAlg.collection_e_target_mean()]
    area_e_corrections = [caloAlg.induction_e_area_mean()/2.3,
                          caloAlg.collection_e_area_mean()/2.3]
    amp_e_corrections = [caloAlg.induction_e_amp_mean()/2.3,
                         caloAlg.collection_e_amp_mean()/2.3]


    return area_q_corrections, amp_q_corrections, area_e_corrections, amp_e_corrections

if __name__ == '__main__':

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

    area_e_constants = [222.7151820208505, 59.975150719617496]
    amp_e_constants  = [222.01797957837144, 59.63675122858065]

    worstCorrection = 0.5

    it = 0
    while worstCorrection > target_error:

        corrections = runCaloMuons(amp_q_constants,
                                   area_q_constants,
                                   amp_e_constants,
                                   area_e_constants,
                                   f)

        area_q_corrections = corrections[0]
        amp_q_corrections = corrections[1]
        area_e_corrections = corrections[2]
        amp_e_corrections = corrections[3]

        # if abs(area_q_corrections[0] - 1.0) > worstCorrection:
        worstCorrection = abs(area_q_corrections[0] - 1.0)
        if abs(area_q_corrections[1] - 1.0) > worstCorrection:
            worstCorrection = abs(area_q_corrections[1] - 1.0)
        if abs(amp_q_corrections[0] - 1.0) > worstCorrection:
            worstCorrection = abs(amp_q_corrections[0] - 1.0)
        if abs(amp_q_corrections[1] - 1.0) > worstCorrection:
            worstCorrection = abs(amp_q_corrections[1] - 1.0)

        print "Worst correction factor is ", worstCorrection

        # print "Correction factors:"
        # print area_q_corrections
        # print amp_q_corrections
        # print area_e_corrections
        # print amp_e_corrections

        area_q_constants[0] = area_q_constants[0] / area_q_corrections[0]
        area_q_constants[1] = area_q_constants[1] / area_q_corrections[1]

        amp_q_constants[0] = amp_q_constants[0] / amp_q_corrections[0]
        amp_q_constants[1] = amp_q_constants[1] / amp_q_corrections[1]

        it += 1

        if (it > 5):
            break





    worstCorrection = 0.5

    it = 0
    while worstCorrection > target_error:

        corrections = runCaloMuons(amp_q_constants,
                                   area_q_constants,
                                   amp_e_constants,
                                   area_e_constants,
                                   f)

        area_q_corrections = corrections[0]
        amp_q_corrections = corrections[1]
        area_e_corrections = corrections[2]
        amp_e_corrections = corrections[3]


        # if abs(area_q_corrections[0] - 1.0) > worstCorrection:
        worstCorrection = abs(area_e_corrections[0] - 1.0)
        if abs(area_e_corrections[1] - 1.0) > worstCorrection:
            worstCorrection = abs(area_e_corrections[1] - 1.0)
        if abs(amp_e_corrections[0] - 1.0) > worstCorrection:
            worstCorrection = abs(amp_e_corrections[0] - 1.0)
        if abs(amp_e_corrections[1] - 1.0) > worstCorrection:
            worstCorrection = abs(amp_e_corrections[1] - 1.0)

        print "Worst correction factor is ", worstCorrection


        area_e_constants[0] = area_e_constants[0] / area_e_corrections[0]
        area_e_constants[1] = area_e_constants[1] / area_e_corrections[1]

        amp_e_constants[0] = amp_e_constants[0] / amp_e_corrections[0]
        amp_e_constants[1] = amp_e_constants[1] / amp_e_corrections[1]

        it += 1

        if (it > 5):
            break

    print "Results of q constants:"
    print "AREA: ", area_q_constants
    print "AMP:  ", amp_q_constants
    print "Results of e constants:"
    print "AREA: ", area_e_constants
    print "AMP:  ", amp_e_constants