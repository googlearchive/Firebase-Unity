#!/bin/bash

#This script merges changes from your own unity projects back into your firebase soure directory

# Store the root
ROOT=$(cd $(dirname $0); pwd)
PARENT=$(cd ../ $(dirname $0); pwd)
FB_EMPTY_PROJECT=$PARENT/empty-project/Assets/Plugins/


echo $SRCDIR;

INDIR=$1
if [ -z "$INDIR" ]; then
  echo "Usage: $0 <plugin-directory>"
  echo "    Imports your Firebase Unity plugin changes from your project to this one <plugin-directory>."
  echo "Note: Requires git!"
  exit 1
fi

# Delete the current watchlist
rm $ROOT/file_watchlist.tmp


#Create a list of files to search for in the remote Unity project based on all the files in the current export directory
cd $FB_EMPTY_PROJECT

# In the new tools directory, create the watch list
find . -name \*.cs -print >> $ROOT/file_watchlist.tmp

cd $ROOT

#Iterate over the file_watchlist and find each file in the specified directory
while read p; do

  #Enter into the remote plugin directory
  cd $1

  #Merge the file into the firebase project
  git merge-file -p $FB_EMPTY_PROJECT/$p $FB_EMPTY_PROJECT/$p $1/$p > $FB_EMPTY_PROJECT/$p
    echo 'Merged ' $p from

  #Return to root
  cd $ROOT
done <file_watchlist.tmp

