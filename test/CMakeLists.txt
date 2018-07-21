# gl_test
make_program("gl_test")

# wl_test
make_program("wl_test")
add_opengl("wl_test")

# al_test
make_program("al_test")
add_openal("al_test")
add_opengl("al_test")
add_vorbis("al_test")
