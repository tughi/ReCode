/* Copyright (C) 2024 Stefan Selariu */

#include "Parser.h"
#include "File.h"

typedef struct Parser {
    Scanner *scanner;
    Parsed_Source *parsed_source;
    uint16_t current_identation;
} Parser;

Token *Parser__peek_token(Parser *self, uint8_t offset) {
    return Scanner__peek_token(self->scanner, offset);
}

bool Parser__matches_three(Parser *self, bool (*first_is)(struct Token *token), bool first_required, bool (*second_is)(Token *token), bool second_required, bool (*third_is)(Token *token)) {
    uint8_t peek_offset = 0;
    if (first_is(Parser__peek_token(self, peek_offset))) {
        peek_offset = peek_offset + 1;
    } else if (first_required) {
        return false;
    }
    if (second_is != NULL) {
        if (second_is(Parser__peek_token(self, peek_offset))) {
            peek_offset = peek_offset + 1;
        } else if (second_required) {
            return false;
        }
    }
    if (third_is != NULL) {
        return third_is(Parser__peek_token(self, peek_offset));
    }
    return true;
}

bool Parser__matches_two(Parser *self, bool (*first_is)(Token *token), bool first_required, bool (*second_is)(Token *token)) {
    return Parser__matches_three(self, first_is, first_required, second_is, true, NULL);
}

bool Parser__matches_one(Parser *self, bool (*first_is)(Token *token)) {
    return Parser__matches_two(self, first_is, true, NULL);
}

bool Parser__matches_end_of_line(Parser *self) {
    return Parser__matches_three(self, Token__is_space, false, Token__is_comment, false, Token__is_end_of_line);
}

Token *Parser__consume_token(Parser *self, bool (*check)(Token *token)) {
    if (Parser__matches_one(self, check)) {
        Token *token = self->scanner->current_token;
        Scanner__next_token(self->scanner);
        return token;
    }
    pWriter__begin_location_message(stderr_writer, self->scanner->current_token->location, WRITER_STYLE__ERROR);
    pWriter__write__cstring(stderr_writer, "Unexpected token");
    pWriter__end_location_message(stderr_writer);
    panic();
}

void Parser__consume_comment(Parser *self) {
    Parser__consume_token(self, Token__is_comment);
}

void Parser__consume_space(Parser *self, uint16_t count) {
    if (Parser__matches_one(self, Token__is_space)) {
        Space_Token *token = (Space_Token *)Parser__consume_token(self, Token__is_space);
        if (token->count != count) {
            pWriter__begin_location_message(stderr_writer, self->scanner->current_token->location, WRITER_STYLE__WARNING);
            pWriter__write__cstring(stderr_writer, "Consumed ");
            pWriter__write__int64(stderr_writer, token->count);
            pWriter__write__cstring(stderr_writer, " spaces where ");
            pWriter__write__int64(stderr_writer, count);
            pWriter__write__cstring(stderr_writer, " were expected");
            pWriter__end_location_message(stderr_writer);
        }
    } else if (count > 0) {
        pWriter__begin_location_message(stderr_writer, self->scanner->current_token->location, WRITER_STYLE__WARNING);
        pWriter__write__cstring(stderr_writer, "Consumed 0 spaces where ");
        pWriter__write__int64(stderr_writer, count);
        pWriter__write__cstring(stderr_writer, " were expected");
        pWriter__end_location_message(stderr_writer);
    }
}

void Parser__consume_end_of_line(Parser *self) {
    if (Parser__matches_two(self, Token__is_space, false, Token__is_comment)) {
        Parser__consume_space(self, 1);
        Parser__consume_comment(self);
    } else {
        Parser__consume_space(self, 0);
    }
    Token *token = Parser__consume_token(self, Token__is_end_of_line);
    if (Token__is_end_of_file(token)) {
        pWriter__begin_location_message(stderr_writer, token->location, WRITER_STYLE__WARNING);
        pWriter__write__cstring(stderr_writer, "Unexpected end of file");
        pWriter__end_location_message(stderr_writer);
    }
}

bool Parser__consume_empty_line(Parser *self) {
    if (Parser__matches_one(self, Token__is_end_of_file)) {
        return false;
    }
    if (Parser__matches_three(self, Token__is_space, false, Token__is_comment, false, Token__is_end_of_line)) {
        if (Parser__matches_two(self, Token__is_space, false, Token__is_comment)) {
            Parser__consume_space(self, self->current_identation * 4);
            Parser__consume_comment(self);
        } else {
            Parser__consume_space(self, 0);
        }
        Parser__consume_token(self, Token__is_end_of_line);
        return true;
    }
    return false;
}

Parsed_Call_Argument *Parser__parse_call_arguments(Parser *self);
Parsed_Expression *Parser__parse_expression(Parser *self);
Parsed_Type *Parser__parse_type(Parser *self);

/*
primary_expression
    | "false"
    | "null"
    | "true"
    | "make" type "(" call_arguments? ")"
    | CHARACTER
    | IDENTIFIER
    | INTEGER ( "i8" | "i16" | "i32" | "i64" | "isize" | "u8" | "u16" | "u32" | "u64" | "usize" )?
    | STRING
*/
Parsed_Expression *Parser__parse_primary_expression(Parser *self) {
    if (Parser__matches_one(self, Token__is_make)) {
        Source_Location *location = Parser__consume_token(self, Token__is_make)->location;
        Parser__consume_space(self, 1);
        Parsed_Type *type = Parser__parse_type(self);
        Parser__consume_space(self, 0);
        Parser__consume_token(self, Token__is_opening_paren);
        Parsed_Call_Argument *first_call_argument = Parser__parse_call_arguments(self);
        Parser__consume_space(self, 0);
        Parser__consume_token(self, Token__is_closing_paren);
        return (Parsed_Expression *)Parsed_Make_Expression__create(location, type, first_call_argument);
    }
    if (Parser__matches_one(self, Token__is_null)) {
        return (Parsed_Expression *)Parsed_Null_Expression__create(Parser__consume_token(self, Token__is_null));
    }
    if (Parser__matches_one(self, Token__is_false)) {
        return (Parsed_Expression *)Parsed_Bool_Expression__create(Parser__consume_token(self, Token__is_false), false);
    }
    if (Parser__matches_one(self, Token__is_true)) {
        return (Parsed_Expression *)Parsed_Bool_Expression__create(Parser__consume_token(self, Token__is_true), true);
    }
    if (Parser__matches_one(self, Token__is_identifier)) {
        return (Parsed_Expression *)Parsed_Symbol_Expression__create(Parser__consume_token(self, Token__is_identifier));
    }
    if (Parser__matches_one(self, Token__is_integer)) {
        Integer_Token *integer_token = (Integer_Token *)Parser__consume_token(self, Token__is_integer);
        Parsed_Named_Type *integer_type = NULL;
        Token *next_token = Parser__peek_token(self, 0);
        if (Token__is_identifier(next_token)) {
            if (next_token->lexeme->data[0] == 'i' || next_token->lexeme->data[0] == 'u') {
                if (next_token->lexeme->length == 2 && next_token->lexeme->data[1] == '8') {
                    integer_type = (Parsed_Named_Type *)Parsed_Named_Type__create(Parser__consume_token(self, Token__is_identifier));
                } else if (next_token->lexeme->length == 3) {
                    if (next_token->lexeme->data[1] == '1' && next_token->lexeme->data[2] == '6') {
                        integer_type = (Parsed_Named_Type *)Parsed_Named_Type__create(Parser__consume_token(self, Token__is_identifier));
                    } else if (next_token->lexeme->data[1] == '3' && next_token->lexeme->data[2] == '2') {
                        integer_type = (Parsed_Named_Type *)Parsed_Named_Type__create(Parser__consume_token(self, Token__is_identifier));
                    } else if (next_token->lexeme->data[1] == '6' && next_token->lexeme->data[2] == '4') {
                        integer_type = (Parsed_Named_Type *)Parsed_Named_Type__create(Parser__consume_token(self, Token__is_identifier));
                    }
                } else if (next_token->lexeme->length == 5) {
                    if (next_token->lexeme->data[1] == 's' && next_token->lexeme->data[2] == 'i' && next_token->lexeme->data[3] == 'z' && next_token->lexeme->data[4] == 'e') {
                        integer_type = (Parsed_Named_Type *)Parsed_Named_Type__create(Parser__consume_token(self, Token__is_identifier));
                    }
                }
            }
        }
        return (Parsed_Expression *)Parsed_Integer_Expression__create(integer_token, integer_type);
    }
    if (Parser__matches_one(self, Token__is_character)) {
        return (Parsed_Expression *)Parsed_Character_Expression__create((Character_Token *)Parser__consume_token(self, Token__is_character));
    }
    if (Parser__matches_one(self, Token__is_string)) {
        return (Parsed_Expression *)Parsed_String_Expression__create((String_Token *)Parser__consume_token(self, Token__is_string));
    }
    if (Parser__matches_one(self, Token__is_opening_paren)) {
        Source_Location *location = Parser__consume_token(self, Token__is_opening_paren)->location;
        Parser__consume_space(self, 0);
        Parsed_Expression *expression = Parser__parse_expression(self);
        Parser__consume_space(self, 0);
        Parser__consume_token(self, Token__is_closing_paren);
        return (Parsed_Expression *)Parsed_Group_Expression__create(location, expression);
    }
    pWriter__begin_location_message(stderr_writer, self->scanner->current_token->location, WRITER_STYLE__ERROR);
    pWriter__write__cstring(stderr_writer, "Unexpected token");
    pWriter__end_location_message(stderr_writer);
    panic();
}

/*
call_argument
    | ( IDENTIFIER ":" )? expression
*/
Parsed_Call_Argument *Parser__parse_call_argument(Parser *self) {
    Source_Location *argument_location = self->scanner->current_token->location;
    Identifier_Token *argument_name = NULL;
    if (Parser__matches_three(self, Token__is_identifier, true, Token__is_space, false, Token__is_colon)) {
        argument_name = (Identifier_Token *)Parser__consume_token(self, Token__is_identifier);
        Parser__consume_space(self, 0);
        Parser__consume_token(self, Token__is_colon);
        Parser__consume_space(self, 1);
    }
    Parsed_Expression *argument_expression = Parser__parse_expression(self);
    return Parsed_Call_Argument__create(argument_location, argument_name, argument_expression);
}

/*
call_arguments
    | call_argument ( ( "," | <EOL> ) call_argument )*
*/
Parsed_Call_Argument *Parser__parse_call_arguments(Parser *self) {
    Parsed_Call_Argument *first_argument = NULL;
    Parsed_Call_Argument *last_argument = NULL;

    bool multiline = false;
    int space_before_argument_list = 0;

    if (Parser__matches_end_of_line(self)) {
        Parser__consume_space(self, 0);
        multiline = true;
        self->current_identation = self->current_identation + 1;
        space_before_argument_list = self->current_identation * 4;
        while (Parser__consume_empty_line(self)) {
            /* ignored */
        }
    }

    while (!Parser__matches_two(self, Token__is_space, false, Token__is_closing_paren)) {
        Parser__consume_space(self, space_before_argument_list);

        Parsed_Call_Argument *argument = Parser__parse_call_argument(self);
        if (last_argument == NULL) {
            first_argument = argument;
        } else {
            last_argument->next_argument = argument;
        }
        last_argument = argument;

        while (Parser__matches_two(self, Token__is_space, false, Token__is_comma)) {
            Parser__consume_space(self, 0);
            Parser__consume_token(self, Token__is_comma);
            Parser__consume_space(self, 1);
            argument = Parser__parse_call_argument(self);
            last_argument->next_argument = argument;
            last_argument = argument;
        }

        if (Parser__matches_end_of_line(self)) {
            Parser__consume_end_of_line(self);
            if (!multiline) {
                pWriter__begin_location_message(stderr_writer, first_argument->location, WRITER_STYLE__WARNING);
                pWriter__write__cstring(stderr_writer, "Multi-line argument list must start on a new line");
                pWriter__end_location_message(stderr_writer);
                multiline = true;
                self->current_identation = self->current_identation + 1;
                space_before_argument_list = self->current_identation * 4;
            }
        } else if (multiline && Parser__matches_two(self, Token__is_space, false, Token__is_closing_paren)) {
            pWriter__begin_location_message(stderr_writer, self->scanner->current_token->location, WRITER_STYLE__WARNING);
            pWriter__write__cstring(stderr_writer, "Multi-line argument list must end on a new line");
            pWriter__end_location_message(stderr_writer);
            multiline = false;
            self->current_identation = self->current_identation - 1;
        }

        if (multiline) {
            while (Parser__consume_empty_line(self)) {
                /* ignored */
            }
        }
    }

    if (multiline) {
        self->current_identation = self->current_identation - 1;
        Parser__consume_space(self, self->current_identation * 4);
    }

    return first_argument;
}

/*
access_expression
    | primary_expression (
        "." (
            "@" |
            "as" "(" type ")" |
            IDENTIFIER
        ) |
        "(" call_arguments ")" |
        "[" expression "]"
    )*
*/
Parsed_Expression *Parser__parse_access_expression(Parser *self) {
    Parsed_Expression *expression = Parser__parse_primary_expression(self);
    while (true) {
        Parsed_Expression *old_expression = expression;
        if (Parser__matches_two(self, Token__is_space, false, Token__is_dot)) {
            Parser__consume_space(self, 0);
            Parser__consume_token(self, Token__is_dot);
            Parser__consume_space(self, 0);
            if (Parser__matches_one(self, Token__is_at)) {
                Source_Location *location = Parser__consume_token(self, Token__is_at)->location;
                expression = (Parsed_Expression *)Parsed_Dereference_Expression__create(location, expression);
            } else if (Parser__matches_one(self, Token__is_as)) {
                Source_Location *location = Parser__consume_token(self, Token__is_as)->location;
                Parser__consume_space(self, 0);
                Parser__consume_token(self, Token__is_opening_paren);
                Parser__consume_space(self, 0);
                Parsed_Type *type = Parser__parse_type(self);
                Parser__consume_space(self, 0);
                Parser__consume_token(self, Token__is_closing_paren);
                expression = (Parsed_Expression *)Parsed_Cast_Expression__create(location, expression, type);
            } else {
                Token *name = Parser__consume_token(self, Token__is_identifier);
                expression = (Parsed_Expression *)Parsed_Member_Access_Expression__create(expression, name);
            }
        }
        if (Parser__matches_two(self, Token__is_space, false, Token__is_opening_paren)) {
            Parser__consume_space(self, 0);
            Parser__consume_token(self, Token__is_opening_paren);
            Parsed_Call_Argument *call_arguments = Parser__parse_call_arguments(self);
            Parser__consume_space(self, 0);
            Parser__consume_token(self, Token__is_closing_paren);
            expression = (Parsed_Expression *)Parsed_Call_Expression__create(expression, call_arguments);
        }
        if (Parser__matches_two(self, Token__is_space, false, Token__is_opening_bracket)) {
            Parser__consume_space(self, 0);
            Parser__consume_token(self, Token__is_opening_bracket);
            Parser__consume_space(self, 0);
            Parsed_Expression *index_expression = Parser__parse_expression(self);
            Parser__consume_space(self, 0);
            Parser__consume_token(self, Token__is_closing_bracket);
            expression = (Parsed_Expression *)Parsed_Array_Access_Expression__create(expression, index_expression);
        }
        if (old_expression == expression) {
            break;
        }
    }
    return expression;
}

/*
unary_expression
    | "-" unary_expression
    | "not" unary_expression
    | "@" unary_expression
    | "sizeof" "(" type ")"
    | access_expression
*/
Parsed_Expression *Parser__parse_unary_expression(Parser *self) {
    if (Parser__matches_one(self, Token__is_minus)) {
        Source_Location *location = Parser__consume_token(self, Token__is_minus)->location;
        Parser__consume_space(self, 0);
        Parsed_Expression *expression = Parser__parse_unary_expression(self);
        return (Parsed_Expression *)Parsed_Minus_Expression__create(location, expression);
    }
    if (Parser__matches_one(self, Token__is_not)) {
        Source_Location *location = Parser__consume_token(self, Token__is_not)->location;
        Parser__consume_space(self, 1);
        Parsed_Expression *expression = Parser__parse_unary_expression(self);
        return (Parsed_Expression *)Parsed_Not_Expression__create(location, expression);
    }
    if (Parser__matches_one(self, Token__is_at)) {
        Source_Location *location = Parser__consume_token(self, Token__is_at)->location;
        Parser__consume_space(self, 0);
        Parsed_Expression *expression = Parser__parse_unary_expression(self);
        return (Parsed_Expression *)Parsed_Address_Of_Expression__create(location, expression);
    }
    if (Parser__matches_one(self, Token__is_sizeof)) {
        Source_Location *location = Parser__consume_token(self, Token__is_sizeof)->location;
        Parser__consume_space(self, 0);
        Parser__consume_token(self, Token__is_opening_paren);
        Parser__consume_space(self, 0);
        Parsed_Type *type = Parser__parse_type(self);
        Parser__consume_space(self, 0);
        Parser__consume_token(self, Token__is_closing_paren);
        return (Parsed_Expression *)Parsed_Sizeof_Expression__create(location, type);
    }
    return Parser__parse_access_expression(self);
}

bool Token__is_mutliplication(Token *self) {
    return Token__is_asterisk(self) || Token__is_slash(self);
}

/*
multiplication
    | unary_expression ( ( "*" | "/" | "//" ) unary_expression )*
*/
Parsed_Expression *Parser__parse_multiplication_expression(Parser *self) {
    Parsed_Expression *expression = Parser__parse_unary_expression(self);
    while (Parser__matches_two(self, Token__is_space, false, Token__is_mutliplication)) {
        Parser__consume_space(self, 1);
        if (Parser__matches_one(self, Token__is_asterisk)) {
            Parser__consume_token(self, Token__is_asterisk);
            Parser__consume_space(self, 1);
            Parsed_Expression *right_expression = Parser__parse_unary_expression(self);
            expression = (Parsed_Expression *)Parsed_Multiply_Expression__create(expression, right_expression);
        } else {
            Parser__consume_token(self, Token__is_slash);
            if (Parser__matches_one(self, Token__is_slash)) {
                Parser__consume_token(self, Token__is_slash);
                Parser__consume_space(self, 1);
                Parsed_Expression *right_expression = Parser__parse_unary_expression(self);
                expression = (Parsed_Expression *)Parsed_Modulo_Expression__create(expression, right_expression);
            } else {
                Parser__consume_space(self, 1);
                Parsed_Expression *right_expression = Parser__parse_unary_expression(self);
                expression = (Parsed_Expression *)Parsed_Divide_Expression__create(expression, right_expression);
            }
        }
    }
    return expression;
}

bool Token__is_addition(Token *self) {
    return Token__is_plus(self) || Token__is_minus(self);
}

/*
addition
    | multiplication ( ( "+" | "-" ) multiplication )*
*/
Parsed_Expression *Parser__parse_addition_expression(Parser *self) {
    Parsed_Expression *expression = Parser__parse_multiplication_expression(self);
    while (Parser__matches_two(self, Token__is_space, false, Token__is_addition)) {
        Parser__consume_space(self, 1);
        if (Parser__matches_one(self, Token__is_plus)) {
            Parser__consume_token(self, Token__is_plus);
            Parser__consume_space(self, 1);
            Parsed_Expression *right_expression = Parser__parse_multiplication_expression(self);
            expression = (Parsed_Expression *)Parsed_Add_Expression__create(expression, right_expression);
        } else {
            Parser__consume_token(self, Token__is_minus);
            Parser__consume_space(self, 1);
            Parsed_Expression *right_expression = Parser__parse_multiplication_expression(self);
            expression = (Parsed_Expression *)Parsed_Substract_Expression__create(expression, right_expression);
        }
    }
    return expression;
}

/*
comparison
    | addition ( ( "<=" | "<" | ">" | ">=") addition )*
*/
Parsed_Expression *Parser__parse_comparison_expression(Parser *self) {
    Parsed_Expression *expression = Parser__parse_addition_expression(self);
    if (Parser__matches_two(self, Token__is_space, false, Token__is_less_than)) {
        Parser__consume_space(self, 1);
        Parser__consume_token(self, Token__is_less_than);
        if (Parser__matches_one(self, Token__is_equals)) {
            Parser__consume_token(self, Token__is_equals);
            Parser__consume_space(self, 1);
            Parsed_Expression *right_expression = Parser__parse_addition_expression(self);
            expression = (Parsed_Expression *)Parsed_Less_Or_Equals_Expression__create(expression, right_expression);
        } else {
            Parser__consume_space(self, 1);
            Parsed_Expression *right_expression = Parser__parse_addition_expression(self);
            expression = (Parsed_Expression *)Parsed_Less_Expression__create(expression, right_expression);
        }
    } else if (Parser__matches_two(self, Token__is_space, false, Token__is_greater_than)) {
        Parser__consume_space(self, 1);
        Parser__consume_token(self, Token__is_greater_than);
        if (Parser__matches_one(self, Token__is_equals)) {
            Parser__consume_token(self, Token__is_equals);
            Parser__consume_space(self, 1);
            Parsed_Expression *right_expression = Parser__parse_addition_expression(self);
            expression = (Parsed_Expression *)Parsed_Greater_Or_Equals_Expression__create(expression, right_expression);
        } else {
            Parser__consume_space(self, 1);
            Parsed_Expression *right_expression = Parser__parse_addition_expression(self);
            expression = (Parsed_Expression *)Parsed_Greater_Expression__create(expression, right_expression);
        }
    }
    return expression;
}

/*
equality
    | comparison ( ( "==" | "!=" ) comparison )*
*/
Parsed_Expression *Parser__parse_equality_expression(Parser *self) {
    Parsed_Expression *expression = Parser__parse_comparison_expression(self);
    if (Parser__matches_three(self, Token__is_space, false, Token__is_equals, true, Token__is_equals)) {
        Parser__consume_space(self, 1);
        Parser__consume_token(self, Token__is_equals);
        Parser__consume_token(self, Token__is_equals);
        Parser__consume_space(self, 1);
        Parsed_Expression *right_expression = Parser__parse_comparison_expression(self);
        expression = (Parsed_Expression *)Parsed_Equals_Expression__create(expression, right_expression);
    } else if (Parser__matches_three(self, Token__is_space, false, Token__is_exclamation_mark, true, Token__is_equals)) {
        Parser__consume_space(self, 1);
        Parser__consume_token(self, Token__is_exclamation_mark);
        Parser__consume_token(self, Token__is_equals);
        Parser__consume_space(self, 1);
        Parsed_Expression *right_expression = Parser__parse_comparison_expression(self);
        expression = (Parsed_Expression *)Parsed_Not_Equals_Expression__create(expression, right_expression);
    }
    return expression;
}

/*
logic_and
    | equality ( "and" equality )*
*/
Parsed_Expression *Parser__parse_logic_and_expression(Parser *self) {
    Parsed_Expression *expression = Parser__parse_equality_expression(self);
    while (Parser__matches_two(self, Token__is_space, false, Token__is_and)) {
        Parser__consume_space(self, 1);
        Parser__consume_token(self, Token__is_and);
        Parser__consume_space(self, 1);
        Parsed_Expression *right_expression = Parser__parse_equality_expression(self);
        expression = (Parsed_Expression *)Parsed_Logic_And_Expression__create(expression, right_expression);
    }
    return expression;
}

/*
logic_or
    | logic_and ( "or" logic_and )*
*/
Parsed_Expression *Parser__parse_logic_or_expression(Parser *self) {
    Parsed_Expression *expression = Parser__parse_logic_and_expression(self);
    while (Parser__matches_two(self, Token__is_space, false, Token__is_or)) {
        Parser__consume_space(self, 1);
        Parser__consume_token(self, Token__is_or);
        Parser__consume_space(self, 1);
        Parsed_Expression *right_expression = Parser__parse_logic_and_expression(self);
        expression = (Parsed_Expression *)Parsed_Logic_Or_Expression__create(expression, right_expression);
    }
    return expression;
}

/*
expression
    | logic_or
*/
Parsed_Expression *Parser__parse_expression(Parser *self) {
    return Parser__parse_logic_or_expression(self);
}

Parsed_Statement *Parser__parse_function(Parser *self, Parsed_Type *receiver_type);

/*
struct
    | "struct" IDENTIFIER "{" ( IDENTIFIER ":" type )* "}"
*/
Parsed_Statement *Parser__parse_struct(Parser *self) {
    Source_Location *struct_location = Parser__consume_token(self, Token__is_struct)->location;
    Parser__consume_space(self, 1);
    Token *struct_name = Parser__consume_token(self, Token__is_identifier);
    Parsed_Struct_Statement *struct_statement = Parsed_Struct_Statement__create(struct_location, struct_name);
    Parser__consume_space(self, 1);
    Parser__consume_token(self, Token__is_opening_brace);
    Parser__consume_end_of_line(self);
    self->current_identation = self->current_identation + 1;
    Parsed_Struct_Member *last_struct_member = NULL;
    while (!Parser__matches_two(self, Token__is_space, false, Token__is_closing_brace)) {
        if (!Parser__consume_empty_line(self)) {
            Parser__consume_space(self, self->current_identation * 4);
            Token *struct_member_name = Parser__consume_token(self, Token__is_identifier);
            Parser__consume_space(self, 0);
            Parser__consume_token(self, Token__is_colon);
            Parser__consume_space(self, 1);
            Parsed_Type *struct_member_type = Parser__parse_type(self);
            Parser__consume_end_of_line(self);
            Parsed_Struct_Member *struct_member = Parsed_Struct_Member__create(struct_member_name, struct_member_type);
            if (last_struct_member == NULL) {
                struct_statement->first_member = struct_member;
                last_struct_member = struct_member;
            } else {
                last_struct_member->next_member = struct_member;
                last_struct_member = struct_member;
            }
        }
    }
    self->current_identation = self->current_identation - 1;
    Parser__consume_space(self, self->current_identation * 4);
    Parser__consume_token(self, Token__is_closing_brace);
    return (Parsed_Statement *)struct_statement;
}

Parsed_Function_Parameter *Parser__parse_function_parameters(Parser *self, Parsed_Type *receiver_type);

/*
trait
    | "trait" IDENTIFIER "{" trait_method* "}"
*/
Parsed_Statement *Parser__parse_trait(Parser *self) {
    Source_Location *trait_location = Parser__consume_token(self, Token__is_trait)->location;
    Parser__consume_space(self, 1);
    Token *trait_name = Parser__consume_token(self, Token__is_identifier);
    Parsed_Trait_Statement *trait_statement = Parsed_Trait_Statement__create(trait_location, trait_name);
    Parser__consume_space(self, 1);
    Parser__consume_token(self, Token__is_opening_brace);
    Parser__consume_end_of_line(self);
    self->current_identation = self->current_identation + 1;
    Parsed_Trait_Method *last_trait_method = NULL;
    while (!Parser__matches_two(self, Token__is_space, false, Token__is_closing_brace)) {
        if (!Parser__consume_empty_line(self)) {
            Parser__consume_space(self, self->current_identation * 4);
            Source_Location *method_location = Parser__consume_token(self, Token__is_func)->location;
            Parser__consume_space(self, 1);
            Token *method_name = Parser__consume_token(self, Token__is_identifier);
            Parser__consume_space(self, 0);
            Parser__consume_token(self, Token__is_opening_paren);
            Parsed_Function_Parameter *first_parameter = Parser__parse_function_parameters(self, (Parsed_Type *)Parsed_Receiver_Type__create(trait_name->location));
            Parser__consume_space(self, 0);
            Parser__consume_token(self, Token__is_closing_paren);
            Parsed_Type *return_type = NULL;
            if (Parser__matches_two(self, Token__is_space, false, Token__is_minus)) {
                Parser__consume_space(self, 1);
                Parser__consume_token(self, Token__is_minus);
                Parser__consume_token(self, Token__is_greater_than);
                Parser__consume_space(self, 1);
                return_type = Parser__parse_type(self);
            }
            Parser__consume_end_of_line(self);
            Parsed_Trait_Method *trait_method = Parsed_Trait_Method__create(method_location, method_name, first_parameter, return_type);
            if (last_trait_method == NULL) {
                trait_statement->first_method = trait_method;
            } else {
                last_trait_method->next_method = trait_method;
            }
            last_trait_method = trait_method;
        }
    }
    self->current_identation = self->current_identation - 1;
    Parser__consume_space(self, self->current_identation * 4);
    Parser__consume_token(self, Token__is_closing_brace);
    return (Parsed_Statement *)trait_statement;
}

/*
union
    | "union" IDENTIFIER "{" ( type )* "}"
*/
Parsed_Statement *Parser__parse_union(Parser *self) {
    Source_Location *union_location = Parser__consume_token(self, Token__is_union)->location;
    Parser__consume_space(self, 1);
    Token *union_name = Parser__consume_token(self, Token__is_identifier);
    Parsed_Union_Statement *union_statement = Parsed_Union_Statement__create(union_location, union_name);
    Parser__consume_space(self, 1);
    Parser__consume_token(self, Token__is_opening_brace);
    Parser__consume_end_of_line(self);
    self->current_identation = self->current_identation + 1;
    Parsed_Union_Variant *last_union_variant = NULL;
    while (!Parser__matches_two(self, Token__is_space, false, Token__is_closing_brace)) {
        if (!Parser__consume_empty_line(self)) {
            Parser__consume_space(self, self->current_identation * 4);
            Parsed_Type *union_variant_type = Parser__parse_type(self);
            Parser__consume_end_of_line(self);
            Parsed_Union_Variant *union_variant = Parsed_Union_Variant__create(union_variant_type);
            if (last_union_variant == NULL) {
                union_statement->first_variant = union_variant;
            } else {
                last_union_variant->next_variant = union_variant;
            }
            last_union_variant = union_variant;
        }
    }
    self->current_identation = self->current_identation - 1;
    Parser__consume_space(self, self->current_identation * 4);
    Parser__consume_token(self, Token__is_closing_brace);
    return (Parsed_Statement *)union_statement;
}

/*
external_type
    | "external" "type" IDENTIFIER
*/
Parsed_Statement *Parser__parse_external_type(Parser *self) {
    Source_Location *location = Parser__consume_token(self, Token__is_external)->location;
    Parser__consume_space(self, 1);
    Parser__consume_token(self, Token__is_type);
    Parser__consume_space(self, 1);
    Token *name = Parser__consume_token(self, Token__is_identifier);
    return (Parsed_Statement *)Parsed_External_Type_Statement__create(location, name);
}

/*
function_parameter
    | ( "anon" | IDENTIFIER )? IDENTIFIER ":" type
*/
Parsed_Function_Parameter *Parser__parse_function_parameter(Parser *self) {
    bool anonymous = false;
    Token *label = NULL;
    Token *name = NULL;
    if (Parser__matches_three(self, Token__is_identifier, true, Token__is_space, true, Token__is_identifier)) {
        label = Parser__consume_token(self, Token__is_identifier);
        Parser__consume_space(self, 1);
        name = Parser__consume_token(self, Token__is_identifier);
        if (Token__is_anon(label)) {
            anonymous = true;
            label = NULL;
        }
    } else {
        label = name = Parser__consume_token(self, Token__is_identifier);
    }
    Parser__consume_space(self, 0);
    Parser__consume_token(self, Token__is_colon);
    Parser__consume_space(self, 1);
    Parsed_Type *type = Parser__parse_type(self);
    return Parsed_Function_Parameter__create(label, name, type);
}

/*
function_parameters
    | function_parameter ( "," function_parameter )*
*/
Parsed_Function_Parameter *Parser__parse_function_parameters(Parser *self, Parsed_Type *receiver_type) {
    Parsed_Function_Parameter *first_parameter = NULL;
    if (!Parser__matches_two(self, Token__is_space, false, Token__is_closing_paren)) {
        Parser__consume_space(self, 0);
        if (receiver_type != NULL) {
            Token *parameter_name = Parser__consume_token(self, Token__is_identifier);
            first_parameter = Parsed_Function_Parameter__create(NULL, parameter_name, receiver_type);
        } else {
            first_parameter = Parser__parse_function_parameter(self);
        }
        Parsed_Function_Parameter *last_parameter = first_parameter;
        while (Parser__matches_two(self, Token__is_space, false, Token__is_comma)) {
            Parser__consume_space(self, 0);
            Parser__consume_token(self, Token__is_comma);
            Parser__consume_space(self, 1);
            last_parameter->next_parameter = Parser__parse_function_parameter(self);
            last_parameter = last_parameter->next_parameter;
        }
    }
    return first_parameter;
}

/*
type
    | "@" type
    | "[" ( expression | "@" ) "]" type
    | IDENTIFIER
    | func "(" function_parameters? ")" ( "->" type )?
*/
Parsed_Type *Parser__parse_type(Parser *self) {
    if (Parser__matches_one(self, Token__is_at)) {
        Source_Location *location = Parser__consume_token(self, Token__is_at)->location;
        Parser__consume_space(self, 0);
        Parsed_Type *type = Parser__parse_type(self);
        return Parsed_Pointer_Type__create(type);
    }
    if (Parser__matches_one(self, Token__is_opening_bracket)) {
        Source_Location *location = Parser__consume_token(self, Token__is_opening_bracket)->location;
        Parser__consume_space(self, 0);
        if (Parser__matches_one(self, Token__is_at)) {
            Parser__consume_token(self, Token__is_at);
            Parser__consume_space(self, 0);
            Parser__consume_token(self, Token__is_closing_bracket);
            Parser__consume_space(self, 0);
            Parsed_Type *item_type = Parser__parse_type(self);
            return (Parsed_Type *)Parsed_Multi_Pointer_Type__create(location, item_type);
        }
        Parsed_Expression *size_expression = Parser__parse_expression(self);
        Parser__consume_space(self, 0);
        Parser__consume_token(self, Token__is_closing_bracket);
        Parser__consume_space(self, 0);
        Parsed_Type *item_type = Parser__parse_type(self);
        return (Parsed_Type *)Parsed_Array_Type__create(location, item_type, size_expression);
    }
    if (Parser__matches_one(self, Token__is_func)) {
        Source_Location *location = Parser__consume_token(self, Token__is_func)->location;
        Parser__consume_space(self, 1);
        Parser__consume_token(self, Token__is_opening_paren);
        Parsed_Function_Parameter *first_parameter = Parser__parse_function_parameters(self, NULL);
        Parser__consume_space(self, 0);
        Parser__consume_token(self, Token__is_closing_paren);
        Parsed_Type *return_type = NULL;
        if (Parser__matches_two(self, Token__is_space, false, Token__is_minus)) {
            Parser__consume_space(self, 1);
            Parser__consume_token(self, Token__is_minus);
            Parser__consume_token(self, Token__is_greater_than);
            Parser__consume_space(self, 1);
            return_type = Parser__parse_type(self);
        }
        return Parsed_Function_Type__create(location, first_parameter, return_type);
    }
    Token *name = Parser__consume_token(self, Token__is_identifier);
    return Parsed_Named_Type__create(name);
}

/*
variable
    | ( "external" | "let") IDENTIFIER ( ":" type )? ( "=" expression )?
*/
Parsed_Statement *Parser__parse_variable(Parser *self) {
    bool is_external;
    Source_Location *location;
    if (Parser__matches_one(self, Token__is_external)) {
        is_external = true;
        location = Parser__consume_token(self, Token__is_external)->location;
    } else {
        is_external = false;
        location = Parser__consume_token(self, Token__is_let)->location;
    }
    Parser__consume_space(self, 1);
    Token *name = Parser__consume_token(self, Token__is_identifier);
    Parsed_Type *type = NULL;
    if (Parser__matches_two(self, Token__is_space, false, Token__is_colon)) {
        Parser__consume_space(self, 0);
        Parser__consume_token(self, Token__is_colon);
        Parser__consume_space(self, 1);
        type = Parser__parse_type(self);
    }
    Parsed_Expression *expression = NULL;
    if (Parser__matches_two(self, Token__is_space, false, Token__is_equals)) {
        Parser__consume_space(self, 1);
        Parser__consume_token(self, Token__is_equals);
        Parser__consume_space(self, 1);
        expression = Parser__parse_expression(self);
    }
    return (Parsed_Statement *)Parsed_Variable_Statement__create(location, name, type, expression, is_external);
}

void Parser__parse_statements(Parser *self, Parsed_Statements *statements);

/*
block
    | "{" statements "}"
*/
Parsed_Block_Statement *Parser__parse_block_statement(Parser *self) {
    Source_Location *location = Parser__consume_token(self, Token__is_opening_brace)->location;
    Parser__consume_end_of_line(self);
    Parsed_Statements *statements = Parsed_Statements__create(false);
    self->current_identation = self->current_identation + 1;
    Parser__parse_statements(self, statements);
    self->current_identation = self->current_identation - 1;
    Parser__consume_space(self, self->current_identation * 4);
    Parser__consume_token(self, Token__is_closing_brace);
    return Parsed_Block_Statement__create(location, statements);
}

/*
function
    | "external"? "func" ( type "." )? IDENTIFIER "(" function_parameter* ")" "->" type block?
*/
Parsed_Statement *Parser__parse_function(Parser *self, Parsed_Type *receiver_type) {
    bool is_external = false;
    if (Parser__matches_one(self, Token__is_external)) {
        is_external = true;
        Parser__consume_token(self, Token__is_external);
        Parser__consume_space(self, 1);
    }
    Source_Location *location = Parser__consume_token(self, Token__is_func)->location;
    Parser__consume_space(self, 1);
    Token *name = NULL;
    if (Parser__matches_three(self, Token__is_identifier, true, Token__is_space, false, Token__is_opening_paren)) {
        name = Parser__consume_token(self, Token__is_identifier);
    } else {
        Parsed_Type *type = Parser__parse_type(self);
        Parser__consume_space(self, 0);
        Parser__consume_token(self, Token__is_dot);
        Parser__consume_space(self, 0);
        name = Parser__consume_token(self, Token__is_identifier);
        if (receiver_type != NULL) {
            pWriter__begin_location_message(stderr_writer, name->location, WRITER_STYLE__ERROR);
            pWriter__write__cstring(stderr_writer, "Function already has a receiver type");
            pWriter__end_location_message(stderr_writer);
            panic();
        }
        receiver_type = type;
    }
    Parser__consume_space(self, 0);
    Parser__consume_token(self, Token__is_opening_paren);
    Parsed_Function_Parameter *first_parameter = Parser__parse_function_parameters(self, receiver_type);
    Parser__consume_space(self, 0);
    Parser__consume_token(self, Token__is_closing_paren);
    Parsed_Type *return_type = NULL;
    if (Parser__matches_two(self, Token__is_space, false, Token__is_minus)) {
        Parser__consume_space(self, 1);
        Parser__consume_token(self, Token__is_minus);
        Parser__consume_token(self, Token__is_greater_than);
        Parser__consume_space(self, 1);
        return_type = Parser__parse_type(self);
    }
    Parsed_Statements *statements = NULL;
    if (Parser__matches_two(self, Token__is_space, false, Token__is_opening_brace)) {
        Parser__consume_space(self, 1);
        Parser__consume_token(self, Token__is_opening_brace);
        Parser__consume_end_of_line(self);
        statements = Parsed_Statements__create(false);
        self->current_identation = self->current_identation + 1;
        Parser__parse_statements(self, statements);
        self->current_identation = self->current_identation - 1;
        Parser__consume_space(self, self->current_identation * 4);
        Parser__consume_token(self, Token__is_closing_brace);
    }
    return Parsed_Function_Statement__create(location, name, receiver_type, first_parameter, return_type, statements, is_external);
}

/*
return
    | "return" expression?
*/
Parsed_Statement *Parser__parse_return_statement(Parser *self) {
    Source_Location *location = Parser__consume_token(self, Token__is_return)->location;
    Parsed_Expression *expression = NULL;
    if (!Parser__matches_end_of_line(self)) {
        Parser__consume_space(self, 1);
        expression = Parser__parse_expression(self);
    }
    return Parsed_Return_Statement__create(location, expression);
}

/*
switch
    | "switch"
        ( IDENTIFIER "in" expression "{" ( "case" type block )* "}"
        | expression "{" ( "case" expression block )* "}"
        | "{" ( "case" expression block )* "}"
        )
*/
Parsed_Statement *Parser__parse_switch_statement(Parser *self) {
    Source_Location *location = Parser__consume_token(self, Token__is_switch)->location;
    Parser__consume_space(self, 1);
    if (Parser__matches_three(self, Token__is_identifier, true, Token__is_space, true, Token__is_in)) {
        Identifier_Token *variant_alias = (Identifier_Token *)Parser__consume_token(self, Token__is_identifier);
        Parser__consume_space(self, 1);
        Parser__consume_token(self, Token__is_in);
        Parser__consume_space(self, 1);
        Parsed_Expression *expression = Parser__parse_expression(self);
        Parser__consume_space(self, 1);
        Parser__consume_token(self, Token__is_opening_brace);
        Parser__consume_end_of_line(self);
        self->current_identation = self->current_identation + 1;
        Parsed_Union_Switch_Case *first_case = NULL;
        Parsed_Union_Switch_Case *last_case = NULL;
        while (!Parser__matches_two(self, Token__is_space, false, Token__is_closing_brace)) {
            if (!Parser__consume_empty_line(self)) {
                Parser__consume_space(self, self->current_identation * 4);
                Parser__consume_token(self, Token__is_case);
                Parser__consume_space(self, 1);
                Parsed_Type *variant_type = Parser__parse_type(self);
                Parser__consume_space(self, 1);
                Parsed_Statement *variant_block = (Parsed_Statement *)Parser__parse_block_statement(self);
                Parsed_Union_Switch_Case *current_case = Parsed_Union_Switch_Case__create(variant_type, variant_block);
                if (first_case == NULL) {
                    first_case = current_case;
                } else {
                    last_case->next_case = current_case;
                }
                last_case = current_case;
            }
        }
        self->current_identation = self->current_identation - 1;
        Parser__consume_space(self, self->current_identation * 4);
        Parser__consume_token(self, Token__is_closing_brace);
        return (Parsed_Statement *)Parsed_Union_Switch_Statement__create(location, variant_alias, expression, first_case);
    }

    todo("Parse switch statement");
}

/*
break
    | "break" ";"
*/
Parsed_Statement *Parser__parse_break_statement(Parser *self) {
    Source_Location *location = Parser__consume_token(self, Token__is_break)->location;
    return Parsed_Break_Statement__create(location);
}

/*
if
    | "if" expression block ( "else" ( if |  block ) )?
*/
Parsed_Statement *Parser__parse_if_statement(Parser *self) {
    Source_Location *location = Parser__consume_token(self, Token__is_if)->location;
    Parser__consume_space(self, 1);
    Parsed_Expression *condition_expression = Parser__parse_expression(self);
    Parser__consume_space(self, 1);
    Parsed_Statement *true_statement = (Parsed_Statement *)Parser__parse_block_statement(self);
    Parsed_Statement *false_statement = NULL;
    if (Parser__matches_two(self, Token__is_space, false, Token__is_else)) {
        Parser__consume_space(self, 1);
        Parser__consume_token(self, Token__is_else);
        Parser__consume_space(self, 1);
        if (Parser__matches_one(self, Token__is_if)) {
            false_statement = Parser__parse_if_statement(self);
        } else {
            false_statement = (Parsed_Statement *)Parser__parse_block_statement(self);
        }
    }
    return Parsed_If_Statement__create(location, condition_expression, true_statement, false_statement);
}

/*
loop
    | "loop" block
*/
Parsed_Statement *Parser__parse_loop_statement(Parser *self) {
    Source_Location *location = Parser__consume_token(self, Token__is_loop)->location;
    Parser__consume_space(self, 1);
    Parsed_Statement *body_statement = (Parsed_Statement *)Parser__parse_block_statement(self);
    return Parsed_Loop_Statement__create(location, body_statement);
}

/*
while
    | "while" expression block
*/
Parsed_Statement *Parser__parse_while_statement(Parser *self) {
    Source_Location *location = Parser__consume_token(self, Token__is_while)->location;
    Parser__consume_space(self, 1);
    Parsed_Expression *condition_expression = Parser__parse_expression(self);
    Parser__consume_space(self, 1);
    Parsed_Statement *body_statement = (Parsed_Statement *)Parser__parse_block_statement(self);
    return Parsed_While_Statement__create(location, condition_expression, body_statement);
}

/*
statement
    | assignment
    | break
    | expression
    | external_type
    | function
    | if
    | loop
    | return
    | struct
    | variable
    | while
*/
Parsed_Statement *Parser__parse_statement(Parser *self) {
    Parser__consume_space(self, self->current_identation * 4);

    if (Parser__matches_two(self, Token__is_external, true, Token__is_space)) {
        if (Token__is_func(Parser__peek_token(self, 2))) {
            return Parser__parse_function(self, NULL);
        }

        if (Token__is_type(Parser__peek_token(self, 2))) {
            return Parser__parse_external_type(self);
        }

        return Parser__parse_variable(self);
    }

    if (Parser__matches_one(self, Token__is_func)) {
        return Parser__parse_function(self, NULL);
    }

    if (Parser__matches_one(self, Token__is_let)) {
        return Parser__parse_variable(self);
    }

    if (Parser__matches_one(self, Token__is_struct)) {
        return Parser__parse_struct(self);
    }

    if (Parser__matches_one(self, Token__is_trait)) {
        return Parser__parse_trait(self);
    }

    if (Parser__matches_one(self, Token__is_union)) {
        return Parser__parse_union(self);
    }

    if (Parser__matches_one(self, Token__is_if)) {
        return Parser__parse_if_statement(self);
    }

    if (Parser__matches_one(self, Token__is_loop)) {
        return Parser__parse_loop_statement(self);
    }

    if (Parser__matches_one(self, Token__is_return)) {
        return Parser__parse_return_statement(self);
    }

    if (Parser__matches_one(self, Token__is_while)) {
        return Parser__parse_while_statement(self);
    }

    if (Parser__matches_one(self, Token__is_break)) {
        return Parser__parse_break_statement(self);
    }

    if (Parser__matches_one(self, Token__is_switch)) {
        return Parser__parse_switch_statement(self);
    }

    Parsed_Expression *expresion = Parser__parse_access_expression(self);

    if (Parser__matches_two(self, Token__is_space, false, Token__is_equals)) {
        Parser__consume_space(self, 1);
        Parser__consume_token(self, Token__is_equals);
        Parser__consume_space(self, 1);
        Parsed_Expression *value_expression = Parser__parse_expression(self);
        return (Parsed_Statement *)Parsed_Assignment_Statement__create(expresion, value_expression);
    }

    return (Parsed_Statement *)Parsed_Expression_Statement__create(expresion);
}

/*
statements
    | ( statement )*
*/
void Parser__parse_statements(Parser *self, Parsed_Statements *statements) {
    while (true) {
        while (Parser__consume_empty_line(self)) {
            /* ignored */
        }

        if (statements->has_globals) {
            if (Parser__matches_three(self, Token__is_space, false, Token__is_comment, false, Token__is_end_of_line)) {
                return;
            }
        } else {
            if (Parser__matches_two(self, Token__is_space, false, Token__is_closing_brace)) {
                return;
            }
        }

        Parsed_Statement *statement = Parser__parse_statement(self);

        Parsed_Statements__append(statements, statement);

        Parser__consume_end_of_line(self);
    }
}

void Parser__parse_source(Parser *self, Source *source) {
    Scanner *other_scanner = self->scanner;

    self->scanner = Scanner__create(source);

    Parser__parse_statements(self, self->parsed_source->statements);

    Token *last_token = Parser__peek_token(self, 0);
    if (!Token__is_end_of_file(last_token)) {
        pWriter__begin_location_message(stderr_writer, last_token->location, WRITER_STYLE__ERROR);
        pWriter__write__cstring(stderr_writer, "Scanner didn't reach end of file");
        pWriter__end_location_message(stderr_writer);
        panic();
    } else if (last_token->location->column != 1) {
        pWriter__begin_location_message(stderr_writer, last_token->location, WRITER_STYLE__WARNING);
        pWriter__write__cstring(stderr_writer, "No new line at the end of file");
        pWriter__end_location_message(stderr_writer);
    }

    self->scanner = other_scanner;
}

Parsed_Source *parse(Source *source) {
    Parser parser;
    parser.scanner = NULL;
    parser.parsed_source = Parsed_Source__create();
    parser.parsed_source->first_source = source;
    parser.current_identation = 0;

    Parser__parse_source(&parser, source);

    return parser.parsed_source;
}
