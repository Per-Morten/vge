#!/bin/bash

# $1: Build target
# $2: Build type
vge_internal_compile()
{
    if [[ ${1} == "editor" ]];
    then
        cmake -DCMAKE_BUILD_TYPE=${2} ${VGE_ROOT_DIR}/editor/src
    fi

    if [[ ${1} == "engine" ]];
    then
        local toolset=""
        if [[ "$(uname)" == "MINGW64_NT-10.0" ]];
        then
            local toolset=-T"v141_clang_c2"
        fi
        cmake ${toolset} -DCMAKE_BUILD_TYPE=${2} ${VGE_SOURCE_DIR}
    fi

    if [[ "$(uname)" != "MINGW64_NT-10.0" ]];
    then
        make "-j8";
    fi
}

vge_compile()
{
    pushd ${VGE_ROOT_DIR} >/dev/null
    local build_type="release"
    local build_target="engine"

    for i in `seq 1 ${#}`;
    do
        if [[ ${!i} =~ -o ]];
        then
            local next=$((i+1));
            build_target=${!next,,};
        fi

        if [[ ${!i} =~ -t ]];
        then
            local next=$((i+1));
            build_type=${!next,,};
        fi

        if [[ ${!i} =~ -h ]];
        then
            printf "Usage: "
            printf "\n\t-o target (editor, engine)"
            printf "\n\t-t type (debug, release, test, rebuild | clean)"
            printf "\n\n"
        fi
    done

    if [[ ${build_type} =~ clean ]];
    then
        rm -rf ./build
        return
    fi

    printf "Building target: %s - %s\n" ${build_target} ${build_type}

    mkdir -p build/${build_target}/${build_type}
    pushd build/${build_target}/ >/dev/null

    vge_internal_compile ${build_target} ${build_type,,^}
    local compile_res=${?}

    popd >/dev/null
    popd >/dev/null

    return ${compile_res}
}

vge_build_tests()
{
    pushd ${VGE_ROOT_DIR} >/dev/null
    if [ ! -d "build" ];
    then
        mkdir build
    fi
    pushd build >/dev/null

    # Setup test
    if [ ! -d "test" ];
    then
        mkdir test
    fi

    pushd test >/dev/null
    cmake -DCMAKE_BUILD_TYPE=Test -DVGE_ENABLE_TESTS:BOOLEAN=1 ${VGE_SOURCE_DIR}
    make "-j8"
    local compile_res=${?}

    popd >/dev/null

    # Pop out of the rest
    popd >/dev/null
    popd >/dev/null

    return ${compile_res}
}

vge_run_tests()
{
    pushd ${VGE_ROOT_DIR} >/dev/null
    cd build/game/test/bin
    for file in ./*
    do
        ./${file} "$@"
    done

    popd >/dev/null
}

vge_continous_game()
{
    # Main idea taken from: https://stackoverflow.com/a/9461685
    chsum1=""
    while [[ true ]]
    do
        chsum2=$(find ${VGE_SOURCE_DIR} ${VGE_RESOURCE_DIR} -type f -exec md5sum {} \;)
        if [[ $chsum1 != $chsum2 ]] ; then
        clear

            # Get window currently in focus
            work_focus=$(xprop -root | awk '/_NET_ACTIVE_WINDOW/ {print $5; exit}')

            # Launch compilation
            vge_compile -o engine -t release

            if [[ $? -eq 0 ]];
            then
                # Kill potentially running ym application
                vge_window_id=$(wmctrl -l | grep -E "vulkan_game_engine$" | awk '{ print $1}')
                identified_count=$(echo ${vge_window_id} | wc -l)

                if [[ identified_count -gt 1 ]];
                then
                    printf "Warning, found more than 1 instance of ym: %s\n" ${ym_window_id}
                    wmctrl -l
                else
                    wmctrl -i -c ${vge_window_id} >> /dev/null 2>&1
                fi

                # Run build and set focus back to sublime
                ./build/engine/release/bin/main &

                # Get pid, used to check if it exists in ps | grep
                local vge_pid=$!
                vge_pid=$(ps | grep "${vge_pid}")

                sleep 0.25

                # Find new ym window and move it.
                local new_window_id=""
                while [[ ((-z ${new_window_id+x} || ${new_window_id} == "")) && $(ps | grep "${vge_pid}") != "" ]]
                do
                    new_window_id=$(wmctrl -l 2>/dev/null | grep -E "vulkan_game_engine$" | awk '{ print $1}')
                done
                wmctrl -i -r ${new_window_id} -e 0,0,0,-1,-1
                wmctrl -i -a ${work_focus}
            fi

            chsum1=$chsum2
        fi
        sleep 2
    done

}

vge_continous_tests()
{
    # Main idea taken from: https://stackoverflow.com/a/9461685
    chsum1=""
    while [[ true ]]
    do
        chsum2=$(find ${VGE_SOURCE_DIR} ${VGE_RESOURCE_DIR} -type f -exec md5sum {} \;)
        if [[ $chsum1 != $chsum2 ]] ; then
        clear
            # Launch compilation
            vge_build_tests

            if [[ $? -eq 0 ]];
            then
                #clear
                vge_run_tests
            fi

            chsum1=$chsum2
        fi
        sleep 2
    done

}
