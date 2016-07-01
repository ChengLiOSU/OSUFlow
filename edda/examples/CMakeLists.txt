# The name of our project is "HELLO". CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8)
project (edda_examples) 


set(VTK_DATA_ROOT      "$ENV{HOME}/project/VTKData" CACHE STRING "Root to VTKData for testing the examples")
#set(PARAVIEW_DATA_ROOT      "$ENV{HOME}/project/VTKData" CACHE STRING "Root to VTKData for testing the examples")


# specify the path to the headers
include_directories(
)

link_directories (
)

add_definitions (-DVTK_DATA_ROOT=${VTK_DATA_ROOT})

# List all simple targets
#LIST(APPEND)
if( ${WITH_GRAPHICS} )
        LIST (APPEND SIMPLE_TARGETS
		eddaFileReaderGlStreamline
		eddaVectorFieldGlStreamline
        )
endif ()
# if ( false ) #${WITH_MPI} )
	# LIST (APPEND SIMPLE_TARGETS
        	# MpiDrawVTK
	# )
	# # MPI Script
        # file (COPY      ${CMAKE_CURRENT_SOURCE_DIR}/MPI_TEST
                # DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
# endif ()



IF(APPLE)
    # The primitive VTK renderer cannot catch keyboard events
    # without packed into the mac bundle
    SET(EXECUTABLE_FLAG MACOSX_BUNDLE)  
ENDIF(APPLE)

# all targets
foreach(TARGET ${SIMPLE_TARGETS} )
  
  if (EDDA_BUILD_WITH_CUDA)
    cuda_add_executable ( ${TARGET} ${EXECUTABLE_FLAG} ${TARGET}.cpp )
  else ()
    SET_SOURCE_FILES_PROPERTIES(${file} PROPERTIES LANGUAGE CXX)
    add_executable ( ${TARGET} ${EXECUTABLE_FLAG} ${TARGET}.cpp )
  endif ()


	target_link_libraries (${TARGET} ${LINK_LIBS} )
endforeach(TARGET)

########################
# # plugin examples
# if ( ${WITH_PARAVIEW_PLUGINS} AND ${WITH_MPI})
	# include_directories ( ../plugins )
	# add_executable (MPIStreamlineRender 
		# MPIStreamlineRender.cpp ../plugins/vtkPOSUFlow.cxx)
	# target_link_libraries( MPIStreamlineRender ${LINK_LIBS})
# endif ()

