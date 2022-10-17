#!/bin/bash

# terminate script
die() {
	echo "$1" >&2
	echo
	exit 1
}


python/pydebug.sh || die "pydebug.sh failed"
rm *.so

make clean
rm -f dist/* valgrind.log
test -d ./venv && rm -r ./venv

make pyslow5 || die "make pyslow5 failed"
python3.7 -m venv ./venv || die "Failed to create virtual environment"
source ./venv/bin/activate || die "Failed to activate virtual environment"
pip install --upgrade pip || die "Failed to update pip"
pip install dist/*.tar.gz
python3 python/example.py || die "Failed to run example.py"
python3 -m unittest -v python/test.py || die "Failed to run test.py"
valgrind --log-file=valgrind.log --leak-check=full --track-origins=yes --suppressions=python/valgrind-python.supp python3 python/example.py || die "Failed to run example.py under valgrind"
tail valgrind.log || die "Failed to tail valgrind.log"
def_lost=$(grep "definitely lost:" valgrind_lazymt_1.log | awk '{print $4}')
ind_lost=$(grep "indirectly lost:" valgrind_lazymt_1.log | awk '{print $4}')
if [ $def_lost -gt 40 ] || [ $ind_lost -gt 0 ]; then
    die "Memory leak detected"
fi
rm -r ./venv

python3.7 -m venv ./venv || die "Failed to create virtual environment"
source ./venv/bin/activate || die "Failed to activate virtual environment"
pip install --upgrade pip || die "Failed to update pip"
pip3 install twine || die "Failed to install twine"
twine check dist/* || die "Failed to check dist/*"
rm -r ./venv

echo "All tests passed"