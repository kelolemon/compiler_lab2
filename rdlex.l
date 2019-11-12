%{
    int a = 1;
%}

/* Definitions, node: \042 is '"'*/
INTEGER             ([0-9]+)
UNTERM_STRING       (\042[^\042\n]*)
STRING              (\042[^\042\n]*\042)
IDENTIFIER          ([_a-zA-Z][_a-zA-Z0-9]*)
OPERATOR            ([\*\-\+\<\>\=\!\%\/\^])
DELIMITER           ([,;:\.\(\)\{}\[\]\?])
SINGLE_COMMENT1     ("//"[^\n]*)
SINGLE_COMMENT2     ("#"[^\n]*)
SINGLE_COMMENT3     ([/][*][^*]*[*]+([^*/][^*]*[*]+)*[/])
SINGLE_COMMENT4     ("@"[^\n]*)
FLOAT               (^[1-9]\d*\.\d*|0\.\d*[1-9]\d*$)

/* rule */
%%

[\n]                { return 119;                           }
[ \t\r\a]+          { return 119;                           }
{SINGLE_COMMENT1}   { /* skip for single line comment */    }
{SINGLE_COMMENT2}   { /* skip for single line commnet */    }
{SINGLE_COMMENT3}   { /*skip */                             }
{SINGLE_COMMENT4}   { /* skip @ commet  */                  }
"+" |
"-" |
"*" |
"/" |
">" |
"<" |
"%" |
"=" |
";" |
"," |
"(" |
")" |
"[" |
"]" |
"{" |
"}"                 { return yytext[0];                     }
"=="                { return 102;                           }
"!="                { return 103;                           }
">="                { return 104;                           }
"<="                { return 105;                           }
{INTEGER}           { return 106;                           }
{FLOAT}             { return 106;                           }
{STRING}            { return 107;                           }
"+="                { return 108;                           }
"-="                { return 109;                           }
"*="                { return 110;                           }
"/="                { return 111;                           }
"%="                { return 112;                           }
"int"               { return 113;                           }
"string"            { return 114;                           }
"void"              { return 115;                           }
"if"                { return 116;                           }
"else"              { return 117;                           }
"while"             { return 118;                           }
"print"             { return 121;                           }
"scan"              { return 122;                           }
"return"            { return 120;                           }
{IDENTIFIER}        { return 2;                             }
%%

int yywrap(void){
    return 1;
}
