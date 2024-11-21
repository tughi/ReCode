#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

struct Tokenizer;

struct String;

struct Span;

struct Number;

struct Space;

struct Other;

struct Stop;

struct Token;

struct String_Builder;

struct Writer;

typedef struct FILE FILE;

struct Tokenizer {
    uint8_t *data;
    int32_t index;
};

struct String {
    uint8_t *data;
    int32_t length;
};

struct Span {
    struct String lexeme;
};

struct Number {
    struct Span span;
    int32_t value;
};

struct Space {
    struct Span span;
    int32_t count;
};

struct Other {
    struct Span span;
};

struct Stop {
    struct Span span;
};

struct Token {
    int32_t variant;
    union {
        struct Number variant_1;
        struct Space variant_2;
        struct Other variant_3;
        struct Stop variant_4;
    };
};

struct String_Builder {
    uint8_t *data;
    int32_t data_size;
    int32_t length;
};

struct Writer {
    void *self;
    void (*write_char)(void *self, uint8_t c);
};

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;
struct Tokenizer *__make_Tokenizer_value(struct Tokenizer value);

struct String *__make_String_value(struct String value);

struct Span *__make_Span_value(struct Span value);

struct Number *__make_Number_value(struct Number value);

struct Space *__make_Space_value(struct Space value);

struct Other *__make_Other_value(struct Other value);

struct Stop *__make_Stop_value(struct Stop value);

struct String_Builder *__make_String_Builder_value(struct String_Builder value);

struct Writer *__make_Writer_value(struct Writer value);

void main(int32_t argc, uint8_t **argv);

bool pTokenizer__has_next_token(struct Tokenizer *self);

struct Token pTokenizer__next_token(struct Tokenizer *self);

struct Token pTokenizer__scan_number_token(struct Tokenizer *self, struct String_Builder *lexeme_builder);

struct Token pTokenizer__scan_space_token(struct Tokenizer *self, struct String_Builder *lexeme_builder);

struct Writer *pWriter__write(struct Writer *self, struct String string);

struct Writer *pWriter__write__1_token(struct Writer *self, struct Token token);

struct String_Builder *pString_Builder__write__1_char(struct String_Builder *self, uint8_t c);

struct String pString_Builder__build(struct String_Builder *self);

struct String_Builder make_string_builder();

struct String_Builder make_string_builder__0_initial_data_size(int32_t initial_data_size);

void pString_Builder__write_char(struct String_Builder *self, uint8_t c);

struct Writer *pWriter__write__1_char(struct Writer *self, uint8_t c);

struct Writer *pWriter__write__1_signed(struct Writer *self, int32_t value);

struct Writer *pWriter__end_line(struct Writer *self);

int32_t fputc(int32_t c, FILE *file);

void pFILE__write_char(FILE *self, uint8_t c);

void *malloc(uint64_t size);

void *realloc(void *block, uint64_t size);

void exit(int32_t code);

struct Tokenizer *__make_Tokenizer_value(struct Tokenizer value) {
    struct Tokenizer *result = (struct Tokenizer *)malloc(sizeof(struct Tokenizer));
    *result = value;
    return result;
}

struct String *__make_String_value(struct String value) {
    struct String *result = (struct String *)malloc(sizeof(struct String));
    *result = value;
    return result;
}

struct Span *__make_Span_value(struct Span value) {
    struct Span *result = (struct Span *)malloc(sizeof(struct Span));
    *result = value;
    return result;
}

struct Number *__make_Number_value(struct Number value) {
    struct Number *result = (struct Number *)malloc(sizeof(struct Number));
    *result = value;
    return result;
}

struct Space *__make_Space_value(struct Space value) {
    struct Space *result = (struct Space *)malloc(sizeof(struct Space));
    *result = value;
    return result;
}

struct Other *__make_Other_value(struct Other value) {
    struct Other *result = (struct Other *)malloc(sizeof(struct Other));
    *result = value;
    return result;
}

struct Stop *__make_Stop_value(struct Stop value) {
    struct Stop *result = (struct Stop *)malloc(sizeof(struct Stop));
    *result = value;
    return result;
}

struct String_Builder *__make_String_Builder_value(struct String_Builder value) {
    struct String_Builder *result = (struct String_Builder *)malloc(sizeof(struct String_Builder));
    *result = value;
    return result;
}

struct Writer *__make_Writer_value(struct Writer value) {
    struct Writer *result = (struct Writer *)malloc(sizeof(struct Writer));
    *result = value;
    return result;
}

#line 1 "tests/10__calculator/test.code"
void main(int32_t argc, uint8_t **argv) {
#line 2 "tests/10__calculator/test.code"
    if (argc != 2) {
#line 3 "tests/10__calculator/test.code"
        exit(1);
    }
#line 6 "tests/10__calculator/test.code"
    struct Tokenizer tokenizer = (struct Tokenizer){.data = argv[1], .index = 0};
#line 11 "tests/10__calculator/test.code"
    struct Writer stdout_writer = (struct Writer){.self = stdout, .write_char = ((void (*)(void *self, uint8_t c)) pFILE__write_char)};
#line 13 "tests/10__calculator/test.code"
    pWriter__end_line(pWriter__write(&stdout_writer, (struct String){.data = "Tokens:", .length = 7}));
#line 15 "tests/10__calculator/test.code"
    while (pTokenizer__has_next_token(&tokenizer)) {
#line 16 "tests/10__calculator/test.code"
        struct Token token = pTokenizer__next_token(&tokenizer);
#line 17 "tests/10__calculator/test.code"
        pWriter__end_line(pWriter__write__1_token(&stdout_writer, token));
    }
#line 20 "tests/10__calculator/test.code"
    struct Token stop_token = pTokenizer__next_token(&tokenizer);
#line 21 "tests/10__calculator/test.code"
    if (stop_token.variant == 4) {
#line 22 "tests/10__calculator/test.code"
        exit(0);
    }
#line 25 "tests/10__calculator/test.code"
    exit(2);
}

#line 35 "tests/10__calculator/test.code"
bool pTokenizer__has_next_token(struct Tokenizer *self) {
#line 36 "tests/10__calculator/test.code"
    return self->data[self->index] != 0;
}

#line 39 "tests/10__calculator/test.code"
struct Token pTokenizer__next_token(struct Tokenizer *self) {
#line 40 "tests/10__calculator/test.code"
    struct String_Builder lexeme_builder = make_string_builder();
#line 41 "tests/10__calculator/test.code"
    uint8_t c = self->data[self->index];
#line 42 "tests/10__calculator/test.code"
    if (c == 0) {
#line 43 "tests/10__calculator/test.code"
        return (struct Token){.variant = 4, .variant_4 = (struct Stop){.span = (struct Span){.lexeme = pString_Builder__build(&lexeme_builder)}}};
    }
#line 49 "tests/10__calculator/test.code"
    if (c >= '0' && c <= '9') {
#line 50 "tests/10__calculator/test.code"
        return pTokenizer__scan_number_token(self, &lexeme_builder);
    }
#line 52 "tests/10__calculator/test.code"
    if (c == ' ') {
#line 53 "tests/10__calculator/test.code"
        return pTokenizer__scan_space_token(self, &lexeme_builder);
    }
#line 55 "tests/10__calculator/test.code"
    self->index = self->index + 1;
#line 56 "tests/10__calculator/test.code"
    return (struct Token){.variant = 3, .variant_3 = (struct Other){.span = (struct Span){.lexeme = pString_Builder__build(pString_Builder__write__1_char(&lexeme_builder, c))}}};
}

#line 63 "tests/10__calculator/test.code"
struct Token pTokenizer__scan_number_token(struct Tokenizer *self, struct String_Builder *lexeme_builder) {
#line 64 "tests/10__calculator/test.code"
    int32_t value = 0;
#line 65 "tests/10__calculator/test.code"
    for (;;) {
#line 66 "tests/10__calculator/test.code"
        uint8_t c = self->data[self->index];
#line 67 "tests/10__calculator/test.code"
        if (c < '0' || c > '9') {
#line 68 "tests/10__calculator/test.code"
            break;
        }
#line 70 "tests/10__calculator/test.code"
        pString_Builder__write__1_char(lexeme_builder, c);
#line 71 "tests/10__calculator/test.code"
        value = value * 10 + ((int32_t) (c - '0'));
#line 72 "tests/10__calculator/test.code"
        self->index = self->index + 1;
    }
#line 74 "tests/10__calculator/test.code"
    return (struct Token){.variant = 1, .variant_1 = (struct Number){.span = (struct Span){.lexeme = pString_Builder__build(lexeme_builder)}, .value = value}};
}

#line 82 "tests/10__calculator/test.code"
struct Token pTokenizer__scan_space_token(struct Tokenizer *self, struct String_Builder *lexeme_builder) {
#line 83 "tests/10__calculator/test.code"
    int32_t count = 0;
#line 84 "tests/10__calculator/test.code"
    for (;;) {
#line 85 "tests/10__calculator/test.code"
        uint8_t c = self->data[self->index];
#line 86 "tests/10__calculator/test.code"
        if (c != ' ') {
#line 87 "tests/10__calculator/test.code"
            break;
        }
#line 89 "tests/10__calculator/test.code"
        pString_Builder__write__1_char(lexeme_builder, c);
#line 90 "tests/10__calculator/test.code"
        count = count + 1;
#line 91 "tests/10__calculator/test.code"
        self->index = self->index + 1;
    }
#line 93 "tests/10__calculator/test.code"
    return (struct Token){.variant = 2, .variant_2 = (struct Space){.span = (struct Span){.lexeme = pString_Builder__build(lexeme_builder)}, .count = count}};
}

#line 108 "tests/10__calculator/test.code"
struct Writer *pWriter__write(struct Writer *self, struct String string) {
#line 109 "tests/10__calculator/test.code"
    struct String string_copy = string;
#line 110 "tests/10__calculator/test.code"
    uint8_t *string_data = ((struct String *) (&string_copy))->data;
#line 111 "tests/10__calculator/test.code"
    uintmax_t index = 0;
#line 112 "tests/10__calculator/test.code"
    while (index < string.length) {
#line 113 "tests/10__calculator/test.code"
        pWriter__write__1_char(self, string_data[index]);
#line 114 "tests/10__calculator/test.code"
        index = index + 1;
    }
#line 116 "tests/10__calculator/test.code"
    return self;
}

#line 151 "tests/10__calculator/test.code"
struct Writer *pWriter__write__1_token(struct Writer *self, struct Token token) {
#line 152 "tests/10__calculator/test.code"
    while (false) {
    }
#line 153 "tests/10__calculator/test.code"
    if (token.variant == 1) {
#line 154 "tests/10__calculator/test.code"
        return pWriter__write__1_signed(pWriter__write(self, (struct String){.data = "Number: ", .length = 8}), token.variant_1.value);
    }
#line 156 "tests/10__calculator/test.code"
    else if (token.variant == 2) {
#line 157 "tests/10__calculator/test.code"
        return pWriter__write__1_signed(pWriter__write(self, (struct String){.data = "Space: ", .length = 7}), token.variant_2.count);
    }
#line 159 "tests/10__calculator/test.code"
    else if (token.variant == 3) {
#line 160 "tests/10__calculator/test.code"
        return pWriter__write(pWriter__write(self, (struct String){.data = "Other: ", .length = 7}), token.variant_3.span.lexeme);
    }
#line 162 "tests/10__calculator/test.code"
    else if (token.variant == 4) {
#line 163 "tests/10__calculator/test.code"
        return pWriter__write(self, (struct String){.data = "Stop", .length = 4});
    }
#line 166 "tests/10__calculator/test.code"
    return self;
}

#line 178 "tests/10__calculator/test.code"
struct String_Builder *pString_Builder__write__1_char(struct String_Builder *self, uint8_t c) {
#line 179 "tests/10__calculator/test.code"
    if (self->length == self->data_size) {
#line 180 "tests/10__calculator/test.code"
        self->data_size = self->data_size + 8;
#line 181 "tests/10__calculator/test.code"
        self->data = ((uint8_t *) realloc(((void *) self->data), ((uint64_t) self->data_size)));
    }
#line 186 "tests/10__calculator/test.code"
    self->data[self->length] = c;
#line 187 "tests/10__calculator/test.code"
    self->length = self->length + 1;
#line 188 "tests/10__calculator/test.code"
    return self;
}

#line 191 "tests/10__calculator/test.code"
struct String pString_Builder__build(struct String_Builder *self) {
#line 192 "tests/10__calculator/test.code"
    pString_Builder__write__1_char(self, 0);
#line 193 "tests/10__calculator/test.code"
    struct String string = (struct String){.data = self->data, .length = self->length - 1};
#line 197 "tests/10__calculator/test.code"
    return *((struct String *) (&string));
}

#line 200 "tests/10__calculator/test.code"
struct String_Builder make_string_builder() {
#line 201 "tests/10__calculator/test.code"
    return make_string_builder__0_initial_data_size(4);
}

#line 204 "tests/10__calculator/test.code"
struct String_Builder make_string_builder__0_initial_data_size(int32_t initial_data_size) {
#line 205 "tests/10__calculator/test.code"
    return (struct String_Builder){.data = ((uint8_t *) malloc(((uint64_t) initial_data_size))), .data_size = initial_data_size, .length = 0};
}

#line 212 "tests/10__calculator/test.code"
void pString_Builder__write_char(struct String_Builder *self, uint8_t c) {
#line 213 "tests/10__calculator/test.code"
    pString_Builder__write__1_char(self, c);
}

#line 222 "tests/10__calculator/test.code"
struct Writer *pWriter__write__1_char(struct Writer *self, uint8_t c) {
#line 223 "tests/10__calculator/test.code"
    self->write_char(self->self, c);
#line 224 "tests/10__calculator/test.code"
    return self;
}

#line 227 "tests/10__calculator/test.code"
struct Writer *pWriter__write__1_signed(struct Writer *self, int32_t value) {
#line 229 "tests/10__calculator/test.code"
    if (value < 0) {
#line 230 "tests/10__calculator/test.code"
        pWriter__write__1_char(self, '-');
#line 231 "tests/10__calculator/test.code"
        return pWriter__write__1_signed(self, -value);
    }
#line 233 "tests/10__calculator/test.code"
    if (value >= 10) {
#line 234 "tests/10__calculator/test.code"
        pWriter__write__1_signed(self, value / 10);
    }
#line 236 "tests/10__calculator/test.code"
    return pWriter__write__1_char(self, ((uint8_t) (value % 10)) + '0');
}

#line 239 "tests/10__calculator/test.code"
struct Writer *pWriter__end_line(struct Writer *self) {
#line 240 "tests/10__calculator/test.code"
    pWriter__write__1_char(self, '\n');
#line 241 "tests/10__calculator/test.code"
    return self;
}

#line 254 "tests/10__calculator/test.code"
void pFILE__write_char(FILE *self, uint8_t c) {
#line 255 "tests/10__calculator/test.code"
    fputc(((int32_t) c), stdout);
}

