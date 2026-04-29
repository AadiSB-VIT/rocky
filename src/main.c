#include <llvm-c/Core.h>
#include <llvm-c/Types.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <stdio.h>
#include <stdlib.h>

#include <rocky/main.h>
#include <rocky/jit.h>

typedef void printnum_fn(int p);

int main() {
    JITContext jit = {0};
    jit_init(&jit);
    
    jit_add_dummy_functions(&jit);
    jit_bake(&jit);
    
    jit_add_more_dummy_functions(&jit);
    jit_bake(&jit);
    
    printnum_fn* fn = (printnum_fn*) jit_lookup_function(&jit, "printnum");
    printnum_fn* fn2 = (printnum_fn*) jit_lookup_function(&jit, "printnum2");
    
    fn(10);
    fn2(10);
    fn(20);
    fn2(20);
    fn(30);
    fn2(30);
    jit_free(&jit);
    
    hello_world();
    
    return 0;
}
