set(RawExampleSrc
    "rawExample/RawServer.cpp"
)

link_libraries(bearnet)
add_executable(rawServer ${RawExampleSrc})
