echo "Cleaning Builds"

cd ../Tests


cd CompileTest
bash Clean.sh
cd ..

cd LoadingTest
bash Clean.sh
cd ..

cd WritingTest
bash Clean.sh
cd ..


cd ../Tools
