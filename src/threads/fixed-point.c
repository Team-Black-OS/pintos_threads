#include "threads/fixed-point.h"
const int ROUNDUP = 1;
const int ROUNDDOWN = 2;
const int ROUNDNEAREST = 3;
const int64_t MULTIPLICAND = 16384;
const int64_t FRAC_MULTIPLICAND = 262144;
int32_t to_int(struct fp_num fixedpoint){
    int32_t res = (fixedpoint.num / MULTIPLICAND);
        return res;
}

struct fp_num to_fp(int32_t num) {
    struct fp_num newnum;
    newnum.num = (num*MULTIPLICAND);
    return newnum;
}

// Adds two fixed-point numbers. Result is a fixed-point number.
struct fp_num add(struct fp_num one,struct fp_num two){
    struct fp_num res;
    res.num = (one.num+two.num);
    return res;
}

// Subtracts two fixed-point numbers. Result is a fixed point number.
struct fp_num subtract_fp(struct fp_num one,struct fp_num two){
    struct fp_num res;
    res.num = (one.num-two.num);
    return res;
}

// Adds a fixed pointer number and an integer number. Result is a fixed-point
// number.
struct fp_num add_fp_int(struct fp_num fixed,int32_t intg){
    struct fp_num res;
    res.num = (fixed.num + (intg*MULTIPLICAND));
    return res;
}

// Subtracts an integer number from a fixed-point number. Result is a fixed-point number.
struct fp_num subtract_int_fp(struct fp_num fixed,int32_t intg){
    struct fp_num res;
    res.num = (fixed.num - (intg*MULTIPLICAND));
    return res;
}

// Multiplies two fixed-point numbers. Result is a fixed-point number.
// Note: There is no overflow detection.
struct fp_num multiply_fp(struct fp_num one, struct fp_num two){
    struct fp_num res;
    res.num = (((int64_t) one.num) * two.num)/MULTIPLICAND;
    return res;
}

// Multiplies a fixed-point number with an integer number. Result is fixed-point.
struct fp_num multiply_int_fp(struct fp_num fixed,int32_t intg){
    struct fp_num res;
    res.num = (fixed.num*intg);
    return res;
}

// Divides two fixed-point numbers. Result is fixed-point.
struct fp_num divide_fp(struct fp_num one,struct fp_num two){
    int64_t test = (int64_t) one.num;
    int64_t test2 = test*MULTIPLICAND;
    struct fp_num newnum;
    newnum.num = test2/two.num;

    return newnum;
}

// Divides a fixed-point number by an integer number. Result is fixed-point.
struct fp_num divide_fp_int(struct fp_num fixed,int32_t intg){
    struct fp_num res;
    res.num = (fixed.num/intg);
    return res;
}

void print_fp(struct fp_num input){
    // Get integer version of the number, multiply by 100 so we can print as
    // decimal.
    int32_t temp = to_int(multiply_int_fp(input,100));
    printf("%d.%d\n",temp/100,temp%100);
}
