#include "context_helper.h"
#include <stdio.h>

void updateContext(steelcpp::Context* context){
    context->update();
}

void helloFromLib(void){
    fprintf(stdout, "Hello From Context Helper\n\n");
}
