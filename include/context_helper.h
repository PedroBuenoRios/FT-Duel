#ifndef CONTEXT_HELPER_H_
#define CONTEXT_HELPER_H_

#include "context.h"
#include  <dlfcn.h>

extern "C" void updateContext(steelcpp::Context* context);
extern "C" void helloFromLib(void);

typedef decltype(updateContext) updateContext_t;
typedef decltype(helloFromLib)  helloFromLib_t;

/*
extern "C"{
    void updateContext(steelcpp::Context* context);
}*/

#endif
