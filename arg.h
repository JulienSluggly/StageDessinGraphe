#ifndef MY_ARG_H
#define MY_ARG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct arg_int {
  int val;
  char ident[20];
  char mess[80];
} argInt;

typedef struct arg_long_long {
  long long val;
  char ident[20];
  char mess[80];
} argLongLong;

typedef struct arg_double {
  double val;
  char ident[20];
  char mess[80];
} argDouble;

typedef struct arg_str {
  char val[80];
  char ident[20];
  char mess[80];
} argStr;

typedef struct arg_bool {
  char ident[20];
  char mess[80];
} argBool;

void giveArgInt(int* data, argInt a, int argc, char* const argv[]);
void giveArgLongLong(long long* data, argLongLong a, int argc, char* const argv[]);
void giveArgDouble(double* data, argDouble a, int argc, char* const argv[]);
void giveArgStr(char* data, argStr a, int argc, char* const argv[]);
void giveArgBool(int* data, argBool a, int argc, char* const argv[]);

#endif
