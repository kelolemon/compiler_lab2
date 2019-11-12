rdcheck: rdcheck.c lex.yy.c
	gcc rdcheck.c lex.yy.c -o rdcheck

rdparser: rdparser.c lex.yy.c
	gcc rdparser.c lex.yy.c -o rdparser

lex.yy.c: rdlex.l
	flex rdlex.l
