#include <zmq.h>
#include <thread>
#include <future>
#include <iostream>
#include <chrono>

void connect_and_send()
{
	auto context = zmq_ctx_new();
	assert(context != nullptr);

	auto client_socket = zmq_socket(context, ZMQ_DEALER);
	assert(client_socket != nullptr);

	zmq_connect(client_socket, "tcp://127.0.0.1:5555");

	zmq_send(client_socket, nullptr, 0, ZMQ_SNDMORE);
	zmq_send(client_socket, "frame1", 6, 0);

	std::this_thread::sleep_for(std::chrono::milliseconds(1));

	zmq_send(client_socket, nullptr, 0, ZMQ_SNDMORE);
	zmq_send(client_socket, "frame2", 6, 0);

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	zmq_send(client_socket, nullptr, 0, ZMQ_SNDMORE);
	zmq_send(client_socket, "frame3", 6, 0);

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	zmq_send(client_socket, nullptr, 0, ZMQ_SNDMORE);
	zmq_send(client_socket, "quit", 4, 0);

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	zmq_close(client_socket);
	zmq_ctx_destroy(context);
}

int main()
{
	std::async([]()
	{
		connect_and_send();
	});

	auto context = zmq_ctx_new();
	assert(context != nullptr);

	auto server_socket = zmq_socket(context, ZMQ_ROUTER);
	assert(server_socket != nullptr);

	auto val = 1;
	auto xx = zmq_setsockopt(server_socket, ZMQ_ROUTER_MANDATORY, &val, sizeof val);
	assert(xx == 0);

	auto bound = zmq_bind(server_socket, "tcp://*:5555");
	assert(bound == 0);

	zmq_pollitem_t items[] = {
		{ server_socket, 0, ZMQ_POLLIN, 0 }
	};

	char rec_buf[1024];

	while (true)
	{
		//  Poll frontend only if we have available workers
		int rc = zmq_poll(items, 1, 100000);

		if (items[0].revents & ZMQ_POLLIN)
		{
			// client id
			zmq_recv(server_socket, rec_buf, sizeof(rec_buf), 0);

			// null frame
			zmq_recv(server_socket, rec_buf, sizeof(rec_buf), 0);

			// data
			int count = zmq_recv(server_socket, rec_buf, sizeof(rec_buf), 0);

			auto aa = std::string(rec_buf, count);

			if (aa == "quit")
				break;

			std::cout << aa.c_str() << std::endl;
		}
	}

	zmq_close(server_socket);
	zmq_ctx_destroy(context);

	return 0;
}

