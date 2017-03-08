/* mbed Microcontroller Library
 * Copyright (c) 20015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __SPWFSA_INTERFACE_OS5_H__
#define __SPWFSA_INTERFACE_OS5_H__

#include "WiFiInterface.h"
#include "SPWFSA01.h"
#include "rtos/Semaphore.h"

#define SPWFSA_SOCKET_COUNT 8
#define SERVER_SOCKET_NO    9


 /** SPWFSAInterface5 class
  *  Implementation of the NetworkStack for the SPWF Device, only support mbed OS5
  */
class SPWFSAInterface5 : public NetworkStack, public WiFiInterface
{
public:
	/**
	 * @brief  SPWFSAInterface5 constructor
	 * @param  tx: Pin USART TX
	 *         rx: Pin USART RX
	 *         rst: reset pin for Spwf module
	 *         wkup: reset pin for Spwf module
	 *         debug : not used
	 * @retval none
	 */
	SPWFSAInterface5(PinName tx, PinName rx, bool debug);
	SPWFSAInterface5(PinName tx, PinName rx, PinName reset = PC_12, PinName wakeup = PC_8, bool debug = false);
	
	/**
	 * @brief  SPWFSAInterface5 destructor
	 * @param  none
	 * @retval none
	 */
	virtual ~SPWFSAInterface5();

	/* ===========================================================
	 * Implementations of WiFiInterface
	 */

	 /** Set the WiFi network credentials
	  *
	  *  @param ssid      Name of the network to connect to
	  *  @param pass      Security passphrase to connect to the network
	  *  @param security  Type of encryption for connection
	  *                   (defaults to NSAPI_SECURITY_NONE)
	  *  @return          0 on success, or error code on failure
	  */
	virtual nsapi_error_t set_credentials(const char *ssid, const char *pass, nsapi_security_t security = NSAPI_SECURITY_NONE);

	/** Set the WiFi network channel
	 *
	 *  @param channel   Channel on which the connection is to be made, or 0 for any (Default: 0)
	 *  @return          0 on success, or error code on failure
	 */
	virtual nsapi_error_t set_channel(uint8_t channel);

	/** Gets the current radio signal strength for active connection
	 *
	 *  @return         Connection strength in dBm (negative value),
	 *                  or 0 if measurement impossible
	 */
	virtual int8_t get_rssi();

	/** Start the interface
	 *
	 *  Attempts to connect to a WiFi network.
	 *
	 *  @param ssid      Name of the network to connect to
	 *  @param pass      Security passphrase to connect to the network
	 *  @param security  Type of encryption for connection (Default: NSAPI_SECURITY_NONE)
	 *  @param channel   Channel on which the connection is to be made, or 0 for any (Default: 0)
	 *  @return          0 on success, or error code on failure
	 */
	virtual nsapi_error_t connect(const char *ssid, const char *pass, nsapi_security_t security = NSAPI_SECURITY_NONE, uint8_t channel = 0);

	/** Start the interface
	 *
	 *  Attempts to start AP mode.
	 *
	 *  @return         0 on success, negative error code on failure
	 */
	virtual nsapi_error_t connect();

	/** Stop the interface
	 *
	 *  @return         0 on success, or error code on failure
	 */
	virtual nsapi_error_t disconnect();

	/** Scan for available networks
	 *
	 *  This function will block. If the @a count is 0, function will only return count of available networks, so that
	 *  user can allocated necessary memory. If the @count is grater than 0 and the @a ap is not NULL it'll be populated
	 *  with discovered networks up to value of @a count.
	 *
	 *  @param  ap       Pointer to allocated array to store discovered AP
	 *  @param  count    Size of allocated @a res array, or 0 to only count available AP
	 *  @return          Number of entries in @a, or if @a count was 0 number of available networks,
	 *                   negative on error see @a nsapi_error
	 */
	virtual nsapi_size_or_error_t scan(WiFiAccessPoint *res, nsapi_size_t count);

	/* ===========================================================
	 * Implementations of NetworkInterface
	 */

	/** Get the local MAC address
	 *
	 *  Provided MAC address is intended for info or debug purposes and
	 *  may not be provided if the underlying network interface does not
	 *  provide a MAC address
	 *
	 *  @return         Null-terminated representation of the local MAC address
	 *                  or null if no MAC address is available
	 */
	virtual const char *get_mac_address();

	/** Get the local IP address
	 *
	 *  @return         Null-terminated representation of the local IP address
	 *                  or null if no IP address has been recieved
	 */
	virtual const char *get_ip_address();

	/** Get the local network mask
	 *
	 *  @return         Null-terminated representation of the local network mask
	 *                  or null if no network mask has been recieved
	 */
	virtual const char *get_netmask();

	/** Get the local gateway
	 *
	 *  @return         Null-terminated representation of the local gateway
	 *                  or null if no network mask has been recieved
	 */
	virtual const char *get_gateway();

	/** Set a static IP address
	 *
	 *  Configures this network interface to use a static IP address.
	 *  Implicitly disables DHCP, which can be enabled in set_dhcp.
	 *  Requires that the network is disconnected.
	 *
	 *  @param address  Null-terminated representation of the local IP address
	 *  @param netmask  Null-terminated representation of the local network mask
	 *  @param gateway  Null-terminated representation of the local gateway
	 *  @return         0 on success, negative error code on failure
	 */
	virtual nsapi_error_t set_network(const char *ip_address, const char *netmask, const char *gateway);

	/** Enable or disable DHCP on the network
	 *
	 *  Enables DHCP on connecting the network. Defaults to enabled unless
	 *  a static IP address has been assigned. Requires that the network is
	 *  disconnected.
	 *
	 *  @param dhcp     True to enable DHCP
	 *  @return         0 on success, negative error code on failure
	 */
	virtual nsapi_error_t set_dhcp(bool dhcp);

	/** Translates a hostname to an IP address with specific version
	 *
	 *  The hostname may be either a domain name or an IP address. If the
	 *  hostname is an IP address, no network transactions will be performed.
	 *
	 *  If no stack-specific DNS resolution is provided, the hostname
	 *  will be resolve using a UDP socket on the stack.
	 *
	 *  @param address  Destination for the host SocketAddress
	 *  @param host     Hostname to resolve
	 *  @param version  IP version of address to resolve, NSAPI_UNSPEC indicates
	 *                  version is chosen by the stack (defaults to NSAPI_UNSPEC)
	 *  @return         0 on success, negative error code on failure
	 */
	virtual nsapi_error_t gethostbyname(const char *host, SocketAddress *address, nsapi_version_t version = NSAPI_UNSPEC);

	/** Add a domain name server to list of servers to query
	 *
	 *  @param addr     Destination for the host address
	 *  @return         0 on success, negative error code on failure
	 */
	virtual nsapi_error_t add_dns_server(const SocketAddress &address);

	/* ===========================================================
	 * Implementations of NetworkStack
	 */

	/*  Set stack options
	 *
	 *  setstackopt allows an application to pass stack-specific options
	 *  to the underlying stack using stack-specific level and option names,
	 *  or to request generic options using levels from nsapi_stack_level_t.
	 *
	 *  For unsupported options, NSAPI_ERROR_UNSUPPORTED is returned
	 *  and the stack is unmodified.
	 *
	 *  @param level    Stack-specific protocol level or nsapi_stack_level_t
	 *  @param optname  Level-specific option name
	 *  @param optval   Option value
	 *  @param optlen   Length of the option value
	 *  @return         0 on success, negative error code on failure
	 */
	virtual nsapi_error_t setstackopt(int level, int optname, const void *optval, unsigned optlen);

	/*  Get stack options
	 *
	 *  getstackopt allows an application to retrieve stack-specific options
	 *  to the underlying stack using stack-specific level and option names,
	 *  or to request generic options using levels from nsapi_stack_level_t.
	 *
	 *  @param level    Stack-specific protocol level or nsapi_stack_level_t
	 *  @param optname  Level-specific option name
	 *  @param optval   Destination for option value
	 *  @param optlen   Length of the option value
	 *  @return         0 on success, negative error code on failure
	 */
	virtual nsapi_error_t getstackopt(int level, int optname, void *optval, unsigned *optlen);
	
protected:
	friend class Socket;
	friend class UDPSocket;
	friend class TCPSocket;
	friend class TCPServer;

	/* ===========================================================
	* Implementations of NetworkStack
	*/

	/** Opens a socket
	 *
	 *  Creates a network socket and stores it in the specified handle.
	 *  The handle must be passed to following calls on the socket.
	 *
	 *  A stack may have a finite number of sockets, in this case
	 *  NSAPI_ERROR_NO_SOCKET is returned if no socket is available.
	 *
	 *  @param handle   Destination for the handle to a newly created socket
	 *  @param proto    Protocol of socket to open, NSAPI_TCP or NSAPI_UDP
	 *  @return         0 on success, negative error code on failure
	 */
	virtual nsapi_error_t socket_open(nsapi_socket_t *handle, nsapi_protocol_t proto);

	/** Close the socket
	 *
	 *  Closes any open connection and deallocates any memory associated
	 *  with the socket.
	 *
	 *  @param handle   Socket handle
	 *  @return         0 on success, negative error code on failure
	 */
	virtual nsapi_error_t socket_close(nsapi_socket_t handle);

	/** Bind a specific address to a socket
	 *
	 *  Binding a socket specifies the address and port on which to recieve
	 *  data. If the IP address is zeroed, only the port is bound.
	 *
	 *  @param handle   Socket handle
	 *  @param address  Local address to bind
	 *  @return         0 on success, negative error code on failure.
	 */
	virtual nsapi_error_t socket_bind(nsapi_socket_t handle, const SocketAddress &address);

	/** Listen for connections on a TCP socket
	 *
	 *  Marks the socket as a passive socket that can be used to accept
	 *  incoming connections.
	 *
	 *  @param handle   Socket handle
	 *  @param backlog  Number of pending connections that can be queued
	 *                  simultaneously
	 *  @return         0 on success, negative error code on failure
	 */
	virtual nsapi_error_t socket_listen(nsapi_socket_t handle, int backlog);

	/** Connects TCP socket to a remote host
	 *
	 *  Initiates a connection to a remote server specified by the
	 *  indicated address.
	 *
	 *  @param handle   Socket handle
	 *  @param address  The SocketAddress of the remote host
	 *  @return         0 on success, negative error code on failure
	 */
	virtual nsapi_error_t socket_connect(nsapi_socket_t handle, const SocketAddress &address);

	/** Accepts a connection on a TCP socket
	 *
	 *  The server socket must be bound and set to listen for connections.
	 *  On a new connection, creates a network socket and stores it in the
	 *  specified handle. The handle must be passed to following calls on
	 *  the socket.
	 *
	 *  A stack may have a finite number of sockets, in this case
	 *  NSAPI_ERROR_NO_SOCKET is returned if no socket is available.
	 *
	 *  This call is non-blocking. If accept would block,
	 *  NSAPI_ERROR_WOULD_BLOCK is returned immediately.
	 *
	 *  @param server   Socket handle to server to accept from
	 *  @param handle   Destination for a handle to the newly created socket
	 *  @param address  Destination for the remote address or NULL
	 *  @return         0 on success, negative error code on failure
	 */
	virtual nsapi_error_t socket_accept(nsapi_socket_t server, nsapi_socket_t *handle, SocketAddress *address = 0);

	/** Send data over a TCP socket
	 *
	 *  The socket must be connected to a remote host. Returns the number of
	 *  bytes sent from the buffer.
	 *
	 *  This call is non-blocking. If send would block,
	 *  NSAPI_ERROR_WOULD_BLOCK is returned immediately.
	 *
	 *  @param handle   Socket handle
	 *  @param data     Buffer of data to send to the host
	 *  @param size     Size of the buffer in bytes
	 *  @return         Number of sent bytes on success, negative error
	 *                  code on failure
	 */
	virtual nsapi_size_or_error_t socket_send(nsapi_socket_t handle, const void *data, nsapi_size_t size);

	/** Receive data over a TCP socket
	 *
	 *  The socket must be connected to a remote host. Returns the number of
	 *  bytes received into the buffer.
	 *
	 *  This call is non-blocking. If recv would block,
	 *  NSAPI_ERROR_WOULD_BLOCK is returned immediately.
	 *
	 *  @param handle   Socket handle
	 *  @param data     Destination buffer for data received from the host
	 *  @param size     Size of the buffer in bytes
	 *  @return         Number of received bytes on success, negative error
	 *                  code on failure
	 */
	virtual nsapi_size_or_error_t socket_recv(nsapi_socket_t handle, void *data, nsapi_size_t size);

	/** Send a packet over a UDP socket
	 *
	 *  Sends data to the specified address. Returns the number of bytes
	 *  sent from the buffer.
	 *
	 *  This call is non-blocking. If sendto would block,
	 *  NSAPI_ERROR_WOULD_BLOCK is returned immediately.
	 *
	 *  @param handle   Socket handle
	 *  @param address  The SocketAddress of the remote host
	 *  @param data     Buffer of data to send to the host
	 *  @param size     Size of the buffer in bytes
	 *  @return         Number of sent bytes on success, negative error
	 *                  code on failure
	 */
	virtual nsapi_size_or_error_t socket_sendto(nsapi_socket_t handle, const SocketAddress &address, const void *data, nsapi_size_t size);

	/** Receive a packet over a UDP socket
	 *
	 *  Receives data and stores the source address in address if address
	 *  is not NULL. Returns the number of bytes received into the buffer.
	 *
	 *  This call is non-blocking. If recvfrom would block,
	 *  NSAPI_ERROR_WOULD_BLOCK is returned immediately.
	 *
	 *  @param handle   Socket handle
	 *  @param address  Destination for the source address or NULL
	 *  @param data     Destination buffer for data received from the host
	 *  @param size     Size of the buffer in bytes
	 *  @return         Number of received bytes on success, negative error
	 *                  code on failure
	 */
	virtual nsapi_size_or_error_t socket_recvfrom(nsapi_socket_t handle, SocketAddress *address, void *buffer, nsapi_size_t size);

	/** Register a callback on state change of the socket
	 *
	 *  The specified callback will be called on state changes such as when
	 *  the socket can recv/send/accept successfully and on when an error
	 *  occurs. The callback may also be called spuriously without reason.
	 *
	 *  The callback may be called in an interrupt context and should not
	 *  perform expensive operations such as recv/send calls.
	 *
	 *  @param handle   Socket handle
	 *  @param callback Function to call on state change
	 *  @param data     Argument to pass to callback
	 */
	virtual void socket_attach(nsapi_socket_t handle, void(*callback)(void *), void *data);

	/*  Set stack-specific socket options
	 *
	 *  The setsockopt allow an application to pass stack-specific hints
	 *  to the underlying stack. For unsupported options,
	 *  NSAPI_ERROR_UNSUPPORTED is returned and the socket is unmodified.
	 *
	 *  @param handle   Socket handle
	 *  @param level    Stack-specific protocol level
	 *  @param optname  Stack-specific option identifier
	 *  @param optval   Option value
	 *  @param optlen   Length of the option value
	 *  @return         0 on success, negative error code on failure
	 */
	virtual nsapi_error_t setsockopt(nsapi_socket_t handle, int level, int optname, const void *optval, unsigned optlen);

	/*  Get stack-specific socket options
	 *
	 *  The getstackopt allow an application to retrieve stack-specific hints
	 *  from the underlying stack. For unsupported options,
	 *  NSAPI_ERROR_UNSUPPORTED is returned and optval is unmodified.
	 *
	 *  @param handle   Socket handle
	 *  @param level    Stack-specific protocol level
	 *  @param optname  Stack-specific option identifier
	 *  @param optval   Destination for option value
	 *  @param optlen   Length of the option value
	 *  @return         0 on success, negative error code on failure
	 */
	virtual nsapi_error_t getsockopt(nsapi_socket_t handle, int level, int optname, void *optval, unsigned *optlen);
	
	virtual NetworkStack *get_stack()
	{
		return this;
	}

private:
	int init(void);

	SPWFSA01 _spwf;
	nsapi_security_t ap_sec;
	char ap_ssid[33]; /* 32 is what 802.11 defines as longest possible name; +1 for the \0 */
	char ap_pass[64]; /* The longest allowed passphrase */
	u_int8_t ap_channel;

	bool _ids[SPWFSA_SOCKET_COUNT];
	bool isListening;
	bool isInitialized;
	volatile bool isSent;
	rtos::Semaphore _read_sem;
};

#endif	// __SPWFSA_INTERFACE_OS5_H__
