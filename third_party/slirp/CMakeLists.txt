## Compile SLirp as its own standalone library

if (WITH_NETWORK)
  set(SLIRP_SOURCES
    "${SIMH_THIRD_PARTY_ROOT}/slirp/arp_table.c"
    "${SIMH_THIRD_PARTY_ROOT}/slirp/bootp.c"
    "${SIMH_THIRD_PARTY_ROOT}/slirp/bootp.h"
    "${SIMH_THIRD_PARTY_ROOT}/slirp/cksum.c"
    "${SIMH_THIRD_PARTY_ROOT}/slirp/dnssearch.c"
    "${SIMH_THIRD_PARTY_ROOT}/slirp/if.c"
    "${SIMH_THIRD_PARTY_ROOT}/slirp/ip_icmp.c"
    "${SIMH_THIRD_PARTY_ROOT}/slirp/ip_input.c"
    "${SIMH_THIRD_PARTY_ROOT}/slirp/ip_output.c"
    "${SIMH_THIRD_PARTY_ROOT}/slirp/mbuf.c"
    "${SIMH_THIRD_PARTY_ROOT}/slirp/misc.c"
    "${SIMH_THIRD_PARTY_ROOT}/slirp/sbuf.c"
    "${SIMH_THIRD_PARTY_ROOT}/slirp/slirp.c"
    "${SIMH_THIRD_PARTY_ROOT}/slirp/socket.c"
    "${SIMH_THIRD_PARTY_ROOT}/slirp/tcp_input.c"
    "${SIMH_THIRD_PARTY_ROOT}/slirp/tcp_output.c"
    "${SIMH_THIRD_PARTY_ROOT}/slirp/tcp_subr.c"
    "${SIMH_THIRD_PARTY_ROOT}/slirp/tcp_timer.c"
    "${SIMH_THIRD_PARTY_ROOT}/slirp/tftp.c"
    "${SIMH_THIRD_PARTY_ROOT}/slirp/udp.c"
    "${SIMH_THIRD_PARTY_ROOT}/slirp_glue/glib_qemu_stubs.c"
    "${SIMH_THIRD_PARTY_ROOT}/slirp_glue/sim_slirp.c")

  add_library(slirp STATIC "${SLIRP_SOURCES}")
  target_compile_definitions(slirp
    PRIVATE
        HAVE_SLIRP_NETWORK
        USE_SIMH_SLIRP_DEBUG
        $<$<AND:$<BOOL:${WIN32}>,$<NOT:$<BOOL:${WINAPI_DEPRECATION}>>>:
            _WINSOCK_DEPRECATED_NO_WARNINGS
            _CRT_NONSTDC_NO_WARNINGS
            _CRT_SECURE_NO_WARNINGS>
      INTERFACE
          HAVE_SLIRP_NETWORK
          USE_SIMH_SLIRP_DEBUG)

  # Slirp uses legacy BSD/GNU libc names such as u_int and caddr_t.
  # In strict C17 mode, glibc hides those unless a feature-test macro
  # requests the extended namespace.
  if (NOT WIN32 AND
      (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR
       CMAKE_C_COMPILER_ID MATCHES ".*Clang"))
    target_compile_definitions(slirp PRIVATE _GNU_SOURCE)
  endif ()

  target_include_directories(slirp
      PRIVATE
          "${SIMH_COMPAT_ROOT}"
          "${SIMH_CORE_ROOT}"
          "${SIMH_RUNTIME_ROOT}"
          "${SIMH_THIRD_PARTY_ROOT}/slirp"
          "${SIMH_THIRD_PARTY_ROOT}/slirp_glue/qemu"
      PUBLIC
          "${SIMH_THIRD_PARTY_ROOT}/slirp_glue"
          $<$<BOOL:${WIN32}>:${SIMH_THIRD_PARTY_ROOT}/slirp_glue/qemu/win32/include>)

  target_compile_options(slirp PRIVATE ${EXTRA_TARGET_CFLAGS})
  target_link_options(slirp PRIVATE ${EXTRA_TARGET_LFLAGS})
  target_link_libraries(slirp PUBLIC $<$<BOOL:${WIN32}>:Iphlpapi>)
else (WITH_NETWORK)
  # Otherwise, just make slirp an empty interface library.
  add_library(slirp INTERFACE)
endif (WITH_NETWORK)
