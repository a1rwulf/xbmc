set(SOURCES ApiMetadataProvider.cpp
            DatabaseMetadataProvider.cpp)

set(HEADERS ApiMetadataProvider.h
            DatabaseMetadataProvider.h
            MetadataProvider.h)

core_add_library(media_providers)
