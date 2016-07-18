#ifndef __MICRO_HTTP_MIDDLEWARE_H__
#define __MICRO_HTTP_MIDDLEWARE_H__

#include "server.h"

/* Route */
#ifndef MAX_HTTP_ROUTES
#define MAX_HTTP_ROUTES  10
#endif
#ifndef STATIC_FILE_FOLDER
#define STATIC_FILE_FOLDER "static/"
#endif

typedef HTTPREQ_CALLBACK SAF;

int AddRoute(HTTPMethod, char *, SAF);
void Dispatch(HTTPReqMessage *, HTTPResMessage *);

#endif
