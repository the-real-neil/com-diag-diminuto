/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the sendmsg(2) ancillary data capability.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the use of sendmsg(2) to send ancillary data
 * using Diminutos IPC4 and IPCL features. The test is complex enough
 * and uses multiple IPC features that it merited its own unit test.
 * It is intended to simulate an actual application albeit on a small
 * scale. It may "simulate" but all of the processes, threads, and
 * sockets are the real thing.
 *
 * I've formatted the log messages and their log levels so that it
 * is straightforward to capture the log in the file and extract
 * useful stuff from it. One of the things this has allowed me to do
 * is to verify that the complete range of ephemeral port numbers,
 * [32768..60999] on my development machine, are being used, and
 * reused. Even though ephemeral ports are being recycled, I have
 * seen this code occasionally fail because of port exaustion with the
 * errno EADDRINUSE from bind(2):
 *
 * diminuto_ipc4_source: bind: "Address already in use" (98)
 *
 * My only working hypothesis at the moment is the recycling of ephemeral
 * port numbers in the kernel is asynchronous with some latency
 * (presumably on the close) and sometimes we can get ahead of it.
 * Note that by default the sockets all have the REUSE ADDRESS option
 * set by the underlying Diminuto library code.
 *
 * Set the environmental variable COM_DIAG_DIMINUTO_LOG_MASK to the
 * value "0xfe" to dial down the log output; or set it to the value
 * "0xff" to see more log output.
 *
 * REFERENCES
 *
 * C. Sridharan, "File Descriptor Transfer over Unix Domain Sockets",
 * CopyConstruct, August 2020
 *
 * U. Naseer et al., "Zero Downtime Release: Disruption-free Load
 * Balancing of a Multi-Billion User Website", ACM SIGCOMM '20,
 * August 2020
 *
 * unix(7) man page
 *
 * socket(7) man page
 *
 * sendmsg(2) man page
 *
 * recvmsg(2) man page
 *
 * cmsg(3) man page
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_containerof.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_interrupter.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_ipcl.h"
#include "com/diag/diminuto/diminuto_list.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_reaper.h"
#include "com/diag/diminuto/diminuto_thread.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

static const char INSTANCEPATH[] = "/tmp/unittest-ipc-ancillary.sock";

static const unsigned int DURATION = 10;

static const unsigned int FRACTION = 10;

/*******************************************************************************
 * GLOBALS
 ******************************************************************************/

static diminuto_ipv4_t serveraddress = 0;
static diminuto_port_t serverport = 0;

/*******************************************************************************
 * TYPES
 ******************************************************************************/

typedef int datum_t;

/*******************************************************************************
 * CLASSES
 ******************************************************************************/

/*
 * A Thread Node is the combination of a Diminuto Thread object with a Diminuto
 * List node object used to manage the Thread Node on a Diminuto List.
 */
typedef struct ThreadNode {
    diminuto_list_t link;
    diminuto_thread_t thread;
} thread_node_t;

/*
 * A Thread Pool is a FIFO of Diminuto Thread Objects managed on a Diminuto
 * List and synchronized using a Diminuto Condition.
 */
typedef struct ThreadPool {
    diminuto_condition_t condition;
    diminuto_list_t head;
} thread_pool_t;

/*
 * Wait until there is a Thread Node on the Thread Pool, remove it from the
 * Pool, and return a pointer to it.
 */
static thread_node_t * thread_node_get(thread_pool_t * pp)
{
    diminuto_list_t * lp = (diminuto_list_t *)0;
    thread_node_t * np = (thread_node_t *)0;

    DIMINUTO_CONDITION_BEGIN(&(pp->condition));
        while (diminuto_list_isempty(&(pp->head))) {
            ASSERT(diminuto_condition_wait(&(pp->condition)) == 0);
        }
        ASSERT((lp = diminuto_list_dequeue(&(pp->head))) != (diminuto_list_t *)0);
        ASSERT((np = diminuto_containerof(thread_node_t, link, lp)) != (thread_node_t *)0);
    DIMINUTO_CONDITION_END;

    return np;
}

/*
 * Insert a Thread Node on to the end of a Thread Pool and signaling any
 * Pool users waiting for the Pool to be non-empty.
 */
static void thread_node_put(thread_pool_t * pp)
{
    diminuto_thread_t * tp = (diminuto_thread_t *)0;
    thread_node_t * np = (thread_node_t *)0;

    ASSERT((tp = diminuto_thread_instance()) != (diminuto_thread_t *)0);
    ASSERT((np = diminuto_containerof(thread_node_t, thread, tp)) != (thread_node_t *)0);
    DIMINUTO_CONDITION_BEGIN(&(pp->condition));
        ASSERT(diminuto_list_enqueue(&(pp->head), &(np->link)) == &(np->link));
        ASSERT(diminuto_condition_signal(&(pp->condition)) == 0);
    DIMINUTO_CONDITION_END;
}

/*
 * Initialize an array of Thread Nodes and place each one on a Thread Pool.
 */
static void thread_pool_init(thread_pool_t * pp, thread_node_t na[], size_t nn, diminuto_thread_function_t * fp)
{
    size_t ii = -1;

    ASSERT(diminuto_condition_init(&(pp->condition)) == &(pp->condition));
    ASSERT(diminuto_list_nullinit(&(pp->head)) == &(pp->head));
    for (ii = 0; ii < nn; ++ii) {
        ASSERT(diminuto_list_nullinit(&na[ii].link) == &na[ii].link);
        ASSERT(diminuto_list_enqueue(&(pp->head), &na[ii].link) == &na[ii].link);
        ASSERT(diminuto_thread_init(&na[ii].thread, fp) == &na[ii].thread);
    }
}

/*
 * Finalize a Thread Pool by removing every possible Thread Node,
 * joining with each if it has not already been done so, and
 * finalize by each one.
 */
static void thread_pool_fini(thread_pool_t * pp, size_t nn)
{
    size_t ii = -1;
    thread_node_t * np = (thread_node_t *)0;

    for (ii = 0; ii < nn; ++ii) {
        np = thread_node_get(pp);
        switch (diminuto_thread_state(&(np->thread))) {
        case DIMINUTO_THREAD_STATE_STARTED:
        case DIMINUTO_THREAD_STATE_RUNNING:
        case DIMINUTO_THREAD_STATE_COMPLETING:
            ASSERT(diminuto_thread_join(&(np->thread), (void **)0) == 0);
            break;
        default:
            /* Do nothing. */
            break;
        }
        ASSERT(diminuto_thread_fini(&(np->thread)) == (diminuto_thread_t *)0);
    }
}

/*******************************************************************************
 * SIMULATED SERVICE CONSUMERS
 ******************************************************************************/

static thread_node_t consumer[10];
static thread_pool_t consumers;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int sn = 0;
static int consuming = 0;
static int consumings = 0;
static int providing = 0;
static int providings = 0;

/*
 * This thread represents the client who exchanges several requests and
 * replies with a server over a single IPv4 stream socket. The exact number
 * of exchanges is passed in as an argument. The thread exits when all of
 * the exchanges have been performed.
 */
static void * client(void * arg /* limit */)
{
    int limit = 0;
    int streamsocket = -1;
    diminuto_ipv4_t nearendaddress = 0;
    diminuto_port_t nearendport = 0;
    diminuto_ipv4_buffer_t nearendbuffer = { '\0', };
    diminuto_ipv4_t farendaddress = 0;
    diminuto_port_t farendport = 0;
    diminuto_ipv4_buffer_t farendbuffer = { '\0', };
    int ii = 0;
    datum_t request = -1;
    datum_t reply = -1;
    ssize_t length = 0;
    int count = 0;
    size_t total = 0;

    limit = (int)(intptr_t)arg;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
        if ((++consuming) > consumings) { consumings = consuming; }
    DIMINUTO_CRITICAL_SECTION_END;

    ASSERT(serverport != 0);
    ASSERT((streamsocket = diminuto_ipc4_stream_consumer(serveraddress, serverport)) >= 0);
    ASSERT(diminuto_ipc4_nearend(streamsocket, &nearendaddress, &nearendport) >= 0);
    ASSERT(diminuto_ipc4_farend(streamsocket, &farendaddress, &farendport) >= 0);
    CHECKPOINT("client: stream %d nearend %s:%d farend %s:%d\n", streamsocket, diminuto_ipc4_address2string(nearendaddress, nearendbuffer, sizeof(nearendbuffer)), nearendport, diminuto_ipc4_address2string(farendaddress, farendbuffer, sizeof(farendbuffer)), farendport);

    for (ii = 0; ii < limit; ++ii) {

        DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
            request = sn++;
        DIMINUTO_CRITICAL_SECTION_END;
        reply = ~request;

        ASSERT((length = diminuto_ipc4_stream_write_generic(streamsocket, &request, sizeof(request), sizeof(request))) == sizeof(request));
        COMMENT("client: stream %d wrote %zd after %d\n", streamsocket, length, ii);

        ASSERT((length = diminuto_ipc4_stream_read_generic(streamsocket, &reply, sizeof(reply), sizeof(reply))) == sizeof(reply));
        COMMENT("client: stream %d read %zd after %d\n", streamsocket, length, ii);

        count += 1;
        total += length;

        ASSERT(request == reply);

    }

    CHECKPOINT("client: stream %d finished %zu after %d\n", streamsocket, total, count);

    ASSERT(count == limit);

    ASSERT(diminuto_ipc_close(streamsocket) >= 0);

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
        --consuming;
    DIMINUTO_CRITICAL_SECTION_END;

    thread_node_put(&consumers);

    return (void *)(intptr_t)ii;
}

/*
 * This process represents the workload for the system, creating and
 * starting client threads running in parallel to make requests of the
 * of the server. The process exits when it receives a SIGINT signal.
 */
static void workload(void)
{
    int ii = 0;
    thread_node_t * np = (thread_node_t *)0;
    void * result = (void *)0;

    CHECKPOINT("workload: starting\n");

    ASSERT(diminuto_interrupter_install(0) >= 0);

    thread_pool_init(&consumers, consumer, countof(consumer), client);

    while (diminuto_interrupter_check() <= 0) {

        np = thread_node_get(&consumers);

        if (diminuto_thread_state(&(np->thread)) != DIMINUTO_THREAD_STATE_INITIALIZED) {
            ASSERT(diminuto_thread_join(&(np->thread), &result) == 0);
            ASSERT((intptr_t)result == (np - consumer));
        }

        ASSERT(diminuto_thread_start(&(np->thread), (void *)(intptr_t)(np - consumer)) == 0);

        ASSERT(diminuto_thread_yield() == 0);

    }

    CHECKPOINT("workload: interrupted\n");

    thread_pool_fini(&consumers, countof(consumer));

    CHECKPOINT("workload: consuming %d\n", consumings);

    CHECKPOINT("workload: exiting\n");
}

/*******************************************************************************
 * SIMULATED SERVICE PROVIDER
 ******************************************************************************/

static thread_node_t provider[10];
static thread_pool_t providers;

/*
 * This thread represents the server that replies to the requests of a 
 * single client over a IPv4 stream socket that is already connected
 * to the client. The stream socket is passed in as an argument. The
 * thread exits when the far end closes the stream socket.
 */
static void * server(void * arg /* streamsocket */)
{
    int streamsocket = -1;
    diminuto_ipv4_t nearendaddress = 0;
    diminuto_port_t nearendport = 0;
    diminuto_ipv4_buffer_t nearendbuffer = { '\0', };
    diminuto_ipv4_t farendaddress = 0;
    diminuto_port_t farendport = 0;
    diminuto_ipv4_buffer_t farendbuffer = { '\0', };
    diminuto_mux_t mux;
    int ready = 0;
    int fd = -1;
    ssize_t length = -1;
    datum_t datum = 0;
    int count = 0;
    size_t total = 0;

    ASSERT((streamsocket = (intptr_t)arg) >= 0);

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
        if ((++providing) > providings) { providings = providing; }
    DIMINUTO_CRITICAL_SECTION_END;

    ASSERT(diminuto_mux_init(&mux) == &mux);

    ASSERT(diminuto_ipc4_nearend(streamsocket, &nearendaddress, &nearendport) >= 0);
    ASSERT(diminuto_ipc4_farend(streamsocket, &farendaddress, &farendport) >= 0);
    CHECKPOINT("server: stream %d nearend %s:%d farend %s:%d\n", streamsocket, diminuto_ipc4_address2string(nearendaddress, nearendbuffer, sizeof(nearendbuffer)), nearendport, diminuto_ipc4_address2string(farendaddress, farendbuffer, sizeof(farendbuffer)), farendport);

    ASSERT(diminuto_mux_register_read(&mux, streamsocket) >= 0);

    while (!0) {

        if ((ready = diminuto_mux_wait(&mux, diminuto_frequency())) > 0) {
            /* Do nothing. */
        } else if (ready == 0) {
            continue;
        } else if (errno == EINTR) {
            CHECKPOINT("server: stream %d interrupted\n", streamsocket);
            break;
        } else {
            FATAL("server: diminuto_mux_wait: error");
        }

        while ((fd = diminuto_mux_ready_read(&mux)) >= 0) {

            ASSERT(fd == streamsocket);
            ASSERT((length = diminuto_ipc4_stream_read_generic(streamsocket, &datum, sizeof(datum), sizeof(datum))) >= 0);
            COMMENT("server: stream %d read %zd after %d\n", streamsocket, length, count);

            if (length == 0) {
                break;
            }

            ASSERT(length == sizeof(datum));
            ASSERT((length = diminuto_ipc4_stream_write_generic(streamsocket, &datum, length, length)) ==  sizeof(datum));
            COMMENT("server: stream %d wrote %zd after %d\n", streamsocket, length, count);

            count += 1;
            total += length;

        }

        if (length == 0) {
            break;
        }

    }

    CHECKPOINT("server: stream %d finished %zu after %d\n", streamsocket, total, count);

    ASSERT(diminuto_mux_close(&mux, streamsocket) >= 0);
    ASSERT(diminuto_mux_fini(&mux) == (diminuto_mux_t *)0);

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
        --providing;
    DIMINUTO_CRITICAL_SECTION_END;

    thread_node_put(&providers);

    return (void *)(uintptr_t)total;
}

/*
 * This thread represents the dispatcher that listens on a request socket
 * in the listen state, creates a new stream socket for every connection
 * request from a client that it accepts, and dispatches a new server
 * thread to handle the request. The request socket is passed in as an
 * argument. The thread exits when it is notified to do so.
 */
static void * dispatcher(void * arg /* requestsocket */)
{
    int requestsocket = -1;
    int ii = -1;
    diminuto_mux_t mux;
    int ready = -1;
    thread_node_t * np = (thread_node_t *)0;
    int count = 0;
    int streamsocket = -1;
    void * result = 0;

    ASSERT((requestsocket = (intptr_t)arg) >= 0);

    CHECKPOINT("dispatcher: request %d starting\n", requestsocket);

    ASSERT(diminuto_mux_init(&mux) == &mux);
    ASSERT(diminuto_mux_register_accept(&mux, requestsocket) >= 0);

    thread_pool_init(&providers, provider, countof(provider), server);

    while (diminuto_thread_notifications() == 0) {

        if ((ready = diminuto_mux_wait(&mux, diminuto_frequency())) > 0) {
            /* Do nothing. */
        } else if (ready == 0) {
            continue;
        } else if (errno == EINTR) {
            CHECKPOINT("dispatcher: request %d interrupted\n", requestsocket);
            continue;
        } else {
            FATAL("dispatcher: diminuto_mux_wait: error");
        }

        ASSERT(diminuto_mux_ready_accept(&mux) == requestsocket);
        count += 1;

        ASSERT((streamsocket = diminuto_ipc4_stream_accept(requestsocket)) >= 0);

        np = thread_node_get(&providers);

        if (diminuto_thread_state(&(np->thread)) != DIMINUTO_THREAD_STATE_INITIALIZED) {
            ASSERT(diminuto_thread_join(&(np->thread), &result) == 0);
        }

        ASSERT(diminuto_thread_start(&(np->thread), (void *)(intptr_t)streamsocket) == 0);

    }

    CHECKPOINT("dispatcher: request %d notified %d\n", requestsocket, count);

    thread_pool_fini(&providers, countof(provider));

    CHECKPOINT("dispatcher: providing %d\n", providings);

    ASSERT(diminuto_mux_unregister_accept(&mux, requestsocket) >= 0);
    ASSERT(diminuto_mux_fini(&mux) == (diminuto_mux_t *)0);

    return (void *)(intptr_t)count;
}

/*
 * This process represents the web server instance. It creates the
 * dispatcher thread. The listen socket is passed via a UNIX domain
 * (local) socket using a control message. When the instance receives
 * a SIGINT signal, it notifies the dispatcher to terminate, it
 * joins with the dispatcher, and then exits.
 */
static void instance(void) 
{
    diminuto_mux_t mux;
    diminuto_thread_t dispatcherthread;
    int activationsocket = -1;
    diminuto_local_t path = { '\0', };
    diminuto_local_t nearendpath = { '\0', };
    diminuto_local_t farendpath = { '\0', };
    char dummy[1] = { '\0' };
    struct iovec vector[1];
    struct msghdr message;
    union { struct cmsghdr alignment; char data[CMSG_SPACE(sizeof(int))]; } control;
    struct cmsghdr * cp = (struct cmsghdr *)0;
    ssize_t length = -1;
    int requestsocket = -1;
    diminuto_ipv4_t nearendaddress = 0;
    diminuto_port_t nearendport = 0;
    diminuto_ipv4_buffer_t nearendbuffer = { '\0', };
    diminuto_ipv4_buffer_t farendbuffer = { '\0', };
    void * result = (void *)0;

    CHECKPOINT("instance: starting\n");

    ASSERT(diminuto_interrupter_install(0) >= 0);

    ASSERT(diminuto_mux_init(&mux) == &mux);

    ASSERT(diminuto_thread_init(&dispatcherthread, dispatcher) == &dispatcherthread);

    ASSERT(diminuto_ipcl_path(INSTANCEPATH, path, sizeof(path)) == (char *)&path);
    ASSERT((activationsocket = diminuto_ipcl_packet_consumer(path)) >= 0);
    ASSERT(diminuto_ipcl_nearend(activationsocket, nearendpath, sizeof(nearendpath)) >= 0);
    ASSERT(diminuto_ipcl_farend(activationsocket, farendpath, sizeof(farendpath)) >= 0);
    CHECKPOINT("instance: activation %d nearend \"%s\" farend \"%s\"\n", activationsocket, diminuto_ipcl_path2string(nearendpath), diminuto_ipcl_path2string(farendpath));

    memset(&message, 0, sizeof(message));
    memset(&control, 0, sizeof(control));

    vector[0].iov_base = dummy; /* You have to receive at least one byte of payload. */
    vector[0].iov_len = sizeof(dummy);

    message.msg_iov = vector;
    message.msg_iovlen = countof(vector);

    message.msg_control = &control;
    message.msg_controllen = sizeof(control);

    ASSERT((length = diminuto_ipcl_packet_receive(activationsocket, &message)) == sizeof(dummy));

    for (cp = CMSG_FIRSTHDR(&message); cp != (struct cmsghdr *)0; cp = CMSG_NXTHDR(&message, cp)) {
        if (cp->cmsg_level != SOL_SOCKET) { continue; }
        if (cp->cmsg_type != SCM_RIGHTS) { continue; }
        if (cp->cmsg_len != CMSG_LEN(sizeof(requestsocket))) { continue; }
        memcpy(&requestsocket, CMSG_DATA(cp), sizeof(requestsocket));
    }

    ASSERT(requestsocket >= 0);
    ASSERT(diminuto_ipc4_nearend(requestsocket, &nearendaddress, &nearendport) >= 0);
    CHECKPOINT("instance: request %d nearend %s:%d farend %s:%d\n", requestsocket, diminuto_ipc4_address2string(nearendaddress, nearendbuffer, sizeof(nearendbuffer)), nearendport, diminuto_ipc4_address2string(0xffffffff, farendbuffer, sizeof(farendbuffer)), 0);

    ASSERT(diminuto_ipc_close(activationsocket) >= 0);

    ASSERT(diminuto_thread_start(&dispatcherthread, (void *)(intptr_t)requestsocket) == 0);

    while (diminuto_interrupter_check() <= 0) {
        diminuto_mux_wait(&mux, -1);
    }

    CHECKPOINT("instance: request %d interrupted\n", requestsocket);

    ASSERT(diminuto_thread_notify(&dispatcherthread) == 0);
    ASSERT(diminuto_thread_join(&dispatcherthread, &result) == 0);
    ASSERT(diminuto_thread_fini(&dispatcherthread) == (diminuto_thread_t *)0);

    ASSERT(diminuto_mux_fini(&mux) == (diminuto_mux_t *)0);

    CHECKPOINT("instance: request %d exiting\n", requestsocket);
}

/*******************************************************************************
 * MAIN PROGRAM
 ******************************************************************************/

int main(int argc, char argv[])
{
    int requestsocket = -1;
    diminuto_ipv4_buffer_t nearendbuffer = { '\0', };
    diminuto_ipv4_buffer_t farendbuffer = { '\0', };
    diminuto_local_t path = { '\0', };
    diminuto_local_t nearendpath = { '\0', };
    diminuto_local_t farendpath = { '\0', };
    int localsocket = -1;
    int activationsocket = -1;
    pid_t workloadpid = -1;
    pid_t instancepid = -1;
    diminuto_mux_t mux;
    int ready = -1;
    char dummy[1] = { '\0' };
    struct iovec vector[1];
    struct msghdr message;
    union { struct cmsghdr alignment; char data[CMSG_SPACE(sizeof(int))]; } control;
    struct cmsghdr * cp = (struct cmsghdr *)0;
    int status = -1;

    CHECKPOINT("main: starting\n");

    SETLOGMASK();

    ASSERT(diminuto_reaper_install(0) >= 0);

    ASSERT(diminuto_mux_init(&mux) == &mux);

    /*
     * In any real application we would have a fixed port number,
     * probably defined as a service, and would have an address
     * that was resolved from a Fully Qualified Domain Name
     * (FQDN). But this unit test is running on one computer,
     * and we'll let the IP stack choose an ephemeral port for
     * us to use.
     */

    ASSERT((requestsocket = diminuto_ipc4_stream_provider(0)) >= 0);
    ASSERT(diminuto_ipc4_nearend(requestsocket, &serveraddress, &serverport) >= 0);
    CHECKPOINT("main: request %d nearend %s:%d farend %s:%d\n", requestsocket, diminuto_ipc4_address2string(serveraddress, nearendbuffer, sizeof(nearendbuffer)), serverport, diminuto_ipc4_address2string(0xffffffff, farendbuffer, sizeof(farendbuffer)), 0);

    /*
     * Workload will queue up clients on the listen socket requesting a
     * connection to a server.
     */

    ASSERT((workloadpid = fork()) >= 0);
    if (workloadpid == 0) {
        (void)diminuto_ipc_close(requestsocket);
        workload();
        EXIT();
    }

    /*
     * When an instance is ready to get a listen socket on which to
     * service clients, it will contact us via this UNIX domain (local)
     * socket. Just like clients will queue up on the listen socket,
     * instances will queue up on the instance socket.
     */

    ASSERT(diminuto_ipcl_path(INSTANCEPATH, path, sizeof(path)) == (char *)&path);
    ADVISE(diminuto_ipcl_remove(path) >= 0);
    ASSERT((localsocket = diminuto_ipcl_packet_provider(path)) >= 0);
    ASSERT(diminuto_ipcl_nearend(localsocket, nearendpath, sizeof(nearendpath)) >= 0);
    CHECKPOINT("main: local %d nearend \"%s\" farend \"%s\"\n", localsocket, diminuto_ipcl_path2string(nearendpath), diminuto_ipcl_path2string((const char *)0));

    ASSERT(diminuto_mux_register_accept(&mux, localsocket) >= 0);

    ASSERT((instancepid = fork()) >= 0);
    if (instancepid == 0) {
        (void)diminuto_ipc_close(requestsocket);
        requestsocket = -1;
        (void)diminuto_ipc_close(localsocket);
        localsocket = -1;
        instance();
        EXIT();
    }

    while (!0) {

        if ((ready = diminuto_mux_wait(&mux, diminuto_frequency())) > 0) {
            /* Do nothing. */
        } else if (ready == 0) {
            continue;
        } else if (errno == EINTR) {
            CHECKPOINT("main: local %d interrupted\n", localsocket);
            break;
        } else {
            FATAL("main: diminuto_mux_wait: error");
        }

        ASSERT(diminuto_mux_ready_accept(&mux) == localsocket);

        ASSERT((activationsocket = diminuto_ipcl_packet_accept(localsocket)) >= 0);
        ASSERT(diminuto_ipcl_nearend(activationsocket, nearendpath, sizeof(nearendpath)) >= 0);
        ASSERT(diminuto_ipcl_farend(activationsocket, farendpath, sizeof(farendpath)) >= 0);
        CHECKPOINT("main: activation %d nearend \"%s\" farend \"%s\"\n", activationsocket, diminuto_ipcl_path2string(nearendpath), diminuto_ipcl_path2string(farendpath));

        /*
         * Yeah, this is crazy: we're going to transfer an open file
         * descriptor - in this case, our listen socket, to another
         * process. It will be received on the far end and processed
         * as if the receiver had done a dup(2) on our socket.
         */

        memset(&message, 0, sizeof(message));
        memset(&control, 0, sizeof(control));

        /*
         * We have to send at least one byte of payload. Both the
         * sendmsg(2) and recvmsg(2) will reflect this by returning
         * the value 1 nominally, even though a bunch of other stuff
         * is passed into the kernel and back out the other end of
         * the Local socket with some processing in between.
         */

        vector[0].iov_base = dummy;
        vector[0].iov_len = sizeof(dummy);

        message.msg_iov = vector;
        message.msg_iovlen = countof(vector);

        message.msg_control = &control;
        message.msg_controllen = sizeof(control);

        cp = CMSG_FIRSTHDR(&message);
        cp->cmsg_level = SOL_SOCKET;
        cp->cmsg_type = SCM_RIGHTS;
        cp->cmsg_len = CMSG_LEN(sizeof(requestsocket));
        memcpy(CMSG_DATA(cp), &requestsocket, sizeof(requestsocket));

        ASSERT(diminuto_ipcl_packet_send(activationsocket, &message) == sizeof(dummy));

        ASSERT(diminuto_ipc_close(activationsocket) >= 0);

        /*
         * In real life we would do this in a loop, passing the
         * listen socket to a new instance process every time the
         * prior one exited. 
         */

        break;

    }

    /*
     * It is during this delay in which all the work gets done.
     */

    diminuto_delay(diminuto_frequency() * DURATION, !0);
    ASSERT(diminuto_reaper_check() == 0);

    /*
     * Tell workload to exit.
     */

    ASSERT(kill(workloadpid, SIGINT) == 0);
    ASSERT(diminuto_reaper_reap_generic(workloadpid, &status, 0) == workloadpid);
    CHECKPOINT("main: request %d workload %d status %d\n", requestsocket, workloadpid, status);
    ASSERT(WIFEXITED(status));
    ASSERT(WEXITSTATUS(status) == 0);

    /*
     * Tell instance to exit.
     */

    ASSERT(kill(instancepid, SIGINT) == 0);
    ASSERT(diminuto_reaper_reap_generic(instancepid, &status, 0) == instancepid);
    CHECKPOINT("main: request %d instance %d status %d\n", requestsocket, instancepid, status);
    ASSERT(WIFEXITED(status));
    ASSERT(WEXITSTATUS(status) == 0);

    /*
     * Clean up.
     */

    CHECKPOINT("main: exiting\n");

    ASSERT(diminuto_mux_close(&mux, localsocket) >= 0);
    ASSERT(diminuto_mux_fini(&mux) == (diminuto_mux_t *)0);
    ASSERT(diminuto_ipcl_remove(path) >= 0);
    ASSERT(diminuto_ipc_close(requestsocket) >= 0);

    EXIT();
}
