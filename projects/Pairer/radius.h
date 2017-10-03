//
// Created by Andrey Khrulev on 18.09.17.
//

#ifndef SAMPLEPROJECT_RADIUS_H
#define SAMPLEPROJECT_RADIUS_H

#define RCSIDH(...)
#define virtual virtual_new
#define new new_new
extern "C" {
#include <freeradius-devel/libradius.h>
#include <freeradius-devel/conf.h>
}
#undef virtual
#undef new
#undef PACKAGE_URL

#endif //SAMPLEPROJECT_RADIUS_H
