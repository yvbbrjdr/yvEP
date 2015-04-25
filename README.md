# yvEncryptedProtocol

Introduction

Hello, I am yvbbrjdr, the inventor of yvEP (yvEncryptedProtocol). My aim is to write an "Internet Protocol" for programmers to use based on UDP and RSA. I want to make it the lightest and safest protocol over the world.

The project I am working on right now is yvEC (yvEncryptedChat) based on yvEP. This is going to be the safest IM software over the world. See the download list below and you will find that this project is still undone. I have just finished writing yvECCLI (yvEncryptedChatCommandLineInterface) and yvECServer but they are functional and easy to use. Try them.

Now I am going to introduce the usage of yvEP. Before you use yvEP to write a program, please make sure that you have installed the pack "libssl-dev" (only in linux right now). Copy "yvEP.hpp", "yvLinuxUDP.hpp" and "yvRSA.hpp" into your project folder and #include "yvEP.hpp" in your project. When compiling the source code, you have to use the command "g++ xxx.cpp -o xxx -lpthread -lssl -lcrypto" or ld will throw countless errors. yvEP is written in the namespace yvEP. So you can using namespace yvEP to make it more convenience.

There are only 4 functions in this protocol. The first one is Init. It has 4 arguments. The first argument is a number indicating the port you want to bind in this app. The second argument is a pointer to a function (See yvEP.hpp for details). When messages come, the function that this pointer points to will run. The third argument and the fourth argument is similar to the second one. The function that the third argument points to will run when some one requires your RSA public key. This is just a notice. You don't have to do anything because your public key will be sent back automaticly. The function that the fourth argument points to will run when receiving others's RSA public key. You can set NULL to the arguments 2, 3 and 4 so that the very function will be disabled. During the function Init, system will automaticly bind the port and generate an RSA key.

The second function is Turn. It has 2 arguments. The first argument is a const char pointer (a string). This string is a domain name or an IP address that you want to connect. The second argument is a number indicating the port of the host you want to connect. During this function, system will try to get public key from the remote host. If succeeding, this function will return true, otherwise it will return false. NOTE THAT NEVER USE THIS FUNCTION IN THE FUNCTION YOU SET IN INIT. YOU WILL FAIL. THIS IS A BUG THAT CANNOT BE REPAIRED FOREVER!

After you Turn to a host and succeed, you can use the function Send. It has only one argument, a const char pointer (a string) including the message you want to send. The message will be sent to the latest host you Turn-ed to. Max length is 240 so if you want to send a longer message please separate the message by yourself.

Finally comes the last function Close. It has no argument. Use it when you want to terminate your app. It will release the ram yvEP used.

If you don't know how to operate yvEC, just see the source code and you will understand. The default server might not be on. 
