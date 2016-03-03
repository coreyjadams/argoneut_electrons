
inputDir="/media/cadams/ArgoneutData/larlite"
mergeDir="/media/cadams/ArgoneutData/merged"
filterDir="/media/cadams/ArgoneutData/filtered"

argoDir="/home/cadams/larlite/UserDev/argoneut_electrons"

get_files()
{
  # takes the run as an argument:
  files=$(ls ${inputDir}/R${1}*)
  echo $files
}

# Takes a run as an argument, merges and filters all of the files
merge_and_filter()
{
  get_files $1
  filteredList=""
  for file in $files; do
    # Get the base filename:
    base=$(basename $file)
    rootName=$(basename -s .root $base)
    mergeName=${rootName}_merged.root
    filteredName=${rootName}_merged_filtered.root

    # This is the merging step
    python ${argoDir}/python/mergeShower.py -s ${inputDir}/${base} -o ${mergeDir}/${mergeName}

    # This is the filter step:
    python ${argoDir}/python/showerFilter.py -s ${mergeDir}/${mergeName} -o ${filterDir}/${filteredName}

    filteredList="${filteredList} ${filterDir}/${filteredName}"

    echo $rootName
    echo $mergeName
    echo $filteredName
  done

  # Now, merge all the filtered files together:
  hadd R${1}_filtered_all.root ${filteredList}
}


# Set up larlite:
source /home/cadams/larlite/config/setup_reco.sh
# Set up the argoneut tools:
source /home/cadams/larlite/UserDev/argoneut_electrons/argo_setup.sh


merge_and_filter 622