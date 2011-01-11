#ifndef SOCKADDR_UTIL_H
#define SOCKADDR_UTIL_H

#include <stdbool.h>
#include <netinet/in.h>
//#include <netdb.h>

#ifdef __cplusplus
extern "C" {
#endif


static const uint32_t SOCKADDR_MAX_STRLEN = INET6_ADDRSTRLEN + 8; //port, :, [];


/*
 *  Initialize a sockadd_in (IPv4) as any ("0.0.0.0:0").
 *  Remember to putaside enough memeory. (sockaddr_storage)
 */
void sockaddr_initAsIPv4Any(struct sockaddr_in * sa);



void sockaddr_initAsIPv6Any(struct sockaddr_in6 * sa);

/*
 *  Initialize a sockaddr from string.
 *  Remember to putaside enough memeory. (sockaddr_storage)
 */
bool sockaddr_initFromString(struct sockaddr *sa,
                             const char *addr_str);


bool sockaddr_initFromIPv4String(struct sockaddr_in *sa,
                                 const char *addr_str);

bool sockaddr_initFromIPv6String(struct sockaddr_in6 *sa,
                                 const char *addr_str);


/*
 * Initialize IPv4 sockaddr from a int addr and a int port.
 * (Use htons and htonl if your data is stored in host format)
 *
 */
bool sockaddr_initFromIPv4Int(struct sockaddr_in *sa,
                              uint32_t addr, 
                              uint16_t port);


/*
 * Initialize IPv6 sockaddr from a int addr and a int port.
 * (Use htons and htonl if your data is stored in host format)
 *
 */
bool sockaddr_initFromIPv6Int(struct sockaddr_in6 *sin,
                              uint8_t ipaddr[16], 
                              uint16_t port);


/*
 *  Checks if the address part is the same.
 *  No cheking of ports or transport protocol 
 */

bool sockaddr_sameAddr(const struct sockaddr * a, 
                       const struct sockaddr * b);


/*
 *  Check if the port is the same.
 *  
 */
bool sockaddr_samePort(const struct sockaddr * a, 
                       const struct sockaddr * b);

/*
 * Check if two sockaddr are alike
 * (IP proto, port and address)
 *
 */
bool sockaddr_alike(const struct sockaddr * a, 
                    const struct sockaddr * b);

/*
 *   Checks if a sockaddr is reasonably initialized
 *
 */
bool sockaddr_isSet(const struct sockaddr * sa);

/*
 * Cheks if a sockaddr has the address of 'any'
 *  
 */
bool sockaddr_isAddrAny(const struct sockaddr * sa);


const char *sockaddr_toString( const struct sockaddr *sa,
                               char *dest,
                               size_t destlen,
                               bool addport);

void sockaddr_copy(struct sockaddr * dst, 
                   const struct sockaddr * src);


void sockaddr_setPort(struct sockaddr * sa, 
                      uint16_t port);

#ifdef __cplusplus
}
#endif

#endif
