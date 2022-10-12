#!/bin/bash

help() {
  echo "Usage: run.sh --bin <binary dir> --script <scripts dir> --input <input dir> --output <output dir>"
  echo "--input <input dir> : directory to input files"
  echo "--output <output file> : directory to output files"
  echo "--bin <binary file> : directory to binary files"
  echo "--script <scripts dir> : directory to scripts"
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
  -b | --bin)
    BINS="$2"
    shift # past argument
    shift # past value
    ;;
  -s | --script)
    SCRIPTS="$2"
    shift # past argument
    shift # past value
    ;;
  -t | --tmp)
    TMP="$2"
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

# check all parameters are set
if [ -z "$INPUT" ]; then
  echo "input directory is not set"
  exit 1
fi
if [ -z "$BINS" ]; then
  echo "binary directory is not set"
  exit 1
fi
if [ -z "$SCRIPTS" ]; then
  echo "scripts directory is not set"
  exit 1
fi

bin_dir=${BINS}
bin_tet_labeling="${bin_dir}/tet_labeling"
bin_lspadding="${bin_dir}/lspadding_pure"
bin_label_restore="${bin_dir}/label_restore"

script_dir=${SCRIPTS}
script_flip="${script_dir}/flip_flipped.py"

input_dir=${INPUT}
echo input_dir: "$input_dir"

output_dir=${OUTPUT}
# default output dir: input_dir/out
if [ -z "$output_dir" ]; then
  output_dir=${input_dir}/out
fi
echo output_dir: "$output_dir"

tmp_dir=${TMP}
# default tmp dir output_dir/tmp
if [ -z "$tmp_dir" ]; then
  tmp_dir=${output_dir}/tmp
fi
echo tmp_dir: "$tmp_dir"

# check all bin and script files exist and are executable
if [ ! -x "$bin_tet_labeling" ]; then
  echo "Error: binary tet_labeling does not exist or is not executable (input: $bin_tet_labeling)"
  exit 1
fi
if [ ! -x "$bin_lspadding" ]; then
  echo "Error: binary lspadding does not exist or is not executable (input: $bin_lspadding)"
  exit 1
fi
if [ ! -x "$bin_label_restore" ]; then
  echo "Error: binary label_restore does not exist or is not executable (input: $bin_label_restore)"
  exit 1
fi
if [ ! -x "$script_flip" ]; then
  echo "Error: script flip_flipped does not exist or is not executable (input: $script_flip)"
  exit 1
fi

# check if packages that are needed by python script are installed
if ! python3 -c $'
from vtkmodules.all import *
import sys'; then
  echo "Error: some packages are not installed"
  exit 1
fi

# create output dir and tmp dir if they do not exist
if [ ! -d "$output_dir" ]; then
  mkdir -p "$output_dir"
fi
if [ ! -d "$tmp_dir" ]; then
  mkdir -p "$tmp_dir"
fi

task() {
  # set env: library path
  file=$1
  echo "file: $file"
  input_surface_mesh=$file
  # file name without extension and path
  file_name=${file##*/}
  file_name=${file_name%.*}
  # file_name last number
  file_name_last_number=${file_name##*_}
  # config file: ends with the same number of vtk
  config_file=$input_dir/bl_config_$file_name_last_number
  echo config_file: "$config_file"
  # test if config file exists
  if [ -f "$config_file" ]; then
    echo "config file exists"
  else
    echo "config file does not exist"
    exit
  fi
  tet_mesh_file=${tmp_dir}/${file_name}_tet.vtk
  echo "tet_mesh_file: $tet_mesh_file"
  padding_mesh_file=${tmp_dir}/${file_name}_volume.vtk
  echo "padding_mesh_file: $padding_mesh_file"
  flip_padding_mesh=${tmp_dir}/${file_name}_volume_flip.vtk
  echo "flip_padding_mesh: $flip_padding_mesh"
  index_vector_file=${tmp_dir}/${file_name}_vector.txt
  echo "index_vector_file: $index_vector_file"

  output_surface_mesh=${output_dir}/${file_name}_surface.vtk
  echo "output_surface_mesh: $output_surface_mesh"
  output_volume_mesh=${output_dir}/${file_name}_volume.vtk
  echo "output_volume_mesh: $output_volume_mesh"

  # abort if error occurs
  if ${bin_tet_labeling} --input "${input_surface_mesh}" --config "${config_file}" --output "${tet_mesh_file}" &&
    ${bin_lspadding} "${tet_mesh_file}" "${padding_mesh_file}" "${index_vector_file}" &&
    python3 ${script_flip} "${padding_mesh_file}" "${flip_padding_mesh}" &&
    ${bin_label_restore} "${input_surface_mesh}" "${flip_padding_mesh}" "${index_vector_file}" "${output_surface_mesh}" &&
    cp "${flip_padding_mesh}" "${output_volume_mesh}"; then
    exit
  fi
}

export LD_LIBRARY_PATH=${bin_dir}/../lib
for file in "$input_dir"/*.vtk; do
  task "$file" &
done

# wait for all tasks to finish
wait

echo "done"
