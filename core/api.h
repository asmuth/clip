/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once
#include <stdlib.h>

/**
 * The fviz C API
 */
#ifdef __cplusplus
extern "C" {
#endif

typedef struct fviz_s fviz_t;

/**
 * Initialize a new fviz context.
 *
 * @returns: A fviz context that must be free'd using `fviz_destroy`
 */
fviz_t* fviz_init();

/**
 * Free a fviz context
 */
void fviz_destroy(fviz_t* ctx);

/**
 * Retrieve the last error message. The returned pointer is valid until the next
 * `fviz_*` method is called on the context.
 */
const char* fviz_get_error(const fviz_t* ctx);

/**
 * Evaluate an fviz expression
 *
 * @returns: One (1) on success and zero (0) if an error has occured
 */
int fviz_eval(fviz_t* ctx, const char* expr);

/**
 * Retrieve the result. Pointer is valid until the next call to fviz_eval
 */
void fviz_get_result(fviz_t* ctx, const void** data, size_t* data_len);

/**
 * Set the output format
 */
void fviz_set_output_format(fviz_t* ctx, const char* format);


#ifdef __cplusplus
} // extern C
#endif

