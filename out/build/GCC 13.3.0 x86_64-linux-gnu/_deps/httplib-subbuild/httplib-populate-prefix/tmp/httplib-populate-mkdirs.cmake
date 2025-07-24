# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/se1d/PGW/out/build/GCC 13.3.0 x86_64-linux-gnu/_deps/httplib-src"
  "/home/se1d/PGW/out/build/GCC 13.3.0 x86_64-linux-gnu/_deps/httplib-build"
  "/home/se1d/PGW/out/build/GCC 13.3.0 x86_64-linux-gnu/_deps/httplib-subbuild/httplib-populate-prefix"
  "/home/se1d/PGW/out/build/GCC 13.3.0 x86_64-linux-gnu/_deps/httplib-subbuild/httplib-populate-prefix/tmp"
  "/home/se1d/PGW/out/build/GCC 13.3.0 x86_64-linux-gnu/_deps/httplib-subbuild/httplib-populate-prefix/src/httplib-populate-stamp"
  "/home/se1d/PGW/out/build/GCC 13.3.0 x86_64-linux-gnu/_deps/httplib-subbuild/httplib-populate-prefix/src"
  "/home/se1d/PGW/out/build/GCC 13.3.0 x86_64-linux-gnu/_deps/httplib-subbuild/httplib-populate-prefix/src/httplib-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/se1d/PGW/out/build/GCC 13.3.0 x86_64-linux-gnu/_deps/httplib-subbuild/httplib-populate-prefix/src/httplib-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/se1d/PGW/out/build/GCC 13.3.0 x86_64-linux-gnu/_deps/httplib-subbuild/httplib-populate-prefix/src/httplib-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
