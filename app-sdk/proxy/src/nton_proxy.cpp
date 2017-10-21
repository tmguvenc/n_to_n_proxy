#include <zmq.h>
#include <nton_proxy.h>
#include <assert.h>
#include <sstream>

N2NProxy::N2NProxy(uint8_t capacity, uint16_t front_port, uint16_t back_port) : 
m_capacity(capacity),
m_context(nullptr),
m_front_socket(nullptr),
m_back_socket(nullptr)
{
	m_context = zmq_ctx_new();
	assert(m_context && "cannot create zmq context");

	m_front_socket = zmq_socket(m_context, ZMQ_ROUTER);
	assert(m_front_socket && "cannot create zmq socket");

	m_back_socket = zmq_socket(m_context, ZMQ_ROUTER);
	assert(m_back_socket && "cannot create zmq socket");

	auto frontAddress = "tcp://*:" + std::to_string(front_port);
	auto backAddress = "tcp://*:" + std::to_string(back_port);

	auto retVal = zmq_bind(m_front_socket, frontAddress.c_str());
	assert(retVal && "cannot bind address");

	retVal = zmq_bind(m_back_socket, backAddress.c_str());
	assert(retVal && "cannot bind address");
}

N2NProxy::~N2NProxy()
{
	if (m_front_socket)	{
		zmq_close(m_front_socket);
		m_front_socket = nullptr;
	}
	if (m_back_socket)	{
		zmq_close(m_back_socket);
		m_back_socket = nullptr;
	}
	if (m_context)	{
		zmq_ctx_destroy(m_context);
		m_context = nullptr;
	}
}

void N2NProxy::start()
{

}