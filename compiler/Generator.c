/* Copyright (C) 2024 Stefan Selariu */

#include "Generator.h"
#include "CDECL.h"
#include "File.h"

typedef struct Generator {
    Writer *writer;
    uint16_t identation;
} Generator;

void Generator__write_source_location(Generator *self, Source_Location *location) {
    pWriter__write__cstring(self->writer, "#line ");
    pWriter__write__int64(self->writer, (int64_t)location->line);
    pWriter__write__cstring(self->writer, " \"");
    pWriter__write__string(self->writer, location->source->file_path);
    pWriter__write__cstring(self->writer, "\"\n");
}

void Generator__generate_expression(Generator *self, Checked_Expression *expression);

void Generator__generate_add_expression(Generator *self, Checked_Add_Expression *expression) {
    Generator__generate_expression(self, expression->super.left_expression);
    pWriter__write__cstring(self->writer, " + ");
    Generator__generate_expression(self, expression->super.right_expression);
}

void Generator__generate_address_of_expression(Generator *self, Checked_Address_Of_Expression *expression) {
    pWriter__write__cstring(self->writer, "&");
    Generator__generate_expression(self, expression->super.other_expression);
}

void Generator__generate_alloc_expression(Generator *self, Checked_Alloc_Expression *expression) {
    pWriter__write__cstring(self->writer, "__alloc_");
    pWriter__write__string(self->writer, ((Checked_Named_Type *)expression->value_expression->type)->name);
    pWriter__write__cstring(self->writer, "_value(");
    Generator__generate_expression(self, expression->value_expression);
    pWriter__write__cstring(self->writer, ")");
}

void Generator__generate_array_access_expression(Generator *self, Checked_Array_Access_Expression *expression) {
    Generator__generate_expression(self, expression->array_expression);
    pWriter__write__cstring(self->writer, "[");
    Generator__generate_expression(self, expression->index_expression);
    pWriter__write__cstring(self->writer, "]");
}

void Generator__generate_bool_expression(Generator *self, Checked_Bool_Expression *expression) {
    if (expression->value) {
        pWriter__write__cstring(self->writer, "true");
    } else {
        pWriter__write__cstring(self->writer, "false");
    }
}

void Generator__generate_call_expression(Generator *self, Checked_Call_Expression *expression) {
    Generator__generate_expression(self, expression->callee_expression);
    pWriter__write__cstring(self->writer, "(");
    Checked_Call_Argument *argument = expression->first_argument;
    while (argument != NULL) {
        Generator__generate_expression(self, argument->expression);
        argument = argument->next_argument;
        if (argument != NULL) {
            pWriter__write__cstring(self->writer, ", ");
        }
    }
    pWriter__write__cstring(self->writer, ")");
}

void Generator__generate_cast_expression(Generator *self, Checked_Cast_Expression *expression) {
    pWriter__write__cstring(self->writer, "((");
    pWriter__write__cdecl(self->writer, NULL, expression->super.type);
    pWriter__write__cstring(self->writer, ") ");
    Generator__generate_expression(self, expression->other_expression);
    pWriter__write__char(self->writer, ')');
}

void pWriter__write__octal_escaped_char(Writer *writer, char value) {
    pWriter__write__char(writer, '\\');
    if (value > (char)64) {
        pWriter__write__char(writer, value / (char)64 % (char)8 + '0');
    }
    if (value > (char)8) {
        pWriter__write__char(writer, value / (char)8 % (char)8 + '0');
    }
    pWriter__write__char(writer, value % (char)8 + '0');
}

void pWriter__write__escaped_char(Writer *writer, char ch) {
    if (ch < (char)32) {
        if (ch == '\n') {
            pWriter__write__cstring(writer, "\\n");
        } else if (ch == '\t') {
            pWriter__write__cstring(writer, "\\t");
        } else {
            pWriter__write__octal_escaped_char(writer, ch);
        }
    } else if (ch < (char)127) {
        if (ch == '\"') {
            pWriter__write__cstring(writer, "\\\"");
        } else if (ch == '\'') {
            pWriter__write__cstring(writer, "\\'");
        } else if (ch == '\\') {
            pWriter__write__cstring(writer, "\\\\");
        } else {
            pWriter__write__char(writer, ch);
        }
    } else {
        pWriter__write__octal_escaped_char(writer, ch);
    }
}

void Generator__generate_character_expression(Generator *self, Checked_Character_Expression *expression) {
    pWriter__write__char(self->writer, '\'');
    pWriter__write__escaped_char(self->writer, expression->value);
    pWriter__write__char(self->writer, '\'');
}

void Generator__generate_dereference_expression(Generator *self, Checked_Dereference_Expression *expression) {
    pWriter__write__cstring(self->writer, "*");
    Generator__generate_expression(self, expression->super.other_expression);
}

void Generator__generate_divide_expression(Generator *self, Checked_Divide_Expression *expression) {
    Generator__generate_expression(self, expression->super.left_expression);
    pWriter__write__cstring(self->writer, " / ");
    Generator__generate_expression(self, expression->super.right_expression);
}

void Generator__generate_equals_expression(Generator *self, Checked_Equals_Expression *expression) {
    Generator__generate_expression(self, expression->super.left_expression);
    pWriter__write__cstring(self->writer, " == ");
    Generator__generate_expression(self, expression->super.right_expression);
}

void Generator__generate_greater_expression(Generator *self, Checked_Greater_Expression *expression) {
    Generator__generate_expression(self, expression->super.left_expression);
    pWriter__write__cstring(self->writer, " > ");
    Generator__generate_expression(self, expression->super.right_expression);
}

void Generator__generate_greater_or_equals_expression(Generator *self, Checked_Greater_Or_Equals_Expression *expression) {
    Generator__generate_expression(self, expression->super.left_expression);
    pWriter__write__cstring(self->writer, " >= ");
    Generator__generate_expression(self, expression->super.right_expression);
}

void Generator__generate_group_expression(Generator *self, Checked_Group_Expression *expression) {
    pWriter__write__cstring(self->writer, "(");
    Generator__generate_expression(self, expression->other_expression);
    pWriter__write__cstring(self->writer, ")");
}

void Generator__generate_integer_expression(Generator *self, Checked_Integer_Expression *expression) {
    pWriter__write__uint64(self->writer, expression->value);
    switch (expression->super.type->kind) {
    case CHECKED_TYPE_KIND__U32:
    case CHECKED_TYPE_KIND__U64:
        pWriter__write__cstring(self->writer, "u");
        break;
    case CHECKED_TYPE_KIND__I64:
        pWriter__write__cstring(self->writer, "l");
        break;
    }
}
void Generator__generate_is_union_variant_expression(Generator *self, Checked_Is_Union_Variant_Expression *expression) {
    Generator__generate_expression(self, expression->union_expression);
    pWriter__write__cstring(self->writer, ".variant");
    if (expression->is_not) {
        pWriter__write__cstring(self->writer, " != ");
    } else {
        pWriter__write__cstring(self->writer, " == ");
    }
    pWriter__write__int64(self->writer, expression->union_variant->index);
}

void Generator__generate_less_expression(Generator *self, Checked_Less_Expression *expression) {
    Generator__generate_expression(self, expression->super.left_expression);
    pWriter__write__cstring(self->writer, " < ");
    Generator__generate_expression(self, expression->super.right_expression);
}

void Generator__generate_less_or_equals_expression(Generator *self, Checked_Less_Or_Equals_Expression *expression) {
    Generator__generate_expression(self, expression->super.left_expression);
    pWriter__write__cstring(self->writer, " <= ");
    Generator__generate_expression(self, expression->super.right_expression);
}

void Generator__generate_logic_and_expression(Generator *self, Checked_Logic_And_Expression *expression) {
    Generator__generate_expression(self, expression->super.left_expression);
    pWriter__write__cstring(self->writer, " && ");
    Generator__generate_expression(self, expression->super.right_expression);
}

void Generator__generate_logic_or_expression(Generator *self, Checked_Logic_Or_Expression *expression) {
    Generator__generate_expression(self, expression->super.left_expression);
    pWriter__write__cstring(self->writer, " || ");
    Generator__generate_expression(self, expression->super.right_expression);
}

void Generator__generate_make_struct_expression(Generator *self, Checked_Make_Struct_Expression *expression) {
    pWriter__write__char(self->writer, '(');
    pWriter__write__cdecl(self->writer, NULL, (Checked_Type *)expression->struct_type);
    pWriter__write__cstring(self->writer, "){");
    Checked_Make_Struct_Argument *argument = expression->first_argument;
    while (argument != NULL) {
        pWriter__write__char(self->writer, '.');
        pWriter__write__string(self->writer, argument->struct_member->name);
        pWriter__write__cstring(self->writer, " = ");
        Generator__generate_expression(self, argument->expression);
        argument = argument->next_argument;
        if (argument != NULL) {
            pWriter__write__cstring(self->writer, ", ");
        }
    }
    pWriter__write__char(self->writer, '}');
}

void Generator__generate_make_union_expression(Generator *self, Checked_Make_Union_Expression *expression) {
    pWriter__write__char(self->writer, '(');
    pWriter__write__cdecl(self->writer, NULL, (Checked_Type *)expression->union_type);
    pWriter__write__cstring(self->writer, "){.variant = ");
    pWriter__write__int64(self->writer, expression->union_variant->index);
    if (expression->union_variant->index > 0) {
        pWriter__write__cstring(self->writer, ", .variant_");
        pWriter__write__int64(self->writer, expression->union_variant->index);
        pWriter__write__cstring(self->writer, " = ");
        Generator__generate_expression(self, expression->expression);
    }
    pWriter__write__char(self->writer, '}');
}

void Generator__generate_member_access_expression(Generator *self, Checked_Member_Access_Expression *expression) {
    Generator__generate_expression(self, expression->object_expression);
    if (expression->object_expression->type->kind == CHECKED_TYPE_KIND__POINTER) {
        pWriter__write__cstring(self->writer, "->");
    } else {
        pWriter__write__cstring(self->writer, ".");
    }
    pWriter__write__string(self->writer, expression->member->name);
}

void Generator__generate_minus_expression(Generator *self, Checked_Minus_Expression *expression) {
    pWriter__write__cstring(self->writer, "-");
    Generator__generate_expression(self, expression->super.other_expression);
}

void Generator__generate_modulo_expression(Generator *self, Checked_Modulo_Expression *expression) {
    Generator__generate_expression(self, expression->super.left_expression);
    pWriter__write__cstring(self->writer, " % ");
    Generator__generate_expression(self, expression->super.right_expression);
}

void Generator__generate_multiply_expression(Generator *self, Checked_Multiply_Expression *expression) {
    Generator__generate_expression(self, expression->super.left_expression);
    pWriter__write__cstring(self->writer, " * ");
    Generator__generate_expression(self, expression->super.right_expression);
}

void Generator__generate_not_expression(Generator *self, Checked_Not_Expression *expression) {
    pWriter__write__cstring(self->writer, "!");
    Generator__generate_expression(self, expression->super.other_expression);
}

void Generator__generate_not_equals_expression(Generator *self, Checked_Not_Equals_Expression *expression) {
    Generator__generate_expression(self, expression->super.left_expression);
    pWriter__write__cstring(self->writer, " != ");
    Generator__generate_expression(self, expression->super.right_expression);
}

void Generator__generate_null_expression(Generator *self, Checked_Null_Expression *expression) {
    pWriter__write__cstring(self->writer, "NULL");
}

void Generator__generate_sizeof_expression(Generator *self, Checked_Sizeof_Expression *expression) {
    pWriter__write__cstring(self->writer, "sizeof(");
    pWriter__write__cdecl(self->writer, NULL, expression->sized_type);
    pWriter__write__cstring(self->writer, ")");
}

void Generator__generate_string_expression(Generator *self, Checked_String_Expression *expression) {
    pWriter__write__cstring(self->writer, "(struct String){.data = ");
    if (expression->value->length == 0) {
        pWriter__write__int64(self->writer, 0);
    } else {
        pWriter__write__char(self->writer, '"');
        size_t index = 0;
        while (index < expression->value->length) {
            pWriter__write__escaped_char(self->writer, expression->value->data[index]);
            index = index + 1;
        }
        pWriter__write__char(self->writer, '"');
    }
    pWriter__write__cstring(self->writer, ", .length = ");
    pWriter__write__int64(self->writer, expression->value->length);
    pWriter__write__char(self->writer, '}');
}

void Generator__generate_string_length_expression(Generator *self, Checked_String_Length_Expression *expression) {
    Generator__generate_expression(self, expression->string_expression);
    if (expression->string_expression->type->kind == CHECKED_TYPE_KIND__POINTER) {
        pWriter__write__cstring(self->writer, "->");
    } else {
        pWriter__write__char(self->writer, '.');
    }
    pWriter__write__cstring(self->writer, "length");
}

void Generator__generate_subtract_expression(Generator *self, Checked_Subtract_Expression *expression) {
    Generator__generate_expression(self, expression->super.left_expression);
    pWriter__write__cstring(self->writer, " - ");
    Generator__generate_expression(self, expression->super.right_expression);
}

void Generator__generate_symbol_expression(Generator *self, Checked_Symbol_Expression *expression) {
    if (expression->symbol->kind == CHECKED_SYMBOL_KIND__UNION_SWITCH_VARIANT) {
        Checked_Union_Switch_Variant_Symbol *variant_symbol = (Checked_Union_Switch_Variant_Symbol *)expression->symbol;
        if (variant_symbol->union_expression->temp_variable_name == NULL) {
            Generator__generate_expression(self, variant_symbol->union_expression);
        } else {
            pWriter__write__string(self->writer, variant_symbol->union_expression->temp_variable_name);
        }
        if (variant_symbol->union_expression->type->kind == CHECKED_TYPE_KIND__POINTER) {
            pWriter__write__cstring(self->writer, "->");
        } else {
            pWriter__write__char(self->writer, '.');
        }
        pWriter__write__cstring(self->writer, "variant_");
        pWriter__write__int64(self->writer, variant_symbol->union_variant->index);
    } else {
        pWriter__write__string(self->writer, expression->symbol->name);
    }
}

void Generator__generate_expression(Generator *self, Checked_Expression *expression) {
    switch (expression->kind) {
    case CHECKED_EXPRESSION_KIND__ADD:
        Generator__generate_add_expression(self, (Checked_Add_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__ADDRESS_OF:
        Generator__generate_address_of_expression(self, (Checked_Address_Of_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__ALLOC:
        Generator__generate_alloc_expression(self, (Checked_Alloc_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__ARRAY_ACCESS:
        Generator__generate_array_access_expression(self, (Checked_Array_Access_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__BOOL:
        Generator__generate_bool_expression(self, (Checked_Bool_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__CALL:
        Generator__generate_call_expression(self, (Checked_Call_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__CAST:
        Generator__generate_cast_expression(self, (Checked_Cast_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__CHARACTER:
        Generator__generate_character_expression(self, (Checked_Character_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__DEREFERENCE:
        Generator__generate_dereference_expression(self, (Checked_Dereference_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__DIVIDE:
        Generator__generate_divide_expression(self, (Checked_Divide_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__EQUALS:
        Generator__generate_equals_expression(self, (Checked_Equals_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__GREATER:
        Generator__generate_greater_expression(self, (Checked_Greater_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__GREATER_OR_EQUALS:
        Generator__generate_greater_or_equals_expression(self, (Checked_Greater_Or_Equals_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__GROUP:
        Generator__generate_group_expression(self, (Checked_Group_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__INTEGER:
        Generator__generate_integer_expression(self, (Checked_Integer_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__IS_UNION_VARIANT:
        Generator__generate_is_union_variant_expression(self, (Checked_Is_Union_Variant_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__LESS:
        Generator__generate_less_expression(self, (Checked_Less_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__LESS_OR_EQUALS:
        Generator__generate_less_or_equals_expression(self, (Checked_Less_Or_Equals_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__LOGIC_AND:
        Generator__generate_logic_and_expression(self, (Checked_Logic_And_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__LOGIC_OR:
        Generator__generate_logic_or_expression(self, (Checked_Logic_Or_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__MAKE_STRUCT:
        Generator__generate_make_struct_expression(self, (Checked_Make_Struct_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__MAKE_UNION:
        Generator__generate_make_union_expression(self, (Checked_Make_Union_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__MEMBER_ACCESS:
        Generator__generate_member_access_expression(self, (Checked_Member_Access_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__MINUS:
        Generator__generate_minus_expression(self, (Checked_Minus_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__MODULO:
        Generator__generate_modulo_expression(self, (Checked_Modulo_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__MULTIPLY:
        Generator__generate_multiply_expression(self, (Checked_Multiply_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__NOT:
        Generator__generate_not_expression(self, (Checked_Not_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__NOT_EQUALS:
        Generator__generate_not_equals_expression(self, (Checked_Not_Equals_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__NULL:
        Generator__generate_null_expression(self, (Checked_Null_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__SIZEOF:
        Generator__generate_sizeof_expression(self, (Checked_Sizeof_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__STRING:
        Generator__generate_string_expression(self, (Checked_String_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__STRING_LENGTH:
        Generator__generate_string_length_expression(self, (Checked_String_Length_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__SUBTRACT:
        Generator__generate_subtract_expression(self, (Checked_Subtract_Expression *)expression);
        break;
    case CHECKED_EXPRESSION_KIND__SYMBOL:
        Generator__generate_symbol_expression(self, (Checked_Symbol_Expression *)expression);
        break;
    default:
        pWriter__begin_location_message(stderr_writer, expression->location, WRITER_STYLE__ERROR);
        pWriter__write__cstring(stderr_writer, "Unsupported expression");
        pWriter__end_location_message(stderr_writer);
        panic();
    }
}

void Generator__write_identation(Generator *self) {
    uint16_t identation = self->identation;
    while (identation > 0) {
        pWriter__write__cstring(self->writer, "    ");
        identation = identation - 1;
    }
}

void Generator__generate_statement(Generator *self, Checked_Statement *statement);
void Generator__generate_statements(Generator *self, Checked_Statements *statements);

void Generator__generate_assignment_statement(Generator *self, Checked_Assignment_Statement *statement) {
    Generator__generate_expression(self, statement->object_expression);
    pWriter__write__cstring(self->writer, " = ");
    Generator__generate_expression(self, statement->value_expression);
    pWriter__write__cstring(self->writer, ";");
}

void Generator__generate_block_statement(Generator *self, Checked_Block_Statement *statement) {
    pWriter__write__cstring(self->writer, "{\n");
    Generator__generate_statements(self, statement->statements);
    Generator__write_identation(self);
    pWriter__write__cstring(self->writer, "}");
}

void Generator__generate_break_statement(Generator *self, Checked_Break_Statement *statement) {
    pWriter__write__cstring(self->writer, "break;");
}

void Generator__generate_expression_statement(Generator *self, Checked_Expression_Statement *statement) {
    Generator__generate_expression(self, statement->expression);
    pWriter__write__cstring(self->writer, ";");
}

void Generator__generate_if_statement(Generator *self, Checked_If_Statement *statement) {
    pWriter__write__cstring(self->writer, "if (");
    Generator__generate_expression(self, statement->condition_expression);
    pWriter__write__cstring(self->writer, ") ");
    Generator__generate_statement(self, statement->true_statement);
    if (statement->false_statement != NULL) {
        pWriter__write__cstring(self->writer, " else ");
        Generator__generate_statement(self, statement->false_statement);
    }
}

void Generator__generate_loop_statement(Generator *self, Checked_Loop_Statement *statement) {
    pWriter__write__cstring(self->writer, "for (;;) ");
    Generator__generate_statement(self, statement->body_statement);
}

void Generator__generate_return_statement(Generator *self, Checked_Return_Statement *statement) {
    pWriter__write__cstring(self->writer, "return");
    if (statement->expression != NULL) {
        pWriter__write__cstring(self->writer, " ");
        Generator__generate_expression(self, statement->expression);
    }
    pWriter__write__cstring(self->writer, ";");
}

void Generator__generate_union_if_statement(Generator *self, Checked_Union_If_Statement *statement) {
    pWriter__write__cstring(self->writer, "if (");
    Generator__generate_expression(self, statement->union_expression);
    pWriter__write__cstring(self->writer, ".variant == ");
    pWriter__write__int64(self->writer, statement->union_variant->index);
    pWriter__write__cstring(self->writer, ") ");
    Generator__generate_statement(self, statement->true_statement);
    if (statement->false_statement != NULL) {
        pWriter__write__cstring(self->writer, " else ");
        Generator__generate_statement(self, statement->false_statement);
    }
}

void Generator__generate_union_switch_statement(Generator *self, Checked_Union_Switch_Statement *statement) {
    // The switch statement is generated as if-else statements to allow the use of break statements within the cases.

    // Store the expression in a variable to avoid evaluating it multiple times.
    statement->expression->temp_variable_name = String__create_from("__switch_");
    String__append_int16_t(statement->expression->temp_variable_name, statement->super.location->line);
    String__append_cstring(statement->expression->temp_variable_name, "_value__");
    pWriter__write__cdecl(self->writer, statement->expression->temp_variable_name, statement->expression->type);
    pWriter__write__cstring(self->writer, " = ");
    Generator__generate_expression(self, statement->expression);
    pWriter__write__char(self->writer, ';');

    Checked_Union_Switch_Case *union_switch_case = statement->first_union_switch_case;
    for (; union_switch_case != NULL; union_switch_case = union_switch_case->next_union_switch_case) {
        pWriter__end_line(self->writer);
        Generator__write_source_location(self, union_switch_case->location);
        Generator__write_identation(self);
        if (union_switch_case != statement->first_union_switch_case) {
            pWriter__write__cstring(self->writer, "else ");
        }
        pWriter__write__cstring(self->writer, "if (");
        pWriter__write__string(self->writer, statement->expression->temp_variable_name);
        if (statement->expression->type->kind == CHECKED_TYPE_KIND__POINTER) {
            pWriter__write__cstring(self->writer, "->");
        } else {
            pWriter__write__char(self->writer, '.');
        }
        pWriter__write__cstring(self->writer, "variant == ");
        pWriter__write__int64(self->writer, union_switch_case->union_variant->index);
        pWriter__write__cstring(self->writer, ") ");
        Generator__generate_statement(self, union_switch_case->statement);
    }

    if (statement->switch_else) {
        pWriter__end_line(self->writer);
        Generator__write_source_location(self, statement->switch_else->location);
        Generator__write_identation(self);
        if (statement->first_union_switch_case != NULL) {
            pWriter__write__cstring(self->writer, "else ");
        }
        Generator__generate_statement(self, statement->switch_else->statement);
    }
}

void Generator__generate_variable_statement(Generator *self, Checked_Variable_Statement *statement) {
    if (statement->is_external) {
        pWriter__write__cstring(self->writer, "extern ");
    }
    pWriter__write__cdecl(self->writer, statement->variable->super.name, statement->variable->super.type);
    if (statement->expression != NULL) {
        pWriter__write__cstring(self->writer, " = ");
        Generator__generate_expression(self, statement->expression);
    }
    pWriter__write__cstring(self->writer, ";");
}

void Generator__generate_while_statement(Generator *self, Checked_While_Statement *statement) {
    pWriter__write__cstring(self->writer, "while (");
    Generator__generate_expression(self, statement->condition_expression);
    pWriter__write__cstring(self->writer, ") ");
    Generator__generate_statement(self, statement->body_statement);
}

void Generator__generate_statement(Generator *self, Checked_Statement *statement) {
    switch (statement->kind) {
    case CHECKED_STATEMENT_KIND__ASSIGNMENT:
        Generator__generate_assignment_statement(self, (Checked_Assignment_Statement *)statement);
        break;
    case CHECKED_STATEMENT_KIND__BLOCK:
        Generator__generate_block_statement(self, (Checked_Block_Statement *)statement);
        break;
    case CHECKED_STATEMENT_KIND__BREAK:
        Generator__generate_break_statement(self, (Checked_Break_Statement *)statement);
        break;
    case CHECKED_STATEMENT_KIND__EXPRESSION:
        Generator__generate_expression_statement(self, (Checked_Expression_Statement *)statement);
        break;
    case CHECKED_STATEMENT_KIND__IF:
        Generator__generate_if_statement(self, (Checked_If_Statement *)statement);
        break;
    case CHECKED_STATEMENT_KIND__LOOP:
        Generator__generate_loop_statement(self, (Checked_Loop_Statement *)statement);
        break;
    case CHECKED_STATEMENT_KIND__RETURN:
        Generator__generate_return_statement(self, (Checked_Return_Statement *)statement);
        break;
    case CHECKED_STATEMENT_KIND__UNION_IF:
        Generator__generate_union_if_statement(self, (Checked_Union_If_Statement *)statement);
        break;
    case CHECKED_STATEMENT_KIND__UNION_SWITCH:
        Generator__generate_union_switch_statement(self, (Checked_Union_Switch_Statement *)statement);
        break;
    case CHECKED_STATEMENT_KIND__VARIABLE:
        Generator__generate_variable_statement(self, (Checked_Variable_Statement *)statement);
        break;
    case CHECKED_STATEMENT_KIND__WHILE:
        Generator__generate_while_statement(self, (Checked_While_Statement *)statement);
        break;
    default:
        pWriter__begin_location_message(stderr_writer, statement->location, WRITER_STYLE__ERROR);
        pWriter__write__cstring(stderr_writer, "Unsupported statement");
        pWriter__end_location_message(stderr_writer);
        panic();
    }
}

void Generator__generate_statements(Generator *self, Checked_Statements *statements) {
    self->identation = self->identation + 1;

    Checked_Statement *statement = statements->first_statement;
    while (statement != NULL) {
        Generator__write_source_location(self, statement->location);

        Generator__write_identation(self);

        Generator__generate_statement(self, statement);

        pWriter__write__cstring(self->writer, "\n");

        statement = statement->next_statement;
    }

    self->identation = self->identation - 1;
}

void Generator__declare_external_type(Generator *self, Checked_External_Type *external_type) {
    pWriter__write__cstring(self->writer, "typedef struct ");
    pWriter__write__string(self->writer, external_type->super.name);
    pWriter__write__char(self->writer, ' ');
    pWriter__write__string(self->writer, external_type->super.name);
    pWriter__write__cstring(self->writer, ";\n");
}

void Generator__declare_function(Generator *self, Checked_Function_Symbol *function_symbol) {
    pWriter__write__cdecl(self->writer, function_symbol->super.name, (Checked_Type *)function_symbol->function_type);
    pWriter__write__cstring(self->writer, ";\n");
}

void Generator__generate_function(Generator *self, Checked_Function_Symbol *function_symbol) {
    if (function_symbol->checked_statements == NULL) {
        return;
    }
    Generator__write_source_location(self, function_symbol->super.location);
    pWriter__write__cdecl(self->writer, function_symbol->super.name, (Checked_Type *)function_symbol->function_type);
    pWriter__write__cstring(self->writer, " {\n");
    Generator__generate_statements(self, function_symbol->checked_statements);
    pWriter__write__cstring(self->writer, "}\n\n");
}

void Generator__declare_struct(Generator *self, Checked_Struct_Type *struct_type) {
    pWriter__write__cdecl(self->writer, NULL, (Checked_Type *)struct_type);
    pWriter__write__cstring(self->writer, ";\n");
}

void Generator__generate_struct(Generator *self, Checked_Struct_Type *struct_type) {
    Checked_Struct_Member *struct_member = struct_type->first_member;
    if (struct_member == NULL) {
        return;
    }
    pWriter__write__cdecl(self->writer, NULL, (Checked_Type *)struct_type);
    pWriter__write__cstring(self->writer, " {\n");
    while (struct_member != NULL) {
        pWriter__write__cstring(self->writer, "    ");
        pWriter__write__cdecl(self->writer, struct_member->name, struct_member->type);
        pWriter__write__cstring(self->writer, ";\n");
        struct_member = struct_member->next_member;
    }
    pWriter__write__cstring(self->writer, "};\n\n");
}

void Generator__declare_alloc_struct_function(Generator *self, Checked_Struct_Type *struct_type) {
    pWriter__write__cdecl(self->writer, NULL, (Checked_Type *)struct_type);
    pWriter__write__cstring(self->writer, " *__alloc_");
    pWriter__write__string(self->writer, struct_type->super.name);
    pWriter__write__cstring(self->writer, "_value(");
    pWriter__write__cdecl(self->writer, NULL, (Checked_Type *)struct_type);
    pWriter__write__cstring(self->writer, " value);\n");
}

void Generator__generate_alloc_struct_function(Generator *self, Checked_Struct_Type *struct_type) {
    pWriter__write__cdecl(self->writer, NULL, (Checked_Type *)struct_type);
    pWriter__write__cstring(self->writer, " *__alloc_");
    pWriter__write__string(self->writer, struct_type->super.name);
    pWriter__write__cstring(self->writer, "_value(");
    pWriter__write__cdecl(self->writer, NULL, (Checked_Type *)struct_type);
    pWriter__write__cstring(self->writer, " value) {\n");
    pWriter__write__cstring(self->writer, "    ");
    pWriter__write__cdecl(self->writer, NULL, (Checked_Type *)struct_type);
    pWriter__write__cstring(self->writer, " *result = (");
    pWriter__write__cdecl(self->writer, NULL, (Checked_Type *)struct_type);
    pWriter__write__cstring(self->writer, " *)malloc(sizeof(");
    pWriter__write__cdecl(self->writer, NULL, (Checked_Type *)struct_type);
    pWriter__write__cstring(self->writer, "));\n");
    pWriter__write__cstring(self->writer, "    *result = value;\n");
    pWriter__write__cstring(self->writer, "    return result;\n");
    pWriter__write__cstring(self->writer, "}\n\n");
}

void Generator__declare_trait(Generator *self, Checked_Trait_Type *trait_type) {
    pWriter__write__cdecl(self->writer, NULL, (Checked_Type *)trait_type);
    pWriter__write__cstring(self->writer, ";\n");
}

void Generator__generate_trait(Generator *self, Checked_Trait_Type *trait_type) {
    Generator__generate_struct(self, trait_type->struct_type);
}

void Generator__declare_union(Generator *self, Checked_Union_Type *union_type) {
    pWriter__write__cdecl(self->writer, NULL, (Checked_Type *)union_type);
    pWriter__write__cstring(self->writer, ";\n");
}

void Generator__generate_union(Generator *self, Checked_Union_Type *union_type) {
    pWriter__write__cdecl(self->writer, NULL, (Checked_Type *)union_type);
    pWriter__write__cstring(self->writer, " {\n");
    pWriter__write__cstring(self->writer, "    int32_t variant;\n");
    Checked_Union_Variant *variant = union_type->first_variant;
    if (variant != NULL) {
        String *variant_name = String__create();
        pWriter__write__cstring(self->writer, "    union {\n");
        while (variant != NULL) {
            if (variant->index != 0) {
                String__clear(variant_name);
                String__append_cstring(variant_name, "variant_");
                String__append_int16_t(variant_name, variant->index);
                pWriter__write__cstring(self->writer, "        ");
                pWriter__write__cdecl(self->writer, variant_name, variant->type);
                pWriter__write__cstring(self->writer, ";\n");
            }
            variant = variant->next_variant;
        }
        pWriter__write__cstring(self->writer, "    };\n");
    }
    pWriter__write__cstring(self->writer, "};\n\n");
}

void Generator__declare_alloc_union_function(Generator *self, Checked_Union_Type *union_type) {
    pWriter__write__cdecl(self->writer, NULL, (Checked_Type *)union_type);
    pWriter__write__cstring(self->writer, " *__alloc_");
    pWriter__write__string(self->writer, union_type->super.name);
    pWriter__write__cstring(self->writer, "_value(");
    pWriter__write__cdecl(self->writer, NULL, (Checked_Type *)union_type);
    pWriter__write__cstring(self->writer, " value);\n");
}

void Generator__define_alloc_union_function(Generator *self, Checked_Union_Type *union_type) {
    pWriter__write__cdecl(self->writer, NULL, (Checked_Type *)union_type);
    pWriter__write__cstring(self->writer, " *__alloc_");
    pWriter__write__string(self->writer, union_type->super.name);
    pWriter__write__cstring(self->writer, "_value(");
    pWriter__write__cdecl(self->writer, NULL, (Checked_Type *)union_type);
    pWriter__write__cstring(self->writer, " value) {\n");
    pWriter__write__cdecl(self->writer, NULL, (Checked_Type *)union_type);
    pWriter__write__cstring(self->writer, " *result = (");
    pWriter__write__cdecl(self->writer, NULL, (Checked_Type *)union_type);
    pWriter__write__cstring(self->writer, " *)malloc(sizeof(");
    pWriter__write__cdecl(self->writer, NULL, (Checked_Type *)union_type);
    pWriter__write__cstring(self->writer, "));\n");
    pWriter__write__cstring(self->writer, "    *result = value;\n");
    pWriter__write__cstring(self->writer, "    return result;\n");
    pWriter__write__cstring(self->writer, "}\n\n");
}

void Generator__define_type(Generator *self, Checked_Type *type) {
    struct Checked_Type_Dependency *dependency = type->first_dependency;
    while (dependency != NULL) {
        if (!dependency->type->has_generated_definition) {
            Generator__define_type(self, dependency->type);
        }
        dependency = dependency->next_dependency;
    }
    switch (type->kind) {
    case CHECKED_TYPE_KIND__STRUCT:
        Generator__generate_struct(self, (Checked_Struct_Type *)type);
        break;
    case CHECKED_TYPE_KIND__TRAIT:
        Generator__generate_trait(self, (Checked_Trait_Type *)type);
        break;
    case CHECKED_TYPE_KIND__UNION:
        Generator__generate_union(self, (Checked_Union_Type *)type);
        break;
    }
    type->has_generated_definition = true;
}

void generate(Writer *writer, Checked_Source *checked_source) {
    Generator generator;
    generator.writer = writer;
    generator.identation = 0;

    Checked_Symbol *checked_symbol;

    pWriter__write__cstring(generator.writer, "#include <inttypes.h>");
    pWriter__end_line(generator.writer);
    pWriter__write__cstring(generator.writer, "#include <stdbool.h>");
    pWriter__end_line(generator.writer);
    pWriter__write__cstring(generator.writer, "#include <stddef.h>");
    pWriter__end_line(generator.writer);
    pWriter__end_line(generator.writer);

    Source *source = checked_source->first_source->next;
    while (source != NULL) {
        if (source->file_path->data[source->file_path->length - 1] == 'h') {
            pWriter__write__cstring(generator.writer, "#include \"");
            pWriter__write__string(generator.writer, source->file_path);
            pWriter__write__char(generator.writer, '"');
            pWriter__end_line(generator.writer);
            pWriter__end_line(generator.writer);
        }
        source = source->next;
    }

    Checked_Function_Symbol *malloc_function = NULL;

    /* Declare all defined types */
    checked_symbol = checked_source->first_symbol;
    while (checked_symbol != NULL) {
        if (checked_symbol->kind == CHECKED_SYMBOL_KIND__TYPE && (checked_symbol->location != NULL && checked_symbol->location->source == checked_source->first_source)) {
            Checked_Named_Type *named_type = ((Checked_Type_Symbol *)checked_symbol)->named_type;
            switch (named_type->super.kind) {
            case CHECKED_TYPE_KIND__EXTERNAL:
                Generator__declare_external_type(&generator, (Checked_External_Type *)named_type);
                break;
            case CHECKED_TYPE_KIND__STRUCT:
                Generator__declare_struct(&generator, (Checked_Struct_Type *)named_type);
                break;
            case CHECKED_TYPE_KIND__TRAIT:
                Generator__declare_trait(&generator, (Checked_Trait_Type *)named_type);
                break;
            case CHECKED_TYPE_KIND__UNION:
                Generator__declare_union(&generator, (Checked_Union_Type *)named_type);
                break;
            }
            pWriter__end_line(generator.writer);
        } else if (checked_symbol->kind == CHECKED_SYMBOL_KIND__FUNCTION && String__equals_cstring(checked_symbol->name, "malloc")) {
            malloc_function = (Checked_Function_Symbol *)checked_symbol;
        }
        checked_symbol = checked_symbol->next_symbol;
    }

    /* Generate all defined types */
    checked_symbol = checked_source->first_symbol;
    while (checked_symbol != NULL) {
        if (checked_symbol->kind == CHECKED_SYMBOL_KIND__TYPE && (checked_symbol->location != NULL && checked_symbol->location->source == checked_source->first_source)) {
            Checked_Type *type = (Checked_Type *)((Checked_Type_Symbol *)checked_symbol)->named_type;
            if (!type->has_generated_definition) {
                Generator__define_type(&generator, type);
            }
        }
        checked_symbol = checked_symbol->next_symbol;
    }

    /* Declare all global variables */
    Checked_Statement *checked_statement = checked_source->statements->first_statement;
    while (checked_statement != NULL) {
        if (checked_statement->kind == CHECKED_STATEMENT_KIND__VARIABLE && checked_statement->location != NULL && checked_statement->location->source == checked_source->first_source) {
            Generator__generate_variable_statement(&generator, (Checked_Variable_Statement *)checked_statement);
            pWriter__end_line(generator.writer);
        } else {
            pWriter__begin_location_message(stderr_writer, checked_statement->location, WRITER_STYLE__ERROR);
            pWriter__write__cstring(stderr_writer, "Unsupported statement");
            pWriter__end_location_message(stderr_writer);
            panic();
        }
        checked_statement = checked_statement->next_statement;
    }

    /* Declare all defined functions */
    checked_symbol = checked_source->first_symbol;
    while (checked_symbol != NULL) {
        if (checked_symbol->location != NULL && checked_symbol->location->source == checked_source->first_source) {
            if (checked_symbol->kind == CHECKED_SYMBOL_KIND__FUNCTION) {
                Generator__declare_function(&generator, (Checked_Function_Symbol *)checked_symbol);
                pWriter__end_line(generator.writer);
            } else if (checked_symbol->kind == CHECKED_SYMBOL_KIND__TYPE && malloc_function != NULL) {
                Checked_Named_Type *named_type = ((Checked_Type_Symbol *)checked_symbol)->named_type;
                if (named_type->super.kind == CHECKED_TYPE_KIND__STRUCT) {
                    Generator__declare_alloc_struct_function(&generator, (Checked_Struct_Type *)named_type);
                    pWriter__end_line(generator.writer);
                } else if (named_type->super.kind == CHECKED_TYPE_KIND__TRAIT) {
                    Generator__declare_alloc_struct_function(&generator, ((Checked_Trait_Type *)named_type)->struct_type);
                    pWriter__end_line(generator.writer);
                } else if (named_type->super.kind == CHECKED_TYPE_KIND__UNION) {
                    Generator__declare_alloc_union_function(&generator, (Checked_Union_Type *)named_type);
                    pWriter__end_line(generator.writer);
                }
            }
        }
        checked_symbol = checked_symbol->next_symbol;
    }

    /* Generate all defined functions */
    checked_symbol = checked_source->first_symbol;
    while (checked_symbol != NULL) {
        if (checked_symbol->location != NULL && checked_symbol->location->source == checked_source->first_source) {
            if (checked_symbol->kind == CHECKED_SYMBOL_KIND__FUNCTION) {
                Generator__generate_function(&generator, (Checked_Function_Symbol *)checked_symbol);
            } else if (checked_symbol->kind == CHECKED_SYMBOL_KIND__TYPE && malloc_function != NULL) {
                Checked_Named_Type *named_type = ((Checked_Type_Symbol *)checked_symbol)->named_type;
                if (named_type->super.kind == CHECKED_TYPE_KIND__STRUCT) {
                    Generator__generate_alloc_struct_function(&generator, (Checked_Struct_Type *)named_type);
                } else if (named_type->super.kind == CHECKED_TYPE_KIND__TRAIT) {
                    Generator__generate_alloc_struct_function(&generator, ((Checked_Trait_Type *)named_type)->struct_type);
                } else if (named_type->super.kind == CHECKED_TYPE_KIND__UNION) {
                    Generator__define_alloc_union_function(&generator, (Checked_Union_Type *)named_type);
                }
            }
        }
        checked_symbol = checked_symbol->next_symbol;
    }
}
