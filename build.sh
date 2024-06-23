#!/bin/bash

# Color Codes
RED="\e[31m"
GREEN="\e[32m"
ENDCOLOR="\e[0m"

C_FLAG=''
B_FLAG=''
R_FLAG=''
V_FLAG=''
M_FLAG=''

CXX="clang++"
FLAGS="-Wall -Wextra -std=c++17 $(pkg-config --cflags glfw3 freetype2)"
INCLUDES="include -Ithird_party/imgui -Ithird_party/portable_file_dialogs -Ithird_party/fonts -Ithird_party/miniaudio -I/usr/include/freetype2"
SRC_M="src/main.cpp"
SRC="third_party/imgui/imgui.cpp third_party/imgui/imgui_draw.cpp third_party/imgui/imgui_tables.cpp third_party/imgui/imgui_widgets.cpp third_party/imgui/imgui_freetype.cpp"
SRC_B="third_party/imgui/imgui_impl_glfw.cpp third_party/imgui/imgui_impl_opengl3.cpp"
LIBS="-ltag -lGL $(pkg-config --static --libs glfw3 freetype2) -lsqlite3 -ldl -lm -lpthread"
EXE_DIR="bin"
EXE="orca"

print_usage() {
    printf "Usage: $0
            \n\t${GREEN}-b${ENDCOLOR} build app \
            \n\t${GREEN}-c${ENDCOLOR} clear ${EXE_DIR}/ \
            \n\t${GREEN}-r${ENDCOLOR} run app \
            \n\t${GREEN}-v${ENDCOLOR} verbose clang messages \
            \n\t${GREEN}-m \"-v -d\"${ENDCOLOR} other clang commands \
            \n\t${GREEN}-p${ENDCOLOR} Show this menu\n" 1>&2
    exit 1
}

if [ ! -d $EXE_DIR ]; then
    mkdir $EXE_DIR
fi

while getopts 'bcrvpm::' flag; do
    case "${flag}" in
    b) B_FLAG='true' ;;    # build
    c) C_FLAG='true' ;;    # clear bin/
    r) R_FLAG='true' ;;    # run app
    v) V_FLAG='-v' ;;      # clang verbose
    m) M_FLAG=${OPTARG} ;; # clang options
    p) print_usage ;;
    *)
        print_usage
        exit 1
        ;;
    esac
done

if [[ $C_FLAG == 'true' ]]; then
    printf "Removing files from $EXE_DIR/\n\n"
    rm -rf $EXE_DIR/$EXE
fi

if [[ $B_FLAG == 'true' ]]; then
    printf "${GREEN}[xx]${ENDCOLOR} Compiling Orca\n"

    SECONDS=0
    $CXX $FLAGS -I $INCLUDES $SRC_M $SRC $SRC_B $LIBS -o $EXE_DIR/$EXE $M_FLAG
    duration=$SECONDS
    seconds=$((duration % 60))

    printf "\nCompilation took "
    if [ "$seconds" -lt 4 ]; then
        printf "${GREEN}$((duration % 60))s${ENDCOLOR}\n"
    else
        printf "${RED}$((duration / 60))m$((duration % 60))s${ENDCOLOR}\n"
    fi
fi

if [[ $R_FLAG == 'true' ]]; then
    cd ./${EXE_DIR} && ./${EXE}
fi
