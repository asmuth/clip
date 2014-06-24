/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2009-2014 Christian Parpart
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * This file is based on "HTTPParser.h" from the x0 web server project
 * by Christian Parpart <trapni@gmail.com> (http://www.xzero.io/).
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef sw_x0_message_processor_h
#define sw_x0_message_processor_h (1)

#include <system_error>
#include <functional>
#include <string>
#include <cctype>
#include <assert.h>

namespace fnordmetric {
namespace web {

/**
 * \class HTTPParser
 * \brief implements an HTTP/1.1 (request/response) message parser and processor
 */
class HTTPParser {
public:
    //! defines whether to parse HTTP requests, responses, or plain messages.
    enum ParseMode { // {{{
        //! the message to parse is an HTTP request.
        REQUEST,
        //! the message to parse is an HTTP response.
        RESPONSE,
        //! the message to parse does not contain either an HTTP request-line nor response status-line but headers and a body.
        MESSAGE
    }; // }}}

    //! defines list of states the parser is in while processing the message.
    enum State { // {{{
        // artificial
        PROTOCOL_ERROR = 1,
        MESSAGE_BEGIN,

        // Request-Line
        REQUEST_LINE_BEGIN = 100,
        REQUEST_METHOD,
        REQUEST_ENTITY_BEGIN,
        REQUEST_ENTITY,
        REQUEST_PROTOCOL_BEGIN,
        REQUEST_PROTOCOL_T1,
        REQUEST_PROTOCOL_T2,
        REQUEST_PROTOCOL_P,
        REQUEST_PROTOCOL_SLASH,
        REQUEST_PROTOCOL_VERSION_MAJOR,
        REQUEST_PROTOCOL_VERSION_MINOR,
        REQUEST_LINE_LF,

        // Status-Line
        STATUS_LINE_BEGIN = 150,
        STATUS_PROTOCOL_BEGIN,
        STATUS_PROTOCOL_T1,
        STATUS_PROTOCOL_T2,
        STATUS_PROTOCOL_P,
        STATUS_PROTOCOL_SLASH,
        STATUS_PROTOCOL_VERSION_MAJOR,
        STATUS_PROTOCOL_VERSION_MINOR,
        STATUS_CODE_BEGIN,
        STATUS_CODE,
        STATUS_MESSAGE_BEGIN,
        STATUS_MESSAGE,
        STATUS_MESSAGE_LF,

        // message-headers
        HEADER_NAME_BEGIN = 200,
        HEADER_NAME,
        HEADER_COLON,
        HEADER_VALUE_BEGIN,
        HEADER_VALUE,
        HEADER_VALUE_LF,
        HEADER_VALUE_END,
        HEADER_END_LF,

        // LWS ::= [CR LF] 1*(SP | HT)
        LWS_BEGIN = 300,
        LWS_LF,
        LWS_SP_HT_BEGIN,
        LWS_SP_HT,

        // message-content
        CONTENT_BEGIN = 400,
        CONTENT,
        CONTENT_ENDLESS = 405,
        CONTENT_CHUNK_SIZE_BEGIN = 410,
        CONTENT_CHUNK_SIZE,
        CONTENT_CHUNK_LF1,
        CONTENT_CHUNK_BODY,
        CONTENT_CHUNK_LF2,
        CONTENT_CHUNK_CR3,
        CONTENT_CHUNK_LF3
    }; // }}}

    class BufferRef {
    public:
      BufferRef() : data_(nullptr), size_(0) {}
      BufferRef(char* data, size_t size) : data_(data), size_(size) {}
      BufferRef(const BufferRef& copy) : data_(copy.data_), size_(copy.size_) {}
      inline void clear() { size_ = 0; }
      inline size_t size() const { return size_; }
      inline char* data() const { return data_; }
      inline bool empty() const { return size_ == 0; }
      inline char* cbegin() const { return data_; }
      inline char* cend() const { return data_ + size_; }
      inline void shr(ssize_t value = 1) { size_ += value; }
      int toInt() const;
      BufferRef ref(size_t offset, size_t count) const;
      BufferRef ref(size_t offset) const;
      static bool iequals(const BufferRef& a, const BufferRef& b);
      static bool iequals(const BufferRef& a, const std::string& b);
    protected:
      char* data_;
      size_t size_;
    };

    bool onMessageBegin(
        const BufferRef& method,
        const BufferRef& entity,
        int versionMajor,
        int versionMinor);

    bool onMessageBegin(
        int versionMajor,
        int versionMinor,
        int code,
        const BufferRef& text);

    bool onMessageBegin();
    bool onMessageHeader(const BufferRef& name, const BufferRef& value);
    bool onMessageHeaderEnd();
    bool onMessageContent(const BufferRef& chunk);
    bool onMessageEnd();
    void onProtocolError(const BufferRef& chunk, size_t offset);

    bool isProcessingHeader() const;
    bool isProcessingBody() const;
    bool isContentExpected() const;

    explicit HTTPParser(ParseMode mode);
    virtual ~HTTPParser() {}

    State state() const;
    const char *state_str() const;


    ssize_t contentLength() const;
    bool isChunked() const { return chunked_; }

    virtual void reset();

private:
    size_t parseFragment(const BufferRef& chunk, size_t* nparsed = nullptr);
    static inline bool isChar(char value);
    static inline bool isControl(char value);
    static inline bool isSeparator(char value);
    static inline bool isToken(char value);
    static inline bool isText(char value);

    // lexer constants
    enum {
        CR = 0x0D,
        LF = 0x0A,
        SP = 0x20,
        HT = 0x09
    };

    ParseMode mode_;
    State state_;       //!< the current parser/processing state

    // implicit LWS handling
    State lwsNext_;     //!< state to apply on successfull LWS
    State lwsNull_;     //!< state to apply on (CR LF) but no 1*(SP | HT)

    // request-line
    BufferRef method_;      //!< HTTP request method
    BufferRef entity_;      //!< HTTP request entity

    int versionMajor_;    //!< HTTP request/response version major
    int versionMinor_;    //!< HTTP request/response version minor

    // status-line
    int code_;        //!< response status code
    BufferRef message_;     //!< response status message

    // current parsed header
    BufferRef name_;
    BufferRef value_;

    // body
    bool chunked_;      //!< whether or not request content is chunked encoded
    ssize_t contentLength_; //!< content length of whole content or current chunk
};

}
}


namespace fnordmetric {
namespace web {

/*! represents the current parser-state this HTTP message processor is in.
 */
inline enum HTTPParser::State HTTPParser::state() const
{
    return state_;
}

/*! represents the remaining length of the content or chunk.
 *
 * If the message body is fixed-sized, that is, if a "Content-Length" message-header
 * was given, then this value represents the remaining size in bytes until
 * the content has been fully processed.
 *
 * If the message body is chunked transfer-encoded, that is, if the "Transfer-Encoding"
 * message-header is set to "chunked", this value represents the remaining size
 * in bytes until the current chunk has been fully processed.
 *
 * A value of 0 means that there is no (further) content to process
 * (e.g. Content-Length of 0 or a already fully processed content body).
 *
 * Otherwise the value is -1, meaning, that we do not or do not yet know
 * the length to be processed. (HTTP/1.0 reply messages e.g. do not
 * contain a hint about the content-length.
 */
inline ssize_t HTTPParser::contentLength() const {
    return contentLength_;
}

inline bool HTTPParser::isProcessingHeader() const {
    // XXX should we include request-line and status-line here, too?
    switch (state_) {
        case HEADER_NAME_BEGIN:
        case HEADER_NAME:
        case HEADER_COLON:
        case HEADER_VALUE_BEGIN:
        case HEADER_VALUE:
        case HEADER_VALUE_LF:
        case HEADER_VALUE_END:
        case HEADER_END_LF:
            return true;
        default:
            return false;
    }
}

inline bool HTTPParser::isProcessingBody() const {
    switch (state_) {
        case CONTENT_BEGIN:
        case CONTENT:
        case CONTENT_ENDLESS:
        case CONTENT_CHUNK_SIZE_BEGIN:
        case CONTENT_CHUNK_SIZE:
        case CONTENT_CHUNK_LF1:
        case CONTENT_CHUNK_BODY:
        case CONTENT_CHUNK_LF2:
        case CONTENT_CHUNK_CR3:
        case CONTENT_CHUNK_LF3:
            return true;
        default:
            return false;
    }
}

inline bool HTTPParser::isContentExpected() const {
    return contentLength_ > 0
        || chunked_
        || (contentLength_ < 0 && mode_ != REQUEST);
}

}
}

#endif
