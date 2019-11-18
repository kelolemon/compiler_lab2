rdcheck: rdcheck.c lex.yy.c
	clang rdcheck.c lex.yy.c -o rdcheck

rdparser: rdparser.c lex.yy.c
	clang rdparser.c lex.yy.c -o rdparser

lex.yy.c: rdlex.l
	flex rdlex.l
