/* Copyright (C) 2011 Codership Oy <info@codership.com> */

#include "garb_config.hpp"
#include "garb_recv_loop.hpp"

#include <galerautils.hpp>

#include <iostream>

#include <stdlib.h> // exit()
#ifdef _MSC_VER
#include <unistd.h> 
#else
#include <unistd.h> // setsid(), chdir()
#endif

#include <fcntl.h>  // open()

namespace garb
{
#ifndef _MSC_VER
void
become_daemon ()
{
    if (pid_t pid = fork())
    {
        if (pid > 0) // parent
        {
            exit(0);
        }
        else
        {
            // I guess we want this to go to stderr as well;
            std::cerr << "Failed to fork daemon process: "
                      << errno << " (" << strerror(errno) << ")";
            gu_throw_error(errno) << "Failed to fork daemon process";
        }
    }

    // child
    if (setsid()<0) // become a new process leader, detach from terminal
    {
        gu_throw_error(errno) << "setsid() failed";
    }
    if (chdir("/")) // detach from potentially removable block devices
    {
        gu_throw_error(errno) << "chdir(\"/\") failed";
    }

    // umask(0);

    // A second fork ensures the process cannot acquire a controlling
    // terminal.
    if (pid_t pid = fork())
    {
        if (pid > 0)
        {
            exit(0);
        }
        else
        {
            gu_throw_error(errno) << "Second fork failed";
        }
    }

    // Close the standard streams. This decouples the daemon from the
    // terminal that started it.
    close(0);
    close(1);
    close(2);

    // Bind standard fds (0, 1, 2) to /dev/null
    for (int fd = 0; fd < 3; ++fd)
    {
#ifdef _MSC_VER
    if (open("NUL", O_RDONLY) < 0)
#else
    if (open("/dev/null", O_RDONLY) < 0)
#endif    
        {
            gu_throw_error(errno) << "Unable to open /dev/null for fd " << fd;
        }
    }
}
#endif
int
main (int argc, char* argv[])
{
    Config config(argc, argv);

    log_info << "Read config: " <<  config << std::endl;
#ifndef _MSC_VER
    if (config.daemon()) become_daemon();
#else
    if (config.daemon())
    {
        log_fatal << "Wont fork on windows right now" << std::endl;
        return 1;
    }
#endif    
    RecvLoop loop (config);

    return 0;
}

} /* namespace garb */

int
main (int argc, char* argv[])
{
    try
    {
        return garb::main (argc, argv);
    }
    catch (std::exception& e)
    {
        log_fatal << e.what();
        return 1;
    }
}

