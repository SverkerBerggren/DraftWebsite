﻿# CMakeList.txt : CMake project for DraftWebsiteServer, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (DraftWebsiteServer "DraftWebsiteServer.cpp" "DraftWebsiteServer.h" "Lobby.cpp" "Lobby.h" "Card.h" "Card.cpp" "DraftServer.h" "DraftServer.cpp" "sqlite3.c")



if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DraftWebsiteServer PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
target_link_libraries(DraftWebsiteServer rpcrt4)