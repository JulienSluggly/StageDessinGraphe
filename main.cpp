#include <string>
#include <stdio.h>
#include <random>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <set>
#include <cstdlib>

#include "graphe.hpp"
#include "dispOpenGL.hpp"
#include "personnel.hpp"
#include "ogdfFunctions.hpp"
#include "utilitaire.hpp"
#include "arg.h"

void initCPUSet(int core=0) {
#if defined(LINUX_OS)
    CPU_ZERO(&cpuset);
    CPU_SET(core, &cpuset);
	sched_setaffinity(0, sizeof(cpuset), &cpuset);
#endif
}

int main(int argc, char *argv[]) {
	return 0;
}