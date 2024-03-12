# if [ $# -ne 2 ]; then
#     echo "no arguments are given"
#     exit 1
# fi

# writefile=$1
# writestr=$2

# # separate the file from is directory
# parent_dir=$(dirname "$writefile")
# file_name=$(basename "$writefile")

# if [ ! -d "$parent_dir" ]; then
#     `mkdir -p "$parent_dir"`
#     if [ $? -ne 0 ]; then
#     exit 1
#     fi
# fi

# cd $parent_dir | echo "$writestr" > $writefile

# exit 0
#!/bin/sh

writefile="$1"
writestr="$2"

# Check if both arguments are provided
if [ $# -ne 2 ]; then
    echo "Error: Insufficient arguments passed to the script."
    exit 1
fi

# Create the directory path if it doesn't exist
mkdir -p "$(dirname "$writefile")"

# Check if the file was successfully created
if [ $? -ne 0 ]; then
    echo "Error: File '$writefile' could not be created."
    exit 1
fi



# write given string in the created file
echo $writestr > $writefile