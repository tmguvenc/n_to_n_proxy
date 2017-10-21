#ifndef NTON_PROXY_H
#define NTON_PROXY_H

#include <macros.h>
#include <stdint.h>

class PROXY_EXPORT N2NProxy
{
public:
	explicit N2NProxy(uint8_t capacity, uint16_t front_port, uint16_t back_port);
	~N2NProxy();

	void start();
	void stop();

private:
	void* m_context;
	void* m_front_socket;
	void* m_back_socket;
	uint8_t m_capacity;
};

#endif