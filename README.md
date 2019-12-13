# Introduction
- 本项目是编译原理课程的实习作业.
- 项目的目的是编写一个支持C语言语法子集的编译器.
- 本项目大体上是跟随南京大学许畅老师的编译原理课程的实习项目要求.
	原网页: https://cs.nju.edu.cn/changxu/2_compiler/index.html
- 本项目的代码仅供参考.


# 文件结构
	/Lab
	    /Code
		Makefile
		lexical.l
		syntax.y
		main.c
		  .
		  .
	    /Test
		test1.cmm
		test2.cmm
		  .
		  .
	    /README

Code目录：	1. 用于存放所有flex，bison，C语言源文件和头文件以及Makefile；
		   2. 使用make parser-debug来生成实习一的parser.
	
Test目录：	1. 用于存放测试文件（如果有输出文件的话）；
		2. 测试文件以".cmm"作为后缀名。



