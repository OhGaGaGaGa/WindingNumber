#!/bin/bash

app_dir="/app"
project_dir="${app_dir}/padding"
scripts_dir="${app_dir}/scripts"
bin_dir="${project_dir}/bin"
lib_dir="${project_dir}/lib"
tmp_dir="${app_dir}/tmp"

help() {
  echo "Usage: run.sh --input <input dir> [--output <output dir>]"
  echo "--input <input dir> : directory to input files"
  echo "--output <output file> : directory to output files (default: input_dir/out)"
  echo "--help : print this help"
}

# parse command line parameters
if [ $# -lt 1 ]; then
  help
  exit 1
fi
while [[ $# -gt 0 ]]; do
  key="$1"
  case $key in
  -i | --input)
    INPUT="$2"
    shift # past argument
    shift # past value
    ;;
  -o | --output)
    OUTPUT="$2"
    shift # past argument
    shift # past value
    ;;
  -h | --help)
    help
    exit 0
    ;;
  *) # unknown option
    echo "Unknown option: $key"
    exit 1
    ;;
  esac
done

LD_LIBRARY_PATH="${lib_dir}" bash "${scripts_dir}"/run.sh --input "${INPUT}" --output "${OUTPUT}" --bin "${bin_dir}" --script "${scripts_dir}" --tmp "${tmp_dir}"
