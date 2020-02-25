void __parseerror(int errcode, int line_no, int offset) {
	static char errstr[7][30] = {
		"NUM_EXPECTED", // Number expect
		"SYM_EXPECTED", // Symbol Expected
		"ADDR_EXPECTED", // Addressing Expected which is A/E/I/R
		"SYM_TOO_LONG", // Symbol Name is too long
		"TOO_MANY_DEF_IN_MODULE", // > 16
		"TOO_MANY_USE_IN_MODULE", // > 16
		"TOO_MANY_INSTR" 
		// total num_instr exceeds memory size (512)
	};
	printf("Parse Error line %d offset %d: %s\n", line_no, offset, errstr[errcode]);
}
