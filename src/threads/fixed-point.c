#include "threads/fixed-point.h"

int32_t to_int(struct fp_num fixedpoint){
    return (fixedpoint.num / MULTIPLICAND);
}

struct fp_num to_fp(int32_t num) {
    struct fp_num newnum;
    newnum.num = (num*MULTIPLICAND);
    return newnum;
}

// Adds two fixed-point numbers. Result is a fixed-point number.
struct fp_num add(struct fp_num one,struct fp_num two){
    one.num = (one.num+two.num);
    return one;
}

// Subtracts two fixed-point numbers. Result is a fixed point number.
struct fp_num subtract_fp(struct fp_num one,struct fp_num two){
    one.num = (one.num-two.num);
    return one;
}

// Adds a fixed pointer number and an integer number. Result is a fixed-point
// number.
struct fp_num add_fp_int(struct fp_num fixed,int32_t intg){
    fixed.num = (fixed.num + (intg*MULTIPLICAND));
    return fixed;
}

// Subtracts an integer number from a fixed-point number. Result is a fixed-point number.
struct fp_num subtract_int_fp(struct fp_num fixed,int32_t intg){
    fixed.num = (fixed.num - (intg*MULTIPLICAND));
    return fixed;
}

// Multiplies two fixed-point numbers. Result is a fixed-point number.
// Note: There is no overflow detection.
struct fp_num multiply_fp(struct fp_num one, struct fp_num two){
    one.num = (((int64_t) one.num) * two.num)/MULTIPLICAND;
    return one;
}

// Multiplies a fixed-point number with an integer number. Result is fixed-point.
struct fp_num multiply_int_fp(struct fp_num fixed,int32_t intg){
    fixed.num = (fixed.num*intg);
    return fixed;
}

// Divides two fixed-point numbers. Result is fixed-point.
struct fp_num divide_fp(struct fp_num one,struct fp_num two){
    one.num = (((int64_t) one.num) * MULTIPLICAND)/two.num;
}

// Divides a fixed-point number by an integer number. Result is fixed-point.
struct fp_num divide_fp_int(struct fp_num fixed,int32_t intg){
    fixed.num = (fixed.num/intg);
}