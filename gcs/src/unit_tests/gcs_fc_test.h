// Copyright (C) 2010 Codership Oy <info@codership.com>

// $Id$

#ifndef __gcs_fc_test__
#define __gcs_fc_test__

#include <check.h>

Suite *gcs_fc_suite(void);
#ifdef _MSC_VER
#include <msvc_sup.h>
#define usleep(x) Sleep(x/1000)
#endif
#endif /* __gcs_fc_test__ */
