folders="/data_linux/argoneut_singles/ \
/data_linux/argoneut_beam_data/ \
/data_linux/argoneut_beam_sim/ \
/data_linux/argoneut_signal_sim/"

for folder in $folders; do
  
  for file in $(ls ${folder}/*reco2d_larlite.root); do
    echo "Processing $file"
    newFile=$(basename -s ".root" $file)_merged.root
    echo "Output going to ${folder}/${newFile}"
    # Process the file:
    python $LARLITE_USERDEVDIR/argoneut_electrons/python/mergeShower.py --source ${file} --data-output ${folder}/$newFile
  done

done
