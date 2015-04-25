echo Compiling yvEIO. . .
g++ yvEIO.cpp -pthread -lssl -lcrypto -o yvEIO
echo Compiling yvECCLI. . .
g++ yvECCLI.cpp -lpthread -lssl -lcrypto -o yvECCLI
echo Compiling yvECServer. . .
g++ yvECServer.cpp -lpthread -lssl -lcrypto -o yvECServer
echo All Done!
