/* Copyright (c) 2014, Fengping Bao <jamol@live.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef __Http1xResponse_H__
#define __Http1xResponse_H__

#include "kmdefs.h"
#include "httpdefs.h"
#include "HttpParserImpl.h"
#include "TcpConnection.h"
#include "Uri.h"
#include "util/kmobject.h"
#include "util/DestroyDetector.h"
#include "HttpResponseImpl.h"
#include "HttpMessage.h"
#include "EventLoopImpl.h"

KUMA_NS_BEGIN

class Http1xResponse : public KMObject, public HttpResponse::Impl, public DestroyDetector, public TcpConnection
{
public:
    Http1xResponse(const EventLoopPtr &loop, std::string ver);
    ~Http1xResponse();
    
    KMError setSslFlags(uint32_t ssl_flags) override;
    KMError attachFd(SOCKET_FD fd, const void* init_data, size_t init_len) override;
    KMError attachSocket(TcpSocket::Impl&& tcp, HttpParser::Impl&& parser, const void* init_data, size_t init_len) override;
    void addHeader(std::string name, std::string value) override;
    KMError sendResponse(int status_code, const std::string& desc, const std::string& ver) override;
    int sendData(const void* data, size_t len) override;
    void reset() override; // reset for connection reuse
    KMError close() override;
    
    const std::string& getMethod() const override { return http_parser_.getMethod(); }
    const std::string& getPath() const override { return http_parser_.getUrlPath(); }
    const std::string& getVersion() const override { return http_parser_.getVersion(); }
    const std::string& getParamValue(std::string name) const override {
        return http_parser_.getParamValue(std::move(name));
    }
    const std::string& getHeaderValue(std::string name) const override {
        return http_parser_.getHeaderValue(std::move(name));
    }
    void forEachHeader(HttpParser::Impl::EnumrateCallback&& cb) override {
        return http_parser_.forEachHeader(std::move(cb));
    }
    
protected:
    KMError handleInputData(uint8_t *src, size_t len) override;
    void onWrite() override;
    void onError(KMError err) override;
    
    // callbacks of HttpParser
    void onHttpData(void* data, size_t len);
    void onHttpEvent(HttpEvent ev);
    
    bool isVersion2() override { return false; }
    
protected:
    void checkHeaders() override;
    void buildResponse(int status_code, const std::string& desc, const std::string& ver);
    void cleanup();
    
protected:
    HttpParser::Impl        http_parser_;
    HttpMessage             http_message_;
    EventLoopToken          loop_token_;
};

KUMA_NS_END

#endif
