# include <stdio.h>
# include <stdlib.h>

// definition of the type from yy lex.
# define not_find 404
# define end 0
# define ID 2
# define add '+'
# define minus '-'
# define bigger '>'
# define smaller '<'
# define equal 102
# define unequal 103
# define not_smaller 104
# define not_bigger 105
# define multiply '*'
# define divide '/'
# define mod '%'
# define number 106
# define string 107
# define assign_equal '='
# define plus_equal 108
# define minus_equal 109
# define multiply_equal 110
# define divide_equal 111
# define mod_equal 112
# define INT 113
# define STR 114
# define VOID 115
# define IF 116
# define ELSE 117
# define WHILE 118
# define BLANK 119
# define RETURN 120
# define PRINT 121
# define SCAN 122
# define debug 0

extern int yylex();
extern char* yytext;
extern FILE* yyin;
extern FILE* yyout;

int check_program();
int check_external_decl();
int check_type();
int check_declarator();
int check_declarator_list();
int check_decl_or_stat();
int check_instr_list();
int check_initializer();
int check_stat_list();
int check_para_list();
int check_para();
int check_stat();
int check_expr_stat();
int check_expr();
int check_cmp_expr();
int check_add_expr();
int check_mul_expr();
int check_primary_expr();
int check_expr_list();
int check_id_list();
void advance();

int token_type;

void advance(){
    for (token_type = yylex();token_type == BLANK; token_type = yylex());
    if (token_type == not_find){
        puts("error");
        exit(0);
    } else if (token_type == end){
        // exit(0);
    }
    fprintf(yyout, "toke_type%d: %s\n", token_type, yytext);
}


/*
 * program
    : external_declaration
    | program external_declaration
    ;
 */

int check_program(){
    if (check_external_decl()){
        if (token_type == end) {
            return 1;
        } else {
            return check_program();
        }
    } else {
        return 0;
    }
}

/*
 * external_declaration
    : type declarator decl_or_stmt
    ;
 */

int check_external_decl(){
    if (check_type()){
        if (check_declarator()){
            if (check_decl_or_stat()){
                return 1;
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

/*
 *decl_or_stmt
    : '{' statement_list '}'
    | '{' '}'
    | ',' declarator_list ';'
    | ';'
    ;
 */

int check_decl_or_stat(){
    if (token_type == '{') {
        advance();

        if (token_type == '}') {
            advance();
            return 1;
        }

        if (check_stat_list()){
            if (token_type == '}') {
                advance();
                return 1;
            } else {
                return 0;
            }
        }
        return 0;
    } else {
        if (token_type == ',') {
            advance();
            if (check_declarator_list()){
                if (token_type == ';') {
                    advance();
                    return 1;
                } else {
                    return 0;
                }
            } else {
                return 0;
            }
        } else {
            if (token_type == ';'){
                advance();
                return 1;
            } else {
                return 0;
            }
        }
    }
}

/*
 * declarator_list
    : declarator
    | declarator_list ',' declarator
    ;
 */

int check_declarator_list(){
    if (check_declarator()){
        if (token_type == ',') {
            advance();
            return check_declarator_list();
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

/*
 * intstr_list
    : initializer
    | intstr_list ',' initializer
    ;
 */

int check_instr_list(){
    if (check_initializer()){
        if (token_type == ','){
            advance();
            return check_instr_list();
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

/*
 * initializer
    : NUMBER
    | STRING
    ;
 */

int check_initializer(){
    if (token_type == number) return 1;
    if (token_type == string) return 1;
    return 0;
}

/*
 * declarator
    : ID
    | ID '=' expr
    | ID '(' parameter_list ')'
    | ID '(' ')'
    | ID '[' expr ']'
    | ID '[' ']'
    | ID '[' expr ']' '=' '{' intstr_list '}'
    | ID '[' ']' '=' '{' intstr_list '}'
    ;

 */

int check_declarator(){
    if (token_type == ID) {
        advance();
        if (token_type == '=') {
            advance();
            if (check_expr()){
                return 1;
            }
            return 0;
        }

        if (token_type == '(') {
            advance();
            if (check_para_list()){
                if (token_type == ')') {
                    advance();
                    return 1;
                }
                return 0;
            }
            if (token_type == ')') {
                advance();
                return 1;
            }
            return 0;
        }
        if (token_type == '[') {
            advance();
            if (check_expr()) {
                if (token_type == ']') {
                    advance();
                    if (token_type == '='){
                        advance();
                        if (token_type == '{'){
                            advance();
                            if (check_instr_list()) {
                                if (token_type == '}') {
                                    advance();
                                    return 1;
                                } else {
                                    return 0;
                                }
                            } else {
                                return 0;
                            }
                        } else {
                            return 0;
                        };
                    } else {
                        return 1;
                    }
                } else {
                    return 0;
                }
            } else {
                if (token_type == ']') {
                    advance();
                    if (token_type == '=') {
                        advance();
                        if (token_type == '{') {
                            advance();
                            if (check_instr_list()) {
                                if (token_type == '}') {
                                    advance();
                                    return 1;
                                } else {
                                    return 0;
                                }
                            } else {
                                return 0;
                            }
                        } else {
                            return 0;
                        }
                    } else {
                        return 1;
                    }
                } else {
                    return 0;
                }
            }
        }
    }
    return 0;
}

/*
 * parameter_list
        : parameter
        | parameter_list ',' parameter
        ;
 */

int check_para_list(){
    if (check_para()){
        if (token_type == ',') {
            advance();
            return check_para_list();
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

/*
 * parameter
        : type ID
        ;
 */

int check_para(){
    if (check_type()){
        if (token_type == ID) {
            advance();
            return 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

/*
 * type
        : INT
        | STR
        | VOID
        ;
 */

int check_type(){
    if (token_type == INT) {
        advance();
        return 1;
    }
    if (token_type == STR) {
        advance();
        return 1;
    }
    if (token_type == VOID) {
        advance();
        return 1;
    };
    return 0;
}

/*
 * statement
    : type declarator_list ';'
    | '{' statement_list '}'
    | expr_statement
    | IF '(' expr ')' statement
    | IF '(' expr ')' statement ELSE statement
    | WHILE '(' expr ')' statement
    | RETURN ';'
    | RETURN expr ';'
    | PRINT ';
    | PRINT expr_list ';'
    | SCAN id_list ';'
    ;
 */

int check_stat(){
    if (check_type()) {
        if (check_declarator_list()) {
            if (token_type == ';') {
                advance();
                return 1;
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    }

    if (token_type == '{') {
        advance();
        if (check_stat_list()) {
            if (token_type == '}') {
                advance();
                return 1;
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    }

    if (check_expr_stat()) {
        return 1;
    }

    if (token_type == IF) {
        advance();
        if (token_type == '(') {
            advance();
            if (check_expr()){
                if (token_type == ')') {
                    advance();
                    if (check_stat()) {
                        if (token_type == ELSE) {
                            advance();
                            if (check_stat()){
                                return 1;
                            } else {
                                return 0;
                            }
                        } else {
                            return 1;
                        }
                    } else {
                        return 0;
                    }
                } else {
                    return 0;
                }
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    }

    if (token_type == WHILE) {
        advance();
        if (token_type == '(') {
            advance();
            if (check_expr()) {
                if (token_type == ')') {
                    advance();
                    if (check_stat()){
                        return 1;
                    } else {
                        return 0;
                    }
                } else {
                    return 0;
                }
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    }

    if (token_type == RETURN) {
        advance();
        if (token_type == ';') {
            advance();
            return 1;
        }
        if (check_expr()) {
            if (token_type == ';') {
                advance();
                return 1;
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    }

    if (token_type == PRINT){
        advance();
        if (token_type == ';') {
            advance();
            return 1;
        }

        if (check_expr_list()) {
            if (token_type == ';') {
                advance();
                return 1;
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    }

    if (token_type ==SCAN) {
        advance();
        if (check_id_list()){
            if (token_type == ';') {
                advance();
                return 1;
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    }
    return 0;
}

/*
 * statement_list
    : statement
    | statement_list statement
    ;
 */

int check_stat_list(){
    if (check_stat()){
        if (token_type == '}') return 1;
        return check_stat_list();
    } else {
        return 0;
    }
}

/*
 * expression_statement
    : ';'
    | expr ';'
    ;
 */

int check_expr_stat(){
    if (token_type == ';'){
        advance();
        return 1;
    } else {
        if (check_expr()){
            if (token_type == ';') {
                advance();
                return 1;
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    }
}


/*
 * expr
    : cmp_expr
    ;
 */


int check_expr(){
    return check_cmp_expr();
}

/*
 * cmp_expr
    : add_expr
    | cmp_expr CMP add_expr
    ;
 */

int check_cmp(int token){
    return token == bigger ||
        token == equal ||
        token == smaller ||
        token == unequal ||
        token == not_smaller ||
        token == not_bigger;
}

int check_cmp_expr(){
    if (check_add_expr()) {
        if (check_cmp(token_type)) {
            advance();
            return check_cmp_expr();
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

/*
 * add_expr
    : mul_expr
    | add_expr '+' mul_expr
    | add_expr '-' mul_expr
    ;

 */


int check_add_expr(){
    if (check_mul_expr()){
        if (token_type == add || token_type == minus){
            advance();
            return check_add_expr();
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

/*
 * mul_expr
    : primary_expr
    | mul_expr '*' primary_expr
    | mul_expr '/' primary_expr
    | mul_expr '%' primary_expr
    | '-' primary_expr
    ;
 */

int check_mul_expr(){
    if (token_type == minus) {
        advance();
        return check_primary_expr();
    }

    if (check_primary_expr()){
        if (token_type == multiply ||
            token_type == divide ||
            token_type == mod) {
            advance();
            return check_mul_expr();
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

/*
 * primary_expr
    : ID '(' expr_list ')'
    | ID '(' ')'
    | '(' expr ')'
    | ID
    | NUMBER
    | STRING
    | ID ASSIGN expr
    | ID '=' expr
    | ID '[' expr ']'
    | ID '[' expr ']' '=' expr
    ;
 */

int check_assign(int token){
    return token == assign_equal ||
           token == plus_equal ||
           token == minus_equal ||
           token == multiply_equal ||
           token == divide_equal ||
           token == mod_equal;
}

int check_primary_expr(){
    if (token_type == ID){
        advance();
        if (token_type == '('){
            advance();
            if (check_expr_list()){
                if (token_type == ')'){
                    advance();
                    return 1;
                } else {
                    return 0;
                }
            } else {
                if (token_type == ')') {
                    advance();
                    return 1;
                } else {
                    return 0;
                }
            }
        } else {
            if (token_type == '=') {
                advance();
                if (check_expr()){
                    return 1;
                } else {
                    return 0;
                }
            } else {
                if (token_type == '[') {
                    advance();
                    if (check_expr()){
                        if (token_type == ']') {
                            advance();
                            if (token_type == '=') {
                                advance();
                                if (check_expr()){
                                    return 1;
                                } else {
                                    return 0;
                                }
                            } else {
                                return 0;
                            }
                        } else {
                            return 1;
                        }
                    } else {
                        return 0;
                    }
                } else {
                    if (check_assign(token_type)){
                        advance();
                        if (check_expr()){
                            return 1;
                        } else {
                            return 0;
                        }
                    } else {
                        return 1;
                    }
                }
            }
        }
    } else {
        if (token_type == '('){
            advance();
            if (check_expr()){
                if (token_type == ')'){
                    advance();
                    return 1;
                } else {
                    return 0;
                }
            } else {
                return 0;
            }
        } else {
            if (token_type == number) {
                advance();
                return 1;
            } else {
                if (token_type == string) {
                    advance();
                    return 1;
                } else {
                    return 0;
                }
            }
        }
    }
}

/*
 * expr_list
    : expr
    | expr_list ',' expr
    ;
 */

int check_expr_list(){
    if (check_expr()){
        if (token_type == ','){
            advance();
            return check_expr_list();
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

/*
 * id_list
    : ID
    | id_list ',' ID
    ;
 */

int check_id_list(){
    if (token_type == ID){
        advance();
        if (token_type == ','){
            advance();
            return check_id_list();
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

int main(int argv, char* argc[]){
    if (argv >= 2){
        yyin = fopen(argc[1], "r");
        if (yyin == NULL) {
            printf("Can not open the file: %s\n", argc[1]);
            return 0;
        }
    }
    if (argv >= 3){
        yyout = fopen(argc[2], "w");
    }
    advance();
    if (check_program()){
        puts("the program is checked successful");
    } else {
        puts("the program can not be checked according to the grammar");
    }
}