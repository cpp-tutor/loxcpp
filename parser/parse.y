%class-name Parse
%parsefun-source parse.cpp
%scanner-token-function scanner.lex()
%token-path Tokens.h
%baseclass-preinclude Includes.h

%stype std::shared_ptr<SType>
%default-actions quiet

%token LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET
%token COMMA, DOT, NEWLINE

%token IDENTIFIER, STRING, NUMBER

%token CLASS, ENDCLASS, ELSE, FALSE, FN, ENDFN, FOR, ENDFOR, IF, THEN, ENDIF, NIL
%token PRINT, RETURN, SUPER, THIS, TRUE, LET, TO, WHILE, ENDWHILE, REPEAT, UNTIL
%token CASE, ENDCASE, WHEN, OF

%right EQUAL, ASSIGN
%left OR
%left AND
%nonassoc NOT_EQUAL, EQUAL_EQUAL 
%nonassoc GREATER, GREATER_EQUAL, LESS, LESS_EQUAL
%left MINUS, PLUS
%left SLASH, STAR
%right NOT UMINUS

%%

program		: // empty
			| program declaration { if (action_stmt) (*action_stmt)(std::dynamic_pointer_cast<Stmt>($2)); }
			;

declaration : classDecl
        	| funDecl
        	| varDecl
            | statement
			| error
			;

classDecl	: CLASS identifier optFunL ENDCLASS					{ $$ = $3; std::dynamic_pointer_cast<StmtClass>($$)->setName(get<std::string>(std::dynamic_pointer_cast<ExprLiteral>($2)->get())); }
			| CLASS identifier LESS identifier optFunL ENDCLASS	{ $$ = $5; std::dynamic_pointer_cast<StmtClass>($$)->setName(get<std::string>(std::dynamic_pointer_cast<ExprLiteral>($2)->get())); std::dynamic_pointer_cast<StmtClass>($$)->setSuper(std::make_shared<ExprVariable>(get<std::string>(std::dynamic_pointer_cast<ExprLiteral>($4)->get()))); }
			;

optFunL		: /* empty */	{ $$ = std::make_shared<StmtClass>(); }
			| funList
			;

funList		: funList funDecl	{ $$ = $1; std::dynamic_pointer_cast<StmtClass>($$)->addMethod(std::dynamic_pointer_cast<StmtFunction>($2)); }
			| funDecl			{ $$ = std::make_shared<StmtClass>(); std::dynamic_pointer_cast<StmtClass>($$)->addMethod(std::dynamic_pointer_cast<StmtFunction>($1)); }
			;

funDecl		: FN identifier LEFT_PAREN optParamL RIGHT_PAREN blockStmts ENDFN	{ $$ = $4; std::dynamic_pointer_cast<StmtFunction>($$)->setName(get<std::string>(std::dynamic_pointer_cast<ExprLiteral>($2)->get())); std::dynamic_pointer_cast<StmtFunction>($$)->setBody(std::dynamic_pointer_cast<StmtBlock>($6)); }
			| LET identifier LEFT_PAREN optParamL RIGHT_PAREN TO expression		{ $$ = $4; std::dynamic_pointer_cast<StmtFunction>($$)->setName(get<std::string>(std::dynamic_pointer_cast<ExprLiteral>($2)->get())); std::dynamic_pointer_cast<StmtFunction>($$)->setBody(std::make_shared<StmtBlock>(std::vector<std::shared_ptr<Stmt>>{ std::make_shared<StmtReturn>(std::dynamic_pointer_cast<Expr>($7)) })); }
			;

optParamL	: /* empty */	{ $$ = std::make_shared<StmtFunction>(); }
			| paramList
			;

paramList	: paramList COMMA identifier	{ $$ = $1; std::dynamic_pointer_cast<StmtFunction>($$)->addParam(get<std::string>(std::dynamic_pointer_cast<ExprLiteral>($3)->get())); }
			| identifier					{ $$ = std::make_shared<StmtFunction>(); std::dynamic_pointer_cast<StmtFunction>($$)->addParam(get<std::string>(std::dynamic_pointer_cast<ExprLiteral>($1)->get())); }
			;

varDecl		: LET identifier ASSIGN expression	{ $$ = std::make_shared<StmtVariable>(get<std::string>(std::dynamic_pointer_cast<ExprLiteral>($2)->get()), std::dynamic_pointer_cast<Expr>($4)); }
			| LET identifier EQUAL expression	{ $$ = std::make_shared<StmtVariable>(get<std::string>(std::dynamic_pointer_cast<ExprLiteral>($2)->get()), std::dynamic_pointer_cast<Expr>($4), true); }
			;

statement   : exprStmt
            | forStmt
            | ifStmt
            | printStmt
            | returnStmt 
        	| whileStmt
			| caseStmt
			| repeatStmt
			;

block		: /* empty */	{ $$ = std::make_shared<StmtBlock>(); }
			| blockStmts
			;

blockStmts	: blockStmts declaration	{ $$ = $1; std::dynamic_pointer_cast<StmtBlock>($$)->append(std::dynamic_pointer_cast<Stmt>($2)); }
			| declaration				{ $$ = std::make_shared<StmtBlock>(); std::dynamic_pointer_cast<StmtBlock>($$)->append(std::dynamic_pointer_cast<Stmt>($1)); }
			;

exprStmt	: expression 	{ $$ = std::make_shared<StmtExpression>(std::dynamic_pointer_cast<Expr>($1)); }
			;

printStmt	: PRINT expression 	{ $$ = std::make_shared<StmtPrint>(std::dynamic_pointer_cast<Expr>($2)); }
			;

returnStmt	: RETURN optExpr 	{ $$ = std::make_shared<StmtReturn>($2 ? std::dynamic_pointer_cast<Expr>($2) : std::make_shared<ExprLiteral>(std::monostate{})); }
			;

ifStmt		: IF expression THEN blockStmts ENDIF					{ $$ = std::make_shared<StmtIf>(std::dynamic_pointer_cast<Expr>($2), std::dynamic_pointer_cast<Stmt>($4), nullptr); }
			| IF expression THEN blockStmts ELSE blockStmts ENDIF	{ $$ = std::make_shared<StmtIf>(std::dynamic_pointer_cast<Expr>($2), std::dynamic_pointer_cast<Stmt>($4), std::dynamic_pointer_cast<Stmt>($6)); }
			;

whileStmt	: WHILE expression blockStmts ENDWHILE	{ $$ = std::make_shared<StmtWhile>(std::dynamic_pointer_cast<Expr>($2), std::dynamic_pointer_cast<Stmt>($3)); }
			;

forStmt		: FOR ternary blockStmts ENDFOR {
													auto expr = std::dynamic_pointer_cast<ExprTernary>($2);
													auto [ loIncl, hiIncl ] = expr->getIncl();
													auto outerBlock = std::make_shared<StmtBlock>();
													outerBlock->append(std::make_shared<StmtVariable>(expr->getName(), loIncl ? expr->getLo() : std::make_shared<ExprBinary>(PLUS, expr->getLo(), std::make_shared<ExprLiteral>(1.0))));
													auto innerBlock = std::make_shared<StmtBlock>(std::dynamic_pointer_cast<StmtBlock>($3)->get());
													innerBlock->append(std::make_shared<StmtExpression>(std::make_shared<ExprAssign>(expr->getName(), std::make_shared<ExprBinary>(PLUS, std::make_shared<ExprVariable>(expr->getName()), std::make_shared<ExprLiteral>(1.0)))));
													outerBlock->append(std::make_shared<StmtWhile>(std::make_shared<ExprBinary>(hiIncl ? LESS_EQUAL : LESS, std::make_shared<ExprVariable>(expr->getName()), expr->getHi()), innerBlock));
													$$ = outerBlock;
												}
			;

caseStmt	: CASE expression OF whenList ENDCASE 					{ $$ = $4; std::dynamic_pointer_cast<StmtCase>($$)->setCaseOf(std::dynamic_pointer_cast<Expr>($2)); }
			| CASE expression OF whenList ELSE blockStmts ENDCASE	{ $$ = $4; std::dynamic_pointer_cast<StmtCase>($$)->setCaseOf(std::dynamic_pointer_cast<Expr>($2)); std::dynamic_pointer_cast<StmtCase>($$)->setElse(std::dynamic_pointer_cast<StmtBlock>($6)); }
			;

whenList	: whenList WHEN expression blockStmts	{ $$ = $1; std::dynamic_pointer_cast<StmtCase>($$)->addWhen(std::dynamic_pointer_cast<Expr>($3), std::dynamic_pointer_cast<StmtBlock>($4)); }
			| WHEN expression blockStmts			{ $$ = std::make_shared<StmtCase>(); std::dynamic_pointer_cast<StmtCase>($$)->addWhen(std::dynamic_pointer_cast<Expr>($2), std::dynamic_pointer_cast<StmtBlock>($3)); }
			;

repeatStmt	: REPEAT blockStmts UNTIL expression	{ $$ = std::make_shared<StmtRepeat>(std::dynamic_pointer_cast<StmtBlock>($2), std::dynamic_pointer_cast<Expr>($4)); }
			;

expression	: assignment
			| unary
			| binary
			| ternary
			| logical
			| primary
			| call
			;

assignment	: identifier ASSIGN expression 				{ $$ = std::make_shared<ExprAssign>(get<std::string>(std::dynamic_pointer_cast<ExprLiteral>($1)->get()), std::dynamic_pointer_cast<Expr>($3)); }
			| primary DOT identifier ASSIGN expression	{ $$ = std::make_shared<ExprSet>(std::dynamic_pointer_cast<Expr>($1), get<std::string>(std::dynamic_pointer_cast<ExprLiteral>($3)->get()), std::dynamic_pointer_cast<Expr>($5)); }
			| call DOT identifier ASSIGN expression		{ $$ = std::make_shared<ExprSet>(std::dynamic_pointer_cast<Expr>($1), get<std::string>(std::dynamic_pointer_cast<ExprLiteral>($3)->get()), std::dynamic_pointer_cast<Expr>($5)); }
			;

unary		: NOT expression						{ $$ = std::make_shared<ExprUnary>(NOT, std::dynamic_pointer_cast<Expr>($2)); }
			| MINUS expression %prec UMINUS			{ $$ = std::make_shared<ExprUnary>(MINUS, std::dynamic_pointer_cast<Expr>($2)); }
			;

binary		: expression EQUAL expression %prec EQUAL_EQUAL	{ $$ = std::make_shared<ExprBinary>(EQUAL_EQUAL, std::dynamic_pointer_cast<Expr>($1), std::dynamic_pointer_cast<Expr>($3)); }
			| expression NOT_EQUAL expression		{ $$ = std::make_shared<ExprBinary>(NOT_EQUAL, std::dynamic_pointer_cast<Expr>($1), std::dynamic_pointer_cast<Expr>($3)); }
			| expression LESS expression			{ $$ = std::make_shared<ExprBinary>(LESS, std::dynamic_pointer_cast<Expr>($1), std::dynamic_pointer_cast<Expr>($3)); }
			| expression LESS_EQUAL expression		{ $$ = std::make_shared<ExprBinary>(LESS_EQUAL, std::dynamic_pointer_cast<Expr>($1), std::dynamic_pointer_cast<Expr>($3)); }
			| expression GREATER expression			{ $$ = std::make_shared<ExprBinary>(GREATER, std::dynamic_pointer_cast<Expr>($1), std::dynamic_pointer_cast<Expr>($3)); }
			| expression GREATER_EQUAL expression	{ $$ = std::make_shared<ExprBinary>(GREATER_EQUAL, std::dynamic_pointer_cast<Expr>($1), std::dynamic_pointer_cast<Expr>($3)); }
			| expression PLUS expression			{ $$ = std::make_shared<ExprBinary>(PLUS, std::dynamic_pointer_cast<Expr>($1), std::dynamic_pointer_cast<Expr>($3)); }
			| expression MINUS expression			{ $$ = std::make_shared<ExprBinary>(MINUS, std::dynamic_pointer_cast<Expr>($1), std::dynamic_pointer_cast<Expr>($3)); }
			| expression STAR expression			{ $$ = std::make_shared<ExprBinary>(STAR, std::dynamic_pointer_cast<Expr>($1), std::dynamic_pointer_cast<Expr>($3)); }
			| expression SLASH expression			{ $$ = std::make_shared<ExprBinary>(SLASH, std::dynamic_pointer_cast<Expr>($1), std::dynamic_pointer_cast<Expr>($3)); }
			;

ternary		: expression LESS identifier LESS expression				{ $$ = std::make_shared<ExprTernary>(std::dynamic_pointer_cast<Expr>($1), false, get<std::string>(std::dynamic_pointer_cast<ExprLiteral>($3)->get()), false, std::dynamic_pointer_cast<Expr>($5)); }
			| expression LESS identifier LESS_EQUAL expression			{ $$ = std::make_shared<ExprTernary>(std::dynamic_pointer_cast<Expr>($1), false, get<std::string>(std::dynamic_pointer_cast<ExprLiteral>($3)->get()), true, std::dynamic_pointer_cast<Expr>($5)); }
			| expression LESS_EQUAL identifier LESS expression			{ $$ = std::make_shared<ExprTernary>(std::dynamic_pointer_cast<Expr>($1), true, get<std::string>(std::dynamic_pointer_cast<ExprLiteral>($3)->get()), false, std::dynamic_pointer_cast<Expr>($5)); }
			| expression LESS_EQUAL identifier LESS_EQUAL expression	{ $$ = std::make_shared<ExprTernary>(std::dynamic_pointer_cast<Expr>($1), true, get<std::string>(std::dynamic_pointer_cast<ExprLiteral>($3)->get()), true, std::dynamic_pointer_cast<Expr>($5)); }
			;

logical     : expression OR expression              { $$ = std::make_shared<ExprLogical>(OR, std::dynamic_pointer_cast<Expr>($1), std::dynamic_pointer_cast<Expr>($3)); }
			| expression AND expression             { $$ = std::make_shared<ExprLogical>(AND, std::dynamic_pointer_cast<Expr>($1), std::dynamic_pointer_cast<Expr>($3)); }
			;

primary		: identifier 							{ $$ = std::make_shared<ExprVariable>(get<std::string>(std::dynamic_pointer_cast<ExprLiteral>($1)->get())); }
 			| NUMBER 	 							{ $$ = std::make_shared<ExprLiteral>(std::stod(scanner.matched())); }
			| STRING     							{ $$ = std::make_shared<ExprLiteral>(scanner.matched().substr(1, scanner.matched().size() - 2)); }
			| TRUE		 							{ $$ = std::make_shared<ExprLiteral>(true); }
			| FALSE		 							{ $$ = std::make_shared<ExprLiteral>(false); }
			| NIL		 							{ $$ = std::make_shared<ExprLiteral>(std::monostate{}); }
			| LEFT_PAREN expression RIGHT_PAREN		{ $$ = std::make_shared<ExprGrouping>(std::dynamic_pointer_cast<Expr>($2)); }
			| THIS									{ $$ = std::make_shared<ExprThis>(); }
			| SUPER	DOT identifier					{ $$ = std::make_shared<ExprSuper>(get<std::string>(std::dynamic_pointer_cast<ExprLiteral>($3)->get())); }
			;

identifier	: IDENTIFIER	{ $$ = std::make_shared<ExprLiteral>(scanner.matched()); }
			;

call		: primary getList	{ $$ = $2; }
			;

optExprL	: /* empty */	{ $$ = std::make_shared<ExprCall>(); }
			| exprList
			;

exprList	: exprList COMMA expression		{ $$ = $1; std::dynamic_pointer_cast<ExprCall>($$)->appendArg(std::dynamic_pointer_cast<Expr>($3)); }
			| expression					{ $$ = std::make_shared<ExprCall>(); std::dynamic_pointer_cast<ExprCall>($$)->appendArg(std::dynamic_pointer_cast<Expr>($1)); }
			;

getList		: getList DOT identifier					{ $$ = std::make_shared<ExprGet>(std::dynamic_pointer_cast<Expr>($1), get<std::string>(std::dynamic_pointer_cast<ExprLiteral>($3)->get())); }
			| getList LEFT_PAREN optExprL RIGHT_PAREN	{ $$ = std::dynamic_pointer_cast<ExprCall>($3); std::dynamic_pointer_cast<ExprCall>($$)->setCallee(std::dynamic_pointer_cast<Expr>($1)); }
			| DOT identifier							{ $$ = std::make_shared<ExprGet>(std::dynamic_pointer_cast<Expr>($0), get<std::string>(std::dynamic_pointer_cast<ExprLiteral>($2)->get())); }
			| LEFT_PAREN optExprL RIGHT_PAREN			{ $$ = std::dynamic_pointer_cast<ExprCall>($2); std::dynamic_pointer_cast<ExprCall>($$)->setCallee(std::dynamic_pointer_cast<Expr>($0)); }
			;

optExpr		: /* empty */	{ $$ = nullptr; }
			| expression
			;
