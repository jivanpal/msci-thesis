#!/bin/bash

debug=0

# Echo usage of this script to STDERR
usage() {
    >&2 echo "Usage: $0 <file to compile> <output file>"
}

# Get a random MD5 hash with 128 bits of entropy
get_random_hash() {
    (dd if=/dev/urandom bs=16 count=1 | md5sum | cut -d' ' -f1) 2> /dev/null
}

tmp_dir="/tmp/com.jivanpal.compile-tex"

# Get a file path to use as a temporary store of data
get_tmp_file_path() {
    echo "${tmp_dir}/$(get_random_hash)"
}

# Replace given line of given file with the contents of another given file.
# This action is destructive, permanently replacing the file in place!
replace_line_with_file() {
    file_to_modify=$1
    line_number=$2
    file_to_insert=$3

    sed -i '' "$line_number {
        r $file_to_insert
        d
    }" $file_to_modify
}

regex='^%__INSERT ".*"$'

# Read input from STDIN, replace lines of the form `%__INSERT "file.txt"`
# with the contents of `file.txt`, then output the result to STDOUT.
evaluate_insert_macros() {
    file_to_evaluate=$(get_tmp_file_path)
    cp /dev/stdin $file_to_evaluate

    if [[ debug -eq 1 ]] ; then
        >&2 echo "Evaluating file at \`$file_to_evaluate\` ..."
    fi

    OLD_IFS=$IFS
    IFS=$'\n'

    # For each INSERT line, in reverse order ...
    for macro_line in $(grep -n $regex $file_to_evaluate | tail -r); do
        line_number=$(cut -d: -f1 <<< $macro_line)
        file_to_insert=$(cut -d' ' -f2 <<< $macro_line | sed -e 's/^.//' -e 's/.$//')

        if [[ debug -eq 1 ]] ; then
            >&2 echo "Inserting file \`$file_to_insert\` at line $line_number ..."
        fi

        if [[ ! -f $file_to_insert ]] ; then
            >&2 echo "File \`$file_to_insert\` does not exist!"
            exit 1
        fi

        tmp_file=$(get_tmp_file_path)
        cp $file_to_insert $tmp_file
        
        if [[ debug -eq 1 ]] ; then
            >&2 echo "Copied \`$file_to_insert\` to \`$tmp_file\`."
        fi
        
        # Execution in a sub-shell is necessary here to avoid overwriting
        # existing variables in this shell's cope. Also allows us to use
        # relative paths in INSERT statements that are relative to the file
        # being inserted.
        (
            dir_name=$(dirname $file_to_insert)
            file_to_insert=$(basename $file_to_insert)

            cd $dir_name
            evaluate_insert_macros < $file_to_insert > $tmp_file
        )

        replace_line_with_file $file_to_evaluate $line_number $tmp_file
        if [[ debug -eq 1 ]] ; then
            >&2 echo "Performed replacement."
        fi
    done
    IFS=$OLD_IFS

    cat $file_to_evaluate
}

if [[ $# -ne 2 ]] ; then
    usage
    exit 1;
fi

invocation_dir=$(pwd)

mkdir -p $tmp_dir
working_file=$(get_tmp_file_path)

input_file=$1
output_file=$2
input_file_dir=$(dirname $input_file)
input_file=$(basename $input_file)

cd $input_file_dir
cp $input_file $working_file

if [[ debug -eq 1 ]] ; then
    >&2 echo "Working file is \`$working_file\`."
fi

cd $invocation_dir

evaluate_insert_macros < $working_file > $output_file

# Cleanup tmpfs
rm -rf $tmp_dir
