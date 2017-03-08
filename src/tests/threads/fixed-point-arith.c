#include "threads/fixed-point.h"

void fixed_point(void){
    // Tests conversion to and from fixed-point
    printf("Testing conversion from fixed-point to integer:\n");
    int32_t test = 37;
    printf("Integer: %d\n",test);
    test = to_int(to_fp(test),2);
    printf("After Conversion: %d\n",test);

    // Test divide_fp.
    struct fp_num one = to_fp(15);
    struct fp_num two = to_fp(2);
    struct fp_num real_quot = divide_fp(one,two);
    int32_t result = to_int(real_quot,2);
    printf("Result of dividing 15 and 2: (Expect 7)\n");
    printf("%d\n",result);

    real_quot = multiply_int_fp(real_quot,8);

    printf("Multiplied by 8 (Expect 60):\n");
    print_fp(real_quot);

    printf("Divided by 2 (Expect 30):\n");

    real_quot = divide_fp(real_quot,two);
    print_fp(real_quot);
    // Test divide_fp_int.
    struct fp_num div1 = to_fp(15);
    print_fp(div1);
    int32_t div2 = 2;
    struct fp_num real_res = divide_fp_int(div1,div2);
    print_fp(real_res);
    int32_t resdiv = to_int(real_res,2);
    printf("Result of dividing 15 and integer 2 (Expect 7): %d\n",resdiv);

    // Test multiply_fp.
    struct fp_num m_one = to_fp(30);
    struct fp_num m_two = to_fp(7);
    int32_t out = to_int(multiply_fp(m_one,m_two),2);
    printf("Result fo multiplying 30 and 7: %d\n",out);

    // Test mulitply_int_fp.
    struct fp_num in = to_fp(30);
    int32_t in_two = 7;
    int32_t res2 = to_int(multiply_int_fp(in,in_two),2);
    printf("Result of multiplying 30 and integer 7: %d\n",res2);
}