# The name of our project is "HELLO". CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8)
project (OSUFlowEdda) 

#set(WITH_PARAVIEW_PLUGINS	OFF	CACHE	BOOL	"Use Paraview Package")
set(OSUFLOWEDDA_DIR      "${CMAKE_PROJECT_DIR}/edda")



#if (${WITH_PARAVIEW_PLUGINS})
#else()
	find_package(VTK REQUIRED)
	include (${VTK_USE_FILE})
#endif()


set (EDDA_INSTALL_DIR "/usr/local" CACHE PATH "EDDA build path")
set (EDDA_VIS_INSTALL_DIR "/usr/local" CACHE PATH "EDDA VIS build path")

include_directories (${EDDA_INSTALL_DIR}/include/edda ${EDDA_VIS_INSTALL_DIR}/include/edda_vis)
link_directories( ${EDDA_INSTALL_DIR}/lib/edda ${EDDA_VIS_INSTALL_DIR}/lib/edda_vis)

find_package (Boost REQUIRED)
if (Boost_FOUND)
  include_directories (${Boost_INCLUDE_DIRS})
  link_libraries (${Boost_LIBRARIES})
endif ()

set (THRUST_DEVICE_SYSTEM "CPP" CACHE STRING "Thrust device backend (CUDA/TBB/OPENMP/CPP).")
set_property(CACHE THRUST_DEVICE_SYSTEM PROPERTY STRINGS "CUDA;TBB;OPENMP;CPP")

## Thrust with CUDA backend
if (${THRUST_DEVICE_SYSTEM} STREQUAL "CUDA")
  find_package(CUDA REQUIRED)
  #list(APPEND CUDA_NVCC_FLAGS "-ffast-math")
  #list(APPEND CUDA_NVCC_FLAGS --compiler-options -fno-strict-aliasing -lineinfo -use_fast_math -Xptxas -dlcm=cg)
  #list(APPEND CUDA_NVCC_FLAGS  "--std=c++11") #cuda 7 feature
  #list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_20,code=sm_20)
  #list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_30,code=sm_30)
  #list(APPEND CUDA_NVCC_FLAGS -gencode arch=compute_35,code=sm_35)
  # To successfully build a library, this is required (rdc=true)
  # See http://stackoverflow.com/questions/13683575/cuda-5-0-separate-compilation-of-library-with-cmake
  set(CUDA_ATTACH_VS_BUILD_RULE_TO_CUDA_FILE ON)
  set(CUDA_SEPARABLE_COMPILATION ON)
  list(APPEND LINK_LIBS ${CUDA_LIBRARIES} ${CUDA_CUDART_LIBRARY})
  set(EDDA_BUILD_WITH_CUDA ON)
## Thrust with TBB backend
elseif (${THRUST_DEVICE_SYSTEM} STREQUAL "TBB")
  add_definitions(-DTHRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_TBB)
  set ( THRUST_INCLUDE_DIR "/usr/local/include" CACHE PATH "Thrust root dir")
  include_directories ( ${THRUST_INCLUDE_DIR} )
  set ( TBB_INSTALL_PATH "/usr/local" CACHE PATH "TBB root dir")
  include_directories(${TBB_INSTALL_PATH}/include)
  link_directories( ${TBB_INSTALL_PATH}/lib)
  list(APPEND LINK_LIBS tbb)
## Thrust with OpenMP backend
elseif (${THRUST_DEVICE_SYSTEM} STREQUAL "OPENMP")
  add_definitions ( -DTHRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_OMP)
  set ( THRUST_INCLUDE_DIR "/usr/local/include" CACHE PATH "Thrust root dir")
  include_directories ( ${THRUST_INCLUDE_DIR} )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
elseif (${THRUST_DEVICE_SYSTEM} STREQUAL "CPP")
  add_definitions ( -DTHRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_CPP)
  set ( THRUST_INCLUDE_DIR "/usr/local/include" CACHE PATH "Thrust root dir")
  include_directories ( ${THRUST_INCLUDE_DIR} )
else ()
  message(ERROR "Invalid setting of THRUST_DEVICE_SYSTEM")
endif ()


# specify the path to the headers
include_directories(
	.
)

# grab all the header files
file(GLOB HEADER_FILES "*.h" )

list(APPEND SRC_FILES
	EddaOSUFlow.cxx
	EddaDatasetProcessor.cxx
	)

# build the project as a library
add_library(${PROJECT_NAME} ${LIB_TYPE}  ${SRC_FILES} ${HEADER_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES 
        DEBUG_OUTPUT_NAME       "${PROJECT_NAME}_d"
        RELEASE_OUTPUT_NAME "${PROJECT_NAME}"
        )

install(
        FILES ${HEADER_FILES}
        DESTINATION include/osuflow/edda)

install(
        TARGETS ${PROJECT_NAME}
        DESTINATION lib/osuflow)


#########################################################
# setup for dependent codes:
#########################################################

# setup link libraries
#message(STATUS "VTKLIBS: ${PARAVIEW_LIBRARIES}")
list (APPEND LINK_LIBS
	OSUFlowEdda
	OSUFlow
	#${PARAVIEW_LIBRARIES}
	${VTK_LIBRARIES}
	)
if (WITH_GRAPHICS)
	list (APPEND LINK_LIBS renderer gcb)
        list (APPEND LINK_LIBS ${OPENGL_LIBRARY} ${GLU_LIBRARY} ${GLUT_LIBRARY})
endif ()

if (WITH_MPI)
#not tested yet
	#list (APPEND LINK_LIBS diy bil ${MPI_C_LIBRARIES})
endif ()
if (WITH_PNETCDF)
#not tested yet
	#list (APPEND LINK_LIBS  pnetcdf )
endif ()

	
list(APPEND LINK_LIBS edda_vis_d edda_d )

# examples
add_subdirectory (examples)

# plugins
#if (${WITH_PARAVIEW_PLUGINS})
#	add_subdirectory (plugins)
#endif()

# tests
#if( ${WITH_GRAPHICS} )
	#add_executable( compareStreamline compareStreamline.cpp )
    #    target_link_libraries ( compareStreamline ${LINK_LIBS})
#endif ()
#if ( ${WITH_MPI} AND ${WITH_PARAVIEW_PLUGINS})
#	add_executable( testMPI testMPI.cpp )
#	target_link_libraries ( testMPI ${LINK_LIBS} )
#endif ()
