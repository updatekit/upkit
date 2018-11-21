AUTOMAKE_OPTIONS = subdir-objects
AM_CFLAGS = -Wall -I. -I../include -I../ext/tinycrypt/lib/include -I../ext/tinydtls -DHAVE_CONFIG_H
AM_CFLAGS += -I../build/posix -Isupport -I../ext/libcoap/include -Imocks
AM_CFLAGS += -DHAVE_CONFIG_H -DUSE_TESTING_KEYS=1 -DWITH_CONNECTION_LIBCOAP=1 -DWITH_UPDATE_AGENT=1
AM_CFLAGS += -DWITH_PLATFORM_POSIX=1 -DENABLE_SIGN=1

if COND_GCOV
  MAYBE_COVERAGE= -fprofile-arcs -ftest-coverage
endif

AM_CFLAGS += $(MAYBE_COVERAGE)

MOSTLYCLEANFILES = idsgrep.aux idsgrep.log idsgrep.blg idsgrep.bbl idsgrep.toc \
				   *.gcda *.gcno *.gcov

LIBS = ../ext/libcoap/.libs/libcoap-1.a ../ext/tinydtls/libtinydtls.a
LIBS += ../ext/tinycrypt/lib/libtinycrypt.a

COMMON_SRC = ../src/core/common/error.c ../src/core/common/identity.c
MEMORY_SRC = ../src/core/memory/memory_objects.c 
SECURITY_SRC = ../src/core/security/verifier.c
NETWORK_SRC = ../src/core/network/connection_config.c ../src/core/network/writer.c
SUPPORT_SRC = support/external_variables.c support/utils.c
PIPELINE_SRC = ../src/core/pipeline/lzss.c ../src/core/pipeline/bspatch.c ../src/core/pipeline/buffer.c

AGENTS_SRC = ../src/agents/update.c ../src/agents/bootloader_agent.c

NETWORK_IMPL = ../src/network/coap/libcoap/connection_libcoap.c ../src/network/coap/libcoap/async_libcoap.c
MEMORY_IMPL = ../src/core/memory/memory_impl.c ../src/platform/posix/memory_posix.c
MANIFEST_IMPL = ../src/core/memory/manifest.c ../src/core/memory/simple_manifest.c

ALL_SRC = $(COMMON_SRC) $(MEMORY_SRC) $(SECURITY_SRC) $(MANIFEST_IMPL) $(NETWORK_SRC)
ALL_SRC += $(NETWORK_IMPL) $(MEMORY_IMPL) $(SUPPORT_SRC) $(PIPELINE_SRC)

MOCK_MEMORY = mocks/memory_mock.c ../src/platform/posix/memory_posix.c
MOCK_MANIFEST = mocks/manifest_mock.c ../src/core/memory/simple_manifest.c
NO_MOCK_SRC = $(COMMON_SRC) $(SECURITY_SRC) $(NETWORK_SRC) $(NETWORK_IMPL) $(MEMORY_SRC) $(SUPPORT_SRC)

TINYCRYPT_SRC = ../src/core/security/tinycrypt.c ../ext/tinycrypt/lib/source/ecc_platform_specific.c
TINYDTLS_SRC = ../src/core/security/tinydtls.c

memory_tests = test_memory test_memory_objects test_manifest
agents_tests = test_update test_bootloader_agent
network_tests = test_receiver test_subscriber test_connection_dtls #test_receiver_memory_invalid
security_tests = test_tinycrypt test_tinydtls
pipeline_tests = test_pipeline test_lzss test_bspatch test_buffer

check_PROGRAMS = $(memory_tests) $(agents_tests) $(network_tests) $(security_tests) $(common_tests) $(pipeline_tests)
TESTS = $(check_PROGRAMS)

# Memory tests
test_memory_SOURCES = memory/test_memory.c $(ALL_SRC) $(TINYCRYPT_SRC)
test_memory_objects_SOURCES = memory/test_memory_objects.c $(ALL_SRC) $(TINYCRYPT_SRC)
test_manifest_SOURCES = memory/test_manifest.c $(ALL_SRC) $(TINYCRYPT_SRC)

# Network tests
test_receiver_SOURCES = network/test_receiver.c $(ALL_SRC) $(TINYDTLS_SRC)
test_subscriber_SOURCES = network/test_subscriber.c $(ALL_SRC) $(TINYDTLS_SRC)
test_connection_dtls_SOURCES = network/test_connection_dtls.c $(ALL_SRC) $(TINYDTLS_SRC)

# Security tests
test_tinycrypt_SOURCES = security/test_tinycrypt.c $(NO_MOCK_SRC) $(MOCK_MEMORY) $(MOCK_MANIFEST) $(TINYCRYPT_SRC)
test_tinydtls_SOURCES = security/test_tinydtls.c $(NO_MOCK_SRC) $(MOCK_MEMORY) $(MOCK_MANIFEST) $(TINYDTLS_SRC)

# Agents tests
test_update_SOURCES = agents/test_update.c $(ALL_SRC) $(AGENTS_SRC) $(TINYDTLS_SRC)
test_bootloader_agent_SOURCES = agents/test_bootloader_agent.c $(ALL_SRC) $(AGENTS_SRC) $(TINYDTLS_SRC)
test_bootloader_agent_CFLAGS = $(AM_CFLAGS) -DRECOVERY_IMAGE=0

# Pipeline tests
test_pipeline_SOURCES = pipeline/test_pipeline.c $(ALL_SCR)
test_lzss_SOURCES = pipeline/test_lzss.c $(ALL_SRC)
test_bspatch_SOURCES = pipeline/test_bspatch.c $(ALL_SRC)
test_buffer_SOURCES = pipeline/test_buffer.c $(ALL_SRC)
