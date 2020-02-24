#/bin/bash

num=$(cat $@ | wc -l)
blanknum=$(egrep "^[[:space:]]*(#.*)?$" $@ | wc -l)

echo $((num - blanknum))
