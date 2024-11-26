#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

struct Value;

struct Value {
    int32_t variant;
    union {
        bool variant_1;
        int32_t variant_2;
    };
};

int32_t main();

#line 6 "tests/09__union/009__nil/test.code"
int32_t main() {
#line 7 "tests/09__union/009__nil/test.code"
    struct Value value = (struct Value){.variant = 0};
#line 9 "tests/09__union/009__nil/test.code"
    if (value.variant != 0) {
#line 10 "tests/09__union/009__nil/test.code"
        return 1;
    }
#line 13 "tests/09__union/009__nil/test.code"
    value = (struct Value){.variant = 2, .variant_2 = 42};
#line 15 "tests/09__union/009__nil/test.code"
    if (value.variant == 0) {
#line 16 "tests/09__union/009__nil/test.code"
        return 2;
    }
#line 19 "tests/09__union/009__nil/test.code"
    return 0;
}

