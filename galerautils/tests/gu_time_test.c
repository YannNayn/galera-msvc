// Copyright (C) 2007 Codership Oy <info@codership.com>

// $Id: gu_time_test.c 986 2009-08-23 18:02:01Z alex $

#include <math.h>
#include <check.h>
#include "gu_time_test.h"
#include "../src/gu_time.h"

START_TEST (gu_time_test)
{
    struct timeval left  = { 1, 900000 }; // 1.9 sec
    struct timeval right = { 5, 400000 }; // 5.4 sec
    double diff, tolerance = 1.0e-15; // double precision tolerance

    diff = gu_timeval_diff (&left, &right);
    fail_if (fabs(3.5 + diff) > tolerance,
             "Expected %f, got %f, delta: %e", -3.5, diff, 3.5 + diff);
    diff = gu_timeval_diff (&right, &left);
    fail_if (fabs(3.5 - diff) > tolerance,
             "Expected %f, got %f, delta: %e", 3.5, diff, 3.5 - diff);
}
END_TEST

Suite *gu_time_suite(void)
{
  Suite *s  = suite_create("Galera time functions");
  TCase *tc = tcase_create("gu_time");

  suite_add_tcase (s, tc);
  tcase_add_test  (tc, gu_time_test);
  return s;
}

