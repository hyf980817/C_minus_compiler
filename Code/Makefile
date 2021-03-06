# 编译器设定和编译选项
CC = gcc
FLEX = flex
BISON = bison
CFLAGS = -std=c99

# 编译目标：src目录下的所有.c文件
CFILES = $(shell find ./ -name "*.c")
OBJS = $(CFILES:.c=.o)
LFILE = $(shell find ./ -name "*.l")
YFILE = $(shell find ./ -name "*.y")
LFC = $(shell find ./ -name "*.l" | sed s/[^/]*\\.l/lex.yy.c/)
YFC = $(shell find ./ -name "*.y" | sed s/[^/]*\\.y/syntax.tab.c/)
LFO = $(LFC:.c=.o)
YFO = $(YFC:.c=.o)

parser: syntax-c lexical
	$(CC) -o parser $(YFC) rbtree.c Translate.c InterCode.c Semantic.c -lfl -ly

parser-debug: syntax-c-debug lexical
	$(CC) -o parser $(YFC) rbtree.c Translate.c InterCode.c Semantic.c -lfl -ly -g

syntax: lexical syntax-c
	$(CC) -c $(YFC) -o $(YFO)


lexical: $(LFILE)
	$(FLEX) -o $(LFC) $(LFILE)

syntax-c: $(YFILE)
	$(BISON) -o $(YFC) -d -v $(YFILE)

syntax-c-debug: $(YFILE)
	$(BISON) -o $(YFC) -d -v -t $(YFILE)


intercodeTest: 
	$(CC) -o intercode_test InterCode.c InterCode_test.c rbtree.c

intercodeTest-debug: 
	$(CC) -o -g intercode_test InterCode.c InterCode_test.c rbtree.c
-include $(patsubst %.o, %.d, $(OBJS))

# 定义的一些伪目标
.PHONY: clean test count
test:
	./parser ../Test/test1.cmm
clean:
	rm -f parser lex.yy.c syntax.tab.c syntax.tab.h syntax.output parser.tree intercode_test out.txt
	rm -f $(OBJS) $(OBJS:.o=.d)
	rm -f $(LFC) $(YFC) $(YFC:.c=.h)
	rm -f *~

count: clean
	wc -l *.l *.y *.c *.h
