cmake_minimum_required(VERSION 3.20)

project(du LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    # /O1 often results in faster code than /O2 due to CPU caching
    string(REPLACE "/O2" "/O1" cl_optimize ${CMAKE_CXX_FLAGS_RELEASE})
    set(CMAKE_CXX_FLAGS_RELEASE ${cl_optimize} CACHE STRING "C++ Release flags" FORCE)

    # Using /Z7 instead of /Zi to avoid blocking while parallel compilers write to the pdb file.
    # This can considerably speed up build times at the cost of larger object files.
    string(REPLACE "/Zi" "/Z7" z_seven ${CMAKE_CXX_FLAGS_DEBUG})
    set(CMAKE_CXX_FLAGS_DEBUG ${z_seven} CACHE STRING "C++ Debug flags" FORCE)
endif()

get_property(isMultiConfig GLOBAL
  PROPERTY GENERATOR_IS_MULTI_CONFIG
)

if (NOT isMultiConfig)
    message("\nBecause you are using a single target generator, you MUST specify")
    message("    a \"--config [Debug|Release]\" option with the cmake --build command\n")

    set(allowedBuildTypes Debug Release)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${allowedBuildTypes}")

    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
    elseif (NOT CMAKE_BUILD_TYPE IN_LIST allowedBuildTypes)
        message(FATAL_ERROR "Unknown build type: ${CMAKE_BUILD_TYPE}")
    endif()
endif()

# Note the requirement that --config Debug is used to get the additional debug files
add_executable(du
    src/mainapp.cpp
    src/strings.cpp

    ../ttlib/src/ttconsole.cpp      # Class for handling zero-terminated char strings.
    ../ttlib/src/ttcstr.cpp      # Class for handling zero-terminated char strings.
    ../ttlib/src/ttcview.cpp     # string_view functionality on a zero-terminated char string.
    ../ttlib/src/ttlibspace.cpp  # ttlib namespace functions
    ../ttlib/src/tttextfile.cpp  # Classes for reading and writing text files.
    ../ttlib/src/ttsview.cpp     # std::string_view with additional methods
    ../ttlib/src/ttcvector.cpp   # Vector class for storing ttlib::cstr strings
    ../ttlib/src/ttmultistr.cpp  # Breaks a single string into multiple strings
    ../ttlib/src/ttparser.cpp    # Command line parser
    ../ttlib/src/ttstrings.cpp

    ../ttlib/src/winsrc/ttwinspace.cpp      # Windows-only ttlib namespace functions
    ../ttlib/src/winsrc/ttloadstr.cpp       # Load a language-specific version of a string resource

    $<$<CONFIG:Debug>:../ttlib/src/winsrc/ttdebug.cpp>  # we use our own version of this to minimize Windows code requirements
)

if (MSVC)
    # /GL -- combined with the Linker flag /LTCG to perform whole program optimization in Release build
    # /FC -- Full path to source code file in diagnostics
    target_compile_options(du PRIVATE "$<$<CONFIG:Release>:/GL>" "/FC" "/W4" "/Zc:__cplusplus" "/utf-8")
    target_link_options(du PRIVATE "$<$<CONFIG:Release>:/LTCG>")

    target_link_options(du PRIVATE "$<$<CONFIG:Debug>:/natvis:../../ttLib/ttlibwin.natvis>")
endif()

target_precompile_headers(du PRIVATE "src/pch.h")

target_include_directories(du PRIVATE 
    src/
    ../ttLib/include
)

