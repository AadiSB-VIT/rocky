#include <rocky/jit.h>

#include <stdio.h>
#include <stdlib.h>

#define MODULE_NAME_MAX 256

static void jit_verify_module_mutable(JITContext* ctx) {
    if (ctx->should_create_module) {
        char namebuf[MODULE_NAME_MAX] = {0};
        snprintf(namebuf, MODULE_NAME_MAX, "rocky_module_%d", ctx->created_module_count);
        
        ctx->current_module.handle = LLVMModuleCreateWithNameInContext((const char*) namebuf, ctx->ctx);
        ctx->current_module.threadsafe_handle = LLVMOrcCreateNewThreadSafeModule(ctx->current_module.handle, ctx->orc_threadsafe_ctx);
        
        ctx->created_module_count += 1;
        ctx->should_create_module = false;
    }
}

void jit_init(JITContext* ctx) {
    memset(ctx, 0, sizeof(JITContext));
    
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();
    
    ctx->ctx = LLVMContextCreate();
    ctx->orc_threadsafe_ctx = LLVMOrcCreateNewThreadSafeContextFromLLVMContext(ctx->ctx);
    ctx->builder = LLVMCreateBuilder();
    
    // TODO(voxel): manip options using LLJITBuilder
    LLVMErrorRef err = LLVMOrcCreateLLJIT(&ctx->jit, NULL);
    if (err) {
        fprintf(stderr, "JIT Engine could not be initialized\n");
        return;
    }
    ctx->jit_dylib = LLVMOrcLLJITGetMainJITDylib(ctx->jit);
    ctx->should_create_module = true;
}


// @Temporary Adds printnum
void jit_add_dummy_functions(JITContext* ctx) {
    jit_verify_module_mutable(ctx);
    LLVMModuleRef module = ctx->current_module.handle;
    
    // Add printf
    LLVMTypeRef printf_args[] = { LLVMPointerType(LLVMInt8Type(), 0) };
    LLVMTypeRef printf_type = LLVMFunctionType(LLVMInt32Type(), printf_args, 1, 1);
    LLVMValueRef printf_fn = LLVMAddFunction(module, "printf", printf_type);
    
    // Add simple printnum function
    LLVMTypeRef printnum_arg_types[] = { LLVMInt32Type() };
    LLVMTypeRef printnum_ret_type = LLVMVoidType();
    LLVMTypeRef printnum_func_type = LLVMFunctionType(printnum_ret_type, printnum_arg_types, 1, 0);
    LLVMValueRef printnum = LLVMAddFunction(module, "printnum", printnum_func_type);
    
    // Add entry BB
    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(printnum, "entry");
    LLVMPositionBuilderAtEnd(ctx->builder, entry);
    
    // Printf call
    LLVMValueRef printf_call_fmtstr = LLVMBuildGlobalStringPtr(ctx->builder, "Hello %d\n", "str");
    LLVMValueRef printf_call_args[] = { printf_call_fmtstr, LLVMGetParam(printnum, 0) };
    LLVMBuildCall2(ctx->builder, printf_type, printf_fn, printf_call_args, 2, "printf_call");
    LLVMBuildRet(ctx->builder, NULL);
}

// @Temporary Adds printnum2
void jit_add_more_dummy_functions(JITContext* ctx) {
    jit_verify_module_mutable(ctx);
    LLVMModuleRef module = ctx->current_module.handle;
    
    // Add printf
    LLVMTypeRef printf_args[] = { LLVMPointerType(LLVMInt8Type(), 0) };
    LLVMTypeRef printf_type = LLVMFunctionType(LLVMInt32Type(), printf_args, 1, 1);
    LLVMValueRef printf_fn = LLVMAddFunction(module, "printf", printf_type);
    
    // Add simple printnum function
    LLVMTypeRef printnum_arg_types[] = { LLVMInt32Type() };
    LLVMTypeRef printnum_ret_type = LLVMVoidType();
    LLVMTypeRef printnum_func_type = LLVMFunctionType(printnum_ret_type, printnum_arg_types, 1, 0);
    LLVMValueRef printnum = LLVMAddFunction(module, "printnum2", printnum_func_type);
    
    // Add entry BB
    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(printnum, "entry");
    LLVMPositionBuilderAtEnd(ctx->builder, entry);
    
    // Printf call
    LLVMValueRef printf_call_fmtstr = LLVMBuildGlobalStringPtr(ctx->builder, "World %d\n", "str");
    LLVMValueRef printf_call_args[] = { printf_call_fmtstr, LLVMGetParam(printnum, 0) };
    LLVMBuildCall2(ctx->builder, printf_type, printf_fn, printf_call_args, 2, "printf_call");
    LLVMBuildRet(ctx->builder, NULL);
}


void jit_bake(JITContext* ctx) {
    LLVMOrcLLJITAddLLVMIRModule(ctx->jit, ctx->jit_dylib, ctx->current_module.threadsafe_handle);
    ctx->should_create_module = true;
}

void_func* jit_lookup_function(JITContext* ctx, char* function_name) {
    LLVMOrcJITTargetAddress ret = 0;
    LLVMOrcLLJITLookup(ctx->jit, &ret, function_name);
    return (void_func*) ret;
}

void jit_free(JITContext* ctx) {
    LLVMDisposeBuilder(ctx->builder);
    
    LLVMOrcDisposeLLJIT(ctx->jit);
    LLVMOrcDisposeThreadSafeContext(ctx->orc_threadsafe_ctx);
}
