#!/bin/bash

function getsrcdir(){
    for element in `ls $1`
    do  
        dir_or_file=$1"/"$element
        if [ -d $dir_or_file ]
        then 
            getsrcdir $dir_or_file
        else
            fileName=${dir_or_file##*/}
            filetype=${dir_or_file##*.}
            dirpath=$1
            if [ $filetype == "js" ]; then 
                filenameNoSub=`basename $fileName .js`
                mv $dir_or_file encrypt.js
                zip $filenameNoSub.zip encrypt.js
                changezip $filenameNoSub.zip
                xxtea $filenameNoSub.zip $filenameNoSub.jsc
                rm -rf encrypt.js
                rm -rf $filenameNoSub.zip
            fi
        fi  
    done
}

cd src
getsrcdir "."
cd ../..
echo "encode src success"
