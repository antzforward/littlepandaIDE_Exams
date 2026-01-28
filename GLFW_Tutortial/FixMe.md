## 过程中处理的问题
### spirv编译过程
前提：并不考虑任意shader 都可以直接别官方支持的工具直接编译。
比如 要将glsl文件编译成spirv，首先这个shader 满足编译的要求
同样 hlsl也相同
#### 1. 编译glsl shader
后缀表示shader类型。
glslangValidator.exe -V shader.vert
glslangValidator.exe -V shader.frag
比较全面的如下
```json
{
  ".vert": "vert", // for a vertex shader
  ".vs": "vert", // for a vertex shader
  ".frag": "frag", // for a fragment shader
  ".fs": "frag", // for a fragment shader
  ".gs": "geom", // for a geometry shader
  ".geom": "geom", // for a geometry shader
  ".comp": "comp", // for a compute shader
  ".tesc": "tesc", // for a tessellation control shader
  ".tese": "tese", // for a tessellation evaluation shader
  ".rgen": "rgen", // for a ray generation shader
  ".rint": "rint", // for a ray intersection shader
  ".rahit": "rahit", // for a ray any hit shader
  ".rchit": "rchit", // for a ray closest shader
  ".rmiss": "rmiss", // for a ray miss shader
  ".rcall": "rcall", // for a ray callable shader
  ".mesh": "mesh", // for a mesh shader
  ".task": "task" // for a task shader
}
```

#### 2.编译 hlsl shader
这个用Microsoft自带的dxc工具，感觉不行 还是用vulkan SDK里面的DXC
dxc.exe -T vs_6_0 -E main -spirv -fspv-target-env=vulkan1.2 -Fo iconVS.spv ICONVert.hlsl
虽然可以编译，但是对opengl的代码需要改动，uniform来自cbuffer了，我都不知道如何改。不过这个cbuffer跟用
```
// Shadertoy风格的uniforms - 所有非不透明uniform都需要指定location
layout(location = 0) uniform vec2 iResolution;
layout(location = 1) uniform float iTime;
layout(location = 2) uniform float iTimeDelta;
layout(location = 3) uniform int iFrame;
layout(location = 4) uniform vec4 iMouse;
layout(location = 5) uniform mat4 mvp;
// hlsl
// 常量缓冲区
cbuffer ShaderParams : register(b0)
{
    float2 iResolution;      // 视口分辨率
    float iTime;            // 运行时间（秒）
    float iTimeDelta;       // 帧间隔时间
    uint iFrame;           // 帧数
    float4 iMouse;         // 鼠标坐标
    // 注意：纹理不放在常量缓冲区
};
```
~~算了，感觉用vulkan的时候用hlsl的话，在编译过程实在要处理的细节太多了。这个用例就先放弃了吧。~~
不行，这个问题还是要解决，支持多个backend是一般引擎的通用要求。

