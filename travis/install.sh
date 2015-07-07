#!/usr/bin/env bash
echo -en "travis_fold:start:\e[0;33m Installing gcc-5 g++-5 cdbs cmake libboost-dev libboost-thread-dev libboost-filesystem-dev libmsgpack-dev binutils-dev libboost-all-dev aptitude valgrind \e[0m\r"
apt-get install -qq gcc-5 g++-5 cdbs cmake libboost-dev libboost-thread-dev libboost-filesystem-dev libmsgpack-dev binutils-dev libboost-all-dev aptitude valgrind ruby1.9.1 ruby1.9.1-dev rubygems1.9.1 irb1.9.1 ri1.9.1 rdoc1.9.1 build-essential libopenssl-ruby1.9.1 libssl-dev zlib1g-dev
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

sudo update-alternatives --install /usr/bin/ruby ruby /usr/bin/ruby1.9.1 400 \
         --slave   /usr/share/man/man1/ruby.1.gz ruby.1.gz \
                        /usr/share/man/man1/ruby1.9.1.1.gz \
        --slave   /usr/bin/ri ri /usr/bin/ri1.9.1 \
        --slave   /usr/bin/irb irb /usr/bin/irb1.9.1 \
        --slave   /usr/bin/rdoc rdoc /usr/bin/rdoc1.9.1
return_code=$?
if [ ${return_code} -ne 0 ]; then
    exit ${return_code}
fi

# now try
ruby --version
return_code=$?
if [ ${return_code} -ne 0 ]; then
    exit ${return_code}
fi


echo -en "travis_fold:end:\e[0;33m Updating alternatives \e[0m\r"