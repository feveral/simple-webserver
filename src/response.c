#include "response.h"
#include "utility.h"
#include "list.h"

Response *responseNew()
{
    Response *response = malloc(sizeof(Response));
    response->status = OK;
    response->headers = listNew();
    response->body = "";
    return response;
}
void responseAddHeader(Response *response, char *header)
{
    listAppendString(response->headers, header);
}
void responseSetStatus(Response *response, Status status)
{
    response->status = status;
}
void responseSetBody(Response *response, char *body)
{
    response->body = body;
}

// Example: "HTTP/1.0 200 OK\r\nHost: 140.114.234.148\r\n\r\ngoodgood"
char* responsePacket(Response *response)
{
    char *packet = "";
    if (response->status == OK) packet = "HTTP/1.0 200 OK\r\n";
    else if (response->status == MOVE_PERMANENTLY) packet = "HTTP/1.0 200 Move Permanently\r\n";
    else if (response->status == FORBIDDEN) packet = "HTTP/1.0 403 Forbidden\r\n";
    else if (response->status == NOT_FOUND) packet = "HTTP/1.0 404 Not Found\r\n";

    // TODO: add headers

    packet = concat(packet, "\r\n");
    packet = concat(packet, response->body);
    return packet;
}