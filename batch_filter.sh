folder=/data_linux/argoneut_beam_data/

for file in $(ls ${folder}/R622_E4001-E6000_larlite_merged.root); do
  echo "Processing $file"
  newFile=$(basename -s ".root" $file)_filtered.root
  echo "Output going to ${folder}/${newFile}"
  # Process the file:
  python $LARLITE_USERDEVDIR/argoneut_electrons/python/showerFilter.py --source ${file} --data-output ${folder}/$newFile
done