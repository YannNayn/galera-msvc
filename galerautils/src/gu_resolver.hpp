/*
 * Copyright (C) 2008-2012 Codership Oy <info@codership.com>
 *
 * $Id$
 */

/*!
 * @file gu_resolver.hpp Simple resolver utility
 */

#ifndef __GU_RESOLVER_HPP__
#define __GU_RESOLVER_HPP__

#include "gu_throw.hpp"
#ifdef _MSC_VER
#include <windows.h>
#include <ws2ipdef.h>
#include <ws2tcpip.h>
typedef unsigned short sa_family_t;

/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KDEWIN_ASM_SOCKET_H
#define KDEWIN_ASM_SOCKET_H

// include everywhere
#include <sys/types.h>

#define IOCPARM_MASK    0x7f            /* parameters must be < 128 bytes */
#define IOC_VOID        0x20000000      /* no parameters */
#define IOC_OUT         0x40000000      /* copy out parameters */
#define IOC_IN          0x80000000      /* copy in parameters */

//#define _IO(x,y)        (IOC_VOID|(x<<8)|y)
//#define _IOR(x,y,t)     (IOC_OUT|(((long)sizeof(t)&IOCPARM_MASK)<<16)|(x<<8)|y)
//#define _IOW(x,y,t)     (IOC_IN|(((long)sizeof(t)&IOCPARM_MASK)<<16)|(x<<8)|y)

#define SIOCATMARK  _IOR('s',  7, u_long)  /* at oob mark? */
#define FIONREAD    _IOR('f', 127, u_long) /* get # bytes to read */
//#define FIONBIO 0x8004667e /* To be compatible with termiost version */
#define REAL_FIONBIO     _IOW('f', 126, u_long) /* set/clear non-blocking i/o */
#define FIOASYNC    _IOW('f', 125, u_long) /* set/clear async i/o */
#define SIOCSHIWAT  _IOW('s',  0, u_long)  /* set high watermark */
#define SIOCGHIWAT  _IOR('s',  1, u_long)  /* get high watermark */
#define SIOCSLOWAT  _IOW('s',  2, u_long)  /* set low watermark */
#define SIOCGLOWAT  _IOR('s',  3, u_long)  /* get low watermark */

/* Needed for if queries */
#define SIOCGIFCONF     _IOW('s', 100, struct ifconf) /* get if list */
#define SIOCGIFFLAGS    _IOW('s', 101, struct ifreq) /* Get if flags */
#define SIOCGIFADDR     _IOW('s', 102, struct ifreq) /* Get if addr */
#define SIOCGIFBRDADDR  _IOW('s', 103, struct ifreq) /* Get if broadcastaddr */
#define SIOCGIFNETMASK  _IOW('s', 104, struct ifreq) /* Get if netmask */
#define SIOCGIFHWADDR   _IOW('s', 105, struct ifreq) /* Get hw addr */
#define SIOCGIFMETRIC   _IOW('s', 106, struct ifreq) /* get metric */
#define SIOCGIFMTU      _IOW('s', 107, struct ifreq) /* get MTU size */

#define SOL_SOCKET      0xffff          /* options for socket level */

#define SO_DEBUG        0x0001          /* turn on debugging info recording */
#define SO_ACCEPTCONN   0x0002          /* socket has had listen() */
#define SO_REUSEADDR    0x0004          /* allow local address reuse */
#define SO_KEEPALIVE    0x0008          /* keep connections alive */
#define SO_DONTROUTE    0x0010          /* just use interface addresses */
#define SO_BROADCAST    0x0020          /* permit sending of broadcast msgs */
#define SO_USELOOPBACK  0x0040          /* bypass hardware when possible */
#define SO_LINGER       0x0080          /* linger on close if data present */
#define SO_OOBINLINE    0x0100          /* leave received OOB data in line */
//#define SO_DONTLINGER   (u_int)(~SO_LINGER)

#define SIOCGIFINDEX 0x8933

/*
 * Additional options.
 */
#define SO_SNDBUF       0x1001          /* send buffer size */
#define SO_RCVBUF       0x1002          /* receive buffer size */
#define SO_SNDLOWAT     0x1003          /* send low-water mark */
#define SO_RCVLOWAT     0x1004          /* receive low-water mark */
#define SO_SNDTIMEO     0x1005          /* send timeout */
#define SO_RCVTIMEO     0x1006          /* receive timeout */
#define SO_ERROR        0x1007          /* get error status and clear */
#define SO_TYPE         0x1008          /* get socket type */



#ifndef IF_NAMESIZE
#define IF_NAMESIZE 16
#endif

struct if_nameindex
{
    unsigned int if_index;
    char *if_name;
};

//---------------------------------------------------------------------------------------

struct ifaddr {
    struct sockaddr ifa_addr;
    union {
        struct sockaddr ifu_broadaddr;
        struct sockaddr ifu_dstaddr;
    } ifa_ifu;
    struct iface *ifa_ifp;
    struct ifaddr *ifa_next;
};


//---------------------------------------------------------------------------------------

struct ifmap {
    unsigned long int mem_start;
    unsigned long int mem_end;
    unsigned short int base_addr;
    unsigned char irq;
    unsigned char dma;
    unsigned char port;
};


struct ifconf {
    int ifc_len;
    union {
        void *ifcu_buf;
        struct ifreq *ifcu_req;
    } ifc_ifcu;
};
 
#define ifc_buf ifc_ifcu.ifcu_buf
#define ifc_req ifc_ifcu.ifcu_req
#define _IOT_ifconf _IOT(_IOTS(struct ifconf),1,0,0,0,0)


 
#ifndef IFHWADDRLEN
#define IFHWADDRLEN 6
#endif
#ifndef IFNAMSIZ
#define IFNAMSIZ IF_NAMESIZE
#endif

struct ifreq {
    union {
        char ifrn_name[IFNAMSIZ];
    } ifr_ifrn;
    union {
        struct sockaddr ifru_addr;
        struct sockaddr ifru_dstaddr;
        struct sockaddr ifru_broadaddr;
        struct sockaddr ifru_netmask;
        struct sockaddr ifru_hwaddr;
        short int ifru_flags;
        int ifru_ivalue;
        int ifru_mtu;
        struct ifmap ifru_map;
        char ifru_slave[IFNAMSIZ];
        char ifru_newname[IFNAMSIZ];
        void *ifru_data;
    } ifr_ifru;
};

#define ifr_name ifr_ifrn.ifrn_name
#define ifr_hwaddr ifr_ifru.ifru_hwaddr
#define ifr_addr ifr_ifru.ifru_addr
#define ifr_dstaddr ifr_ifru.ifru_dstaddr
#define ifr_broadaddr ifr_ifru.ifru_broadaddr
#define ifr_netmask ifr_ifru.ifru_netmask
#define ifr_flags ifr_ifru.ifru_flags
#define ifr_metric ifr_ifru.ifru_ivalue
#define ifr_mtu ifr_ifru.ifru_mtu
#define ifr_map ifr_ifru.ifru_map
#define ifr_slave ifr_ifru.ifru_slave
#define ifr_data ifr_ifru.ifru_data
#define ifr_ifindex ifr_ifru.ifru_ivalue
#define ifr_bandwidth ifr_ifru.ifru_ivalue
#define ifr_qlen ifr_ifru.ifru_ivalue
#define ifr_newname ifr_ifru.ifru_newname
#define _IOT_ifreq _IOT(_IOTS(char),IFNAMSIZ,_IOTS(char),16,0,0)
#define _IOT_ifreq_short _IOT(_IOTS(char),IFNAMSIZ,_IOTS(short),1,0,0)
#define _IOT_ifreq_int _IOT(_IOTS(char),IFNAMSIZ,_IOTS(int),1,0,0)

#endif // KDEWIN_ASM_SOCKET_H

#else
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include <string>


// Forward declarations
namespace gu
{
    class URI;
} // namespace gu

// Declarations
namespace gu
{
    namespace net
    {
        /*!
         * @class Sockaddr
         *
         * @brief Class encapsulating struct sockaddr.
         *
         * Class encapsulating struct sockaddr and providing
         * simple interface to access sockaddr fields.
         */
        class Sockaddr;

        /*!
         * @class IMReq
         *
         * @brief Class encapsulating imreq structs.
         */
        class MReq;

        /*!
         * @class Addrinfo
         *
         * @brief Class encapsulating struct addrinfo.
         *
         * Class encapsulating struct addrinfo and providing interface
         * to access addrinfo fields.
         */
        class Addrinfo;

        /*!
         * Resolve address given in @uri
         *
         * @return Addrinfo object representing address
         *
         * @throw gu::Exception in case of failure
         */
        Addrinfo resolve(const gu::URI& uri);
    } // namespace net
} // namespace gu


class gu::net::Sockaddr
{
public:
    /*!
     * Default constuctor.
     *
     * @param sa     Pointer to sockaddr struct
     * @param sa_len Length  of sockaddr struct
     */
    Sockaddr(const sockaddr* sa, socklen_t sa_len);

    /*!
     * Copy constructor.
     *
     * @param sa Reference to Sockaddr
     */
    Sockaddr(const Sockaddr& sa);

    /*!
     * Destructor
     */
    ~Sockaddr();

    /*!
     * Get address family.
     *
     * @return Address family
     */
    sa_family_t get_family() const { return sa_->sa_family; }

    /*!
     * Get port in network byte order. This is applicable only
     * for AF_INET, AF_INET6.
     *
     * @return Port in nework byte order
     */
    unsigned short get_port() const
    {
        switch(sa_->sa_family)
        {
        case AF_INET:
            return reinterpret_cast<const sockaddr_in*>(sa_)->sin_port;
        case AF_INET6:
            return reinterpret_cast<const sockaddr_in6*>(sa_)->sin6_port;
        default:
            gu_throw_fatal;
        }
    }

    /*!
     * Get pointer to address. Return value is pointer to void,
     * user must do casting by himself.
     *
     * @todo: Figure out how this could be done in type safe way.
     *
     * @return Void pointer to address element.
     */
    const void* get_addr() const
    {
        switch(sa_->sa_family)
        {
        case AF_INET:
            return &reinterpret_cast<const sockaddr_in*>(sa_)->sin_addr;
        case AF_INET6:
            return &reinterpret_cast<const sockaddr_in6*>(sa_)->sin6_addr;
        default:
            gu_throw_fatal  << "invalid address family: " << sa_->sa_family;
        }
    }

    socklen_t get_addr_len() const
    {
        switch(sa_->sa_family)
        {
        case AF_INET:
            return sizeof(reinterpret_cast<const sockaddr_in*>(sa_)->sin_addr);
        case AF_INET6:
            return sizeof(reinterpret_cast<const sockaddr_in6*>(sa_)->sin6_addr);
        default:
            gu_throw_fatal;
        }
    }

    /*!
     * Get non-const reference to sockaddr struct.
     *
     * @return Non-const reference to sockaddr struct.
     */
    sockaddr& get_sockaddr() { return *sa_; }

    /*!
     * Get const reference to sockaddr struct.
     *
     * @return Const reference to sockaddr struct.
     */
    const sockaddr& get_sockaddr() const { return *sa_; }

    /*!
     * Get length of sockaddr struct.
     *
     * @return Length of sockaddr struct
     */
    socklen_t get_sockaddr_len() const { return sa_len_; }

    bool is_multicast() const;
    bool is_broadcast() const;
    bool is_anyaddr()   const;

    static Sockaddr get_anyaddr(const Sockaddr& sa)
    {
        Sockaddr ret(sa);
        switch(ret.sa_->sa_family)
        {
        case AF_INET:
            reinterpret_cast<sockaddr_in*>(ret.sa_)->sin_addr.s_addr = 0;
            break;
        case AF_INET6:
            memset(&reinterpret_cast<sockaddr_in6*>(ret.sa_)->sin6_addr,
                   0, sizeof(struct in6_addr));
            break;
        default:
            gu_throw_fatal << "invalid address family: " << ret.sa_->sa_family;
        }
        return ret;
    }

    Sockaddr& operator=(const Sockaddr& sa)
    {
        memcpy(sa_, sa.sa_, sa_len_);
        return *this;
    }

private:

    sockaddr* sa_;
    socklen_t sa_len_;
};


class gu::net::MReq
{
public:

    MReq(const Sockaddr& mcast_addr, const Sockaddr& if_addr);
    ~MReq();

    const void* get_mreq() const        { return mreq_; }
    socklen_t get_mreq_len() const      { return mreq_len_; }
    int get_ipproto() const             { return ipproto_; }
    int get_add_membership_opt() const  { return add_membership_opt_; }
    int get_drop_membership_opt() const { return drop_membership_opt_; }
    int get_multicast_if_opt() const    { return multicast_if_opt_; }
    int get_multicast_loop_opt() const  { return multicast_loop_opt_; }
    int get_multicast_ttl_opt() const   { return multicast_ttl_opt_; }
    const void* get_multicast_if_value() const;
    int get_multicast_if_value_size() const;

private:

    MReq(const MReq&);
    void operator=(const MReq&);

    void* mreq_;
    socklen_t mreq_len_;
    int ipproto_;
    int add_membership_opt_;
    int drop_membership_opt_;
    int multicast_if_opt_;
    int multicast_loop_opt_;
    int multicast_ttl_opt_;
};


class gu::net::Addrinfo
{
public:
    /*!
     * Default constructor.
     *
     * @param ai Const reference to addrinfo struct
     */
    Addrinfo(const addrinfo& ai);

    /*!
     * Copy costructor.
     *
     * @param ai Const reference to Addrinfo object to copy
     */
    Addrinfo(const Addrinfo& ai);

    /*!
     * Copy constructor that replaces @ai sockaddr struct.
     *
     * @param ai Const reference to Addrinfo object to copy
     * @param sa Const reference to Sockaddr struct that replaces
     *           @ai sockaddr data
     */
    Addrinfo(const Addrinfo& ai, const Sockaddr& sa);

    /*!
     * Destructor.
     */
    ~Addrinfo();

    /*!
     * Get address family, AF_INET, AF_INET6 etc.
     *
     * @return Address family
     */
    int get_family() const { return ai_.ai_family; }

    /*!
     * Get socket type, SOCK_STREAM, SOCK_DGRAM etc
     *
     * @return Socket type
     */
    int get_socktype() const { return ai_.ai_socktype; }

    /*!
     * Get protocol.
     *
     * @return Protocol
     */
    int get_protocol() const { return ai_.ai_protocol; }

    /*!
     * Get length of associated sockaddr struct
     *
     * @return Length of associated sockaddr struct
     */
    socklen_t get_addrlen() const { return ai_.ai_addrlen; }

    /*!
     * Get associated Sockaddr object.
     *
     * @return Associated Sockaddr object
     */

    Sockaddr get_addr() const
    { return Sockaddr(ai_.ai_addr, ai_.ai_addrlen); }

    /*!
     * Get string representation of the addrinfo.
     *
     * @return String representation of the addrinfo
     */
    std::string to_string() const;

private:

    addrinfo ai_;
};


#endif /* __GU_RESOLVER_HPP__ */
