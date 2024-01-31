%class-name Lex
%lex-source "lex.cpp"

number      [0-9]+(\.[0-9]+)?
identifier  [A-Za-z_][A-Za-z0-9_]*
string      \"([^\"]|\n)*\"
comment     "#".*

%%

and             return Tokens::AND;
class           return Tokens::CLASS;
endclass        return Tokens::ENDCLASS;
else            return Tokens::ELSE;
false           return Tokens::FALSE;
fn              return Tokens::FN;
endfn           return Tokens::ENDFN;
for             return Tokens::FOR;
endfor          return Tokens::ENDFOR;
if              return Tokens::IF;
endif           return Tokens::ENDIF;
nil             return Tokens::NIL;
or              return Tokens::OR;
print           return Tokens::PRINT;
return          return Tokens::RETURN;
super           return Tokens::SUPER;
this            return Tokens::THIS;
true            return Tokens::TRUE;
let             return Tokens::LET;
repeat          return Tokens::REPEAT;
until           return Tokens::UNTIL;
while           return Tokens::WHILE;
endwhile        return Tokens::ENDWHILE;
then            return Tokens::THEN;
not             return Tokens::NOT;
case            return Tokens::CASE;
endcase         return Tokens::ENDCASE;
when            return Tokens::WHEN;
of              return Tokens::OF;
{number}        return Tokens::NUMBER;
{identifier}    return Tokens::IDENTIFIER;
{string}        return Tokens::STRING;
"="             return Tokens::EQUAL;
"<-"            return Tokens::ASSIGN;
"->"            return Tokens::TO;
"("             return Tokens::LEFT_PAREN;
")"             return Tokens::RIGHT_PAREN;
"{"             return Tokens::LEFT_BRACE;
"}"             return Tokens::RIGHT_BRACE;
"["             return Tokens::LEFT_BRACKET;
"]"             return Tokens::RIGHT_BRACKET;
","             return Tokens::COMMA;
"."             return Tokens::DOT;
"-"             return Tokens::MINUS;
"+"             return Tokens::PLUS;
"/"             return Tokens::SLASH;
"*"             return Tokens::STAR;
"/="            return Tokens::NOT_EQUAL;
">"             return Tokens::GREATER;
">="            return Tokens::GREATER_EQUAL;
"<"             return Tokens::LESS;
"<="            return Tokens::LESS_EQUAL;
{comment}       accept(matched().length());
\n|\r|\t|" "    accept(1);
.               return matched()[0];
