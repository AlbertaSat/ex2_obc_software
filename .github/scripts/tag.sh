# arg should be a branch name
ref=$1
tagdate=`date +"%Y-%m-%d"`
git tag $tagdate.$ref
git push origin $tagdate.$ref

