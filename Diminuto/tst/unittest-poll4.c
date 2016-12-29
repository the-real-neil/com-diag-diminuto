/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_poll.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_fletcher.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>

#if defined(__arm__)
static const size_t TOTAL = 1024 * 1024 * 100;
#else
static const size_t TOTAL = 1024 * 1024 * 1024;
#endif

int main(int argc, char ** argv)
{
    extern int diminuto_alarm_debug;

    SETLOGMASK();
    diminuto_alarm_debug = !0;

    {
        int listener;
        pid_t pid;
        diminuto_port_t rendezvous = 0;
        static const uint8_t ACK = '\006';

        TEST();

        ASSERT((listener = diminuto_ipc4_stream_provider(0)) >= 0);
        ASSERT(diminuto_ipc4_nearend(listener, (diminuto_ipv4_t *)0, &rendezvous) == 0);
        ASSERT(rendezvous > 0);

        ASSERT((pid = fork()) >= 0);

        if (pid) {

            /* PRODUCER */

            diminuto_ipv4_t address;
            diminuto_port_t port;
            diminuto_poll_t poll;
            int producer;
            uint8_t output[1 << (sizeof(uint8_t) * 8)];
            uint8_t input[1 << (sizeof(uint8_t) * 8)];
            uint8_t datum;
            ssize_t sent;
            ssize_t received;
            ssize_t used;
            ssize_t available;
            uint8_t * here;
            uint8_t * there;
            uint8_t * current;
            size_t totalsent;
            size_t totalreceived;
            int status;
            int ready;
            int fd;
            int timeouts;
            int alarms;
            sigset_t mask;
            ssize_t readable;
            double percentage;
            uint8_t output_a;
            uint8_t output_b;
            uint16_t output_16;
            uint8_t input_a;
            uint8_t input_b;
            uint16_t input_16;

            ASSERT(sigemptyset(&mask) == 0);
            ASSERT(sigaddset(&mask, SIGALRM) == 0);
            ASSERT(sigprocmask(SIG_BLOCK, &mask, (sigset_t *)0) == 0);

            diminuto_poll_init(&poll);

            ASSERT(diminuto_poll_register_accept(&poll, listener) >= 0);
            ASSERT(diminuto_poll_unregister_signal(&poll, SIGALRM) >= 0);

            diminuto_poll_dump(&poll);

            while (!0) {
                if ((ready = diminuto_poll_wait(&poll, diminuto_frequency())) > 0) {
                    break;
                } else if (ready == 0) {
                    DIMINUTO_LOG_INFORMATION("listener timed out\n");
                    ++timeouts;
                } else if (errno != EINTR) {
                    FATAL("diminuto_poll_wait: error");
                } else if (diminuto_alarm_check()) {
                    DIMINUTO_LOG_INFORMATION("listener alarmed\n");
                    ++alarms;
                } else {
                    FATAL("diminuto_poll_wait: interrupted");
                }
            }

            ASSERT((fd = diminuto_poll_ready_accept(&poll)) >= 0);
            ASSERT(fd == listener);

            ASSERT((producer = diminuto_ipc4_stream_accept_generic(fd, &address, &port)) >= 0);

            ASSERT(diminuto_poll_register_read(&poll, producer) >= 0);
            ASSERT(diminuto_poll_register_write(&poll, producer) >= 0);

            diminuto_poll_dump(&poll);

            ASSERT(diminuto_alarm_install(0) == 0);
            ASSERT(diminuto_timer_oneshot(diminuto_frequency()) == 0);

            here = output;
            used = sizeof(output);
            sent = 0;

            there = input;
            available = sizeof(input);
            received = 0;

            timeouts = 0;
            alarms = 0;

            totalsent = 0;
            totalreceived = 0;

            datum = 0;
            do {
                output[datum] = datum;
            } while ((++datum) > 0);

            memset(input, 0, sizeof(input));

            input_a = input_b = output_a = output_b = 0;

            ASSERT((sent = diminuto_ipc4_datagram_send_generic(producer, &ACK, sizeof(ACK), address, 0, MSG_OOB)) == sizeof(ACK));
            DIMINUTO_LOG_DEBUG("producer ACKing   %10s %10d %10u %7.3lf%%\n", "", sent, 0, 0.0);

            do {

                while (!0) {
                    if ((ready = diminuto_poll_wait(&poll, diminuto_frequency() / 10)) > 0) {
                        break;
                    } else if (ready == 0) {
                        DIMINUTO_LOG_INFORMATION("producer timed out\n");
                        ++timeouts;
                    } else if (errno != EINTR) {
                        FATAL("diminuto_poll_wait: error");
                    } else if (diminuto_alarm_check()) {
                        DIMINUTO_LOG_INFORMATION("producer alarmed\n");
                        ++alarms;
                    } else {
                        FATAL("diminuto_poll_wait: interrupted");
                    }
                }

                while ((fd = diminuto_poll_ready_write(&poll)) >= 0) {

                    ASSERT(fd == producer);

                    if (totalsent < TOTAL) {

                        if (used > (TOTAL - totalsent)) {
                            used = TOTAL - totalsent;
                        }

                        ASSERT((sent = diminuto_ipc4_stream_write_generic(fd, here, 1, used)) > 0);
                        ASSERT(sent <= used);
                        output_16 = diminuto_fletcher_16(here, sent, &output_a, &output_b);

                        totalsent += sent;
                        percentage = totalsent;
                        percentage *= 100;
                        percentage /= TOTAL;
                        DIMINUTO_LOG_INFORMATION("producer sent     %10s %10d %10u %7.3lf%%\n", "", sent, totalsent, percentage);

                        here += sent;
                        used -= sent;

                        if (used == 0) {
                            here = output;
                            used = sizeof(output);
                        }

                    }

                }

                current = there;

                if (available > (TOTAL - totalreceived)) {
                    available = TOTAL - totalreceived;
                }

                while ((fd = diminuto_poll_ready_read(&poll)) >= 0) {

                    ASSERT(fd == producer);

                    ASSERT((readable = diminuto_fd_readable(fd)) > 0);
                    ASSERT((received = diminuto_ipc4_stream_read(fd, there, available)) > 0);
                    ASSERT(received <= available);
                    input_16 = diminuto_fletcher_16(there, received, &input_a, &input_b);

                    totalreceived += received;
                    DIMINUTO_LOG_INFORMATION("producer received %10d %10d %10u\n", readable, received, totalreceived);

                    there += received;
                    available -= received;

                    while (current < there) {
                        ASSERT(*(current++) == (datum++));
                    }

                    if (available == 0) {
                        there = input;
                        available = sizeof(input);
                        memset(input, 0, sizeof(input));
                    }

                }

            } while (totalreceived < TOTAL);

            ASSERT(diminuto_poll_close(&poll, producer) == 0);
            ASSERT(diminuto_poll_close(&poll, listener) == 0);
            diminuto_poll_fini(&poll);

            ASSERT(input_16 == output_16);

            ADVISE(timeouts > 0);
            ADVISE(alarms > 0);

            EXPECT(waitpid(pid, &status, 0) == pid);
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);

            STATUS();

        } else {

            /* CONSUMER */

            diminuto_poll_t poll;
            int consumer;
            uint8_t buffer[64];
            ssize_t sent;
            ssize_t received;
            size_t totalsent;
            size_t totalreceived;
            int ready;
            int fd;
            int done;
            int proceed;
            ssize_t readable;
            double percentage;

            ASSERT(diminuto_ipc4_close(listener) >= 0);

            diminuto_poll_init(&poll);

            ASSERT((consumer = diminuto_ipc4_stream_consumer(diminuto_ipc4_address("localhost"), rendezvous)) >= 0);
            ASSERT(diminuto_poll_register_read(&poll, consumer) >= 0);
            ASSERT(diminuto_poll_register_urgent(&poll, consumer) >= 0);

            totalreceived = 0;
            totalsent = 0;
            done = 0;

            do {

                while (!0) {
                    if ((ready = diminuto_poll_wait(&poll, -1)) > 0) {
                        break;
                    } else if (ready == 0) {
                        diminuto_yield();
                    } else if (errno == EINTR) {
                        DIMINUTO_LOG_DEBUG("consumer interrupted\n");
                    } else {
                        FATAL("diminuto_poll_wait");
                    }
                }

                if (!proceed) {
                    while ((fd = diminuto_poll_ready_urgent(&poll)) >= 0) {
                        ASSERT(fd == consumer);
                        buffer[0] = '\0';
                        ASSERT((received = diminuto_ipc4_datagram_receive_generic(fd, buffer, 1, (diminuto_ipv4_t *)0, (diminuto_port_t *)0, MSG_OOB | MSG_DONTWAIT)) == 1);
                        DIMINUTO_LOG_DEBUG("consumer ACKed    %10d %10d %10u %7.3lf%%\n", 0, received, 0, 0.0);
                        ASSERT(buffer[0] == ACK);
                        if (buffer[0] == ACK) {
                            proceed = !0;
                            break;
                        }
                    }
                    if (!proceed) {
                        continue;
                    }
                }

                while ((fd = diminuto_poll_ready_read(&poll)) >= 0) {

                    ASSERT(fd == consumer);

                    ASSERT((readable = diminuto_fd_readable(fd)) >= 0);
                    ASSERT((received = diminuto_ipc4_stream_read(fd, buffer, sizeof(buffer))) >= 0);
                    ASSERT(received <= sizeof(buffer));

                    totalreceived += received;
                    percentage = totalreceived;
                    percentage *= 100;
                    percentage /= TOTAL;
                    DIMINUTO_LOG_INFORMATION("consumer received %10d %10d %10u %7.3lf%%\n", readable, received, totalreceived, percentage);

                    if (received == 0) {
                        done = !0;
                        break;
                    }

                    sent = 0;
                    while (sent < received) {
                        ASSERT((sent = diminuto_ipc4_stream_write_generic(fd,  buffer + sent, 1, received - sent)) > 0);
                        ASSERT(sent <= received);

                        totalsent += sent;
                        DIMINUTO_LOG_INFORMATION("consumer sent     %10s %10d %10u\n", "", sent, totalsent);

                        received -= sent;
                    }

                }

            } while (!done);

            ASSERT(diminuto_poll_close(&poll, consumer) == 0);
            diminuto_poll_fini(&poll);

            exit(0);
        }


    }

    EXIT();
}
