/*
Module to enable fixed-point simple arithmetic operations using a
32-bit signed integer.




*/
#ifndef FIXED_POINT_H
#define FIXED_POINT_H
#include <stdint.h>
#define MULTIPLICAND 16384
struct fp_num {
int32_t num;
};

// Returns the integer value (Rounded to zero) of the given
// fp_num structure.
int32_t to_int(struct fp_num);

// Returns the fixed-point value of the given integer number.
struct fp_num to_fp(int32_t num);

// Adds two fixed-point numbers. Result is a fixed-point number.
struct fp_num add(struct fp_num,struct fp_num);

// Subtracts two fixed-point numbers. Result is a fixed point number.
struct fp_num subtract_fp(struct fp_num,struct fp_num);

// Adds a fixed pointer number and an integer number. Result is a fixed-point
// number.
struct fp_num add_fp_int(struct fp_num,int32_t);

// Subtracts an integer number from a fixed-point number. Result is a fixed-point number.
struct fp_num subtract_int_fp(struct fp_num,int32_t);

// Multiplies two fixed-point numbers. Result is a fixed-point number.
// Note: There is no overflow detection.
struct fp_num multiply_fp(struct fp_num, struct fp_num);

// Multiplies a fixed-point number with an integer number. Result is fixed-point.
struct fp_num multiply_int_fp(struct fp_num,int32_t);

// Divides two fixed-point numbers. Result is fixed-point.
struct fp_num divide_fp(struct fp_num,struct fp_num);

// Divides a fixed-point number by an integer number. Result is fixed-point.
struct fp_num divide_fp_int(struct fp_num,int32_t);

#endif