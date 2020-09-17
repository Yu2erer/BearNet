set(RawHeaderSrc
    "RawHeader/RawServer.cpp"
)

link_libraries(bearnet)
add_executable(RawServer ${RawHeaderSrc})
