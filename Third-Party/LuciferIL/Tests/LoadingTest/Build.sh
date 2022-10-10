mkdir Build

cd Build
cmake ..
cmake --build . --target Lucifer-LoadingTest -j $1

cd ..
cd Binaries
./Lucifer-LoadingTest
cd ..