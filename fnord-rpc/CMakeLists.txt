project(FNORD_RPC)

include_directories(../)

add_library(fnord-rpc OBJECT
    RPC.cc
    RPCClient.cc
    ServerGroup.cc)
