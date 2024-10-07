# Tests

Unit tests are configured by CMake parameter `-DBRINGAUTO_TESTS=ON`
start unit tests:

```asm
cd _build
cmake -DBRINGAUTO_TESTS=ON ..
make
ctest
```

## Integration tests

Integration tests by StateSmurf are decribed in [test/smurf/README.md](test/smurf/README.md).