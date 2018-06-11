cmake_minimum_required(VERSION 3.9)
project(fflib VERSION 2.0.0 DESCRIPTION "fflib 2.0")
include(GNUInstallDirs)

find_package(MPI REQUIRED)
include_directories(
  ${MPI_INCLUDE_PATH} 
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_library(fflib SHARED 
src/ff.c
src/ffcomp.h
src/ffdatatype.h
src/ff.h
src/ffinternal.h
src/ffop.c
src/ffop.h
src/ffop_mem.h
src/ffcomp.c
src/ffcomp.h
src/ffprogress.c
src/ffprogress.h
src/ffrecv.c
src/ffrecv.h
src/ffsend.c
src/ffsend.h
src/ffstorage.h
src/ffstorage.c
src/components/mpi/ffmpi.c
src/components/mpi/ffmpi.h
src/components/mpi/ffop_mpi.c
src/components/mpi/ffop_mpi.h
src/components/mpi/ffop_mpi_init.c
src/components/mpi/ffop_mpi_progresser.c
src/components/mpi/ffop_mpi_progresser.c.bak
src/components/mpi/ffop_mpi_progresser.h
src/components/mpi/ffop_mpi_recv.c
src/components/mpi/ffop_mpi_send.c
src/components/mpi/ffmpi_types.h
src/utils/ffarman.c
src/utils/ffarman.h
src/components/gcomp/ffgcomp.c
src/components/gcomp/ffgcomp.h
src/components/gcomp/ffop_gcomp.c
src/components/gcomp/ffop_gcomp.h
src/components/gcomp/ffop_gcomp_operator.c
src/components/gcomp/ffop_gcomp_operator.h
src/components/gcomp/ffgcomp_types.h
src/bindings/mpi_binding.h
)
set_target_properties(fflib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 2
    PUBLIC_HEADER src/ff.h)
configure_file(fflib.pc.in fflib.pc @ONLY)
target_include_directories(fflib PRIVATE src)
target_link_libraries(fflib ${MPI_LIBRARIES})

set(CMAKE_C_FLAGS_DEBUG "-DFFDEBUG -g")

install(TARGETS fflib
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/fflib.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

#--------------------------------------------------
# Tests
#--------------------------------------------------
add_subdirectory(test)

