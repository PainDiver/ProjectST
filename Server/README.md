### 데디케이트서버 없이 StandAlone에서 매칭이나,로그인이나, 뭐 그런 DB나 잡다한 역할을 하려고 만들어진 서버
<br>
프로토버퍼를 이용해서 C#서버와 C++ 클라 둘다 패킷직렬화를 자동화시키는 프로젝트도 포함함
<br>
<br>
언리얼 클라 C++, 서버 C#
<br>
<br>
C# 서버 Sln에서 NuGetPackage로 ProtoBuffer를 설치하고, C++ 클라이언트에는 소스파일 포함, lib,dll 연결이 필요함
<br>
<br>

> # C# 서버 프로토버퍼

1. NuGetPackage로 ProtoBuffer를 설치 (사용용도로만)


> # C++ 언리얼 클라 프로토버퍼

1. vcpkg를 이용해 설치 (이건 ProtoBuf 깃 들어가면 다 나와있음)
2. vcpkg 내 Installed 폴더에서 C++에서 필요한 소스파일,lib,dll을 가져오기 lib(libprotobuf.lib,absei_dll.lib,zlib.lib), dll(libprotobuf.dll,zlib1.dll,abseil_dll.dll)필요
3. 언리얼 클라에서 사용하는 플러그인 내 build.cs에서 lib 추가, dll은 Binaries내부에 스테이징필요, 소스파일은 경로설정
4. 정의되지 않은 전처리기 2개 port_def.inc에 따로정의함 (build.cs에 정의하면 작동안됨)
````Cpp
#define PROTOBUF_BUILTIN_ATOMIC 0
#ifdef _DEBUG
#define	PROTOBUF_ENABLE_DEBUG_LOGGING_MAY_LEAK_PII 1
#else
#define	PROTOBUF_ENABLE_DEBUG_LOGGING_MAY_LEAK_PII 0
#endif
````

> # C# 패킷 제너레이터
1. vcpkg를 이용해 설치 할 경우, tools폴더에 protoc.exe와 dll들 확보
2. 해당 폴더를 C#패킷제네레이터 프로젝트폴더에 배치 후, 프로젝트 실행으로 인자를 주면서 실행시키면됨
