#!/usr/bin/env bash
pushd build

echo -en "travis_fold:start:\e[0;33m CMake is building ... \e[0m\r"
cmake -DCMAKE_BUILD_TYPE=Coverage ..
return_code=$?
if [ ${return_code} -ne 0 ]; then
    exit ${return_code}
fi
echo -en "travis_fold:end:\e[0;33m CMake is building ... \e[0m\r"

echo -en "travis_fold:start:\e[0;33mBuilding project\e[0m\r"
make
return_code=$?
if [ ${return_code} -ne 0 ]; then
    exit ${return_code}
fi
echo -en "travis_fold:end:\e[0;33mBuilding project\e[0m\r"

echo -en "travis_fold:start:\e[0;33mTesting application\e[0m\r"
make test ARGS="-V"
return_code=$?
if [ ${return_code} -ne 0 ]; then
    exit ${return_code}
fi
echo -en "travis_fold:end:\e[0;33mTesting application\e[0m\r"

popd