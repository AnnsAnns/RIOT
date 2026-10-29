// SPDX-FileCopyrightText: 2026 RIOT community
// SPDX-License-Identifier: LGPL-2.1-only

//! Native test for the Rust API of the URI parser.

#![no_std]

use riot_wrappers::{println, riot_main};
use rust_riotmodules::uri_parser::{is_absolute, parse, split_query, QueryParam};

extern crate rust_riotmodules;

riot_main!(main);

fn main() {
    if test_all() {
        println!("SUCCESS");
    } else {
        println!("FAILED");
    }
}

fn test_all() -> bool {
    test_absolute_uri()
        && test_relative_uri()
        && test_absolute_detection()
        && test_invalid_uri()
        && test_query_split()
}

fn test_absolute_uri() -> bool {
    let uri = b"coap://RIOT:test@[fe80:db8::1%eth2]:5683/.well-known/core?v=1";
    let parsed = match parse(uri) {
        Ok(parsed) => parsed,
        Err(_) => {
            println!("FAIL: could not parse absolute URI");
            return false;
        }
    };

    if parsed.scheme != Some(&b"coap"[..]) {
        println!("FAIL: scheme");
        return false;
    }
    if parsed.userinfo != Some(&b"RIOT:test"[..]) {
        println!("FAIL: userinfo");
        return false;
    }
    if parsed.host != Some(&b"[fe80:db8::1%eth2]"[..]) {
        println!("FAIL: host");
        return false;
    }
    if parsed.ipv6addr != Some(&b"fe80:db8::1"[..]) {
        println!("FAIL: ipv6addr");
        return false;
    }
    if parsed.zoneid != Some(&b"eth2"[..]) {
        println!("FAIL: zoneid");
        return false;
    }
    if parsed.port_str != Some(&b"5683"[..]) || parsed.port != 5683 {
        println!("FAIL: port");
        return false;
    }
    if parsed.path != Some(&b"/.well-known/core"[..]) {
        println!("FAIL: path");
        return false;
    }
    if parsed.query != Some(&b"v=1"[..]) {
        println!("FAIL: query");
        return false;
    }

    true
}

fn test_relative_uri() -> bool {
    let parsed = match parse(b"/.well-known/core?v=1") {
        Ok(parsed) => parsed,
        Err(_) => {
            println!("FAIL: could not parse relative URI");
            return false;
        }
    };

    if parsed.scheme.is_some() || parsed.host.is_some() {
        println!("FAIL: relative URI has scheme or host");
        return false;
    }
    if parsed.path != Some(&b"/.well-known/core"[..]) {
        println!("FAIL: relative path");
        return false;
    }
    if parsed.query != Some(&b"v=1"[..]) {
        println!("FAIL: relative query");
        return false;
    }

    true
}

fn test_absolute_detection() -> bool {
    if !is_absolute(b"coap://example.org") {
        println!("FAIL: absolute URI was not detected");
        return false;
    }
    if is_absolute(b"/foo:bar") || is_absolute(b"./this:that") {
        println!("FAIL: relative URI was detected as absolute");
        return false;
    }

    true
}

fn test_invalid_uri() -> bool {
    if parse(b"coap://[::1").is_ok() {
        println!("FAIL: invalid URI was accepted");
        return false;
    }

    true
}

fn test_query_split() -> bool {
    let mut params: [Option<QueryParam>; 4] = [None; 4];

    if split_query(b"foo=&=&bar=1", &mut params) != Ok(3) {
        println!("FAIL: split query count");
        return false;
    }

    let first = params[0].unwrap();
    let second = params[1].unwrap();
    let third = params[2].unwrap();

    if first.name != b"foo" || first.value != b"" {
        println!("FAIL: first query parameter");
        return false;
    }
    if second.name != b"" || second.value != b"" {
        println!("FAIL: second query parameter");
        return false;
    }
    if third.name != b"bar" || third.value != b"1" {
        println!("FAIL: third query parameter");
        return false;
    }

    if split_query(b"this=0&is=1&a=very&long=3&query=foo", &mut params) != Err(-2) {
        println!("FAIL: truncated query split");
        return false;
    }

    if split_query(b"&", &mut params) != Err(-1) {
        println!("FAIL: broken query split");
        return false;
    }

    if split_query(b"", &mut params) != Ok(0) {
        println!("FAIL: empty query split");
        return false;
    }

    true
}
