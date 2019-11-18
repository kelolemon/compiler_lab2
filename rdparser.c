# include <stdio.h>
# include <stdlib.h>
# include <string.h>

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

# define token_Compound_list "Compound_list"
# define token_external_decl "Decl_Stmt"
# define token_Decl_List "Decl_List"
# define token_number "number_lvalue"
# define token_string "string_lvalue"
# define token_v "Value_varible"
# define token_v_decl "Value_Decl"
# define token_func_decl "Func_Decl"
# define token_instr_list "Instr_List"
# define token_v_list "value_list"
# define token_para "parameter"
# define token_type_id "type"
# define token_stat_list "Stmt_List"
# define token_rt_stat "Ret_Stmt"
# define token_IF_stat "If_Stmt"
# define token_While_stat "While_Stmt"
# define token_Pint_stat "Print_Stmt"
# define token_Scan_stat "Scan_Stmt"
# define token_Else_stat "Else_Stmt"
# define token_expr_stat "Expr_Stmt"
# define token_expr "Expr"
# define token_cmp_expr "Cmp_Expr"
# define token_mul_expr "Mul_Expr"
# define token_primary_expr "Primary_Expr"
# define token_add_expr "Add_Expr"
# define token_expr_list "Expr_List"
# define token_id_list "ID_List"
# define token_operator "Operator"
# define token_var_ref "Var_Ref"
# define token_func_ref "Func_Ref"

int token_type;

// define syn tree
typedef struct _ast ast;
typedef struct _ast* past;
struct _ast{
	char* token_type;
	char* value;
	past left;
	past right;
};

// extern function from rdlex.c

extern int yylex();
extern char* yytext;
extern FILE* yyin;
extern FILE* yyout;

// function definition
past new_node(char* token_type, char* value, past l, past r);
void show_tree(past node, int high);
void advance();
past build_program();
past build_external_decl();
past build_type();
past build_declarator();
past build_declarator_list();
past build_decl_or_stat();
past build_instr_list();
past build_initializer();
past build_stat_list();
past build_para_list();
past build_para();
past build_stat();
past build_expr_stat();
past build_expr();
past build_cmp_expr();
past build_add_expr();
past build_mul_expr();
past build_primary_expr();
past build_expr_list();
past build_id_list();
int check_cmp(int token);

// syn tree basic operation
past new_node(char* token_type, char* value, past l, past r){
	past node = (past) malloc(sizeof(ast));
	if (node == NULL) {
		puts("ERROR! out of memory");
		exit(0);
	}
	node->token_type = token_type;
	node->value = value;
	node->left = l;
	node->right = r;
	return node;
}

void show_tree(past node, int high){
	if (node == NULL) return;
	if (node->token_type == token_Compound_list) {
		show_tree(node->left, high);
		show_tree(node->right, high);
		return;
	}

	for (int i = 0; i < high; i++) fprintf(yyout,"\t");
	fprintf(yyout, "%s: %s\n", node->token_type, node->value);
	show_tree(node->left, high + 1);
	show_tree(node->right, high + 1);
}

void advance(){
	for (token_type = yylex(); token_type == BLANK; token_type = yylex());
}

/*
 * program
    : external_declaration
    | program external_declaration
    ;
 */

past build_program(){
	past node = build_external_decl();
	if (node) {
		if (token_type == end) {
			return node;
		}
		node = new_node(token_Compound_list, "", node, build_program());
		return node;
	}
	return node;
}

/*
 * external_declaration
    : type declarator decl_or_stmt
    ;
 */

past build_external_decl(){
	past node = build_type();
	if (node) {
		node = new_node(token_external_decl, "", node,
								new_node(token_Compound_list, "", build_declarator(),
												NULL));
		node->right->right = build_decl_or_stat();
	}
	return node;
}

/*
 *decl_or_stmt
    : '{' statement_list '}'
    | '{' '}'
    | ',' declarator_list ';'
    | ';'
    ;
 */

past build_decl_or_stat(){
	past node = NULL;
	if (token_type == '{') {
		advance();
		node = build_stat_list();
		if (node) node = new_node(token_Compound_list, "", node, NULL);
		if (token_type == '}') advance();
		return node;
	}

	if (token_type == ',') {
		advance();
		node = build_declarator_list();
		if (node) node = new_node(token_Decl_List, "", node, NULL);
		if (token_type == ';') advance();
		return node;
	}

	if (token_type == ';') {
		advance();
		return node;
	}

	return node;
}

/*
 * declarator_list
    : declarator
    | declarator_list ',' declarator
    ;
 */

past build_declarator_list() {
	past node = build_declarator();
	if (node) {
		if (token_type == ',') {
			advance();
			node = new_node(token_Compound_list, "", node, build_declarator_list());
		} else node = new_node(token_Compound_list, "", node, NULL);
	}
	return node;
}

/*
 * intstr_list
    : initializer
    | intstr_list ',' initializer
    ;
 */

past build_instr_list(){
	past node = build_initializer();
	if (node) {
		if (token_type == ','){
			advance();
			node = new_node(token_Compound_list, "", node, build_instr_list());
		} else node = new_node(token_Compound_list, "", node, NULL);
	}
	return node;
}

/*
 * initializer
    : NUMBER
    | STRING
    ;
 */

past build_initializer(){
	past node = NULL;
	if (token_type == number) {
		node = new_node(token_number, strdup(yytext), NULL, NULL);
		advance();
		return node;
	}

	if (token_type == string) {
		node = new_node(token_string, strdup(yytext), NULL, NULL);
		advance();
		return node;
	}

	return node;
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

past build_declarator(){
	past node = NULL;
	if (token_type == ID) {
		node = new_node(token_v_decl, strdup(yytext), NULL, NULL);
		advance();

		if (token_type == '=') {
			advance();
			node->right = build_expr();
			return node;
		}

		if (token_type == '(') {
			advance();
			node->token_type = token_func_decl;
			node->left = build_para_list();
			if (token_type == ')') advance();
			return node;
		}

		if (token_type == '[') {
			advance();
			node->token_type = token_v_list;
			node->left = build_expr();
			if (token_type == ']') {
				advance();
				if (token_type == '=') {
					advance();
					if (token_type == '{') {
						node->right = build_expr();
						node = node->right;
						node = new_node(token_instr_list, "", node, NULL);
						if (token_type == '}') advance();
					}
				}
			}
			return node;
		}
		return node;
	}

	return node;
}

/*
 * parameter_list
        : parameter
        | parameter_list ',' parameter
        ;
 */

past build_para_list(){
	past node = build_para();
	if (node){
		if (token_type == ',') {
			advance();
			node = new_node(token_Compound_list, "", node, build_para_list());
		} else {
			node = new_node(token_Compound_list,"", node, NULL);
		}
		return node;
	}
	return node;
}

/*
 * parameter
        : type ID
        ;
 */

past build_para(){
	past node = build_type();
	past tmp = NULL;
	if (token_type == ID) {
		tmp = new_node(token_v, strdup(yytext), NULL, NULL);
		advance();
		node = new_node(token_para, "", node, tmp);
	}
	return node;
}

/*
 * type
        : INT
        | STR
        | VOID
        ;
 */

past build_type(){
	past node = NULL;
	if (token_type != INT && token_type != STR && token_type != VOID){
		return node;
	}

	node = new_node(token_type_id, strdup(yytext), NULL, NULL);
	advance();
	return node;
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

past build_stat(){
	past node = NULL;
	if (token_type == '{') {
		advance();
		node = new_node(token_stat_list, "", build_stat_list(), NULL);
		if (token_type == '}') {
			advance();
		}
		return node;
	}

	if (token_type == IF) {
		advance();
		if (token_type == '(') {
			advance();
			node = build_expr();
			if (token_type == ')') {
				advance();
				node = new_node(token_IF_stat, "", node, build_stat());
				if (token_type == ELSE) {
					advance();
					past tmp = new_node(token_Else_stat, "", build_stat(), NULL);
					node->right->right = tmp;
				}
			}
		}
		return node;
	}

	if (token_type == WHILE){
		advance();
		if (token_type == '(') {
			advance();
			node = build_expr();
			if (token_type == ')') {
				advance();
				node = new_node(token_While_stat, "", node, build_stat());
			}
		}
		return node;
	}

	if (token_type == RETURN) {
		advance();
		if (token_type == ';') {
			advance();
			node = new_node(token_rt_stat, "", NULL, NULL);
			return node;
		}

		node = build_expr();
		if (token_type == ';') {
			advance();
			node = new_node(token_rt_stat, "", node, NULL);
			return node;
		}
	}

	if (token_type == PRINT) {
		advance();
		if (token_type == ';') {
			advance();
			node = new_node(token_Pint_stat, "", NULL, NULL);
			return node;
		}

		node = build_expr_list();
		if (token_type == ';') {
			advance();
			node = new_node(token_Pint_stat, "", node, NULL);
			return node;
		}
	}

	if (token_type == SCAN) {
		advance();
		node = build_id_list();
		if (token_type == ';'){
			advance();
			node = new_node(token_Scan_stat, "", node, NULL);
			return node;
		}
	}

	node = build_type();
	if (node){
		node = new_node(token_external_decl, "", node, build_declarator_list());
		if (token_type == ';') advance();
		return node;
	}

	node = build_expr_stat();
	if (node) {
		return node;
	}

	return node;
}

/*
 * statement_list
    : statement
    | statement_list statement
    ;
 */

past build_stat_list(){
	past node = build_stat();
	if (node) node = new_node(token_Compound_list, "", node, build_stat_list());
	return node;
}

/*
 * expression_statement
     : ';'
     | expr ';'
     ;
 */

past build_expr_stat(){
	past node = NULL;
	if (token_type == ';') {
		advance();
		node = new_node(token_expr_stat, "", NULL, NULL);
		return node;
	}

	node = build_expr();
	if (token_type == ';') {
		advance();
		node = new_node(token_expr_stat, "", node, NULL);
		return node;
	}
	return node;
}

/*
 * expr
    : cmp_expr
    ;
 */

past build_expr(){
	past node = build_cmp_expr();
	if (node == NULL) {
		return node;
	}
	return node;
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

past build_cmp_expr(){
	past node = build_add_expr();
	if (node) if (check_cmp(token_type)){
		node = new_node(token_operator, strdup(yytext), node, NULL);
		advance();
		node->right = build_cmp_expr();
	}

	return node;
}

/*
 * add_expr
    : mul_expr
    | add_expr '+' mul_expr
    | add_expr '-' mul_expr
    ;

 */

past build_add_expr(){
	past node = build_mul_expr();
	if (node) if (token_type == add || token_type == minus) {
		node = new_node(token_operator, strdup(yytext), node, NULL);
		advance();
		node->right = build_add_expr();
	}

	return node;
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

past build_mul_expr(){
	past node = NULL;
	if (token_type == minus) {
		advance();
		node = new_node(token_operator, "-", build_primary_expr(), NULL);
		return node;
	}

	node = build_primary_expr();
	if (node) {
		if (token_type == multiply ||
			token_type == divide ||
			token_type == mod) {
			node = new_node(token_operator, strdup(yytext), node, NULL);
			advance();
			node->right = build_mul_expr();
		}
	}

	return node;
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

past build_primary_expr(){
	past node = NULL;

	if (token_type == number) {
		node = new_node(token_number, strdup(yytext), NULL, NULL);
		advance();
		return node;
	}

	if (token_type == string) {
		node = new_node(token_string, strdup(yytext), NULL, NULL);
		advance();
		return node;
	}

	if (token_type == '(') {
		advance();
		node = build_expr();
		if (token_type == ')') {
			advance();
		}
		return node;
	}

	if (token_type == ID) {
		node = new_node(token_var_ref, strdup(yytext), NULL, NULL);
		advance();


		if (token_type == '=' && check_assign(token_type)) {
			node = new_node(token_operator, strdup(yytext), node, NULL);
			advance();
			node->right = build_expr();
			return node;
		}


		if (token_type == '(') {
			advance();
			node->token_type = token_func_ref;
			node->left = build_expr_list();
			if (token_type == ')') {
				advance();
			}
			return node;
		}

		if (token_type == '[') {
			advance();
			node->token_type = token_v_list;
			node->left = build_expr();
			if (token_type == ']') {
				advance();
				if (token_type == '=') {
					advance();
					node = new_node(token_operator, "=", node, build_expr());
				}
			}
			return node;
		}
		return node;
	}
	return node;
}

/*
 * expr_list
    : expr
    | expr_list ',' expr
    ;
 */

past build_expr_list(){
	past node = build_expr();
	if (node) if (token_type == ',') {
		advance();
		node = new_node(token_Compound_list, "", node, build_expr_list());
	}
	return node;
}

/*
 * id_list
    : ID
    | id_list ',' ID
    ;
 */

past build_id_list(){
	past node = NULL;
	if (token_type == ID) {
		node = new_node(token_v, strdup(yytext), NULL, NULL);
		advance();
		if (node) if (token_type == ',') {
			advance();
			node = new_node(token_Compound_list, "", node, build_id_list());
		}
	}
	return node;
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
	past root = build_program();
	show_tree(root, 0);
	return 0;
}
