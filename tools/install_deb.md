<center> <font face=red size=5> Linux 系统下安装 deb 文件库 </font> </center>
[TOC]

&emsp;本文介绍在 linux ubuntu系统下安装 .deb 后缀名的库, 以CUDA为例
#### ①解压 deb 文件安装

```javascript 
sudo dpkg -i cuda-repo-<distro>_<version>_amd64.deb
sudo apt-get update
sudo apt-get install cuda
```

#### ②可能失败的情况
&emsp;假如存在 .deb 文件当中部分库安装失败的情况, 则需要切换到系统 /var 目录进入指定文件夹挨个使用 dpkg 命令安装库, 比如:
```javascript 
dpkg -i libcudnn8_8.2.1.32-1+cuda10.2_amd64.deb
```

#### ③添加至系统环境和链接目录
&emsp;假如存在多个版本的库, 可以采用链接的方式, 这样方便切换
```javascript 
sudo rm /usr/local/cuda
sudo ln -s /usr/local/cuda-<version> /usr/local/cuda
```
	
这样添加至系统环境变量时，就可以直接使用 /usr/local/cuda 的路径, 而不需要单独指定具体的版本目录
```javascript 
export PATH=$PATH:/usr/local/cuda/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/cuda/lib64
```
进入 /etc/ld.so.conf.d 目录, 添加链接文件即可, 比如新建一个 cuda 文本，内容如下
```javascript 
/usr/local/cuda/bin
/usr/local/cuda/lib64
```

