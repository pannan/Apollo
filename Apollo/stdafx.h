// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <windows.h>
#include <wrl/client.h>
#include <wrl.h>
#include <d3d11.h>
#include <vector>
#include <list>
#include <string>
#include <assert.h>
#include <memory>

#include "imgui.h"

#define GLYPH_PI 3.14159265f

#define SAFE_DELETE( x ) {if(x){delete (x);(x)=NULL;}}


// TODO:  在此处引用程序需要的其他头文件
