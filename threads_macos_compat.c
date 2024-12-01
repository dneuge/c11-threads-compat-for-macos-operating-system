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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <sched.h>

#include "utils.h"

#include "threads_macos_compat.h"

int mtx_init(mtx_t *mutex, int type) {
    int err = 0;
    int fin_err = 0;

    if (type != mtx_plain && type != (mtx_plain | mtx_recursive)) {
        printf("[threads_macos_compat] mtx_init unsupported type requested: %d\n", type);
        return thrd_error;
    }

    pthread_mutexattr_t attr = {0};
    err = pthread_mutexattr_init(&attr);
    if (err) {
        printf("[threads_macos_compat] pthread_mutexattr_init error: %d %s\n", err, strerror(err));
        return thrd_error;
    }
    
    if (type & mtx_recursive) {
        err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        if (err) {
            printf("[threads_macos_compat] pthread_mutexattr_settype PTHREAD_MUTEX_RECURSIVE error: %d %s\n", err, strerror(err));
            goto end;
        }
    }
    
    err = pthread_mutex_init(mutex, &attr);
    if (err) {
        printf("[threads_macos_compat] pthread_mutex_init error: %d %s\n", err, strerror(err));
    }

end:
    fin_err = pthread_mutexattr_destroy(&attr);
    if (fin_err) {
        printf("[threads_macos_compat] pthread_mutexattr_destroy error: %d %s\n", fin_err, strerror(fin_err));
    }
    
    return err ? thrd_error : thrd_success;
}

void mtx_destroy(mtx_t *mutex) {
    int err = pthread_mutex_destroy(mutex);
    if (err) {
        printf("[threads_macos_compat] pthread_mutex_destroy error: %d %s\n", err, strerror(err));
    }
}

int mtx_lock(mtx_t *mutex) {
    int err = pthread_mutex_lock(mutex);
    if (err) {
        printf("[threads_macos_compat] pthread_mutex_lock error: %d %s\n", err, strerror(err));
    }

    return err ? thrd_error : thrd_success;
}

int mtx_unlock(mtx_t *mutex) {
    int err = pthread_mutex_unlock(mutex);
    if (err) {
        printf("[threads_macos_compat] pthread_mutex_unlock error: %d %s\n", err, strerror(err));
    }

    return err ? thrd_error : thrd_success;
}

typedef struct {
    thrd_start_t actual_func;
    void *actual_arg;
    int res;
} wrapped_thread_t;

static void* wrap_thread_func(void *arg) {
    wrapped_thread_t *wrapped = arg;
    wrapped->res = wrapped->actual_func(wrapped->actual_arg);
    return arg;
}

int thrd_create(thrd_t *thr, thrd_start_t func, void *arg) {
    wrapped_thread_t *wrapped = zmalloc(sizeof(wrapped_thread_t));
    if (!wrapped) {
        printf("[threads_macos_compat] thrd_create out of memory?\n");
        return thrd_error;
    }

    wrapped->actual_func = func;
    wrapped->actual_arg = arg;

    int err = pthread_create(thr, NULL, wrap_thread_func, wrapped);
    if (err) {
        printf("[threads_macos_compat] pthread_create error: %d %s\n", err, strerror(err));
        free(wrapped);
    }

    return err ? thrd_error : thrd_success;
}

int thrd_sleep(const struct timespec *duration, struct timespec *remaining) {
    return nanosleep(duration, remaining);
}

int thrd_join(thrd_t thr, int *res) {
    wrapped_thread_t *wrapped = NULL;
    int err = pthread_join(thr, (void**) &wrapped);
    if (err) {
        printf("[threads_macos_compat] pthread_join error: %d %s\n", err, strerror(err));
        return thrd_error;
    }

    if (!wrapped) {
        printf("[threads_macos_compat] joined thread returned NULL (not our wrapper?!)\n");

        if (res) {
            *res = 0;
        }
    } else {
        if (res) {
            *res = wrapped->res;
        }

        free(wrapped);
    }

    return thrd_success;
}

void inline thrd_yield() {
    sched_yield();
}

int cnd_init(cnd_t *cond) {
    int err = pthread_cond_init(cond, NULL);
    if (err) {
        printf("[threads_macos_compat] pthread_cond_init error: %d %s\n", err, strerror(err));
        return (err == ENOMEM) ? thrd_nomem : thrd_error;
    }

    return thrd_success;
}

void cnd_destroy(cnd_t *cond) {
    int err = pthread_cond_destroy(cond);
    if (err) {
        printf("[threads_macos_compat] pthread_cond_destroy error: %d %s\n", err, strerror(err));
    }
}

int cnd_wait(cnd_t *cond, mtx_t *mutex) {
    int err = pthread_cond_wait(cond, mutex);
    if (err) {
        printf("[threads_macos_compat] pthread_cond_wait error: %d %s\n", err, strerror(err));
        return thrd_error;
    }
    
    return thrd_success;
}

int cnd_broadcast(cnd_t *cond) {
    int err = pthread_cond_broadcast(cond);
    if (err) {
        printf("[threads_macos_compat] pthread_cond_broadcast error: %d %s\n", err, strerror(err));
        return thrd_error;
    }

    return thrd_success;
}

int cnd_timedwait(cnd_t *cond, mtx_t *mutex, const struct timespec *time_point) {
    // FIXME: C11 time_point should be UTC-based but POSIX threads do not specify any time zone ("system time"?)
    int err = pthread_cond_timedwait(cond, mutex, time_point);
    if (err) {
        if (err == ETIMEDOUT) {
            return thrd_timedout;
        }

        printf("[threads_macos_compat] pthread_cond_timedwait error: %d %s\n", err, strerror(err));
        return thrd_error;
    }
    
    return thrd_success;
}
