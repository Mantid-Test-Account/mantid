set ( SRC_FILES
        src/MuonNexusReader.cpp
        src/NexusClasses.cpp
        src/NexusFileIO.cpp
)

set ( INC_FILES
        inc/MantidNexus/MuonNexusReader.h
        inc/MantidNexus/NexusClasses.h
        inc/MantidNexus/NexusFileIO.h
)

set ( TEST_FILES
	test/NexusAPITest.h
)

if (COVERALLS)
    foreach( loop_var ${SRC_FILES} ${INC_FILES})
      set_property(GLOBAL APPEND PROPERTY COVERAGE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/${loop_var}")
    endforeach(loop_var)
endif()

add_definitions ( -DIN_NEXUS_CPP_LIBRARY )

# Add the target for this directory
add_library ( Nexus ${SRC_FILES} ${INC_FILES})
# Set the name of the generated library
set_target_properties ( Nexus PROPERTIES OUTPUT_NAME MantidNexus 
                                         COMPILE_DEFINITIONS IN_MANTID_NEXUS )

if (OSX_VERSION VERSION_GREATER 10.8)
  set_target_properties ( Nexus PROPERTIES INSTALL_RPATH "@loader_path/../MacOS" )
endif () 

# Add to the 'Framework' group in VS
set_property ( TARGET Nexus PROPERTY FOLDER "MantidFramework" )

include_directories ( inc )

target_link_libraries ( Nexus LINK_PRIVATE ${TCMALLOC_LIBRARIES_LINKTIME} ${MANTIDLIBS} ${NEXUS_C_LIBRARIES} ${NEXUS_LIBRARIES} )

# if ( CXXTEST_FOUND )
#  cxxtest_add_test ( NexusTest ${TEST_FILES} )
# target_link_libraries( NexusTest LINK_PRIVATE ${TCMALLOC_LIBRARIES_LINKTIME} Nexus  )
#  add_dependencies ( FrameworkTests NexusTest )
#  set_property ( TARGET NexusTest PROPERTY FOLDER "UnitTests" )
# endif ()

###########################################################################
# Installation settings
###########################################################################

install ( TARGETS Nexus ${SYSTEM_PACKAGE_TARGET} DESTINATION ${LIB_DIR} )
