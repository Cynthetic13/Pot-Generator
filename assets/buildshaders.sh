#!/usr/bin/env bash
help()
{
    echo ""
    echo "Usage:"
    echo "$0 -s shaderFile"
    echo -e "\t-s HLSL Shader"
    echo -e "\tMultiple shaders is supported (-s vertShader -s fragShader)"
    echo -e ""
    echo -e "\tFile must include vert or frag in the file name"
    echo -e "\tRecommend format: ShaderName.vert.hlsl or ShaderName.frag.hlsl"
    exit 1
}

compileShaders()
{
    SHADER="$1"

    echo "Compiling shader: $SHADER"

    shadercross "$SHADER" -o "${SHADER%.hlsl}.spv"
    shadercross "$SHADER" -o "${SHADER%.hlsl}.msl"
    shadercross "$SHADER" -o "${SHADER%.hlsl}.dxil"
}

shaders=()

while getopts "s:" opt
do
    case "$opt" in
        s) shaders+=("$OPTARG") ;;
        *) help ;;
    esac
done

if [ ${#shaders[@]} -eq 0 ]; then
    echo "No shaders provided."
    help
fi


for shader in "${shaders[@]}"; do
    compileShaders "$shader"
done
