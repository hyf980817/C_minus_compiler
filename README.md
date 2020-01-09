# Introduction
- 本项目是编译原理课程的实习作业.
- 项目的目的是编写一个支持C语言语法子集的编译器.
- 本项目大体上是跟随南京大学许畅老师的编译原理课程的实习项目要求.
	原网页: https://cs.nju.edu.cn/changxu/2_compiler/index.html
- 本项目的代码仅供参考.
- 环境: Ubuntu 18.04. 要求安装gcc,flex,bison.

# 文件结构
	/Lab
        /.vscode
        launch.json
		setting.json
	    /Code
		Makefile
		lexical.l
		syntax.y
		Tree.h
		InterCode.c
		InterCode.h
		rbtree.c
		Translate.c
		InterCode_test.c
		  .
	    /Test
		test1.cmm
		test2.cmm
		  .
		  .
	    /README

Code目录：	
1. 用于存放所有flex，bison，C语言源文件和头文件以及Makefile；   
2. 使用make parser-debug或make parser来生成实习一的parser.
	
Test目录：	
1. 用于存放测试文件（如果有输出文件的话）；
2. 测试文件以".cmm"作为后缀名。

.vscode目录: 
用于通过VS code调试该项目
调试步骤: 
- VS code 安装扩展C/C++(如果是使用WSL, 还需安装Remote-WSL, 通过VS code打开项目根目录) 
- 修改.vscode目录下的launch.json中的program,cwd分别为parser和Code文件夹的绝对路径
- args为运行时输入的参数
- 在Code目录下执行make parser-debug
- VS code下按F5即可开始调试

测试:
- Code目录下执行make parser
- Code目录下执行: ./parser \[测试文件路径\]
	如: ./parser ../Test/test1.cmm

输入和输出:
- 调用程序时第一个参数即为输入程序的路径 如: ./parser ../Test/test1.cmm
- 如果有错误,会直接输出到控制台
- 如果无错误, 语法树打印结果输出到parser.tree文件中, 中间代码翻译结果输出到out.txt中