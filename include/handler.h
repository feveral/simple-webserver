#ifndef HANDLER_H
#define HANDLER_H

#include "request.h"
#include "response.h"

typedef Response *(*Handler)(Request *);

Response *staticHandler(Request *req);
Response *directoryHandler(Request *req);
Response *cgiHandler(Request *req);

#endif