typedef struct token {
	char* token_text;
	int line_no;
	int offset;
} token;

typedef struct int_token {
	int int_token_text;
	int line_no;
	int offset;
} int_token;

typedef struct symbol_struct {
	//token symbol;
	char token_text[17];
	char* err_msg;
	int used;
	int mod_no;
	int line_no;
	int offset;
	int val;
} symbol_struct;
