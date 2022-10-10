echo "Building Tests"

cd ../Tests


echo "Building And Running CompileTest"
cd CompileTest
bash Build.sh
cd ..

echo "Building And Running Loading Test"
cd LoadingTest
bash Build.sh
cd ..

echo "Building And Running Writing Test"
cd WritingTest
bash Build.sh
cd ..

echo "Done With Tests, Check Console Log For Errors"
cd ../Tools
