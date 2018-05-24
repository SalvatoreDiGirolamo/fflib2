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
src/ffoperator.h
src/ffop.h
src/ffop_mem.h
src/ffprogress.c
src/ffprogress.h
src/ffrecv.c
src/ffrecv.h
src/ffsend.c
src/ffsend.h
src/ffstorage.h
src/ffstorage.c
src/mpi/ffmpi.c
src/mpi/ffmpi.h
src/mpi/ffop_mpi.c
src/mpi/ffop_mpi.h
src/mpi/ffop_mpi_init.c
src/mpi/ffop_mpi_progresser.c
src/mpi/ffop_mpi_progresser.c.bak
src/mpi/ffop_mpi_progresser.h
src/mpi/ffop_mpi_recv.c
src/mpi/ffop_mpi_send.c
src/utils/ffarman.c
src/utils/ffarman.h
)
set_target_properties(fflib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 2
    PUBLIC_HEADER src/ff.h)
configure_file(fflib.pc.in fflib.pc @ONLY)
target_include_directories(fflib PRIVATE src)
target_link_libraries(fflib ${MPI_LIBRARIES})


install(TARGETS fflib
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/fflib.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

#--------------------------------------------------
# Tests
#--------------------------------------------------
add_subdirectory(test)

