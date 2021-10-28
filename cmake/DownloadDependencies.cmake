
####
#   add dependency
####
include(FetchContent)
message(STATUS "add dependency: QtMarkdown")
FetchContent_Declare(
        QtMarkdown
        GIT_REPOSITORY https://github.com/PikachuHy/QtMarkdown.git
        GIT_PROGRESS ON
        GIT_SHALLOW ON
)
FetchContent_MakeAvailable(QtMarkdown)
message(STATUS "add dependency: QtJieba")
FetchContent_Declare(
        QtJieba
        GIT_REPOSITORY https://github.com/PikachuHy/QtJieba.git
        GIT_TAG qt
        GIT_PROGRESS ON
        GIT_SHALLOW ON
)
FetchContent_MakeAvailable(QtJieba)
message(STATUS "add dependency: QtWordReader")
FetchContent_Declare(
        QtWordReader
        GIT_REPOSITORY https://github.com/PikachuHy/QtWordReader.git
        GIT_PROGRESS ON
        GIT_SHALLOW ON
)
FetchContent_MakeAvailable(QtWordReader)
if (NOT ANDROID)
    message(STATUS "add dependency: CuteLogger")
    FetchContent_Declare(
            CuteLogger
            GIT_REPOSITORY https://github.com/PikachuHy/CuteLogger.git
            GIT_TAG qt6
            GIT_SHALLOW ON
    )
    FetchContent_MakeAvailable(CuteLogger)
endif ()
message(STATUS "add dependency: cLaTeXMath")
FetchContent_Declare(
        cLaTeXMath
        GIT_REPOSITORY https://github.com/PikachuHy/cLaTeXMath.git
        GIT_PROGRESS ON
        GIT_SHALLOW ON
        GIT_TAG openmath-fix
)
set(QT ON)
set(HAVE_LOG OFF)
FetchContent_MakeAvailable(cLaTeXMath)
