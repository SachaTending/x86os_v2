if [ -n "$1" ]
then
    git add *
    git commit -m "$1"
    git push
    echo Pushed source code, commit message: $1
else
    echo Usage:
    echo "   $0 \"commit message\""
fi