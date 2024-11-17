#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

struct Integer;

struct Value;

struct Integer {
    int32_t variant;
    union {
        int8_t variant_1;
        int32_t variant_2;
    };
};

struct Value {
    int32_t variant;
    union {
        bool variant_1;
        struct Integer variant_2;
    };
};

int32_t main();

#line 11 "tests/09__union/003__assign_union_value/test.code"
int32_t main() {
#line 12 "tests/09__union/003__assign_union_value/test.code"
    struct Value value;
#line 13 "tests/09__union/003__assign_union_value/test.code"
    value = (struct Value){.variant = 1, .variant_1 = true};
#line 14 "tests/09__union/003__assign_union_value/test.code"
    struct Value other_value;
#line 15 "tests/09__union/003__assign_union_value/test.code"
    other_value = value;
#line 16 "tests/09__union/003__assign_union_value/test.code"
    return 0;
}

