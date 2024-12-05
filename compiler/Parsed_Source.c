/* Copyright (C) 2024 Stefan Selariu */

#include "Parsed_Source.h"

Parsed_Type *Parsed_Type__create_kind(Parsed_Type_Kind kind, size_t kind_size, Source_Location *location) {
    Parsed_Type *type = (Parsed_Type *)malloc(kind_size);
    type->kind = kind;
    type->location = location;
    return type;
}

Parsed_Array_Type *Parsed_Array_Type__create(Source_Location *location, Parsed_Type *item_type, Parsed_Expression *size_expression) {
    Parsed_Array_Type *type = (Parsed_Array_Type *)Parsed_Type__create_kind(PARSED_TYPE_KIND__ARRAY, sizeof(Parsed_Array_Type), location);
    type->item_type = item_type;
    type->size_expression = size_expression;
    return type;
}

Parsed_Function_Parameter *Parsed_Function_Parameter__create(Token *label, Token *name, Parsed_Type *type) {
    Parsed_Function_Parameter *parameter = (Parsed_Function_Parameter *)malloc(sizeof(Parsed_Function_Parameter));
    parameter->label = label;
    parameter->name = name;
    parameter->type = type;
    parameter->next_parameter = NULL;
    return parameter;
}

Parsed_Type *Parsed_Function_Type__create(Source_Location *location, Parsed_Function_Parameter *first_parameter, Parsed_Type *return_type) {
    Parsed_Function_Type *type = (Parsed_Function_Type *)Parsed_Type__create_kind(PARSED_TYPE_KIND__FUNCTION, sizeof(Parsed_Function_Type), location);
    type->first_parameter = first_parameter;
    type->return_type = return_type;
    return (Parsed_Type *)type;
}

Parsed_Type *Parsed_Multi_Pointer_Type__create(Source_Location *location, Parsed_Type *item_type) {
    Parsed_Multi_Pointer_Type *type = (Parsed_Multi_Pointer_Type *)Parsed_Type__create_kind(PARSED_TYPE_KIND__MULTI_POINTER, sizeof(Parsed_Multi_Pointer_Type), location);
    type->item_type = item_type;
    return (Parsed_Type *)type;
}

Parsed_Type *Parsed_Named_Type__create(Token *name) {
    Parsed_Named_Type *type = (Parsed_Named_Type *)Parsed_Type__create_kind(PARSED_TYPE_KIND__NAMED, sizeof(Parsed_Named_Type), name->location);
    type->name = name->lexeme;
    return (Parsed_Type *)type;
}

Parsed_Type *Parsed_Pointer_Type__create(Parsed_Type *other_type) {
    Parsed_Pointer_Type *type = (Parsed_Pointer_Type *)Parsed_Type__create_kind(PARSED_TYPE_KIND__POINTER, sizeof(Parsed_Pointer_Type), other_type->location);
    type->other_type = other_type;
    return (Parsed_Type *)type;
}

Parsed_Receiver_Type *Parsed_Receiver_Type__create(Source_Location *location) {
    return (Parsed_Receiver_Type *)Parsed_Type__create_kind(PARSED_TYPE_KIND__RECEIVER, sizeof(Parsed_Receiver_Type), location);
}

Parsed_Expression *Parsed_Expression__create_kind(Parsed_Expression_Kind kind, size_t kind_size, Source_Location *location) {
    Parsed_Expression *expression = (Parsed_Expression *)malloc(kind_size);
    expression->kind = kind;
    expression->location = location;
    return expression;
}

Parsed_Alloc_Expression *Parsed_Alloc_Expression__create(Source_Location *location, Parsed_Expression *value_expression) {
    Parsed_Alloc_Expression *expression = (Parsed_Alloc_Expression *)Parsed_Expression__create_kind(PARSED_EXPRESSION_KIND__ALLOC, sizeof(Parsed_Alloc_Expression), location);
    expression->value_expression = value_expression;
    return expression;
}

Parsed_Binary_Expression *Parsed_Binary_Expression__create_kind(Parsed_Expression_Kind kind, Parsed_Expression *left_expression, Parsed_Expression *right_expression) {
    Parsed_Binary_Expression *expression = (Parsed_Binary_Expression *)Parsed_Expression__create_kind(kind, sizeof(Parsed_Binary_Expression), left_expression->location);
    expression->left_expression = left_expression;
    expression->right_expression = right_expression;
    return expression;
}

Parsed_Unary_Expression *Parsed_Unary_Expression__create_kind(Parsed_Expression_Kind kind, size_t kind_size, Source_Location *location, Parsed_Expression *other_expression) {
    Parsed_Unary_Expression *expression = (Parsed_Unary_Expression *)Parsed_Expression__create_kind(kind, kind_size, location);
    expression->other_expression = other_expression;
    return expression;
}

Parsed_Literal_Expression *Parsed_Literal_Expression__create_kind(Parsed_Expression_Kind kind, size_t kind_size, Token *literal) {
    Parsed_Literal_Expression *expression = (Parsed_Literal_Expression *)Parsed_Expression__create_kind(kind, kind_size, literal->location);
    expression->literal = literal;
    return expression;
}

Parsed_Add_Expression *Parsed_Add_Expression__create(Parsed_Expression *left_expression, Parsed_Expression *right_expression) {
    return (Parsed_Add_Expression *)Parsed_Binary_Expression__create_kind(PARSED_EXPRESSION_KIND__ADD, left_expression, right_expression);
}

Parsed_Address_Of_Expression *Parsed_Address_Of_Expression__create(Source_Location *location, Parsed_Expression *other_expression) {
    return (Parsed_Address_Of_Expression *)Parsed_Unary_Expression__create_kind(PARSED_EXPRESSION_KIND__ADDRESS_OF, sizeof(Parsed_Address_Of_Expression), location, other_expression);
}

Parsed_Array_Access_Expression *Parsed_Array_Access_Expression__create(Parsed_Expression *array_expression, Parsed_Expression *index_expression) {
    Parsed_Array_Access_Expression *expression = (Parsed_Array_Access_Expression *)Parsed_Expression__create_kind(PARSED_EXPRESSION_KIND__ARRAY_ACCESS, sizeof(Parsed_Array_Access_Expression), array_expression->location);
    expression->array_expression = array_expression;
    expression->index_expression = index_expression;
    return expression;
}

Parsed_Bool_Expression *Parsed_Bool_Expression__create(Token *literal, bool value) {
    Parsed_Bool_Expression *expression = (Parsed_Bool_Expression *)Parsed_Literal_Expression__create_kind(PARSED_EXPRESSION_KIND__BOOL, sizeof(Parsed_Bool_Expression), literal);
    expression->value = value;
    return expression;
}

Parsed_Call_Argument *Parsed_Call_Argument__create(Source_Location *location, Identifier_Token *name, Parsed_Expression *expression) {
    Parsed_Call_Argument *argument = (Parsed_Call_Argument *)malloc(sizeof(Parsed_Call_Argument));
    argument->location = location;
    argument->name = name;
    argument->expression = expression;
    argument->next_argument = NULL;
    return argument;
}

Parsed_Call_Expression *Parsed_Call_Expression__create(Parsed_Expression *callee_expression, Parsed_Call_Argument *first_argument) {
    Parsed_Call_Expression *expression = (Parsed_Call_Expression *)Parsed_Expression__create_kind(PARSED_EXPRESSION_KIND__CALL, sizeof(Parsed_Call_Expression), callee_expression->location);
    expression->callee_expression = callee_expression;
    expression->first_argument = first_argument;
    return expression;
}

Parsed_Cast_Expression *Parsed_Cast_Expression__create(Source_Location *location, Parsed_Expression *other_expression, Parsed_Type *type) {
    Parsed_Cast_Expression *expression = (Parsed_Cast_Expression *)Parsed_Unary_Expression__create_kind(PARSED_EXPRESSION_KIND__CAST, sizeof(Parsed_Cast_Expression), location, other_expression);
    expression->type = type;
    return expression;
}

Parsed_Character_Expression *Parsed_Character_Expression__create(Character_Token *literal) {
    Parsed_Character_Expression *expression = (Parsed_Character_Expression *)Parsed_Literal_Expression__create_kind(PARSED_EXPRESSION_KIND__CHARACTER, sizeof(Parsed_Character_Expression), (Token *)literal);
    expression->value = literal->value;
    return expression;
}

Parsed_Dereference_Expression *Parsed_Dereference_Expression__create(Source_Location *location, Parsed_Expression *other_expression) {
    return (Parsed_Dereference_Expression *)Parsed_Unary_Expression__create_kind(PARSED_EXPRESSION_KIND__DEREFERENCE, sizeof(Parsed_Dereference_Expression), location, other_expression);
}

Parsed_Divide_Expression *Parsed_Divide_Expression__create(Parsed_Expression *left_expression, Parsed_Expression *right_expression) {
    return (Parsed_Divide_Expression *)Parsed_Binary_Expression__create_kind(PARSED_EXPRESSION_KIND__DIVIDE, left_expression, right_expression);
}

Parsed_Equals_Expression *Parsed_Equals_Expression__create(Parsed_Expression *left_expression, Parsed_Expression *right_expression) {
    return (Parsed_Equals_Expression *)Parsed_Binary_Expression__create_kind(PARSED_EXPRESSION_KIND__EQUALS, left_expression, right_expression);
}

Parsed_Greater_Expression *Parsed_Greater_Expression__create(Parsed_Expression *left_expression, Parsed_Expression *right_expression) {
    return (Parsed_Greater_Expression *)Parsed_Binary_Expression__create_kind(PARSED_EXPRESSION_KIND__GREATER, left_expression, right_expression);
}

Parsed_Greater_Or_Equals_Expression *Parsed_Greater_Or_Equals_Expression__create(Parsed_Expression *left_expression, Parsed_Expression *right_expression) {
    return (Parsed_Greater_Or_Equals_Expression *)Parsed_Binary_Expression__create_kind(PARSED_EXPRESSION_KIND__GREATER_OR_EQUALS, left_expression, right_expression);
}

Parsed_Group_Expression *Parsed_Group_Expression__create(Source_Location *location, Parsed_Expression *other_expression) {
    Parsed_Group_Expression *expression = (Parsed_Group_Expression *)Parsed_Expression__create_kind(PARSED_EXPRESSION_KIND__GROUP, sizeof(Parsed_Group_Expression), location);
    expression->other_expression = other_expression;
    return expression;
}

Parsed_Integer_Expression *Parsed_Integer_Expression__create(Integer_Token *literal, Parsed_Named_Type *type) {
    Parsed_Integer_Expression *expression = (Parsed_Integer_Expression *)Parsed_Literal_Expression__create_kind(PARSED_EXPRESSION_KIND__INTEGER, sizeof(Parsed_Integer_Expression), (Token *)literal);
    expression->value = literal->value;
    expression->type = type;
    return expression;
}

Parsed_Is_Expression *Parsed_Is_Expression__create(Parsed_Expression *value_expression, Parsed_Type *runtime_type, bool is_not) {
    Parsed_Is_Expression *expression = (Parsed_Is_Expression *)Parsed_Expression__create_kind(PARSED_EXPRESSION_KIND__IS, sizeof(Parsed_Is_Expression), value_expression->location);
    expression->value_expression = value_expression;
    expression->runtime_type = runtime_type;
    expression->is_not = is_not;
    return expression;
}

Parsed_Less_Expression *Parsed_Less_Expression__create(Parsed_Expression *left_expression, Parsed_Expression *right_expression) {
    return (Parsed_Less_Expression *)Parsed_Binary_Expression__create_kind(PARSED_EXPRESSION_KIND__LESS, left_expression, right_expression);
}

Parsed_Less_Or_Equals_Expression *Parsed_Less_Or_Equals_Expression__create(Parsed_Expression *left_expression, Parsed_Expression *right_expression) {
    return (Parsed_Less_Or_Equals_Expression *)Parsed_Binary_Expression__create_kind(PARSED_EXPRESSION_KIND__LESS_OR_EQUALS, left_expression, right_expression);
}

Parsed_Logic_And_Expression *Parsed_Logic_And_Expression__create(Parsed_Expression *left_expression, Parsed_Expression *right_expression) {
    return (Parsed_Logic_And_Expression *)Parsed_Binary_Expression__create_kind(PARSED_EXPRESSION_KIND__LOGIC_AND, left_expression, right_expression);
}

Parsed_Logic_Or_Expression *Parsed_Logic_Or_Expression__create(Parsed_Expression *left_expression, Parsed_Expression *right_expression) {
    return (Parsed_Logic_Or_Expression *)Parsed_Binary_Expression__create_kind(PARSED_EXPRESSION_KIND__LOGIC_OR, left_expression, right_expression);
}

Parsed_Member_Access_Expression *Parsed_Member_Access_Expression__create(Parsed_Expression *object_expression, Token *member_name) {
    Parsed_Member_Access_Expression *expression = (Parsed_Member_Access_Expression *)Parsed_Expression__create_kind(PARSED_EXPRESSION_KIND__MEMBER_ACCESS, sizeof(Parsed_Member_Access_Expression), object_expression->location);
    expression->object_expression = object_expression;
    expression->member_name = member_name;
    return expression;
}

Parsed_Minus_Expression *Parsed_Minus_Expression__create(Source_Location *location, Parsed_Expression *other_expression) {
    return (Parsed_Minus_Expression *)Parsed_Unary_Expression__create_kind(PARSED_EXPRESSION_KIND__MINUS, sizeof(Parsed_Minus_Expression), location, other_expression);
}

Parsed_Modulo_Expression *Parsed_Modulo_Expression__create(Parsed_Expression *left_expression, Parsed_Expression *right_expression) {
    return (Parsed_Modulo_Expression *)Parsed_Binary_Expression__create_kind(PARSED_EXPRESSION_KIND__MODULO, left_expression, right_expression);
}

Parsed_Multiply_Expression *Parsed_Multiply_Expression__create(Parsed_Expression *left_expression, Parsed_Expression *right_expression) {
    return (Parsed_Multiply_Expression *)Parsed_Binary_Expression__create_kind(PARSED_EXPRESSION_KIND__MULTIPLY, left_expression, right_expression);
}

Parsed_Not_Expression *Parsed_Not_Expression__create(Source_Location *location, Parsed_Expression *other_expression) {
    return (Parsed_Not_Expression *)Parsed_Unary_Expression__create_kind(PARSED_EXPRESSION_KIND__NOT, sizeof(Parsed_Not_Expression), location, other_expression);
}

Parsed_Not_Equals_Expression *Parsed_Not_Equals_Expression__create(Parsed_Expression *left_expression, Parsed_Expression *right_expression) {
    return (Parsed_Not_Equals_Expression *)Parsed_Binary_Expression__create_kind(PARSED_EXPRESSION_KIND__NOT_EQUALS, left_expression, right_expression);
}

Parsed_Null_Expression *Parsed_Null_Expression__create(Token *literal) {
    return (Parsed_Null_Expression *)Parsed_Literal_Expression__create_kind(PARSED_EXPRESSION_KIND__NULL, sizeof(Parsed_Null_Expression), literal);
}

Parsed_Sizeof_Expression *Parsed_Sizeof_Expression__create(Source_Location *location, Parsed_Type *type) {
    Parsed_Sizeof_Expression *expression = (Parsed_Sizeof_Expression *)Parsed_Expression__create_kind(PARSED_EXPRESSION_KIND__SIZEOF, sizeof(Parsed_Sizeof_Expression), location);
    expression->type = type;
    return expression;
}

Parsed_String_Expression *Parsed_String_Expression__create(String_Token *literal) {
    Parsed_String_Expression *expression = (Parsed_String_Expression *)Parsed_Literal_Expression__create_kind(PARSED_EXPRESSION_KIND__STRING, sizeof(Parsed_String_Expression), (Token *)literal);
    expression->value = literal->value;
    return expression;
}

Parsed_Subtract_Expression *Parsed_Subtract_Expression__create(Parsed_Expression *left_expression, Parsed_Expression *right_expression) {
    return (Parsed_Subtract_Expression *)Parsed_Binary_Expression__create_kind(PARSED_EXPRESSION_KIND__SUBTRACT, left_expression, right_expression);
}

Parsed_Symbol_Expression *Parsed_Symbol_Expression__create(Token *name) {
    Parsed_Symbol_Expression *expression = (Parsed_Symbol_Expression *)Parsed_Expression__create_kind(PARSED_EXPRESSION_KIND__SYMBOL, sizeof(Parsed_Symbol_Expression), name->location);
    expression->name = name;
    return expression;
}

Parsed_Statement *Parsed_Statement__create_kind(Parsed_Statement_Kind kind, size_t kind_size, Source_Location *location) {
    Parsed_Statement *statement = (Parsed_Statement *)malloc(kind_size);
    statement->kind = kind;
    statement->location = location;
    statement->next_statement = NULL;
    return statement;
}

bool Parsed_Statement__is_type_statement(Parsed_Statement *statement) {
    switch (statement->kind) {
    case PARSED_STATEMENT_KIND__EXTERNAL_TYPE:
    case PARSED_STATEMENT_KIND__STRUCT:
    case PARSED_STATEMENT_KIND__TRAIT:
    case PARSED_STATEMENT_KIND__UNION:
        return true;
    }
    return false;
}

Parsed_Named_Statement *Parsed_Named_Statement__create_kind(Parsed_Statement_Kind kind, size_t kind_size, Source_Location *location, Token *name) {
    Parsed_Named_Statement *statement = (Parsed_Named_Statement *)Parsed_Statement__create_kind(kind, kind_size, location);
    statement->name = name;
    return statement;
}

Parsed_Assignment_Statement *Parsed_Assignment_Statement__create(Parsed_Expression *object_expression, Parsed_Expression *value_expression) {
    Parsed_Assignment_Statement *statement = (Parsed_Assignment_Statement *)Parsed_Statement__create_kind(PARSED_STATEMENT_KIND__ASSIGNMENT, sizeof(Parsed_Assignment_Statement), object_expression->location);
    statement->object_expression = object_expression;
    statement->value_expression = value_expression;
    return statement;
}

Parsed_Block_Statement *Parsed_Block_Statement__create(Source_Location *location, Parsed_Statements *statements) {
    Parsed_Block_Statement *statement = (Parsed_Block_Statement *)Parsed_Statement__create_kind(PARSED_STATEMENT_KIND__BLOCK, sizeof(Parsed_Block_Statement), location);
    statement->statements = statements;
    return statement;
}

Parsed_Statement *Parsed_Break_Statement__create(Source_Location *location) {
    return Parsed_Statement__create_kind(PARSED_STATEMENT_KIND__BREAK, sizeof(Parsed_Break_Statement), location);
}

Parsed_Expression_Statement *Parsed_Expression_Statement__create(Parsed_Expression *expression) {
    Parsed_Expression_Statement *statement = (Parsed_Expression_Statement *)Parsed_Statement__create_kind(PARSED_STATEMENT_KIND__EXPRESSION, sizeof(Parsed_Expression_Statement), expression->location);
    statement->expression = expression;
    return statement;
}

Parsed_External_Type_Statement *Parsed_External_Type_Statement__create(Source_Location *location, Token *name) {
    return (Parsed_External_Type_Statement *)Parsed_Named_Statement__create_kind(PARSED_STATEMENT_KIND__EXTERNAL_TYPE, sizeof(Parsed_External_Type_Statement), location, name);
}

Parsed_Statement *Parsed_Function_Statement__create(Source_Location *location, Token *name, Parsed_Type *receiver_type, Parsed_Function_Parameter *first_parameter, Parsed_Type *resturn_type, Parsed_Statements *statements, bool is_external) {
    Parsed_Function_Statement *statement = (Parsed_Function_Statement *)Parsed_Named_Statement__create_kind(PARSED_STATEMENT_KIND__FUNCTION, sizeof(Parsed_Function_Statement), location, name);
    statement->receiver_type = receiver_type;
    statement->first_parameter = first_parameter;
    statement->return_type = resturn_type;
    statement->statements = statements;
    statement->is_external = is_external;
    return (Parsed_Statement *)statement;
}

Parsed_Statement *Parsed_If_Statement__create(Source_Location *location, Parsed_Expression *condition_expression, Parsed_Statement *true_statement, Parsed_Statement *false_statement) {
    Parsed_If_Statement *statement = (Parsed_If_Statement *)Parsed_Statement__create_kind(PARSED_STATEMENT_KIND__IF, sizeof(Parsed_If_Statement), location);
    statement->condition_expression = condition_expression;
    statement->true_statement = true_statement;
    statement->false_statement = false_statement;
    return (Parsed_Statement *)statement;
}

Parsed_Statement *Parsed_Loop_Statement__create(Source_Location *location, Parsed_Statement *body_statement) {
    Parsed_Loop_Statement *statement = (Parsed_Loop_Statement *)Parsed_Statement__create_kind(PARSED_STATEMENT_KIND__LOOP, sizeof(Parsed_Loop_Statement), location);
    statement->body_statement = body_statement;
    return (Parsed_Statement *)statement;
}

Parsed_Statement *Parsed_Return_Statement__create(Source_Location *location, Parsed_Expression *expression) {
    Parsed_Return_Statement *statement = (Parsed_Return_Statement *)Parsed_Statement__create_kind(PARSED_STATEMENT_KIND__RETURN, sizeof(Parsed_Return_Statement), location);
    statement->expression = expression;
    return (Parsed_Statement *)statement;
}

Parsed_Struct_Member *Parsed_Struct_Member__create(Token *name, Parsed_Type *type) {
    Parsed_Struct_Member *member = (Parsed_Struct_Member *)malloc(sizeof(Parsed_Struct_Member));
    member->name = name;
    member->type = type;
    member->next_member = NULL;
    return member;
}

Parsed_Struct_Statement *Parsed_Struct_Statement__create(Source_Location *location, Token *name) {
    Parsed_Struct_Statement *statement = (Parsed_Struct_Statement *)Parsed_Named_Statement__create_kind(PARSED_STATEMENT_KIND__STRUCT, sizeof(Parsed_Struct_Statement), location, name);
    statement->first_member = NULL;
    return statement;
}

Parsed_Switch_Case *Parsed_Switch_Else__create(Source_Location *location) {
    Parsed_Switch_Case *switch_else = (Parsed_Switch_Case *)malloc(sizeof(Parsed_Switch_Case));
    switch_else->kind = PARSED_SWITCH_CASE_KIND__ELSE;
    switch_else->location = location;
    switch_else->statement = NULL;
    switch_else->next_case = NULL;
    return switch_else;
}

Parsed_Switch_Case *Parsed_Switch_Expression__create(Source_Location *location, Parsed_Expression *expression) {
    Parsed_Switch_Case *switch_case = (Parsed_Switch_Case *)malloc(sizeof(Parsed_Switch_Case));
    switch_case->kind = PARSED_SWITCH_CASE_KIND__EXPRESSION;
    switch_case->location = location;
    switch_case->expression = expression;
    switch_case->statement = NULL;
    switch_case->next_case = NULL;
    return switch_case;
}

Parsed_Switch_Case *Parsed_Switch_Variant__create(Source_Location *location, Parsed_Type *type, Identifier_Token *alias) {
    Parsed_Switch_Case *switch_variant = (Parsed_Switch_Case *)malloc(sizeof(Parsed_Switch_Case));
    switch_variant->kind = PARSED_SWITCH_CASE_KIND__VARIANT;
    switch_variant->location = location;
    switch_variant->variant.type = type;
    switch_variant->variant.alias = alias;
    switch_variant->statement = NULL;
    switch_variant->next_case = NULL;
    return switch_variant;
}

Parsed_Switch_Statement *Parsed_Switch_Statement__create(Source_Location *location, Parsed_Expression *expression, Parsed_Switch_Case *first_case) {
    Parsed_Switch_Statement *statement = (Parsed_Switch_Statement *)Parsed_Statement__create_kind(PARSED_STATEMENT_KIND__SWITCH, sizeof(Parsed_Switch_Statement), location);
    statement->expression = expression;
    statement->first_case = first_case;
    return statement;
}

Parsed_Trait_Method *Parsed_Trait_Method__create(Source_Location *location, Token *name, Parsed_Function_Parameter *first_parameter, Parsed_Type *return_type) {
    Parsed_Trait_Method *method = (Parsed_Trait_Method *)malloc(sizeof(Parsed_Trait_Method));
    method->location = location;
    method->name = name;
    method->first_parameter = first_parameter;
    method->return_type = return_type;
    method->next_method = NULL;
    return method;
}

Parsed_Trait_Statement *Parsed_Trait_Statement__create(Source_Location *location, Token *name) {
    Parsed_Trait_Statement *statement = (Parsed_Trait_Statement *)Parsed_Named_Statement__create_kind(PARSED_STATEMENT_KIND__TRAIT, sizeof(Parsed_Trait_Statement), location, name);
    statement->first_method = NULL;
    return statement;
}

Parsed_Union_Variant *Parsed_Union_Variant__create(Parsed_Type *type) {
    Parsed_Union_Variant *variant = (Parsed_Union_Variant *)malloc(sizeof(Parsed_Union_Variant));
    variant->type = type;
    variant->next_variant = NULL;
    return variant;
}

Parsed_Union_Statement *Parsed_Union_Statement__create(Source_Location *location, Token *name) {
    Parsed_Union_Statement *statement = (Parsed_Union_Statement *)Parsed_Named_Statement__create_kind(PARSED_STATEMENT_KIND__UNION, sizeof(Parsed_Union_Statement), location, name);
    statement->first_variant = NULL;
    return statement;
}

Parsed_Union_If_Statement *Parsed_Union_If_Statement__create(Source_Location *location, Identifier_Token *variant_alias, Parsed_Expression *expression, Parsed_Type *variant_type, Parsed_Statement *true_statement, Parsed_Statement *false_statement) {
    Parsed_Union_If_Statement *statement = (Parsed_Union_If_Statement *)Parsed_Statement__create_kind(PARSED_STATEMENT_KIND__UNION_IF, sizeof(Parsed_Union_If_Statement), location);
    statement->variant_alias = variant_alias;
    statement->expression = expression;
    statement->variant_type = variant_type;
    statement->true_statement = true_statement;
    statement->false_statement = false_statement;
    return statement;
}

Parsed_Variable_Statement *Parsed_Variable_Statement__create(Source_Location *location, Token *name, Parsed_Type *type, Parsed_Expression *expression, bool is_external) {
    Parsed_Variable_Statement *statement = (Parsed_Variable_Statement *)Parsed_Named_Statement__create_kind(PARSED_STATEMENT_KIND__VARIABLE, sizeof(Parsed_Variable_Statement), location, name);
    statement->type = type;
    statement->expression = expression;
    statement->is_external = is_external;
    return statement;
}

Parsed_Statement *Parsed_While_Statement__create(Source_Location *location, Parsed_Expression *condition_expression, Parsed_Statement *body_statement) {
    Parsed_While_Statement *statement = (Parsed_While_Statement *)Parsed_Statement__create_kind(PARSED_STATEMENT_KIND__WHILE, sizeof(Parsed_While_Statement), location);
    statement->condition_expression = condition_expression;
    statement->body_statement = body_statement;
    return (Parsed_Statement *)statement;
}

Parsed_Statements *Parsed_Statements__create(bool has_globals) {
    Parsed_Statements *statements = (Parsed_Statements *)malloc(sizeof(Parsed_Statements));
    statements->first_statement = NULL;
    statements->last_statement = NULL;
    statements->has_globals = has_globals;
    return statements;
}

void Parsed_Statements__append(Parsed_Statements *self, Parsed_Statement *statement) {
    if (self->first_statement == NULL) {
        self->first_statement = statement;
    } else {
        self->last_statement->next_statement = statement;
    }
    self->last_statement = statement;
}

Parsed_Source *Parsed_Source__create() {
    Parsed_Source *parsed_source = (Parsed_Source *)malloc(sizeof(Parsed_Source));
    parsed_source->first_source = NULL;
    parsed_source->statements = Parsed_Statements__create(true);
    return parsed_source;
}
