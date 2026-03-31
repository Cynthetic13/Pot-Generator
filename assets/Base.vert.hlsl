struct Input {
    float4 position : POSITION;
    float4 color : TEXCOORD0;
};

struct Output {
    float4 position : SV_POSITION;
    float4 color : TEXCOORD0;
};

Output main(Input input) {
    Output o;
    o.position = input.position;
    o.color = input.color;
    return o;
};