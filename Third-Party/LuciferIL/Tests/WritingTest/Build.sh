mkdir Build

cd Build
cmake ..
cmake --build . --target Lucifer-WritingTest -j $1

cd ..
cd Binaries
./Lucifer-WritingTest
cd ..