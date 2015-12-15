showroom
###########

author
---------
thomas fischer

about
-----
this is a university project
it shows basic opengl, level-of-detail and sorting of transparent objects

controls
---------
wasd    - moving around
mouse   - looking around
1       - toggle transparent sorting
2       - toggle FPS info
3       - set light position to current camera position

build
------
    packages
    ---------
    -) tinyobjloader -> https://syoyo.github.io/tinyobjloader/
    -) SFML 2.3.2 for Visual Studio 2015 C++ 64Bit -> http://www.sfml-dev.org/download/sfml/2.3.2/
    
    folder structure
    -----------------
                          [root]
                /            |             \
       tinyobjloader    SFML-2.3.2       Showroom
                                            (contains Showroom.sln)

    know issues
    ------------
    -) please don't start the project in debug mode when you want to load big (>5mb) obj files, it is horribly slow
    -) no textures on transparent materials

credit
-------
    -) abarth model by ForgetTheNorm    -> http://www.blendswap.com/user/ForgetTheNorm
    -) bmw model by mikepan             -> http://www.blendswap.com/user/mikepan
