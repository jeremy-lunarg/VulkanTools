#! /bin/bash

SITE="vulkan.gpuinfo.org"
for id in 1456 1804 1451 1999 2110 2658 2664 3242 3241 3238 3237 3232 3231 3228 3225 3235 3199 3195 3206 3182 3183 3169 3143 3165 3098 3097 3070 3069
do
    FILE="gpuinfo_$id.json"
    if [ ! -f $FILE ]
    then
        URL="https://$SITE/api/v2/devsim/getreport.php?id=$id"
        echo "Fetching record $id"
        curl -s -S -o $FILE $URL
    fi
done
