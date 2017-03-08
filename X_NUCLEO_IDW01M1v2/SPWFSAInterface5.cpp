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

#include "SPWFSAInterface5.h"

// Various timeouts for different SPWF operations
#define SPWF_CONNECT_TIMEOUT 20000
#define SPWF_SEND_TIMEOUT    500
#define SPWF_RECV_TIMEOUT    500
#define SPWF_MISC_TIMEOUT    15000

/** spwf_socket class
 *  Implementation of SPWF socket structure
 */
struct spwf_socket {
	int id;
	int server_port;
	nsapi_protocol_t proto;
	bool connected;
	bool secure;
};

SPWFSAInterface5::SPWFSAInterface5(PinName tx, PinName rx, bool debug)
	: _spwf(tx, rx, PC_12, PC_8, debug)
{
	memset(_ids, 0, sizeof(_ids));
	isInitialized = false;
	isListening = false;
	isSent = false;

	memset(ap_ssid, 0, sizeof(ap_ssid));
	memset(ap_pass, 0, sizeof(ap_pass));
	ap_sec = NSAPI_SECURITY_NONE;
	ap_channel = 0;
}

SPWFSAInterface5::SPWFSAInterface5(PinName tx, PinName rx, PinName reset, PinName wakeup, bool debug)
	: _spwf(tx, rx, reset, wakeup, debug)
{
	memset(_ids, 0, sizeof(_ids));
	isInitialized = false;
	isListening = false;
}

SPWFSAInterface5::~SPWFSAInterface5()
{
}

int SPWFSAInterface5::set_credentials(const char *ssid, const char *pass, nsapi_security_t security)
{
	memset(ap_ssid, 0, sizeof(ap_ssid));
	strncpy(ap_ssid, ssid, sizeof(ap_ssid));

	memset(ap_pass, 0, sizeof(ap_pass));
	strncpy(ap_pass, pass, sizeof(ap_pass));

	ap_sec = security;

	return 0;
}

int SPWFSAInterface5::set_channel(uint8_t channel)
{
	ap_channel = channel;
	return 0;
}

int8_t SPWFSAInterface5::get_rssi()
{
	return _spwf.getRSSI();
}

int SPWFSAInterface5::connect(const char *ap, const char *pass_phrase, nsapi_security_t security, uint8_t channel)
{
	int mode;

	//initialize the device before connecting
	if (!isInitialized)
	{
		if (!init())
			return NSAPI_ERROR_DEVICE_ERROR;
	}

	switch (security)
	{
	case NSAPI_SECURITY_NONE:
		mode = 0;
		pass_phrase = NULL;
		break;
	case NSAPI_SECURITY_WEP:
		mode = 1;
		break;
	case NSAPI_SECURITY_WPA:
	case NSAPI_SECURITY_WPA2:
		mode = 2;
		break;
	default:
		mode = 2;
		break;
	}
	if (_spwf.connect((char*)ap, (char*)pass_phrase, mode)) {
		return NSAPI_ERROR_OK;
	}
	return NSAPI_ERROR_NO_CONNECTION;
}

int SPWFSAInterface5::connect()
{
	_spwf.setTimeout(SPWF_CONNECT_TIMEOUT);

	// Startup AP mode
	if (!_spwf.startup(3)) {
		return NSAPI_ERROR_DEVICE_ERROR;
	}

	if (!_spwf.dhcp(1)) {
		return NSAPI_ERROR_DHCP_FAILURE;
	}
	
	return NSAPI_ERROR_OK;
}

int SPWFSAInterface5::disconnect()
{
	return (_spwf.disconnect());
}

nsapi_size_or_error_t SPWFSAInterface5::scan(WiFiAccessPoint *res, unsigned count)
{
	return _spwf.scan(res, count);
}

const char *SPWFSAInterface5::get_mac_address()
{
	return _spwf.getMACAddress();
}

const char *SPWFSAInterface5::get_ip_address()
{
	return _spwf.getIPAddress();
}

const char *SPWFSAInterface5::get_netmask()
{
	return _spwf.getNetmask();
}

const char *SPWFSAInterface5::get_gateway()
{
	return _spwf.getGateway();
}

nsapi_error_t SPWFSAInterface5::set_network(const char *ip_address, const char *netmask, const char *gateway)
{
	return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_error_t SPWFSAInterface5::set_dhcp(bool dhcp)
{
	return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_error_t SPWFSAInterface5::gethostbyname(const char *host, SocketAddress *address, nsapi_version_t version)
{
	return NetworkStack::gethostbyname(host, address, version);
}

nsapi_error_t SPWFSAInterface5::add_dns_server(const SocketAddress &address)
{
	return NetworkStack::add_dns_server(address);
}

nsapi_error_t SPWFSAInterface5::setstackopt(int level, int optname, const void *optval, unsigned optlen)
{
	return NSAPI_ERROR_UNSUPPORTED;
}

nsapi_error_t SPWFSAInterface5::getstackopt(int level, int optname, void *optval, unsigned *optlen)
{
	return NSAPI_ERROR_UNSUPPORTED;
}

int SPWFSAInterface5::socket_open(void **handle, nsapi_protocol_t proto)
{
	int id = -1;

	struct spwf_socket *socket = new struct spwf_socket;
	if (!socket) {
		return NSAPI_ERROR_NO_SOCKET;
	}

	socket->id = id;
	socket->server_port = id;
	socket->proto = proto;
	socket->connected = false;
	socket->secure = false;
	*handle = socket;
	return 0;
}

int SPWFSAInterface5::socket_close(void *handle)
{
	struct spwf_socket *socket = (struct spwf_socket *)handle;
	int err = 0;

	if (socket->id != -1)
	{
		if (_spwf.close(socket->id)) {
			if (socket->id == SERVER_SOCKET_NO)
				isListening = false;
			else
				_ids[socket->id] = false;
		}
		else err = NSAPI_ERROR_DEVICE_ERROR;
	}

	delete socket;
	return err;
}

int SPWFSAInterface5::socket_bind(void *handle, const SocketAddress &address)
{
	struct spwf_socket *socket = (struct spwf_socket *)handle;
	socket->server_port = address.get_port();
	return 0;
}

int SPWFSAInterface5::socket_listen(void *handle, int backlog)
{
	return NSAPI_ERROR_UNSUPPORTED;
}

int SPWFSAInterface5::socket_connect(void *handle, const SocketAddress &addr)
{
	//    int sock_id = 99;
	int sock_id = -1;
	const char* proto;
	struct spwf_socket *socket = (struct spwf_socket *)handle;
	if (socket->secure)
		proto = "s";
	else
		proto = (socket->proto == NSAPI_UDP) ? "u" : "t";

	if (!_spwf.open(proto, &sock_id, addr.get_ip_address(), addr.get_port())) {//sock ID is allocated NOW
		return NSAPI_ERROR_DEVICE_ERROR;
	}

	//TODO: Maintain a socket table to map socket ID to host & port
	//TODO: lookup on client table to see if already socket is allocated to same host/port
	//multimap <char *, vector <uint16_t> > ::iterator i = c_table.find((char*)ip);

	if (sock_id <= SPWFSA_SOCKET_COUNT)
	{
		socket->id = sock_id;//the socket ID of this Socket instance
		_ids[socket->id] = true;
		socket->connected = true;
	}
	else
		return NSAPI_ERROR_NO_SOCKET;

	return 0;
}

int SPWFSAInterface5::socket_accept(void *server, void **socket, SocketAddress *addr)
{
	return NSAPI_ERROR_UNSUPPORTED;
}

int SPWFSAInterface5::socket_send(void *handle, const void *data, unsigned size)
{
	struct spwf_socket *socket = (struct spwf_socket *)handle;
	//int err;

	/*if(socket->id==SERVER_SOCKET_NO)
		{
			if(socket->server_port==-1 || !isListening)
				return NSAPI_ERROR_NO_SOCKET; //server socket not bound or not listening

			err = _spwf.socket_server_write((uint16_t)size, (char*)data);
		}
	else
		{
			err = _spwf.send(socket->id, (char*)data, (uint32_t)size);
		}*/
	if (!_spwf.send(socket->id, (char*)data, (uint32_t)size)) {
		return NSAPI_ERROR_DEVICE_ERROR;
	}
	isSent = true;
	return size;
}

int SPWFSAInterface5::socket_recv(void *handle, void *data, unsigned size)
{
	struct spwf_socket *socket = (struct spwf_socket *)handle;
	int32_t recv;

    for (int i = 0; i < 10; i++) // To resolve busy issue
    {
        //CHECK:Receive for both Client and Server Sockets same?
        recv = _spwf.recv(socket->id, (char*)data, (uint32_t)size);
        if (recv >= 0)
        {
			printf("SPWFSAInterface5 socket_recv %d\r\n", recv);
            return recv;
        }
        else if (recv == -1) 
        {
            wait_ms(300);
        }
        else
        {
            // Error
            break;
        }
    }
	printf("!!!!!!!!!!!!!!!!!!!!!recv %d\r\n", recv);
	if (recv == -1)
		recv = NSAPI_ERROR_WOULD_BLOCK;//send this if we want to block call (else timeout will happen)
	else if (recv == -2)
		recv = NSAPI_ERROR_DEVICE_ERROR;
	else if (recv == -3)
		recv = NSAPI_ERROR_NO_CONNECTION;
	isSent = false;
	return recv;
}

int SPWFSAInterface5::socket_sendto(void *handle, const SocketAddress &addr, const void *data, unsigned size)
{
	struct spwf_socket *socket = (struct spwf_socket *)handle;
	if (!socket->connected) {
		int err = socket_connect(socket, addr);
		if (err < 0) {
			return err;
		}
	}

	return socket_send(socket, data, size);
}

int SPWFSAInterface5::socket_recvfrom(void *handle, SocketAddress *addr, void *data, unsigned size)
{
	struct spwf_socket *socket = (struct spwf_socket *)handle;
	return socket_recv(socket, data, size);
}

void SPWFSAInterface5::socket_attach(void *handle, void(*callback)(void *), void *data)
{
	//No implementation yet
}

int SPWFSAInterface5::setsockopt(void *handle, int level, int optname, const void *optval, unsigned optlen)
{
	//    struct spwf_socket *socket = (struct spwf_socket *)handle;      

	switch (optname) {
	case NSAPI_REUSEADDR: /*!< Allow bind to reuse local addresses */
	case NSAPI_KEEPALIVE: /*!< Enables sending of keepalive messages */
	case NSAPI_LINGER:    /*!< Keeps close from returning until queues empty */
	case NSAPI_SNDBUF:    /*!< Sets send buffer size */
	case NSAPI_RCVBUF:    /*!< Sets recv buffer size */
	default:
		printf("SPWFSAInterface5::setsockopt> ERROR!!!! Unknown optname: %d \r\n", optname);
		return -1;
	}
	return NSAPI_ERROR_UNSUPPORTED;
}

int SPWFSAInterface5::getsockopt(void *handle, int level, int optname, void *optval, unsigned *optlen)
{
	return NSAPI_ERROR_UNSUPPORTED;
}

int SPWFSAInterface5::init(void)
{
	if (_spwf.startup(0)) {
		isInitialized = true;
		return true;
	}
	else return NSAPI_ERROR_DEVICE_ERROR;
}
