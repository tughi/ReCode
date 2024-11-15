#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

struct Shape;

struct Square;

struct Circle;

struct Shape {
    void *self;
    int32_t (*area)(void *self);
};

struct Square {
    int32_t side;
};

struct Circle {
    int32_t radius;
};

struct Shape *__make_Shape_value(struct Shape value);

struct Square *__make_Square_value(struct Square value);

struct Circle *__make_Circle_value(struct Circle value);

int32_t pSquare__area(struct Square *self);

int32_t pCircle__area(struct Circle *self);

int32_t main();

void *malloc(uint64_t size);

struct Shape *__make_Shape_value(struct Shape value) {
    struct Shape *result = (struct Shape *)malloc(sizeof(struct Shape));
    *result = value;
    return result;
}

struct Square *__make_Square_value(struct Square value) {
    struct Square *result = (struct Square *)malloc(sizeof(struct Square));
    *result = value;
    return result;
}

struct Circle *__make_Circle_value(struct Circle value) {
    struct Circle *result = (struct Circle *)malloc(sizeof(struct Circle));
    *result = value;
    return result;
}

#line 9 "tests/07__trait/004__make_heap_trait_variable/test.code"
int32_t pSquare__area(struct Square *self) {
#line 10 "tests/07__trait/004__make_heap_trait_variable/test.code"
    return self->side * self->side;
}

#line 17 "tests/07__trait/004__make_heap_trait_variable/test.code"
int32_t pCircle__area(struct Circle *self) {
#line 18 "tests/07__trait/004__make_heap_trait_variable/test.code"
    return 312 * self->radius * self->radius / 100;
}

#line 21 "tests/07__trait/004__make_heap_trait_variable/test.code"
int32_t main() {
#line 22 "tests/07__trait/004__make_heap_trait_variable/test.code"
    struct Square *square = __make_Square_value((struct Square){.side = 10});
#line 23 "tests/07__trait/004__make_heap_trait_variable/test.code"
    struct Shape *square_shape = __make_Shape_value((struct Shape){.self = square, .area = ((int32_t (*)(void *self)) pSquare__area)});
#line 24 "tests/07__trait/004__make_heap_trait_variable/test.code"
    struct Circle *circle = __make_Circle_value((struct Circle){.radius = 10});
#line 25 "tests/07__trait/004__make_heap_trait_variable/test.code"
    struct Shape *circle_shape = __make_Shape_value((struct Shape){.self = circle, .area = ((int32_t (*)(void *self)) pCircle__area)});
#line 27 "tests/07__trait/004__make_heap_trait_variable/test.code"
    if (square_shape->area(square_shape->self) != 100) {
#line 28 "tests/07__trait/004__make_heap_trait_variable/test.code"
        return 1;
    }
#line 31 "tests/07__trait/004__make_heap_trait_variable/test.code"
    if (circle_shape->area(circle_shape->self) != 312) {
#line 32 "tests/07__trait/004__make_heap_trait_variable/test.code"
        return 2;
    }
#line 35 "tests/07__trait/004__make_heap_trait_variable/test.code"
    return 0;
}

