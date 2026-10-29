// SPDX-FileCopyrightText: 2020 HAW Hamburg
// SPDX-License-Identifier: LGPL-2.1-only

//! A minimal, non-destructive URI parser.
//!
//! This is a Rust rewrite of RIOT's original `uri_parser` module. It keeps the
//! C API from `uri_parser.h`, and also exposes a small safe Rust API.
//!
//! @see <https://tools.ietf.org/html/rfc3986>

#![no_std]

use core::ffi::{c_char, c_int, CStr};
use core::ptr;
use core::slice;

const MAX_PORT_STR_LEN: usize = 5;

/// Error returned by [`parse`].
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct ParseError;

/// Result of parsing a URI reference with the safe Rust API.
#[derive(Debug, PartialEq, Eq)]
pub struct Uri<'a> {
    /// Scheme, if the input was an absolute URI.
    pub scheme: Option<&'a [u8]>,
    /// User information, if present.
    pub userinfo: Option<&'a [u8]>,
    /// Host. For IPv6 addresses, this includes the brackets and zone ID.
    pub host: Option<&'a [u8]>,
    /// IPv6 address without brackets and zone ID.
    pub ipv6addr: Option<&'a [u8]>,
    /// IPv6 zone ID without the leading `%`.
    pub zoneid: Option<&'a [u8]>,
    /// Port as it appears in the URI.
    pub port_str: Option<&'a [u8]>,
    /// Port number.
    pub port: u16,
    /// Path.
    pub path: Option<&'a [u8]>,
    /// Query without the leading `?`.
    pub query: Option<&'a [u8]>,
}

/// One query parameter returned by [`split_query`].
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct QueryParam<'a> {
    /// Parameter name.
    pub name: &'a [u8],
    /// Parameter value.
    pub value: &'a [u8],
}

/// C layout of `uri_parser_result_t` from `uri_parser.h`.
#[repr(C)]
pub struct UriParserResultC {
    pub scheme: *const c_char,
    pub userinfo: *const c_char,
    pub host: *const c_char,
    pub ipv6addr: *const c_char,
    pub zoneid: *const c_char,
    pub port_str: *const c_char,
    pub path: *const c_char,
    pub query: *const c_char,
    pub port: u16,
    pub scheme_len: u16,
    pub userinfo_len: u16,
    pub host_len: u16,
    pub ipv6addr_len: u16,
    pub zoneid_len: u16,
    pub port_str_len: u16,
    pub path_len: u16,
    pub query_len: u16,
}

/// C layout of `uri_parser_query_param_t` from `uri_parser.h`.
#[repr(C)]
pub struct QueryParamC {
    pub name: *const c_char,
    pub value: *const c_char,
    pub name_len: u16,
    pub value_len: u16,
}

#[derive(Default)]
struct Parsed {
    scheme: Option<(usize, usize)>,
    userinfo: Option<(usize, usize)>,
    host: Option<(usize, usize)>,
    ipv6addr: Option<(usize, usize)>,
    zoneid: Option<(usize, usize)>,
    port_str: Option<(usize, usize)>,
    port: u16,
    path: Option<(usize, usize)>,
    query: Option<(usize, usize)>,
}

fn find(bytes: &[u8], start: usize, end: usize, needle: u8) -> Option<usize> {
    (start..end).find(|&i| bytes[i] == needle)
}

fn rfind(bytes: &[u8], start: usize, end: usize, needle: u8) -> Option<usize> {
    (start..end).rev().find(|&i| bytes[i] == needle)
}

/// Check whether a URI reference is in absolute form.
pub fn is_absolute(uri: &[u8]) -> bool {
    let colon = match find(uri, 0, uri.len(), b':') {
        Some(colon) => colon,
        None => return false,
    };

    if !uri[0].is_ascii_alphabetic() {
        return false;
    }

    for &byte in &uri[..colon] {
        if !(byte.is_ascii_alphanumeric() || byte == b'+' || byte == b'-' || byte == b'.') {
            return false;
        }
    }

    true
}

fn parse_absolute(uri: &[u8], result: &mut Parsed) -> bool {
    let colon = match find(uri, 0, uri.len(), b':') {
        Some(colon) => colon,
        None => return false,
    };

    if uri[0] == b':' {
        return false;
    }

    result.scheme = Some((0, colon));

    let has_authority = (uri.len() - colon) > 2 && uri[colon + 1] == b'/' && uri[colon + 2] == b'/';

    let mut pos = if has_authority { colon + 3 } else { colon + 1 };

    if pos >= uri.len() {
        return true;
    }

    if has_authority {
        pos = match consume_authority(uri, pos, result) {
            Some(pos) => pos,
            None => return false,
        };
    }

    if pos < uri.len() {
        consume_path(uri, pos, result);
    }

    true
}

fn consume_authority(uri: &[u8], start: usize, result: &mut Parsed) -> Option<usize> {
    let authority_end = find(uri, start, uri.len(), b'/').unwrap_or(uri.len());
    result.host = Some((start, authority_end));

    if let Some(at) = find(uri, start, authority_end, b'@') {
        result.userinfo = Some((start, at));
        let offset = at - start + 1;
        result.host = Some((start + offset, authority_end));
    }

    let (host_start, host_end) = result.host?;
    if host_start == host_end {
        return Some(authority_end);
    }

    let mut ipv6_end = None;
    if uri[host_start] == b'[' {
        let close = find(uri, host_start, host_end, b']')?;
        ipv6_end = Some(close);

        if let Some(percent) = find(uri, host_start, close, b'%') {
            let zone_start = percent + 1;
            if zone_start == close {
                return None;
            }
            result.zoneid = Some((zone_start, close));
            result.ipv6addr = Some((host_start + 1, percent));
        } else {
            result.ipv6addr = Some((host_start + 1, close));
        }
    }

    let search_start = ipv6_end.unwrap_or(host_start);
    if let Some(colon) = rfind(uri, search_start, host_end, b':') {
        let port_start = colon + 1;

        if port_start == host_end {
            return None;
        }

        let port_len = host_end - port_start;
        if port_len > MAX_PORT_STR_LEN {
            return None;
        }

        let mut port = 0u32;
        for &byte in &uri[port_start..host_end] {
            if !byte.is_ascii_digit() {
                return None;
            }
            port = port * 10 + u32::from(byte - b'0');
        }

        if port > u32::from(u16::MAX) {
            return None;
        }

        result.port = port as u16;
        result.port_str = Some((port_start, host_end));
        result.host = Some((host_start, host_end - (port_len + 1)));
    }

    Some(authority_end)
}

fn consume_path(uri: &[u8], start: usize, result: &mut Parsed) {
    result.path = Some((start, uri.len()));

    if let Some(query_start) = find(uri, start, uri.len(), b'?') {
        result.query = Some((query_start + 1, uri.len()));
        result.path = Some((start, query_start));
    }
}

fn parse_internal(uri: &[u8]) -> Option<Parsed> {
    let mut result = Parsed::default();

    if is_absolute(uri) {
        if !parse_absolute(uri, &mut result) {
            return None;
        }
    } else {
        consume_path(uri, 0, &mut result);
    }

    Some(result)
}

fn slice_of(uri: &[u8], range: Option<(usize, usize)>) -> Option<&[u8]> {
    range.map(|(start, end)| &uri[start..end])
}

/// Parse a URI reference with the safe Rust API.
///
/// Fragments are not handled. An empty URI returns [`ParseError`].
pub fn parse(uri: &[u8]) -> Result<Uri<'_>, ParseError> {
    if uri.is_empty() || uri[0] == 0 {
        return Err(ParseError);
    }

    let parsed = parse_internal(uri).ok_or(ParseError)?;

    Ok(Uri {
        scheme: slice_of(uri, parsed.scheme),
        userinfo: slice_of(uri, parsed.userinfo),
        host: slice_of(uri, parsed.host),
        ipv6addr: slice_of(uri, parsed.ipv6addr),
        zoneid: slice_of(uri, parsed.zoneid),
        port_str: slice_of(uri, parsed.port_str),
        port: parsed.port,
        path: slice_of(uri, parsed.path),
        query: slice_of(uri, parsed.query),
    })
}

trait ParamWriter {
    fn len(&self) -> usize;
    fn set_name(&mut self, index: usize, start: usize, end: Option<usize>);
    fn set_value(&mut self, index: usize, start: usize, end: Option<usize>);
}

fn split_query_impl<W: ParamWriter>(query: &[u8], writer: &mut W) -> Result<usize, i32> {
    if query.is_empty() || writer.len() == 0 {
        return Ok(0);
    }

    let mut index = 0usize;
    let mut name_start = Some(0usize);
    let mut value_start = None;

    writer.set_name(0, 0, None);

    let mut pos = 0usize;
    while pos < query.len() {
        match query[pos] {
            b'&' | b'#' => {
                let start = value_start.ok_or(-1)?;
                writer.set_value(index, start, Some(pos));

                if query[pos] == b'#' {
                    pos = query.len();
                } else if index + 1 < writer.len() {
                    index += 1;
                    name_start = Some(pos + 1);
                    writer.set_name(index, pos + 1, None);
                    value_start = None;
                } else {
                    return Err(-2);
                }
            }
            b'=' => {
                let start = name_start.ok_or(-1)?;
                if value_start.is_some() {
                    return Err(-1);
                }
                writer.set_name(index, start, Some(pos));
                value_start = Some(pos + 1);
                writer.set_value(index, pos + 1, None);
            }
            _ => {}
        }
        pos += 1;
    }

    let start = value_start.ok_or(-1)?;
    writer.set_value(index, start, Some(query.len()));

    Ok(index + 1)
}

struct SliceWriter<'q, 'p> {
    query: &'q [u8],
    params: &'p mut [Option<QueryParam<'q>>],
}

impl<'q, 'p> ParamWriter for SliceWriter<'q, 'p> {
    fn len(&self) -> usize {
        self.params.len()
    }

    fn set_name(&mut self, index: usize, start: usize, end: Option<usize>) {
        let end = end.unwrap_or(start);
        self.params[index] = Some(QueryParam {
            name: &self.query[start..end],
            value: &[],
        });
    }

    fn set_value(&mut self, index: usize, start: usize, end: Option<usize>) {
        let end = end.unwrap_or(start);
        let query = self.query;
        let param = self.params[index]
            .as_mut()
            .expect("parameter name must be set first");
        param.value = &query[start..end];
    }
}

/// Split a query string into name-value pairs.
///
/// `params` must have room for the number of pairs. The function returns the
/// number of filled entries. On error it returns `-1`, or `-2` when the query
/// has more pairs than `params` can hold.
pub fn split_query<'q>(
    query: &'q [u8],
    params: &mut [Option<QueryParam<'q>>],
) -> Result<usize, i32> {
    let mut writer = SliceWriter { query, params };
    split_query_impl(query, &mut writer)
}

struct CWriter {
    base: *const c_char,
    params: *mut QueryParamC,
    len: usize,
}

impl ParamWriter for CWriter {
    fn len(&self) -> usize {
        self.len
    }

    fn set_name(&mut self, index: usize, start: usize, end: Option<usize>) {
        unsafe {
            let param = self.params.add(index);
            (*param).name = self.base.add(start);
            if let Some(end) = end {
                (*param).name_len = (end - start) as u16;
            }
        }
    }

    fn set_value(&mut self, index: usize, start: usize, end: Option<usize>) {
        unsafe {
            let param = self.params.add(index);
            (*param).value = self.base.add(start);
            if let Some(end) = end {
                (*param).value_len = (end - start) as u16;
            }
        }
    }
}

unsafe fn fill_result(out: *mut UriParserResultC, base: *const c_char, parsed: &Parsed) {
    ptr::write_bytes(out, 0, 1);
    let out = &mut *out;

    if let Some((start, end)) = parsed.scheme {
        out.scheme = base.add(start);
        out.scheme_len = (end - start) as u16;
    }
    if let Some((start, end)) = parsed.userinfo {
        out.userinfo = base.add(start);
        out.userinfo_len = (end - start) as u16;
    }
    if let Some((start, end)) = parsed.host {
        out.host = base.add(start);
        out.host_len = (end - start) as u16;
    }
    if let Some((start, end)) = parsed.ipv6addr {
        out.ipv6addr = base.add(start);
        out.ipv6addr_len = (end - start) as u16;
    }
    if let Some((start, end)) = parsed.zoneid {
        out.zoneid = base.add(start);
        out.zoneid_len = (end - start) as u16;
    }
    if let Some((start, end)) = parsed.port_str {
        out.port_str = base.add(start);
        out.port_str_len = (end - start) as u16;
    }
    out.port = parsed.port;
    if let Some((start, end)) = parsed.path {
        out.path = base.add(start);
        out.path_len = (end - start) as u16;
    }
    if let Some((start, end)) = parsed.query {
        out.query = base.add(start);
        out.query_len = (end - start) as u16;
    }
}

unsafe fn zero_result(out: *mut UriParserResultC) {
    if !out.is_null() {
        ptr::write_bytes(out, 0, 1);
    }
}

unsafe fn process_bytes(
    result: *mut UriParserResultC,
    uri: *const c_char,
    uri_len: usize,
) -> c_int {
    if result.is_null() || uri.is_null() || uri_len == 0 {
        zero_result(result);
        return -1;
    }

    let bytes = slice::from_raw_parts(uri as *const u8, uri_len);
    if bytes[0] == 0 {
        zero_result(result);
        return -1;
    }

    match parse_internal(bytes) {
        Some(parsed) => {
            fill_result(result, uri, &parsed);
            0
        }
        None => {
            zero_result(result);
            -1
        }
    }
}

/// C API: check whether a URI reference is in absolute form.
///
/// # Safety
///
/// `uri` must be null, or point to at least `uri_len` readable bytes.
#[no_mangle]
pub unsafe extern "C" fn uri_parser_is_absolute(uri: *const c_char, uri_len: usize) -> bool {
    if uri.is_null() || uri_len == 0 {
        return false;
    }

    let bytes = slice::from_raw_parts(uri as *const u8, uri_len);
    is_absolute(bytes)
}

/// C API: check whether a zero-terminated URI reference is in absolute form.
///
/// # Safety
///
/// `uri` must be null, or point to a valid zero-terminated string.
#[no_mangle]
pub unsafe extern "C" fn uri_parser_is_absolute_string(uri: *const c_char) -> bool {
    if uri.is_null() {
        return false;
    }

    is_absolute(CStr::from_ptr(uri).to_bytes())
}

/// C API: parse a URI reference.
///
/// # Safety
///
/// `result` must be null, or point to writable memory for `UriParserResultC`.
/// `uri` must be null, or point to at least `uri_len` readable bytes.
#[no_mangle]
pub unsafe extern "C" fn uri_parser_process(
    result: *mut UriParserResultC,
    uri: *const c_char,
    uri_len: usize,
) -> c_int {
    process_bytes(result, uri, uri_len)
}

/// C API: parse a zero-terminated URI reference.
///
/// # Safety
///
/// `result` must be null, or point to writable memory for `UriParserResultC`.
/// `uri` must be null, or point to a valid zero-terminated string.
#[no_mangle]
pub unsafe extern "C" fn uri_parser_process_string(
    result: *mut UriParserResultC,
    uri: *const c_char,
) -> c_int {
    if result.is_null() || uri.is_null() {
        zero_result(result);
        return -1;
    }

    let bytes = CStr::from_ptr(uri).to_bytes();
    process_bytes(result, uri, bytes.len())
}

/// C API: split a query string into name-value pairs.
///
/// # Safety
///
/// `uri` must be null, or point to a valid `UriParserResultC`.
/// `params` must be null, or point to `params_len` writable entries.
/// The query pointer in `uri` must point to `query_len` readable bytes.
#[no_mangle]
pub unsafe extern "C" fn uri_parser_split_query(
    uri: *const UriParserResultC,
    params: *mut QueryParamC,
    params_len: usize,
) -> c_int {
    if uri.is_null() || params.is_null() {
        return -1;
    }

    let query_ptr = (*uri).query;
    let query_len = (*uri).query_len as usize;

    if query_ptr.is_null() || query_len == 0 || params_len == 0 {
        return 0;
    }

    let query = slice::from_raw_parts(query_ptr as *const u8, query_len);
    let mut writer = CWriter {
        base: query_ptr,
        params,
        len: params_len,
    };

    match split_query_impl(query, &mut writer) {
        Ok(parsed) => parsed as c_int,
        Err(error) => error,
    }
}
