/* Intern API header som definerer TR-objektet, thokn, simlk, sep. 2011 */
#include "geo_lab.h"
struct TR {
    union geo_lab *plab_in, *plab_out;
    int   err;
};

struct XYZ {
    double x, y, z;
    int err;
}; 