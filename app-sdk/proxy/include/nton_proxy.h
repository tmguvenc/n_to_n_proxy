#ifndef NTON_PROXY_H
#define NTON_PROXY_H

#include <macros.h>
#include <stdint.h>
#include <atomic>
#include <future>

class PROXY_EXPORT N2NProxy
{
public:
	explicit N2NProxy(uint16_t front_port, uint16_t back_port);
	~N2NProxy();

	void start();
	void stop();

private:
	void start_internal();
	void init();
	void destroy();
	void read_internal(void*);

	void* m_context;
	void* m_front_socket;
	void* m_back_socket;

	char m_buffer[64 * 1024];
	char m_src_clientId[80];
	char m_dst_clientId[80];

	uint16_t m_front_port, m_back_port;
	std::atomic<bool> m_run;
	std::future<void> m_loop;
};

#endif