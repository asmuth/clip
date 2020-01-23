/**
 * This file is part of the "clip" project
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
 * The clip C API
 */
#ifdef __cplusplus
extern "C" {
#endif

#define FVIZ_API __attribute__((visibility ("default")))

typedef struct clip_s clip_t;

/**
 * Initialize a new clip context.
 *
 * @returns: A clip context that must be free'd using `clip_destroy`
 */
FVIZ_API
clip_t* clip_init();

/**
 * Free a clip context
 */
FVIZ_API
void clip_destroy(clip_t* ctx);

/**
 * Retrieve the last error message. The returned pointer is valid until the next
 * `clip_*` method is called on the context.
 */
FVIZ_API
const char* clip_get_error(const clip_t* ctx);

/**
 * Evaluate an clip expression
 *
 * @returns: One (1) on success and zero (0) if an error has occured
 */
FVIZ_API
int clip_eval(clip_t* ctx, const char* expr);

/**
 * Retrieve the result. Pointer is valid until the next call to clip_eval
 */
FVIZ_API
void clip_get_result(clip_t* ctx, const void** data, size_t* data_len);

/**
 * Set the output format
 */
FVIZ_API
void clip_set_output_format(clip_t* ctx, const char* format);


#ifdef __cplusplus
} // extern C
#endif

