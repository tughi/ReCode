#include "Source.h"

#include <stdlib.h>

Source *source__create(char *data) {
    Source *self = (Source *)malloc(sizeof(Source));
    self->data = data;
    self->index = 0;
    self->line = 1;
    self->column = 1;
    return self;
}

char source__peek(Source *self) {
    return self->data[self->index];
}

int source__current_line(Source *self) {
    return self->line;
}

int source__current_column(Source *self) {
    return self->column;
}

char source__advance(Source *self, int (*accepts)(char)) {
    char next_char = self->data[self->index];
    if (next_char && (*accepts)(next_char)) {
        self->index += 1;
        if (next_char == '\n') {
            self->line += 1;
            self->column = 1;
        } else if (next_char == '\t') {
            self->column = (self->column / TAB_SIZE + 1) * TAB_SIZE + 1;
        } else {
            self->column += 1;
        }
        return next_char;
    }
    return 0;
}
