# In Development

## Setup

conan install .. -pr=beatspeak

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_TOOLCHAIN_PREFIX=llvm -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ ..

make

## TODO

1. Implement butterworth filter

2. build haskell test/dev server to graph data; this will be needed to debug the data progression from batch to pre PCA

3. refactor code simplify and apply a more functional approach, leaning on pure fucntions

4. Implement PCA

5. Implement Signal selection

6. Generate heartrate. 

7. TBD

8. Implemnet Haskell server with api

9. build frontend in Flutter

10. release version 1.0