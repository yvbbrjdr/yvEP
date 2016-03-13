mkdir build
cd build
mkdir yvEP yvEC yvECS
cd yvEP
qmake ../../yvEncryptedProtocol/yvEncryptedProtocol.pro
make
cd ../yvEC
qmake ../../yvEncryptedChat/yvEncryptedChat.pro
make
cd ../yvECS
qmake ../../yvEncryptedChatServer/yvEncryptedChatServer.pro
make
cd ..
cp lib/* /usr/lib
cp yvEC/yvEncryptedChat /usr/bin/yvEC
cp yvECS/yvEncryptedChatServer /usr/bin/yvECS
