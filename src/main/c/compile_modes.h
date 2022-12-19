#ifndef COMPILE_MODES_H
#define COMPILE_MODES_H


#define PRODUCTION_COMPILE_MODE

// #define DEBUG_COMPILE_MODE

#ifdef PRODUCTION_COMPILE_MODE
    #ifdef DEBUG_COMPILE_MODE
        #error cannot complete compile, debug mode is active
    #endif
#endif

#endif //COMPILE_MODES_H