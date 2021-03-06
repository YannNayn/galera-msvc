// Copyright (C) 2007 Codership Oy <info@codership.com>

// $Id: gu_fifo_test.c 2364 2011-09-16 16:32:07Z alex $

#include <check.h>
#include "gu_fifo_test.h"
#include "../src/galerautils.h"

#define FIFO_LENGTH 10000L
#ifndef EBADFD
#define EBADFD WSAENOTSOCK
#endif
START_TEST (gu_fifo_test)
{
    int err;
    gu_fifo_t* fifo;
    long i;
#ifdef _MSC_VER
	unsigned __int32 * item;
#else
    size_t* item;
#endif
    long used;
#ifdef _MSC_VER
#if GU_WORDSIZE == 64
	printf("\n______________________________\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\nWARNING ....\nWARNING ...\nWARNING ..\nWARNING .\nWARNING:\n64 bits issue:Treating item as 32 bits unsigned int in test %s(%d)-%s\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n______________________________\n",__FILE__,__LINE__,__FUNCTION__);
#endif
#endif
    fifo = gu_fifo_create (0, 1);
    fail_if (fifo != NULL);

    fifo = gu_fifo_create (1, 0);
    fail_if (fifo != NULL);

    fifo = gu_fifo_create (1, 1);
    fail_if (fifo == NULL);
    gu_fifo_close   (fifo);
    mark_point();
    gu_fifo_destroy (fifo);
    mark_point();

    fifo = gu_fifo_create (FIFO_LENGTH, sizeof(i));
    fail_if (fifo == NULL);
    fail_if (gu_fifo_length(fifo) != 0, "fifo->used is %lu for an empty FIFO",
             gu_fifo_length(fifo));

    // fill FIFO
    for (i = 0; i < FIFO_LENGTH; i++) {
        item = gu_fifo_get_tail (fifo);
        fail_if (item == NULL, "could not get item %ld", i);
        *item = i;
        gu_fifo_push_tail (fifo);
    }

    used = i;
    fail_if (gu_fifo_length(fifo) != used, "used is " SIZET_PRINTF_SPEC ", expected " SIZET_PRINTF_SPEC "", 
             used, gu_fifo_length(fifo));

    // test pop
    for (i = 0; i < used; i++) {
        int err;
        
        item = gu_fifo_get_head (fifo, &err);
        fail_if (item == NULL, "could not get item %ld", i);
#ifdef _MSC_VER
#if GU_WORDSIZE == 32        
        fail_if (*item != (ulong)i, "got %ld, expected %ld", *item, i);
#else
		{
			unsigned __int32 _item;
			size_t _i;
			_item = *item;
			_i = (size_t)i;
	        fail_if (_item != _i, "got %ld, expected %ld", *item, i);
		}
#endif        
#else        
        fail_if (*item != (ulong)i, "got %ld, expected %ld", *item, i);
#endif        
        gu_fifo_pop_head (fifo);
    }

    fail_if (gu_fifo_length(fifo) != 0,
             "gu_fifo_length() for empty queue is %ld",
             gu_fifo_length(fifo));

    gu_fifo_close (fifo);

    
    item = gu_fifo_get_head (fifo, &err);
    fail_if (item != NULL);
    fail_if (err  != -ENODATA);

    gu_fifo_destroy (fifo);
}
END_TEST

static pthread_mutex_t
sync_mtx = PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t
sync_cond = PTHREAD_COND_INITIALIZER;

#define ITEM 12345

static void*
cancel_thread (void* arg)
{
    size_t* item;
    int     err;

    gu_fifo_t* q = arg;

    /* sync with parent */
    pthread_mutex_lock (&sync_mtx);
    pthread_cond_signal (&sync_cond);
    pthread_mutex_unlock (&sync_mtx);

    
    /* try to get from non-empty queue */
    item = gu_fifo_get_head (q, &err);
    fail_if (NULL != item, "Got item %p: " SIZET_PRINTF_SPEC "", item, item ? *item : 0);
    fail_if (-ECANCELED != err);

    /* signal end of the first gu_fifo_get_head() */
    pthread_mutex_lock (&sync_mtx);
    pthread_cond_signal (&sync_cond);
    /* wait until gets are resumed */
    pthread_cond_wait (&sync_cond, &sync_mtx);

    item = gu_fifo_get_head (q, &err);
    fail_if (NULL == item);
    fail_if (ITEM != *item);
    gu_fifo_pop_head (q);

    /* signal end of the 2nd gu_fifo_get_head() */
    pthread_cond_signal (&sync_cond);
    pthread_mutex_unlock (&sync_mtx);

    /* try to get from empty queue (should block) */
    item = gu_fifo_get_head (q, &err);
    fail_if (NULL != item);
    fail_if (-ECANCELED != err);

    /* signal end of the 3rd gu_fifo_get_head() */
    pthread_mutex_lock (&sync_mtx);
    pthread_cond_signal (&sync_cond);
    /* wait until fifo is closed */
    pthread_cond_wait (&sync_cond, &sync_mtx);

    item = gu_fifo_get_head (q, &err);
    fail_if (NULL != item);
    fail_if (-ECANCELED != err);

    /* signal end of the 4th gu_fifo_get_head() */
    pthread_cond_signal (&sync_cond);
    /* wait until fifo is resumed */
    pthread_cond_wait (&sync_cond, &sync_mtx);
    pthread_mutex_unlock (&sync_mtx);

    item = gu_fifo_get_head (q, &err);
    fail_if (NULL != item);
    fail_if (-ENODATA != err);

    return NULL;
}

START_TEST(gu_fifo_cancel_test)
{
    pthread_t thread;
    int err;
    gu_fifo_t* q = gu_fifo_create (FIFO_LENGTH, sizeof(size_t));

    size_t* item = gu_fifo_get_tail (q);
    fail_if (item == NULL);
    *item = ITEM;
    gu_fifo_push_tail (q);

    pthread_mutex_lock (&sync_mtx);

    
    pthread_create (&thread, NULL, cancel_thread, q);

    /* sync with child thread */
    gu_fifo_lock (q);
    pthread_cond_wait (&sync_cond, &sync_mtx);

    
    err = gu_fifo_cancel_gets (q);
    fail_if (0 != err);
    err = gu_fifo_cancel_gets (q);
    fail_if (-EBADFD != err);

    /* allow the first gu_fifo_get_head() */
    gu_fifo_release (q);
    mark_point();

    /* wait for the first gu_fifo_get_head() to complete */
    pthread_cond_wait (&sync_cond, &sync_mtx);

    err = gu_fifo_resume_gets (q);
    fail_if (0 != err);
    err = gu_fifo_resume_gets (q);
    fail_if (-EBADFD != err);

    /* signal that now gets are resumed */
    pthread_cond_signal (&sync_cond);
    /* wait for the 2nd gu_fifo_get_head() to complete */
    pthread_cond_wait (&sync_cond, &sync_mtx);

    /* wait a bit to make sure 3rd gu_fifo_get_head() is blocked 
     * (even if it is not - still should work)*/
    usleep (100000 /* 0.1s */);
    err = gu_fifo_cancel_gets (q);
    fail_if (0 != err);

    /* wait for the 3rd gu_fifo_get_head() to complete */
    pthread_cond_wait (&sync_cond, &sync_mtx);

    gu_fifo_close (q); // closes for puts, but the q still must be canceled

    pthread_cond_signal (&sync_cond);
    /* wait for the 4th gu_fifo_get_head() to complete */
    pthread_cond_wait (&sync_cond, &sync_mtx);

    gu_fifo_resume_gets (q); // resumes gets

    pthread_cond_signal (&sync_cond);
    pthread_mutex_unlock (&sync_mtx);

    mark_point();

    pthread_join(thread, NULL);
}
END_TEST

Suite *gu_fifo_suite(void)
{
    Suite *s  = suite_create("Galera FIFO functions");
    TCase *tc = tcase_create("gu_fifo");

    suite_add_tcase (s, tc);
    tcase_add_test  (tc, gu_fifo_test);
    tcase_add_test  (tc, gu_fifo_cancel_test);
    return s;
}

