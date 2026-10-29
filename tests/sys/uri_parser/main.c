/*
 * SPDX-FileCopyrightText: 2026 RIOT community
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Native test for the Rust implementation of the URI parser
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "uri_parser.h"

static int _failed;

static void _check_part(const char *name, const char *actual, uint16_t actual_len,
                        const char *expected)
{
    size_t expected_len = strlen(expected);

    if (actual_len != expected_len) {
        printf("FAIL: %s: expected length %u, got %u\n", name,
               (unsigned)expected_len, (unsigned)actual_len);
        _failed = 1;
        return;
    }

    if (expected_len && ((actual == NULL) ||
                         (memcmp(actual, expected, expected_len) != 0))) {
        printf("FAIL: %s: expected '%.*s', got '%.*s'\n", name,
               (int)expected_len, expected, (int)actual_len,
               actual ? actual : "(null)");
        _failed = 1;
    }
}

static void _check_absolute_uri(void)
{
    const char *uri = "coap://RIOT:test@[fe80:db8::1%eth2]:5683/.well-known/core?v=1";
    uri_parser_result_t res;

    if (uri_parser_process_string(&res, uri) != 0) {
        printf("FAIL: could not parse '%s'\n", uri);
        _failed = 1;
        return;
    }

    _check_part("scheme", res.scheme, res.scheme_len, "coap");
    _check_part("userinfo", res.userinfo, res.userinfo_len, "RIOT:test");
    _check_part("host", res.host, res.host_len, "[fe80:db8::1%eth2]");
    _check_part("ipv6addr", res.ipv6addr, res.ipv6addr_len, "fe80:db8::1");
    _check_part("zoneid", res.zoneid, res.zoneid_len, "eth2");
    _check_part("port_str", res.port_str, res.port_str_len, "5683");
    _check_part("path", res.path, res.path_len, "/.well-known/core");
    _check_part("query", res.query, res.query_len, "v=1");

    if (res.port != 5683) {
        printf("FAIL: port: expected 5683, got %u\n", (unsigned)res.port);
        _failed = 1;
    }
}

static void _check_relative_uri(void)
{
    const char *uri = "/.well-known/core?v=1";
    uri_parser_result_t res;

    if (uri_parser_process_string(&res, uri) != 0) {
        printf("FAIL: could not parse '%s'\n", uri);
        _failed = 1;
        return;
    }

    if (res.scheme != NULL || res.host != NULL) {
        printf("FAIL: relative URI has scheme or host\n");
        _failed = 1;
    }

    _check_part("path", res.path, res.path_len, "/.well-known/core");
    _check_part("query", res.query, res.query_len, "v=1");
}

static void _check_absolute_detection(void)
{
    if (!uri_parser_is_absolute_string("coap://example.org")) {
        printf("FAIL: absolute URI was not detected\n");
        _failed = 1;
    }

    if (uri_parser_is_absolute_string("/foo:bar")) {
        printf("FAIL: relative URI was detected as absolute\n");
        _failed = 1;
    }

    if (uri_parser_is_absolute_string("./this:that")) {
        printf("FAIL: relative URI was detected as absolute\n");
        _failed = 1;
    }
}

static void _check_invalid_uri(void)
{
    uri_parser_result_t res;

    if (uri_parser_process_string(&res, "coap://[::1") != -1) {
        printf("FAIL: invalid URI was accepted\n");
        _failed = 1;
    }
}

static void _check_query_split(void)
{
    uri_parser_result_t res;
    uri_parser_query_param_t params[4];
    const char *query = "foo=&=&bar=1";

    memset(&res, 0, sizeof(res));
    memset(params, 0, sizeof(params));
    res.query = query;
    res.query_len = strlen(query);

    int count = uri_parser_split_query(&res, params, 4);
    if (count != 3) {
        printf("FAIL: split query returned %d, expected 3\n", count);
        _failed = 1;
        return;
    }

    _check_part("name[0]", params[0].name, params[0].name_len, "foo");
    _check_part("value[0]", params[0].value, params[0].value_len, "");
    _check_part("name[1]", params[1].name, params[1].name_len, "");
    _check_part("value[1]", params[1].value, params[1].value_len, "");
    _check_part("name[2]", params[2].name, params[2].name_len, "bar");
    _check_part("value[2]", params[2].value, params[2].value_len, "1");
}

static void _check_query_split_limits(void)
{
    uri_parser_result_t res;
    uri_parser_query_param_t params[4];
    const char *query = "this=0&is=1&a=very&long=3&query=foo";

    memset(&res, 0, sizeof(res));
    memset(params, 0, sizeof(params));
    res.query = query;
    res.query_len = strlen(query);

    if (uri_parser_split_query(&res, params, 4) != -2) {
        printf("FAIL: truncated query split did not return -2\n");
        _failed = 1;
    }

    memset(&res, 0, sizeof(res));
    memset(params, 0, sizeof(params));
    res.query = "&";
    res.query_len = 1;

    if (uri_parser_split_query(&res, params, 4) != -1) {
        printf("FAIL: broken query split did not return -1\n");
        _failed = 1;
    }
}

int main(void)
{
    _check_absolute_uri();
    _check_relative_uri();
    _check_absolute_detection();
    _check_invalid_uri();
    _check_query_split();
    _check_query_split_limits();

    if (_failed) {
        puts("FAILED");
        return 1;
    }

    puts("SUCCESS");
    return 0;
}
