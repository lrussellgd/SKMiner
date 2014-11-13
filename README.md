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

However, currently you must enter all GPU devices you wish to use in the "devices" section.

If you have 3 GPU's in your system then it will look like: 

"devices" : "0,1,2"


If you wish to use the GPU settings then you much set them up for each GPU. For example:

"gpu-engine" : "1080,950,1080"


Settings can be passed along to the miner on startup, such as with a .bat file.

They will replace settings in the config file. 

However, this has not fully been tested and it's recommended to use the config file for now.



BUILDING
---------

You will need these libraries to build from source:

jansson 2.7
boost 1.56.0
OpenSSL

You will also need the AMD ADL files. See the readme.txt in /source/ADL_SDK


Windows users can build from source right now, providing you have the libraries. I built using VS2013.


Linux users may have difficulty building from source currently. I'm working on making the source fully Linux Compatible.




If you have an questions, comments, or concerns then please head over to 

http://coinshieldtalk.org/
