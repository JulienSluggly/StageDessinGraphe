#include "arg.h"

void giveArgInt(int* data, argInt a, int argc, char* const argv[]){
  (*data)=a.val;
  for (int i=1; i<argc; i++) {
    if (strcmp (a.ident,argv[i]) == 0){
      if (i+1<argc) (*data)=atoi(argv[i+1]);
      else {
	printf("Usage : %s %s <int>\n", argv[0], argv[i]);
	exit(1);
      }
    }
  }
}

void giveArgLongLong(long long* data, argLongLong a, int argc, char* const argv[]){
  (*data)=a.val;
  for (int i=1; i<argc; i++) {
    if (strcmp (a.ident,argv[i]) == 0){
      if (i+1<argc) (*data)=atol(argv[i+1]);
      else {
	printf("Usage : %s %s <long long>\n", argv[0], argv[i]);
	exit(1);
      }
    }
  }
}


void giveArgDouble(double* data, argDouble a, int argc, char* const argv[]){
  (*data)=a.val;
  for (int i=1; i<argc; i++) {
    if (strcmp (a.ident,argv[i]) == 0){
      if (i+1<argc) (*data)=atof(argv[i+1]);
      else {
	printf("Usage : %s %s <double>\n", argv[0], argv[i]);
	exit(1);
      }
    }
  }
}

void giveArgBool(int* data, argBool a, int argc, char* const argv[]){
  (*data)=0;
  for (int i=1; i<argc; i++)
    if (strcmp (a.ident,argv[i]) == 0)
      (*data)=1;
}

void giveArgStr(char* data, argStr a, int argc, char* const argv[]){
  int done = 0;
  for (int i=1; i<argc; i++) {
    if (strcmp (a.ident,argv[i]) == 0){
      if (i+1<argc) {
	strcat(data,argv[i+1]);
	done = 1;
      }
      else {
	printf("Usage : %s %s <string>\n", argv[0], argv[i]);
	exit(1);
      }
    }
  }
  if (done==0) strcat(data,a.val);
}
