#!/usr/bin/env bash
echo -en "travis_fold:start:\e[0;33m Installing gcc-5 g++-5 cdbs cmake libboost-dev libboost-thread-dev libboost-filesystem-dev libmsgpack-dev binutils-dev libboost-all-dev aptitude valgrind \e[0m\r"
apt-get install -qq gcc-5 g++-5 cdbs cmake libboost-dev libboost-thread-dev libboost-filesystem-dev libmsgpack-dev binutils-dev libboost-all-dev aptitude valgrind rubygems
return_code=$?
if [ ${return_code} -ne 0 ]; then
    exit ${return_code}
fi
echo -en "travis_fold:end:\e[0;33m Installing gcc-5 g++-5 cdbs cmake libboost-dev libboost-thread-dev libboost-filesystem-dev libmsgpack-dev binutils-dev libboost-all-dev aptitude valgrind \e[0m\r"

echo -en "travis_fold:start:\e[0;33m Installing cpp-coveralls\e[0m\r"
pip install cpp-coveralls
return_code=$?
if [ ${return_code} -ne 0 ]; then
    exit ${return_code}
fi
echo -en "travis_fold:end:\e[0;33m Installing cpp-coveralls\e[0m\r"

echo -en "travis_fold:start:\e[0;33m Installing lcov_1.11\e[0m\r"
wget http://ftp.de.debian.org/debian/pool/main/l/lcov/lcov_1.11.orig.tar.gz
return_code=$?
if [ ${return_code} -ne 0 ]; then
    exit ${return_code}
fi
tar xf lcov_1.11.orig.tar.gz
return_code=$?
if [ ${return_code} -ne 0 ]; then
    exit ${return_code}
fi
make -C lcov-1.11/ install
return_code=$?
if [ ${return_code} -ne 0 ]; then
    exit ${return_code}
fi
gem install coveralls-lcov
return_code=$?
if [ ${return_code} -ne 0 ]; then
    exit ${return_code}
fi
echo -en "travis_fold:end:\e[0;33m Installing lcov_1.11\e[0m\r"

echo -en "travis_fold:start:\e[0;33m Installing boost\e[0m\r"
aptitude search boost
return_code=$?
if [ ${return_code} -ne 0 ]; then
    exit ${return_code}
fi
echo -en "travis_fold:end:\e[0;33m Installing boost\e[0m\r"

echo -en "travis_fold:start:\e[0;33m Updating alternatives \e[0m\r"
update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-5 90
return_code=$?
if [ ${return_code} -ne 0 ]; then
    exit ${return_code}
fi
update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-5 90
return_code=$?
if [ ${return_code} -ne 0 ]; then
    exit ${return_code}
fi
echo -en "travis_fold:end:\e[0;33m Updating alternatives \e[0m\r"