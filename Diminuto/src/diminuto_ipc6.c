/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * WORK IN PROGRESS!
 */

#include "com/diag/diminuto/diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static inline void ntoh6(diminuto_ipv6_t * addressp) {
    size_t ii;

    for (ii = 0; ii < countof(addressp->u16); ++ii) {
        addressp->u16[ii] = ntohs(addressp->u16[ii]);
    }
}

static inline void hton6(diminuto_ipv6_t * addressp) {
    size_t ii;

    for (ii = 0; ii < countof(addressp->u16); ++ii) {
        addressp->u16[ii] = htons(addressp->u16[ii]);
    }
}

static int ipv62ipv4(diminuto_ipv6_t address, diminuto_ipv4_t * addressp)
{
    int result = 0;

    if (address.u16[5] != 0xffff) {
        /* Do nothing. */
    } else if (address.u16[0] != 0x0000) {
        /* Do nothing. */
    } else if (address.u16[1] != 0x0000) {
        /* Do nothing. */
    } else if (address.u16[2] != 0x0000) {
        /* Do nothing. */
    } else if (address.u16[3] != 0x0000) {
        /* Do nothing. */
    } else if (address.u16[4] != 0x0000) {
        /* Do nothing. */
    } else {
        result = !0;
        if (addressp != (diminuto_ipv4_t *)0) {
            memcpy(addressp, &(address.u16[6]), sizeof(*addressp));
        }
    }

    return result;
}

int diminuto_ipc6_ipv62ipv4(diminuto_ipv6_t address, diminuto_ipv4_t * addressp)
{
    int result;

    hton6(&address);
    if (!(result = ipv62ipv4(address, addressp))) {
        /* Do nothing. */
    } else if (addressp == (diminuto_ipv4_t *)0) {
        /* Do nothing. */
    } else {
        *addressp = ntohl(*addressp);
    }

    return result;
}

static void ipv42ipv6(diminuto_ipv4_t address, diminuto_ipv6_t * addressp)
{
    addressp->u16[0] = 0x0000;
    addressp->u16[1] = 0x0000;
    addressp->u16[2] = 0x0000;
    addressp->u16[3] = 0x0000;
    addressp->u16[4] = 0x0000;
    addressp->u16[5] = 0xffff;
    memcpy(&(addressp->u16[6]), &address, sizeof(address));
}

void diminuto_ipc6_ipv42ipv6(diminuto_ipv4_t address, diminuto_ipv6_t * addressp)
{
    ipv42ipv6(htonl(address), addressp);
    ntoh6(addressp);
}

static int identify(struct sockaddr * sap, diminuto_ipv6_t * addressp, diminuto_port_t * portp) {
    int result = -1;

    if (sap->sa_family == AF_INET) {
        struct sockaddr_in * sa4p;
        result = 0;
        sa4p = (struct sockaddr_in *)sap;
        if (addressp != (diminuto_ipv6_t *)0) {
            ipv42ipv6(sa4p->sin_addr.s_addr, addressp);
            ntoh6(addressp);
        }
        if (portp != (diminuto_port_t *)0) {
            *portp = ntohs(sa4p->sin_port);
        }
    } else if (sap->sa_family == AF_INET6) {
        struct sockaddr_in6 * sa6p;
        result = 0;
        sa6p = (struct sockaddr_in6 *)sap;
        if (addressp != (diminuto_ipv6_t *)0) {
            memcpy(addressp->u16, sa6p->sin6_addr.s6_addr, sizeof(addressp->u16));
            ntoh6(addressp);
        }
        if (portp != (diminuto_port_t *)0) {
            *portp = ntohs(sa6p->sin6_port);
        }
    } else {
        errno = EINVAL;
        diminuto_perror("diminuto_ipc6: sin_family");
    }

    return result;
}

diminuto_ipv6_t * diminuto_ipc6_addresses(const char * hostname)
{
	diminuto_ipv6_t * addresses = (diminuto_ipv6_t *)0;
    struct  hostent * hostp;
    struct in_addr in4;
    struct in6_addr in6;
    size_t index;
    size_t limit;

    if (inet_pton(AF_INET6, hostname, &in6) == 1) {
        addresses = (diminuto_ipv6_t *)malloc(sizeof(diminuto_ipv6_t) * 2);
        memcpy(addresses[0].u16, in6.s6_addr, sizeof(addresses[0].u16));
        ntoh6(&(addresses[0]));
        memset(addresses[1].u16, 0, sizeof(addresses[1].u16));
    } else if (inet_pton(AF_INET, hostname, &in4) == 1) {
        addresses = (diminuto_ipv6_t *)malloc(sizeof(diminuto_ipv6_t) * 2);
        ipv42ipv6(in4.s_addr, &(addresses[0]));
        ntoh6(&(addresses[0]));
        memset(addresses[1].u16, 0, sizeof(addresses[1].u16));
    } else if ((hostp = gethostbyname(hostname)) == (struct hostent *)0) {
        /* Do nothing: no host entry. */
    } else if (hostp->h_addrtype == AF_INET6) {
        for (limit = 0; hostp->h_addr_list[limit] != 0; ++limit) {
            continue;
        }
        if (limit > 0) {
            addresses = (diminuto_ipv6_t *)malloc(sizeof(diminuto_ipv6_t) * (limit + 1));
            for (index = 0; index < limit; ++index) {
                memset(addresses[index].u16, 0, sizeof(addresses[index].u16));
                memcpy(addresses[index].u16, hostp->h_addr_list[index], (hostp->h_length < (int)sizeof(addresses[index].u16)) ? hostp->h_length : sizeof(addresses[index].u16));
                ntoh6(&(addresses[index]));
            }
            memset(addresses[index].u16, 0, sizeof(addresses[index].u16));
        }
    } else if (hostp->h_addrtype == AF_INET) {
        for (limit = 0; hostp->h_addr_list[limit] != 0; ++limit) {
            continue;
        }
        if (limit > 0) {
            addresses = (diminuto_ipv6_t *)malloc(sizeof(diminuto_ipv6_t) * (limit + 1));
            for (index = 0; index < limit; ++index) {
                memset(&in4, 0, sizeof(in4));
                memcpy(&in4, hostp->h_addr_list[index], (hostp->h_length < (int)sizeof(in4)) ? hostp->h_length : sizeof(in4));
                ipv42ipv6(in4.s_addr, &(addresses[index]));
                ntoh6(&(addresses[index]));
            }
            memset(addresses[index].u16, 0, sizeof(addresses[index].u16));
        }
    } else {
        /* Do nothing. */
    }

    return addresses;
}

diminuto_ipv6_t diminuto_ipc6_address(const char * hostname)
{
    diminuto_ipv6_t address6 = { 0 };
    struct addrinfo hints = { 0 };
    struct in6_addr * in6p;
    struct in_addr * in4p;
    struct addrinfo * infop = (struct addrinfo *)0;

    /*
     * The ipc6 feature prefers IPv6 addresses if they are available. So we
     * specify those first, then we settle for IPv4, than as a last ditch
     * we try any, which aren't likely to be useful.
     */

    hints.ai_family = AF_INET6;
    if ((getaddrinfo(hostname, (const char *)0, &hints, &infop)) == 0) {
        /* Do nothing. */
    } else {
        hints.ai_family = AF_INET;
        if ((getaddrinfo(hostname, (const char *)0, &hints, &infop)) == 0) {
            /* Do nothing. */
        } else {
            hints.ai_family = AF_UNSPEC;
            if ((getaddrinfo(hostname, (const char *)0, &hints, &infop)) == 0) {
                /* Do nothing. */
            } else {
                infop = (struct addrinfo *)0;
            }
        }
    }

    if (infop != (struct addrinfo *)0) {
        identify(infop->ai_addr, &address6, (diminuto_port_t *)0);
        freeaddrinfo(infop);
    }

    return address6;
}

const char * diminuto_ipc6_colonnotation(diminuto_ipv6_t address, char * buffer, size_t length)
{
    if (length > 0) {
        struct in6_addr in6;
        char temporary[INET6_ADDRSTRLEN];

        hton6(&address);
        memcpy(in6.s6_addr, address.u16, sizeof(in6.s6_addr));
        inet_ntop(AF_INET6, &in6, temporary, sizeof(temporary));

        strncpy(buffer, temporary, length);
        buffer[length - 1] = '\0';
    }

    return buffer;
}

int diminuto_ipc6_stream_accept(int fd, diminuto_ipv6_t * addressp, diminuto_port_t * portp)
{
    struct sockaddr_in6 sa;
    socklen_t length;
    int newfd;

    length = sizeof(sa);
    if ((newfd = accept(fd, (struct sockaddr *)&sa, &length)) < 0) {
        diminuto_perror("diminuto_ipc6_accept: accept");
        newfd = -1;
    } else if (length != sizeof(sa)) {
        errno = EINVAL;
        diminuto_perror("diminuto_ipc6_accept: length");
        newfd = -2;
    } else {
        identify((struct sockaddr *)&sa, addressp, portp);
    }
   
    return newfd;
}

int diminuto_ipc6_stream_consumer(diminuto_ipv6_t address, diminuto_port_t port)
{
    struct sockaddr_in6 sa;
    socklen_t length;
    int fd;
    int rc;

    length = sizeof(sa);
    memset(&sa, 0, length);
    sa.sin6_family = AF_INET6;
    hton6(&address);
    memcpy(sa.sin6_addr.s6_addr, address.u16, sizeof(sa.sin6_addr.s6_addr));
    sa.sin6_port = htons(port);

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        diminuto_perror("diminuto_ipc6_consumer: socket");
    } else if ((rc = connect(fd, (struct sockaddr *)&sa, length)) < 0) {
        diminuto_perror("diminuto_ipc6_consumer: connect");
        diminuto_ipc6_close(fd);
        fd = -2;
    }

    return fd;
}

ssize_t diminuto_ipc6_datagram_receive_flags(int fd, void * buffer, size_t size, diminuto_ipv6_t * addressp, diminuto_port_t * portp, int flags)
{
    ssize_t total;
    const char * bp;
    struct sockaddr_in sa;
    socklen_t length;

    bp = (char *) buffer;
    length = sizeof(sa);
    memset(&sa, 0, length);

    if ((total = recvfrom(fd, buffer, size, flags, (struct sockaddr *)&sa, &length)) == 0) {
        /* Do nothing: not sure what this means. */
    } else if (total > 0) {
        identify((struct sockaddr *)&sa, addressp, portp);
    } else if ((errno != EINTR) && (errno != EAGAIN)) { 
        diminuto_perror("diminuto_ipc6_datagram_receive_flags: recvfrom");
    } else {
        /* Do nothing: timeout or poll. */
    }

    return total;
}

ssize_t diminuto_ipc6_datagram_send_flags(int fd, const void * buffer, size_t size, diminuto_ipv6_t address, diminuto_port_t port, int flags)
{
    ssize_t total;
    const char * bp;
    struct sockaddr_in6 sa;
    socklen_t length;

    bp = (char *) buffer;

    length = sizeof(sa);
    memset(&sa, 0, length);
    sa.sin6_family = AF_INET6;
    hton6(&address);
    memcpy(sa.sin6_addr.s6_addr, address.u16, sizeof(sa.sin6_addr.s6_addr));
    sa.sin6_port = htons(port);

    if ((total = sendto(fd, buffer, size, flags, (struct sockaddr *)&sa, sizeof(sa))) == 0) {
        /* Do nothing: not sure what this means. */
    } else if (total > 0) {
        /* Do nothing: nominal case. */
    } else if ((errno != EINTR) && (errno != EAGAIN) && (errno != EWOULDBLOCK)) {
        diminuto_perror("diminuto_ipc6_datagram_send_flags: sendto");
    } else {
        /* Do nothing: timeout or poll. */
    }

    return total;
}

ssize_t diminuto_ipc6_datagram_receive(int fd, void * buffer, size_t size, diminuto_ipv6_t * addressp, diminuto_port_t * portp)
{
    return diminuto_ipc6_datagram_receive_flags(fd, buffer, size, addressp, portp, 0);
}

ssize_t diminuto_ipc6_datagram_send(int fd, const void * buffer, size_t size, diminuto_ipv6_t address, diminuto_port_t port)
{
    return diminuto_ipc6_datagram_send_flags(fd, buffer, size, address, port, 0);
}

int diminuto_ipc6_nearend(int fd, diminuto_ipv6_t * addressp, diminuto_port_t * portp)
{
    int rc;
    struct sockaddr_in6 sa;
    socklen_t length;

    length = sizeof(sa);
    if ((rc = getsockname(fd, (struct sockaddr *)&sa, &length)) < 0) {
        diminuto_perror("diminuto_ipc6_nearend: getsockname");
    } else {
        identify((struct sockaddr *)&sa, addressp, portp);
    }

    return rc;
}

int diminuto_ipc6_farend(int fd, diminuto_ipv6_t * addressp, diminuto_port_t * portp)
{
    int rc;
    struct sockaddr_in6 sa;
    socklen_t length;

    length = sizeof(sa);
    if ((rc = getpeername(fd, (struct sockaddr *)&sa, &length)) < 0) {
        diminuto_perror("diminuto_ipc6_farend: getpeername");
    } else {
        identify((struct sockaddr *)&sa, addressp, portp);
    }

    return rc;
}