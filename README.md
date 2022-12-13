# Benchmark-for-SQLite
Implementation of SQLite benchmark program (SQLiteBench) using C++ SQLite API

### 1. How to compile the `SQLiteBench`

```
cd sqlite3Bench
mkdir build && cd build
CC=gcc CXX=g++ cmake .. -DCMAKE_BUILD_TYPE=Release
make -j
```

### 2. How to run the `SQLiteBench`
```
./sqliteBench --journal_mode=off --benchmarks=directfillrandom
```
Options for "--benchmarks"
- directfillrandom (INSERT random keys)
- directfillseq (INSERT sequential keys)
- updateuniform (INSERT then UPDATE the uniform key)
- filldeleteuniform (INSERT and DELETE the uniform key)
