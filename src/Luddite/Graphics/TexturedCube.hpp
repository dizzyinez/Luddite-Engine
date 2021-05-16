using namespace Diligent;
namespace TexturedCube
{
RefCntAutoPtr<IBuffer> CreateVertexBuffer(IRenderDevice* pDevice)
{
        // Layout of this structure matches the one we defined in the pipeline state
        struct Vertex
        {
                float3 pos;
                float2 uv;
        };

        // Cube vertices

        //      (-1,+1,+1)________________(+1,+1,+1)
        //               /|              /|
        //              / |             / |
        //             /  |            /  |
        //            /   |           /   |
        //(-1,-1,+1) /____|__________/(+1,-1,+1)
        //           |    |__________|____|
        //           |   /(-1,+1,-1) |    /(+1,+1,-1)
        //           |  /            |   /
        //           | /             |  /
        //           |/              | /
        //           /_______________|/
        //        (-1,-1,-1)       (+1,-1,-1)
        //

        // clang-format off
        Vertex CubeVerts[] =
        {
                {float3(-1, -1, -1), float2(0, 1)},
                {float3(-1, +1, -1), float2(0, 0)},
                {float3(+1, +1, -1), float2(1, 0)},
                {float3(+1, -1, -1), float2(1, 1)},

                {float3(-1, -1, -1), float2(0, 1)},
                {float3(-1, -1, +1), float2(0, 0)},
                {float3(+1, -1, +1), float2(1, 0)},
                {float3(+1, -1, -1), float2(1, 1)},

                {float3(+1, -1, -1), float2(0, 1)},
                {float3(+1, -1, +1), float2(1, 1)},
                {float3(+1, +1, +1), float2(1, 0)},
                {float3(+1, +1, -1), float2(0, 0)},

                {float3(+1, +1, -1), float2(0, 1)},
                {float3(+1, +1, +1), float2(0, 0)},
                {float3(-1, +1, +1), float2(1, 0)},
                {float3(-1, +1, -1), float2(1, 1)},

                {float3(-1, +1, -1), float2(1, 0)},
                {float3(-1, +1, +1), float2(0, 0)},
                {float3(-1, -1, +1), float2(0, 1)},
                {float3(-1, -1, -1), float2(1, 1)},

                {float3(-1, -1, +1), float2(1, 1)},
                {float3(+1, -1, +1), float2(0, 1)},
                {float3(+1, +1, +1), float2(0, 0)},
                {float3(-1, +1, +1), float2(1, 0)}
        };
        // clang-format on

        BufferDesc VertBuffDesc;
        VertBuffDesc.Name = "Cube vertex buffer";
        VertBuffDesc.Usage = USAGE_IMMUTABLE;
        VertBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
        VertBuffDesc.uiSizeInBytes = sizeof(CubeVerts);
        BufferData VBData;
        VBData.pData = CubeVerts;
        VBData.DataSize = sizeof(CubeVerts);
        RefCntAutoPtr<IBuffer> pCubeVertexBuffer;

        pDevice->CreateBuffer(VertBuffDesc, &VBData, &pCubeVertexBuffer);

        return pCubeVertexBuffer;
};

RefCntAutoPtr<IBuffer> CreateIndexBuffer(IRenderDevice* pDevice)
{
        // clang-format off
        Uint32 Indices[] =
        {
                2, 0, 1, 2, 3, 0,
                4, 6, 5, 4, 7, 6,
                8, 10, 9, 8, 11, 10,
                12, 14, 13, 12, 15, 14,
                16, 18, 17, 16, 19, 18,
                20, 21, 22, 20, 22, 23
        };
        // clang-format on

        BufferDesc IndBuffDesc;
        IndBuffDesc.Name = "Cube index buffer";
        IndBuffDesc.Usage = USAGE_IMMUTABLE;
        IndBuffDesc.BindFlags = BIND_INDEX_BUFFER;
        IndBuffDesc.uiSizeInBytes = sizeof(Indices);
        BufferData IBData;
        IBData.pData = Indices;
        IBData.DataSize = sizeof(Indices);
        RefCntAutoPtr<IBuffer> pBuffer;
        pDevice->CreateBuffer(IndBuffDesc, &IBData, &pBuffer);
        return pBuffer;
}

RefCntAutoPtr<ITexture> LoadTexture(IRenderDevice* pDevice, const char* Path)
{
        TextureLoadInfo loadInfo;
        loadInfo.IsSRGB = true;
        RefCntAutoPtr<ITexture> pTex;
        CreateTextureFromFile(Path, loadInfo, pDevice, &pTex);
        return pTex;
}
}