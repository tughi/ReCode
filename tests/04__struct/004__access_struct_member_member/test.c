#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

struct Point;

struct Line;

struct Point {
    int32_t x;
    int32_t y;
};

struct Line {
    struct Point p1;
    struct Point p2;
};

int32_t test__main();

int32_t main(int argc, const char **argv) {
    return test__main();
}

#line 13 "tests/04__struct/004__access_struct_member_member/test.code"
int32_t test__main() {
#line 14 "tests/04__struct/004__access_struct_member_member/test.code"
    struct Line line = (struct Line){.p1 = (struct Point){.x = 16, .y = 32}, .p2 = (struct Point){.x = 48, .y = 64}};
#line 18 "tests/04__struct/004__access_struct_member_member/test.code"
    return line.p2.y - 64;
}

