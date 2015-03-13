SKMiner
=======
AMD GPU Miner for the SK1024 Algorithm used in the GPU Channel of CoinShield. 


Created by Liam Russell A.K.A BitSlapper 

CSD: 2QscvkN2ddvKHhQZD9RjYS6nGXSLSSgQYJ6peCCXV8RNP8FZj2r


Optimized Kernel by djm34

CSD: 2S2pCpRXyb8Lpre52U3Xjq2MguSdaea5YGjVTsJqgZBfL2S24ag



RUNNING
--------
The config file is in /resources/config. 

It works much in the same way as sgminer.  


Settings can be passed along to the miner on startup, such as with a .bat file.

They will replace settings in the config file. 

For Example:

setx GPU_FORCE_64BIT_PTR 1
SKMiner.exe --connections 127.0.0.1:9325 user x password x --algorithm sk1024_djm

These are the only settings required to start the miner, the config file is no longer necessary.
Of course this will put you on the lowest settings and cause poor performance.

To see a full list of the optional config parameters view the file:

 /documents/config_parameters.txt


Also, there is an available Web GUI. It resides in /resourse/ui .

Simply open the SKMinerGUI.html file while the miner is running. 

The Web GUI refreshes every 5 seconds so it might take 5 to 10 seconds before you start seeing any information.
 

BUILDING
---------

You will need these libraries to build from source:

boost 1.56.0, OpenSSL, json spirit, The C++ Rest SDK

You will also need the AMD ADL files. See the readme.txt in /source/ADL_SDK 

and of course the AMD APP SDK


Windows users can build from source right now, providing you have the libraries. I built using VS2013.


Linux users may have difficulty building from source currently. I'm working on making the source fully Linux Compatible.




If you have an questions, comments, or concerns then please head over to 

http://coinshieldtalk.org/
