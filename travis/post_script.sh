#!/usr/bin/env bash
pushd build

echo -en "travis_fold:start:\e[0;33m Makeing coverage \e[0m\r"
make coverage
return_code=$?
if [ ${return_code} -ne 0 ]; then
    exit ${return_code}
fi
echo -en "travis_fold:end:\e[0;33m Makeing coverage \e[0m\r"

echo -en "travis_fold:start:\e[0;33m Uploading coverage \e[0m\r"
coveralls-lcov --repo-token ${COVERALLS_TOKEN} coverage.info
return_code=$?
if [ ${return_code} -ne 0 ]; then
    exit ${return_code}
fi
echo -en "travis_fold:end:\e[0;33m Uploading coverage \e[0m\r"

popd