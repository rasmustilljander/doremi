//struct GSOutput
//{
//	float4 pos : SV_POSITION;
//};
//
//[maxvertexcount(3)]
//void main(
//	triangle float4 input[3] : SV_POSITION, 
//	inout TriangleStream< GSOutput > output
//)
//{
//	for (uint i = 0; i < 3; i++)
//	{
//		GSOutput element;
//		element.pos = input[i];
//		output.Append(element);
//	}
//}
//


cbuffer TextureDataBuffer : register(b0)
{
    float2 position;
    float2 origo;
    float2 halfsize;
    float2 txtPos;
    float2 txtSize;
    float2 filler;
};

struct GeometryOutput
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

struct GeometryInput
{
    uint vertexId : INDEX;
};

[maxvertexcount(6)]
void GS_main(point GeometryInput input[1], inout TriangleStream<GeometryOutput> outStream)
{
    GeometryOutput output1; // Top right
    GeometryOutput output2; // Top left
    GeometryOutput output3; // Bot right
    GeometryOutput output4; // Bot left

                            // Position should be position + origo +- halfwidth
                            // DirectX is -1 left, -1 bot,
                            // If input is 0->1 , we multiply with 2, then -1 in X
                            // If 0 is top, we multiply with 2
    output1.position = float4((position.x + origo.x - halfsize.x)*2.0f - 1.0f, (position.y + origo.y - halfsize.y)*-2.0f + 1.0f, 0, 1);
    output2.position = float4((position.x + origo.x + halfsize.x)*2.0f - 1.0f, (position.y + origo.y - halfsize.y)*-2.0f + 1.0f, 0, 1);
    output3.position = float4((position.x + origo.x - halfsize.x)*2.0f - 1.0f, (position.y + origo.y + halfsize.y)*-2.0f + 1.0f, 0, 1);
    output4.position = float4((position.x + origo.x + halfsize.x)*2.0f - 1.0f, (position.y + origo.y + halfsize.y)*-2.0f + 1.0f, 0, 1);

    output1.texCoord = float2(txtPos.x, txtPos.y);
    output2.texCoord = float2(txtPos.x + txtSize.x, txtPos.y);
    output3.texCoord = float2(txtPos.x, txtPos.y + txtSize.y);
    output4.texCoord = float2(txtPos.x + txtSize.x, txtPos.y + txtSize.y);


    outStream.Append(output1);
    outStream.Append(output2);
    outStream.Append(output4);

    outStream.RestartStrip();

    outStream.Append(output1);
    outStream.Append(output4);
    outStream.Append(output3);
}

//[maxvertexcount(6)]
//void GS_main(point GeometryInput input[1], inout TriangleStream<GeometryOutput> outStream)
//{
//    GeometryOutput output1; // Top right
//    GeometryOutput output2; // Top left
//    GeometryOutput output3; // Bot right
//    GeometryOutput output4; // Bot left
//
//    // Position should be position + origo +- halfwidth
//    // DirectX is -1 left, -1 bot,
//    // If input is 0->1 , we multiply with 2, then -1 in X
//    // If 0 is top, we multiply with 2
//    output1.position = float4((input[0].position.x + input[0].origo.x + input[0].halfsize.x)*2.0f - 1.0f, (input[0].position.y + input[0].origo.y + input[0].halfsize.y)*-2.0f + 1.0f, 0, 1);
//    output2.position = float4((input[0].position.x + input[0].origo.x - input[0].halfsize.x)*2.0f - 1.0f, (input[0].position.y + input[0].origo.y + input[0].halfsize.y)*-2.0f + 1.0f, 0, 1);
//    output3.position = float4((input[0].position.x + input[0].origo.x + input[0].halfsize.x)*2.0f - 1.0f, (input[0].position.y + input[0].origo.y - input[0].halfsize.y)*-2.0f + 1.0f, 0, 1);
//    output4.position = float4((input[0].position.x + input[0].origo.x - input[0].halfsize.x)*2.0f - 1.0f, (input[0].position.y + input[0].origo.y - input[0].halfsize.y)*-2.0f + 1.0f, 0, 1);
//
//    output1.texCoord = float2(input[0].txtPos.x                     , input[0].txtPos.y);
//    output2.texCoord = float2(input[0].txtPos.x + input[0].txtSize.x, input[0].txtPos.y);
//    output3.texCoord = float2(input[0].txtPos.x                     , input[0].txtPos.y + input[0].txtSize.y);
//    output4.texCoord = float2(input[0].txtPos.x + input[0].txtSize.x, input[0].txtPos.y + input[0].txtSize.y);
//
//
//    outStream.Append(output1);
//    outStream.Append(output2);
//    outStream.Append(output4);    
//
//    outStream.RestartStrip();
//
//    outStream.Append(output1);
//    outStream.Append(output4);
//    outStream.Append(output3);
//}