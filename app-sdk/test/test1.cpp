#include <utils.h>
#include <connector.h>
#include <thread>
#include <future>
#include <iostream>

int main()
{
	auto context = zmq_ctx_new();
	assert(context != nullptr);

	auto server_socket = zmq_socket(context, ZMQ_ROUTER);
	assert(context != nullptr);

	auto val = 1;
	auto xx = zmq_setsockopt(server_socket, ZMQ_ROUTER_MANDATORY, &val, sizeof val);
	assert(xx == 0);

#ifdef USE_CURVE
	int opt = 1;
	std::string secretKey("Sd[BRNU[GQ6YL<P5-O!b]{pD@^yxNQ).Iln9%eU1");

	auto curve_server = zmq_setsockopt(server_socket, ZMQ_CURVE_SERVER, &opt, sizeof opt);
	assert(curve_server == 0);

	auto secret_key = zmq_setsockopt(server_socket, ZMQ_CURVE_SECRETKEY, secretKey.c_str(), secretKey.length());
	assert(secret_key == 0);
#endif

	auto bound = zmq_bind(server_socket, "tcp://*:5555");
	assert(bound == 0);

	const std::string module_name("test");

	auto connector_loop = [module_name]()
	{
		auto connector = new Connector("tcp://localhost:5555", module_name.c_str());
		connector->connect();

		auto counter = 4;
		while (counter--)
		{
			connector->heartbeat(-1);
		}

		delete connector;
	};

	auto conn_feature = std::async(std::launch::async, connector_loop);

	std::string client_name;
	auto client_message = recv_client_message(server_socket, client_name);
	assert(client_message->type() == Init);

	auto counter = 4;

	while (counter--)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(6000));

		// send ping message
        //auto req = std::make_unique<ServerMessage>();
        std::unique_ptr<ServerMessage> req(new ServerMessage);
		req->set_type(Ping);
		send_server_message(server_socket, req.get(), client_name);

		// expect pong message
		std::string client_name_ping;
		auto client = recv_client_message(server_socket, client_name_ping);
		assert(client->type() == Pong);

		std::cout << "Round trip: " << counter << '\n';
	}

	conn_feature.get();

	zmq_close(server_socket);
	zmq_ctx_destroy(context);

	return 0;
}

