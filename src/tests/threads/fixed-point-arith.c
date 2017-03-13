#include "threads/fixed-point.h"

void fixed_point(void){
    printf("Dividing 59/60. Expect .98\n");

    struct fp_num one;
    struct fp_num two;
    struct fp_num three = to_fp(5);
    one = divide_fp(to_fp(59),to_fp(60));
    one = multiply_fp(one,three);
    print_fp(one);
    printf("Dividing 1/60. Expect .01667\n");
    two = divide_fp(to_fp(1),to_fp(60));
    print_fp(two);
}