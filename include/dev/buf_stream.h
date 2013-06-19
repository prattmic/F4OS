#ifndef DEV_BUF_STREAM_H_INCLUDED
#define DEV_BUF_STREAM_H_INCLUDED

/* Create a stream resource around of given buffer
 * of a given length.  Reading from this resource will
 * read until the NULL terminator is reached, or len
 * characters have been read.  Writing to this resource
 * will sequentially write the characters to the buffer,
 * until len-1 characters have been written, leaving a
 * a NULL terminator. */
rd_t open_buf_stream(char *buf, uint32_t len);

#endif
