project(FNORD_MDB)

include_directories(../)
include_directories(../3rdparty)

add_library(fnord-mdb OBJECT
    ../3rdparty/liblmdb/mdb.c
    ../3rdparty/liblmdb/midl.c
    MDB.cc
    MDBTransaction.cc)
