
if [ -z ${LARLITE_BASEDIR+x} ]; then 
  echo "Must set up larlite to use this!";
  return 
fi

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
 DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
 SOURCE="$(readlink "$SOURCE")"
 [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

# This section extends the path and python path to run from anywhere
# export DIR=$LARLITE_USERDEVDIR/DisplayTool/

export PYTHONPATH=$DIR/:$PYTHONPATH