#include "threads/fixed-point.h"

void fixed_point(void){
    // Tests conversion to and from fixed-point
    printf("Testing conversion from fixed-point to integer:\n");
    int32_t test = 37;
    printf("Integer: %d\n",test);
    test = to_int(to_fp(test));
    printf("After Conversion: %d\n",test);

    // Testing arithmetic operations:
    struct fp_num one = to_fp(15);
    struct fp_num two = to_fp(2);

    int32_t result = to_int(divide_fp(one,two));

    printf("Result of dividing 15 and 2: (Expect 7)\n");
    printf("%d\n",result);

}