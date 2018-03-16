AUTOMAKE_OPTIONS = subdir-objects
AM_CFLAGS = -Wall -I. -I../include -I../ext/tinycrypt/lib/include -I../ext/tinydtls -DHAVE_CONFIG_H
AM_CFLAGS += -I../build/posix -Isupport -I../ext/unity/src -I../ext/libcoap/include -Imocks
AM_CFLAGS += -DHAVE_CONFIG_H

LIBS = ../ext/libcoap/.libs/libcoap-1.a ../ext/tinydtls/libtinydtls.a
LIBS += ../ext/tinycrypt/lib/libtinycrypt.a

UNITY_SRC = ../ext/unity/src/unity.c
COMMON_SRC = ../src/common/error.c ../src/common/identity.c
MEMORY_SRC = ../src/memory/memory_objects.c 
SECURITY_SRC = ../src/security/verifier.c
NETWORK_SRC = ../src/network/receiver.c ../src/network/subscriber.c

NETWORK_IMPL = ../build/posix/transport_libcoap.c ../build/posix/async_libcoap.c
MEMORY_IMPL = ../src/memory/memory.c ../build/posix/memory_file_posix.c
MANIFEST_IMPL = ../src/memory/manifest.c ../src/memory/simple_manifest.c

ALL_SRC = $(UNITY_SRC) $(COMMON_SRC) $(MEMORY_SRC) $(SECURITY_SRC) $(MANIFEST_IMPL) $(NETWORK_SRC)
ALL_SRC += $(NETWORK_IMPL) $(MEMORY_IMPL)

MOCK_MEMORY = mocks/memory_mock.c ../build/posix/memory_file_posix.c
MOCK_MANIFEST = mocks/manifest_mock.c ../src/memory/simple_manifest.c
NO_MOCK_SRC = $(UNITY_SRC) $(COMMON_SRC) $(SECURITY_SRC) $(NETWORK_SRC) $(NETWORK_IMPL) $(MEMORY_SRC)


TINYCRYPT_SRC = ../src/security/tinycrypt.c ../ext/tinycrypt/lib/source/ecc_platform_specific.c
TINYDTLS_SRC = ../src/security/tinydtls.c

memory_tests = test_memory test_memory_objects test_manifest
integration_tests = test_logic
network_tests = test_receiver test_receiver_memory_invalid test_subscriber test_transport_dtls
security_tests = test_tinycrypt test_tinydtls

check_PROGRAMS = $(memory_tests)  $(integration_tests) test_receiver test_subscriber test_transport_dtls $(security_tests)
TESTS = $(check_PROGRAMS)

# Memory tests
test_memory_SOURCES = memory/test_memory.c $(ALL_SRC) $(TINYCRYPT_SRC)
test_memory_objects_SOURCES = memory/test_memory_objects.c $(ALL_SRC) $(TINYCRYPT_SRC)
test_manifest_SOURCES = memory/test_manifest.c $(ALL_SRC) $(TINYCRYPT_SRC)

# Network tests
test_receiver_SOURCES = network/test_receiver.c $(ALL_SRC) $(TINYDTLS_SRC)
#test_receiver_memory_invalid_SOURCES
test_subscriber_SOURCES = network/test_receiver.c $(ALL_SRC) $(TINYDTLS_SRC)
test_transport_dtls_SOURCES = network/test_transport_dtls.c $(ALL_SRC) $(TINYDTLS_SRC)

# Security tests
test_tinycrypt_SOURCES = security/test_tinycrypt.c $(NO_MOCK_SRC) $(MOCK_MEMORY) $(MOCK_MANIFEST) $(TINYCRYPT_SRC)
test_tinydtls_SOURCES = security/test_tinydtls.c $(NO_MOCK_SRC) $(MOCK_MEMORY) $(MOCK_MANIFEST) $(TINYDTLS_SRC)

# Integration tests
test_logic_SOURCES = integration/test_logic.c $(ALL_SRC) $(TINYDTLS_SRC)
