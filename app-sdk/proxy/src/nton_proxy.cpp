#include <zmq.h>
#include <nton_proxy.h>
#include <assert.h>
#include <sstream>

#define POLL_IVL 1000

N2NProxy::N2NProxy(uint16_t front_port, uint16_t back_port) : 
m_front_port(front_port),
m_back_port(back_port),
m_run(false)
{

}

N2NProxy::~N2NProxy()
{

}

void N2NProxy::start()
{
	m_loop = std::async([this]()
	{
		start_internal();
	});
}

void N2NProxy::stop()
{
	m_run.store(false);

	// wait until the loop thread is finished
	if (m_loop.valid())
		m_loop.get();
}

void N2NProxy::start_internal()
{
	init();

	zmq_pollitem_t items[] = {
		{ m_front_socket, 0, ZMQ_POLLIN, 0 },
		{ m_back_socket, 0, ZMQ_POLLIN, 0 }
	};

	m_run.store(true);

	char buffer[64 * 1024];
	char src_clientId[80];
	char dst_clientId[80];
	int dst_clientId_len, buffer_len;

	while (m_run)
	{
		if (zmq_poll(items, 2, POLL_IVL) == -1)
			break; // interrupt

		// if front_socket has incoming message
		if (items[0].revents & ZMQ_POLLIN)
		{
			// source client id
			zmq_recv(m_front_socket, src_clientId, sizeof(src_clientId), 0);

			// null frame
			zmq_recv(m_front_socket, buffer, sizeof(buffer), 0);

			// destination client id
			dst_clientId_len = zmq_recv(m_front_socket, dst_clientId, sizeof(dst_clientId), 0);

			// data
			buffer_len = zmq_recv(m_front_socket, buffer, sizeof(buffer), 0);

			zmq_send(m_back_socket, dst_clientId, dst_clientId_len, ZMQ_SNDMORE);
			zmq_send(m_back_socket, nullptr, 0, ZMQ_SNDMORE);
			zmq_send(m_back_socket, buffer, buffer_len, 0);
		}

		// if back_socket has incoming message
		if (items[1].revents & ZMQ_POLLIN)
		{
			// source client id
			zmq_recv(m_back_socket, src_clientId, sizeof(src_clientId), 0);

			// null frame
			zmq_recv(m_back_socket, buffer, sizeof(buffer), 0);

			// destination client id
			dst_clientId_len = zmq_recv(m_back_socket, dst_clientId, sizeof(dst_clientId), 0);

			// data
			buffer_len = zmq_recv(m_back_socket, buffer, sizeof(buffer), 0);

			zmq_send(m_front_socket, dst_clientId, dst_clientId_len, ZMQ_SNDMORE);
			zmq_send(m_front_socket, nullptr, 0, ZMQ_SNDMORE);
			zmq_send(m_front_socket, buffer, buffer_len, 0);
		}
	}

	destroy();
}

void N2NProxy::init()
{
	m_context = zmq_ctx_new();
	assert(m_context && "cannot create zmq context");

	m_front_socket = zmq_socket(m_context, ZMQ_ROUTER);
	assert(m_front_socket && "cannot create zmq socket");

	m_back_socket = zmq_socket(m_context, ZMQ_ROUTER);
	assert(m_back_socket && "cannot create zmq socket");

	auto frontAddress = "tcp://*:" + std::to_string(m_front_port);
	auto backAddress = "tcp://*:" + std::to_string(m_back_port);

	auto retVal = zmq_bind(m_front_socket, frontAddress.c_str());
	assert(retVal && "cannot bind address");

	retVal = zmq_bind(m_back_socket, backAddress.c_str());
	assert(retVal && "cannot bind address");
}

void N2NProxy::destroy()
{
	zmq_close(m_front_socket);
	zmq_close(m_back_socket);
	zmq_ctx_destroy(m_context);
}