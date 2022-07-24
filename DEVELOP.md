# Development information

## Test

All test cases are included under the test/ dir.  
These test code depend on gtest.

### configure option

  --enable-test

### Install gtest packages on host linux.

#### Ubuntu:
libgtest-dev googletest googletest-tools

### Run

  ./tools/run-test.sh


## gcov support

This library support gcov support build.

### configure option

  --enable-gcov

#### Ubuntu:
lcov

### Run

  ./tools/gen-test-report.sh


