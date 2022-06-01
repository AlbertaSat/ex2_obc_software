# arg1 is the author's name, arg2 is the git hash
author=$1
hash=$2

tagdate=`date +"%Y-%m-%d"`
tagstr=$tagdate.$author.${hash:0:10}

git tag $tagstr
git push origin $tagstr

