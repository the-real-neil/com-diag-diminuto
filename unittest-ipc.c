/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_ipc.h"
#include "diminuto_time.h"
#include "diminuto_timer.h"
#include "diminuto_delay.h"
#include "diminuto_unittest.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

static const diminuto_port_t PORT1 = 65535;
static const diminuto_port_t PORT2 = 65534;

int main(void)
{
    {
        diminuto_ipv4_t address;
        char buffer[64];

        address = diminuto_ipc_address("127.0.0.1");
        EXPECT(address == 0x7f000001UL);

        address = diminuto_ipc_address("localhost");
        EXPECT(address == 0x7f000001UL);

        address = diminuto_ipc_address("www.diag.com");
        EXPECT(address != 0UL);

        printf("\"www.diag.com\"=0x%08x=%s\n", address, diminuto_ipc_dotnotation(address, buffer, sizeof(buffer)));

        address = diminuto_ipc_address("invalid.domain");
        EXPECT(address == 0UL);
    }

    {
        diminuto_port_t port;

        port = diminuto_ipc_port("80", NULL);
        EXPECT(port == 80);

        port = diminuto_ipc_port("80", "tcp");
        EXPECT(port == 80);

        port = diminuto_ipc_port("80", "udp");
        EXPECT(port == 80);

        port = diminuto_ipc_port("http", NULL);
        EXPECT(port == 80);    

        port = diminuto_ipc_port("http", "tcp");
        EXPECT(port == 80);

        port = diminuto_ipc_port("tftp", "udp");
        EXPECT(port == 69);

        port = diminuto_ipc_port("login", "tcp");
        EXPECT(port == 513);

        port = diminuto_ipc_port("login", "tcp");
        EXPECT(port == 513);

        port = diminuto_ipc_port("login", "udp");
        EXPECT(port == 0);

        port = diminuto_ipc_port("who", NULL);
        EXPECT(port == 513);

        port = diminuto_ipc_port("who", "tcp");
        EXPECT(port == 0);

        port = diminuto_ipc_port("who", "udp");
        EXPECT(port == 513);

        port = diminuto_ipc_port("unknown", NULL);
        EXPECT(port == 0);

        port = diminuto_ipc_port("unknown", "tcp");
        EXPECT(port == 0);

        port = diminuto_ipc_port("unknown", "udp");
        EXPECT(port == 0);

    }

    {
        int fd;

        EXPECT((fd = diminuto_ipc_datagram_peer(PORT1)) >= 0);
        EXPECT(diminuto_ipc_close(fd) >= 0);
    }

    {
        int fd;

        EXPECT((fd = diminuto_ipc_stream_consumer(diminuto_ipc_address("www.diag.com"), diminuto_ipc_port("http", NULL))) >= 0);
        EXPECT(diminuto_ipc_close(fd) >= 0);

        EXPECT((fd = diminuto_ipc_stream_consumer(diminuto_ipc_address("www.amazon.com"), diminuto_ipc_port("http", NULL))) >= 0);
        EXPECT(diminuto_ipc_close(fd) >= 0);
    }

    {
        int fd;
    
        EXPECT((fd = diminuto_ipc_stream_provider(0xfff0)) >= 0);
        EXPECT(diminuto_ipc_close(fd) >= 0);
    }

    {
        int fd;

        EXPECT((fd = diminuto_ipc_stream_provider(0xfff0)) >= 0);

        EXPECT(diminuto_ipc_set_nonblocking(fd, !0) >= 0);
        EXPECT(diminuto_ipc_set_nonblocking(fd, 0) >= 0);

        EXPECT(diminuto_ipc_set_reuseaddress(fd, !0) >= 0);
        EXPECT(diminuto_ipc_set_reuseaddress(fd, 0) >= 0);

        EXPECT(diminuto_ipc_set_keepalive(fd, !0) >= 0);
        EXPECT(diminuto_ipc_set_keepalive(fd, 0) >= 0);

        if (geteuid() == 0) {
            EXPECT(diminuto_ipc_set_debug(fd, !0) >= 0);
            EXPECT(diminuto_ipc_set_debug(fd, 0) >= 0);
        }

        EXPECT(diminuto_ipc_set_linger(fd, 1000000UL) >= 0);
        EXPECT(diminuto_ipc_set_linger(fd, 0) >= 0);

        EXPECT(diminuto_ipc_close(fd) >= 0);
    }

    {
        int fd1;
        int fd2;
        const char * MSG1 = "Chip Overclock";
        const char * MSG2 = "Digital Aggregates Corporation";
        char buffer[64];
        diminuto_ipv4_t address = 0;
        diminuto_port_t port = 0;

        EXPECT((fd1 = diminuto_ipc_datagram_peer(PORT1)) >= 0);
        EXPECT((fd2 = diminuto_ipc_datagram_peer(PORT2)) >= 0);

        /* This only works because the kernel buffers socket data. */

        EXPECT((diminuto_ipc_datagram_send(fd1, MSG1, sizeof(MSG1), diminuto_ipc_address("localhost"), PORT2)) == sizeof(MSG1));
        EXPECT((diminuto_ipc_datagram_receive(fd2, buffer, sizeof(buffer), &address, &port)) == sizeof(MSG1));
        EXPECT(address == diminuto_ipc_address("localhost"));
        EXPECT(port == PORT1);

        EXPECT((diminuto_ipc_datagram_send(fd2, MSG2, sizeof(MSG2), diminuto_ipc_address("localhost"), PORT1)) == sizeof(MSG2));
        EXPECT((diminuto_ipc_datagram_receive(fd1, buffer, sizeof(buffer), &address, &port)) == sizeof(MSG2));
        EXPECT(address == diminuto_ipc_address("localhost"));
        EXPECT(port == PORT2);

        EXPECT(diminuto_ipc_close(fd1) >= 0);
        EXPECT(diminuto_ipc_close(fd2) >= 0);
    }

    {
        int fd;
        char buffer[1];
        diminuto_ipv4_t address = 0x12345678;
        diminuto_port_t port = 0x9abc;

        EXPECT((fd = diminuto_ipc_datagram_peer(PORT1)) >= 0);
        EXPECT(diminuto_ipc_set_nonblocking(fd, !0) >= 0);
        EXPECT((diminuto_ipc_datagram_receive(fd, buffer, sizeof(buffer), &address, &port)) < 0);
        EXPECT(errno == EAGAIN);
        EXPECT(address == 0x12345678);
        EXPECT(port == 0x9abc);
        EXPECT(diminuto_ipc_close(fd) >= 0);
    }

    {
        int fd;
        char buffer[1];
        diminuto_ipv4_t address = 0x12345678;
        diminuto_port_t port = 0x9abc;

        EXPECT((fd = diminuto_ipc_datagram_peer(PORT1)) >= 0);
        EXPECT((diminuto_ipc_datagram_receive_flags(fd, buffer, sizeof(buffer), &address, &port, MSG_DONTWAIT)) < 0);
        EXPECT(errno == EAGAIN);
        EXPECT(address == 0x12345678);
        EXPECT(port == 0x9abc);
        EXPECT(diminuto_ipc_close(fd) >= 0);
    }

    {
        int fd;
        char buffer[1];
        diminuto_ipv4_t address = 0x12345678;
        diminuto_port_t port = 0x9abc;
        diminuto_usec_t before;
        diminuto_usec_t after;

        EXPECT((fd = diminuto_ipc_datagram_peer(PORT1)) >= 0);
        EXPECT(diminuto_alarm_install(0) >= 0);
        diminuto_timer_oneshot(2000000ULL);
        before = diminuto_time();
#if 1
        EXPECT((diminuto_ipc_datagram_receive(fd, buffer, sizeof(buffer), &address, &port)) < 0);
#else
        diminuto_delay(5000000ULL, !0);
#endif
        after = diminuto_time();
        EXPECT(diminuto_alarm_check());
        EXPECT((after - before) >= 2000000ULL);
        EXPECT(errno == EINTR);
        EXPECT(address == 0x12345678);
        EXPECT(port == 0x9abc);
        EXPECT(diminuto_ipc_close(fd) >= 0);
    }

#if 0
    {
        uint64_t address;
        diminuto_port_t port;
        int rendezvous;
        int consumer;
        int producer;
        int index;
        char sent;
        char received;

        EXPECT((rendezvous = diminuto_ipc_provider(0xfff0)) >= 0);
        EXPECT((consumer = diminuto_ipc_consumer(diminuto_ipc_address("localhost"), 0xfff0)) >= 0);

        address = 0;
        EXPECT(*producer = diminuto_ipc_accept(rendezvous, address)) >= 0);
        printf("consumer=0x%08x=%s\n",
            address, diminuto_ipc_dotnotation(address, buffer, sizeof(buffer)));
        EXPECT(ADDRESS == 0x7f000001UL);

        /* This only works because the kernel buffers socket data. */

        for (int index = 0; index < 256; ++index) {

            char sent = index;
            char received = ~sent;

        rc = ::write(producer, &sent, 1);
        if (rc != 1) {
            errorf("%s[%d]: (%u!=%u) (%d)!\n",
                __FILE__, __LINE__, rc, 1, errno);
            ++errors;
        }

        rc = ::read(consumer, &received, 1);
        if (rc != 1) {
            errorf("%s[%d]: (%u!=%u) (%d)!\n",
                __FILE__, __LINE__, rc, 1, errno);
            ++errors;
        }
        if (received != sent) {
            errorf("%s[%d]: (0x%x!=0x%x)!\n",
                __FILE__, __LINE__, received, sent);
            ++errors;
        }

        sent = index;
        received = ~sent;

        rc = ::write(consumer,  &sent, 1);
        if (rc != 1) {
            errorf("%s[%d]: (%u!=%u) (%d)!\n",
                __FILE__, __LINE__, rc, 1, errno);
            ++errors;
        }

        rc = ::read(producer, &received, 1);
        if (rc != 1) {
            errorf("%s[%d]: (%u!=%u) (%d)!\n",
                __FILE__, __LINE__, rc, 1, errno);
            ++errors;
        }
        if (received != sent) {
            errorf("%s[%d]: (0x%x!=0x%x)!\n",
                __FILE__, __LINE__, received, sent);
            ++errors;
        }
    }

    rc = diminuto_ipc_close(producer);
    if (rc != producer) {
        errorf("%s[%d]: (%u!=%u) (%d)!\n",
            __FILE__, __LINE__, rc, fd, errno);
        ++errors;
    }
    rc = diminuto_ipc_close(consumer);
    if (rc != consumer) {
        errorf("%s[%d]: (%u!=%u) (%d)!\n",
            __FILE__, __LINE__, rc, fd, errno);
        ++errors;
    }
    rc = diminuto_ipc_close(rendezvous);
    if (rc != rendezvous) {
        errorf("%s[%d]: (%u!=%u) (%d)!\n",
            __FILE__, __LINE__, rc, fd, errno);
        ++errors;
    }

    printf("%s[%d]: StreamSockets\n", __FILE__, __LINE__);

    rendezvous = diminuto_ipc_provider(0xfff0);
    if (rendezvous < 0) {
        errorf("%s[%d]: (%d<%d) (%d)!\n",
            __FILE__, __LINE__, rendezvous, 0, errno);
        ++errors;
    }

    consumer = diminuto_ipc_consumer(diminuto_ipc_address("localhost"), 0xfff0);
    if (consumer < 0) {
        errorf("%s[%d]: (%d<%d) (%d)!\n",
            __FILE__, __LINE__, consumer, 0, errno);
        ++errors;
    }

    producer = diminuto_ipc_accept(rendezvous);
    if (producer < 0) {
        errorf("%s[%d]: (%u<%u) (%d)!\n",
            __FILE__, __LINE__, producer, 0, errno);
        ++errors;
    }

    StreamSocket nearend(consumer);
    nearend.show();
    Input& nearendin = nearend.input();
    nearendin.show();
    Output& nearendout = nearend.output();
    nearendout.show();

    StreamSocket farend(producer);
    farend.show();
    Input& farendin = farend.input();
    farendin.show();
    Output& farendout = farend.output();
    farendout.show();

    // This only works because the kernel buffers socket data.

    for (int index = 0; index < 256; ++index) {

        char sent = index;
        char received = ~sent;

        rc = farendout(sent);
        if (rc == EOF) {
            errorf("%s[%d]: (%u==%u) (%d)!\n",
                __FILE__, __LINE__, rc, EOF, errno);
            ++errors;
        }

        received = nearendin();
        if (received != sent) {
            errorf("%s[%d]: (0x%x!=0x%x)!\n",
                __FILE__, __LINE__, received, sent);
            ++errors;
        }

        sent = index;
        received = ~sent;

        rc = nearendout(sent);
        if (rc == EOF) {
            errorf("%s[%d]: (%u==%u) (%d)!\n",
                __FILE__, __LINE__, rc, EOF, errno);
            ++errors;
        }

        received = farendin();
        if (received != sent) {
            errorf("%s[%d]: (0x%x!=0x%x)!\n",
                __FILE__, __LINE__, received, sent);
            ++errors;
        }
    }

    rc = diminuto_ipc_close(producer);
    if (rc != producer) {
        errorf("%s[%d]: (%u!=%u) (%d)!\n",
            __FILE__, __LINE__, rc, fd, errno);
        ++errors;
    }
    rc = diminuto_ipc_close(consumer);
    if (rc != consumer) {
        errorf("%s[%d]: (%u!=%u) (%d)!\n",
            __FILE__, __LINE__, rc, fd, errno);
        ++errors;
    }
    rc = diminuto_ipc_close(rendezvous);
    if (rc != rendezvous) {
        errorf("%s[%d]: (%u!=%u) (%d)!\n",
            __FILE__, __LINE__, rc, fd, errno);
        ++errors;
    }

    printf("%s[%d]: Files\n", __FILE__, __LINE__);

    rendezvous = diminuto_ipc_provider(0xfff0);
    if (rendezvous < 0 ) {
        errorf("%s[%d]: (%d<%d) (%d)!\n",
            __FILE__, __LINE__, rendezvous, 0, errno);
        ++errors;
    }

    consumer = diminuto_ipc_consumer(diminuto_ipc_address("localhost"), 0xfff0);
    if (consumer < 0) {
        errorf("%s[%d]: (%d<%d) (%d)!\n",
            __FILE__, __LINE__, consumer, 0, errno);
        ++errors;
    }

    FILE* nearendfile = ::fdopen(consumer, "r+");
    if (nearendfile == 0) {
        errorf("%s[%d]: (0x%x==0x%x) (%d)!\n",
            __FILE__, __LINE__, nearendfile, 0, errno);
        ++errors;
    }

    producer = diminuto_ipc_accept(rendezvous);
    if (producer < 0) {
        errorf("%s[%d]: (%d<%d) (%d)!\n",
            __FILE__, __LINE__, producer, 0, errno);
        ++errors;
    }

    FILE* farendfile = ::fdopen(producer, "r+");
    if (farendfile == 0) {
        errorf("%s[%d]: (0x%x==0x%x) (%d)!\n",
            __FILE__, __LINE__, farendfile, 0, errno);
        ++errors;
    }

    FileInput nearendfileinput(nearendfile);
    nearendfileinput.show();
    FileOutput nearendfileoutput(nearendfile);
    nearendfileoutput.show();

    FileInput farendfileinput(farendfile);
    farendfileinput.show();
    FileOutput farendfileoutput(farendfile);
    farendfileoutput.show();

    // This only works because the kernel buffers socket data.

    for (int index = 0; index < 256; ++index) {

        char sent = index;
        char received = ~sent;

        rc = farendfileoutput(sent);
        if (rc == EOF) {
            errorf("%s[%d]: (%u==%u) (%d)!\n",
                __FILE__, __LINE__, rc, EOF, errno);
            ++errors;
        }
        farendfileoutput();

        received = nearendfileinput();
        if (received != sent) {
            errorf("%s[%d]: (0x%x!=0x%x)!\n",
                __FILE__, __LINE__, received, sent);
            ++errors;
        }

        sent = index;
        received = ~sent;

        rc = nearendfileoutput(sent);
        if (rc == EOF) {
            errorf("%s[%d]: (%u==%u) (%d)!\n",
                __FILE__, __LINE__, rc, EOF, errno);
            ++errors;
        }
        nearendfileoutput();

        received = farendfileinput();
        if (received != sent) {
            errorf("%s[%d]: (0x%x!=0x%x)!\n",
                __FILE__, __LINE__, received, sent);
            ++errors;
        }
    }

    std::fclose(nearendfile);
    std::fclose(farendfile);

    rc = diminuto_ipc_close(rendezvous);
    if (rc != rendezvous) {
        errorf("%s[%d]: (%u!=%u) (%d)!\n",
            __FILE__, __LINE__, rc, fd, errno);
        ++errors;
    }

    printf("%s[%d]: end errors=%d\n",
        __FILE__, __LINE__, errors);
#endif

    EXIT();
}
