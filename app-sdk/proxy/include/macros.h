#ifndef MACROS_H
#define MACROS_H

#ifdef proxy_EXPORTS
#	define PROXY_EXPORT __declspec(dllexport)
#else
#	define PROXY_EXPORT __declspec(dllimport)
#endif

#endif