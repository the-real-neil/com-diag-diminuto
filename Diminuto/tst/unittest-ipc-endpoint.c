/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Endpoint portion of the IPC feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Endpoint portion of the IPC feature.
 */

#include <stdlib.h>
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_dump.h"

#define FQDN "diag.com"
#define IPV4 "205.178.189.131"
#define IPV6 "2607:f8b0:400f:805::200e"
#define SERVICE "http"
#define PORT "80"
#define UNIX "unix-domain.sock"

#define SETUP \
        char * endpoint; \
        diminuto_ipv4_buffer_t ipv4buffer; \
        diminuto_ipv6_buffer_t ipv6buffer; \
        diminuto_ipc_endpoint_t parse; \
        int rc \

static const char * type2string(int type)
{
    static char buffer[sizeof(int) * 3];
    const char * result = "invalid";
    switch (type) {
    case AF_UNSPEC: result = "AF_UNSPEC";   break;
    case AF_INET:   result = "AF_INET";     break;
    case AF_INET6:  result = "AF_INET6";    break;
    case AF_UNIX:   result = "AF_UNIX";     break;
    default:
        snprintf(buffer, sizeof(buffer), "%d", type);
        buffer[sizeof(buffer) - 1] = '\0';
        result = buffer;
        break;
    }
    return result;
}

#define DISPLAY \
        COMMENT("endpoint=\"%s\" type=%s ipv4=%s ipv6=%s tcp=%d udp=%d path=\"%s\"\n", \
            endpoint, \
            type2string(parse.type), \
            diminuto_ipc4_address2string(parse.ipv4, ipv4buffer, sizeof(ipv4buffer)), \
            diminuto_ipc6_address2string(parse.ipv6, ipv6buffer, sizeof(ipv6buffer)), \
            parse.tcp, \
            parse.udp, \
            (parse.path == (const char *)0) ? "" : parse.path \
        )

#define VERIFYINET(_POINTER_, _IPV4_, _IPV6_, _TCP_, _UDP_) \
    do { \
        const diminuto_ipc_endpoint_t * _pp_ = (_POINTER_); \
        EXPECT(diminuto_ipc4_compare(&(_pp_->ipv4), &(_IPV4_)) == 0); \
        EXPECT(diminuto_ipc6_compare(&(_pp_->ipv6), &(_IPV6_)) == 0); \
        EXPECT(_pp_->tcp == (_TCP_)); \
        EXPECT(_pp_->udp == (_UDP_)); \
        EXPECT(_pp_->path == (const char *)0); \
        EXPECT(((diminuto_ipc6_compare(&(_pp_->ipv6), &DIMINUTO_IPC6_UNSPECIFIED) != 0) && (_pp_->type == AF_INET6)) || ((diminuto_ipc4_compare(&(_pp_->ipv4), &DIMINUTO_IPC4_UNSPECIFIED) != 0) && (_pp_->type == AF_INET)) || ((_pp_->tcp != 0) && (_pp_->type == AF_INET)) || ((_pp_->udp != 0) && (_pp_->type == AF_INET)) || (_pp_->type == AF_INET)); \
    } while (0)

#define VERIFYUNIX(_POINTER_, _PATH_) \
    do { \
        const diminuto_ipc_endpoint_t * _pp_ = (_POINTER_); \
        EXPECT(diminuto_ipc4_compare(&(_pp_->ipv4), &DIMINUTO_IPC4_UNSPECIFIED) == 0); \
        EXPECT(diminuto_ipc6_compare(&(_pp_->ipv6), &DIMINUTO_IPC6_UNSPECIFIED) == 0); \
        EXPECT(_pp_->tcp == 0); \
        EXPECT(_pp_->udp == 0); \
        EXPECT(strcmp(_pp_->path, (_PATH_)) == 0); \
        EXPECT(_pp_->type == AF_UNIX); \
    } while (0)

int main(int argc, char * argv[])
{
    const char fqdn[] = FQDN;
    diminuto_ipv4_t unspecified4;
    diminuto_ipv6_t unspecified6;
    diminuto_ipv4_t localhost4;
    diminuto_ipv6_t localhost6;
    diminuto_ipv4_t fqdn4;
    diminuto_ipv6_t fqdn6;
    diminuto_ipv4_t address4;
    diminuto_ipv6_t address46;
    diminuto_ipv6_t address6;
    diminuto_port_t ephemeral;
    diminuto_port_t port;
    diminuto_port_t tcp;
    diminuto_port_t udp;

    SETLOGMASK();

    COMMENT("FQDN=\"%s\"\n", FQDN);
    COMMENT("IPV4=\"%s\"\n", IPV4);
    COMMENT("IPV6=\"%s\"\n", IPV6);
    COMMENT("SERVICE=\"%s\"\n", SERVICE);
    COMMENT("PORT=\"%s\"\n", PORT);
    COMMENT("UNIX=\"%s\"\n", UNIX);

    unspecified4 = DIMINUTO_IPC4_UNSPECIFIED;
    unspecified6 = DIMINUTO_IPC6_UNSPECIFIED;
    localhost4 = diminuto_ipc4_address("localhost");
    localhost6 = diminuto_ipc6_address("localhost");
    fqdn4 = diminuto_ipc4_address(fqdn);
    fqdn6 = diminuto_ipc6_address(fqdn);
    address4 = diminuto_ipc4_address(IPV4);
    address46 = diminuto_ipc6_address("::ffff:" IPV4);
    address6 = diminuto_ipc6_address(IPV6);
    ephemeral = 0;
    port = atoi(PORT);
    tcp = diminuto_ipc_port(SERVICE, "tcp");
    udp = diminuto_ipc_port(SERVICE, "udp");

    {
        diminuto_ipv4_buffer_t ipv4buffer;
        diminuto_ipv6_buffer_t ipv6buffer;
        COMMENT("unspecified4=%s\n", diminuto_ipc4_address2string(unspecified4, ipv4buffer, sizeof(ipv4buffer)));
        COMMENT("unspecified6=%s\n", diminuto_ipc6_address2string(unspecified6, ipv6buffer, sizeof(ipv6buffer)));
        COMMENT("localhost4=%s\n", diminuto_ipc4_address2string(localhost4, ipv4buffer, sizeof(ipv4buffer)));
        COMMENT("localhost6=%s\n", diminuto_ipc6_address2string(localhost6, ipv6buffer, sizeof(ipv6buffer)));
        COMMENT("fqdn4=%s\n", diminuto_ipc4_address2string(fqdn4, ipv4buffer, sizeof(ipv4buffer)));
        COMMENT("fqdn6=%s\n", diminuto_ipc6_address2string(fqdn6, ipv6buffer, sizeof(ipv6buffer)));
        COMMENT("address4=%s\n", diminuto_ipc4_address2string(address4, ipv4buffer, sizeof(ipv4buffer)));
        COMMENT("address46=%s\n", diminuto_ipc6_address2string(address46, ipv6buffer, sizeof(ipv6buffer)));
        COMMENT("address6=%s\n", diminuto_ipc6_address2string(address6, ipv6buffer, sizeof(ipv6buffer)));
        COMMENT("ephemeral=%d\n", ephemeral);
        COMMENT("port=%d\n", port);
        COMMENT("tcp=%d\n", tcp);
        COMMENT("udp=%d\n", udp);
    }

    /*
     * Below are the sunny day scenarios I expect to succeed. To test
     * arbitary endpoint strings, see the endpoint utility.
     */

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = PORT, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, unspecified4, unspecified6, port, port);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = ":" PORT, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, unspecified4, unspecified6, port, port);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = SERVICE, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, unspecified4, unspecified6, tcp, udp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = ":" SERVICE, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, unspecified4, unspecified6, tcp, udp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "localhost", &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, localhost4, localhost6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "localhost:" PORT, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, localhost4, localhost6, port, port);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "localhost:" SERVICE, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, localhost4, localhost6, tcp, udp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, fqdn4, fqdn6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN ":" PORT, &parse);
        EXPECT(rc == 0);
        VERIFYINET(&parse, fqdn4, fqdn6, port, port);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = FQDN ":" SERVICE, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, fqdn4, fqdn6, tcp, udp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = IPV4, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, address4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = IPV4 ":" PORT, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, address4, unspecified6, port, port);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = IPV4 ":" SERVICE, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, address4, unspecified6, tcp, udp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::]", &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, unspecified4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::]:" PORT, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, unspecified4, unspecified6, port, port);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::]:" SERVICE, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, unspecified4, unspecified6, tcp, udp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "0", &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, unspecified4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "0.0.0.0", &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, unspecified4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "0.0.0.0:0", &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, unspecified4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "", &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, unspecified4, unspecified6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::ffff:" IPV4 "]", &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, unspecified4, address46, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::ffff:" IPV4 "]:" PORT, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, unspecified4, address46, port, port);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[::ffff:" IPV4 "]:" SERVICE, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, unspecified4, address46, tcp, udp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[" IPV6 "]", &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, unspecified4, address6, ephemeral, ephemeral);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[" IPV6 "]:" PORT, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, unspecified4, address6, port, port);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "[" IPV6 "]:" SERVICE, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYINET(&parse, unspecified4, address6, tcp, udp);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = UNIX, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYUNIX(&parse, endpoint);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "/" UNIX, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYUNIX(&parse, endpoint);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "./" UNIX, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYUNIX(&parse, endpoint);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "../" UNIX, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYUNIX(&parse, endpoint);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "/run/" UNIX, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYUNIX(&parse, endpoint);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "./run/" UNIX, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYUNIX(&parse, endpoint);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "../run/" UNIX, &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYUNIX(&parse, endpoint);
        STATUS();
    }

    {
        SETUP;
        TEST();
        rc = diminuto_ipc_endpoint(endpoint = "I-hope-this-os-an-unresolvable-domain-name.com", &parse);
        EXPECT(rc == 0);
        DISPLAY;
        VERIFYUNIX(&parse, endpoint);
        STATUS();
    }

    EXIT();
}
