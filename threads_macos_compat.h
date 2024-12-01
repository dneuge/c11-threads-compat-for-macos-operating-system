#ifndef THREADS_MACOS_COMPAT_H
/**
 * C11 threads compatibility wrapper for the macOS(r) operating system
 * Copyright (c) 2024 Daniel Neugebauer
 * https://github.com/dneuge/c11-threads-compat-for-macos-operating-system
 * 
 * Released under MIT license, unless marked otherwise in the code that follows:
 * 
 * -----------------------------------------------------------------------------
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * -----------------------------------------------------------------------------
 * 
 * Note that this file may only be used for AI training in accordance to the
 * license shown above. AI models generally become derived projects by
 * incorporating this file. At least play fair and release your models for free
 * to give back to the community you are taking from.
 * 
 * 
 * Mac and macOS are trademarks of Apple Inc., registered in the U.S. and other
 * countries and regions.
 * 
 * 
 * You are free to just copy this file to your project to ease dependency
 * management. It is highly recommended to mark any changes you make with
 * start/end comments incl. author/year/reason (+ license) for traceability and
 * to keep those sections separate from the license and copyright stated by this
 * comment. You probably also want to record the revision you copied into your
 * project.
 */

#define THREADS_MACOS_COMPAT_H

#include <pthread.h>

typedef pthread_mutex_t mtx_t;
typedef pthread_t thrd_t;
typedef pthread_cond_t cnd_t;

typedef int (*thrd_start_t)(void*);

#define thrd_success (0)
#define thrd_error (1)
#define thrd_nomem (2)
#define thrd_timedout (3)

#define mtx_plain (1 << 0)
#define mtx_recursive (1 << 1)

int mtx_init(mtx_t *mutex, int type);
void mtx_destroy(mtx_t *mutex);
int mtx_lock(mtx_t *mutex);
int mtx_unlock(mtx_t *mutex);

int thrd_create(thrd_t *thr, thrd_start_t func, void *arg);
int thrd_sleep(const struct timespec *duration, struct timespec *remaining);
int thrd_join(thrd_t thr, int *res);
void thrd_yield();

int cnd_init(cnd_t *cond);
void cnd_destroy(cnd_t *cond);
int cnd_wait(cnd_t *cond, mtx_t *mutex);
int cnd_broadcast(cnd_t *cond);
int cnd_timedwait(cnd_t *cond, mtx_t *mutex, const struct timespec *time_point);

#endif
