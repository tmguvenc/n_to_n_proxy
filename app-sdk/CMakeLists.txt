cmake_minimum_required(VERSION 3.1)

project(ntonproxy)

set(DEPENDS_FOLDER "D:/External" CACHE PATH "Dependency Path")

# zeromq
set(ZEROMQ_INCLUDE ${DEPENDS_FOLDER}/zeromq_secure/x64/include)
set(ZEROMQ_LIBS ${DEPENDS_FOLDER}/zeromq_secure/x64/lib)
set(ZEROMQ_BINS ${DEPENDS_FOLDER}/zeromq_secure/x64/bin)
set(ZEROMQ_LINK libzmq)
set(ZEROMQ_DLL "${ZEROMQ_BINS}/libzmq.dll" "${ZEROMQ_BINS}/libsodium.dll")

enable_testing()

add_subdirectory(proxy)
add_subdirectory(test)