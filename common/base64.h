#pragma once

char * base64_encode(const void * bindata, int binlength);
void * base64_decode(const char * base64, size_t * pbinSize);
