# - Find curl
# Find the native CURL headers and libraries.
#
#  CURL_INCLUDE_DIRS - where to find curl/curl.h, etc.
#  CURL_LIBRARIES    - List of libraries when using curl.
#  CURL_FOUND        - True if curl found.

# Look for the header file.
FIND_PATH(CURL_INCLUDE_DIR curl/curl.h
  $ENV{INCLUDE}
  "$ENV{LIB_DIR}/include"
  /usr/local/include
  /usr/include
  #mingw
  c:/msys/local/include
  NO_DEFAULT_PATH
  )

MARK_AS_ADVANCED(CURL_INCLUDE_DIR)

# Look for the library.
FIND_LIBRARY(CURL_LIBRARY NAMES curl libcurl_imp PATHS
  $ENV{LIB}
  "$ENV{LIB_DIR}/lib"
  /usr/local/lib
  /usr/lib
  c:/msys/local/lib
  NO_DEFAULT_PATH
  )

MARK_AS_ADVANCED(CURL_LIBRARY)

IF(CURL_INCLUDE_DIR)
  MESSAGE(STATUS "Curl include was found")
ENDIF(CURL_INCLUDE_DIR)
IF(CURL_LIBRARY)
  MESSAGE(STATUS "Curl lib was found")
ENDIF(CURL_LIBRARY)

# Copy the results to the output variables.
IF(CURL_INCLUDE_DIR AND CURL_LIBRARY)
  SET(CURL_FOUND 1)
  SET(CURL_LIBRARIES ${CURL_LIBRARY})
  SET(CURL_INCLUDE_DIRS ${CURL_INCLUDE_DIR})
ELSE(CURL_INCLUDE_DIR AND CURL_LIBRARY)
  SET(CURL_FOUND 0)
  SET(CURL_LIBRARIES)
  SET(CURL_INCLUDE_DIRS)
ENDIF(CURL_INCLUDE_DIR AND CURL_LIBRARY)

# Report the results.
IF(CURL_FOUND)
   IF (NOT CURL_FIND_QUIETLY)
      MESSAGE(STATUS "Found CURL: ${CURL_LIBRARY}")
   ENDIF (NOT CURL_FIND_QUIETLY)
ELSE(CURL_FOUND)
  SET(CURL_DIR_MESSAGE "CURL was not found.")

  IF(CURL_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "${CURL_DIR_MESSAGE}")
  ELSE(CURL_FIND_REQUIRED)
    IF(NOT CURL_FIND_QUIETLY)
      MESSAGE(STATUS "${CURL_DIR_MESSAGE}")
    ENDIF(NOT CURL_FIND_QUIETLY)
    # Avoid cmake complaints if CURL is not found
    SET(CURL_INCLUDE_DIR "")
    SET(CURL_LIBRARY "")
  ENDIF(CURL_FIND_REQUIRED)

ENDIF(CURL_FOUND)
