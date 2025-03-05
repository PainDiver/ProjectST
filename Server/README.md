데디케이트서버 없이 StandAlone에서 매칭이나,로그인이나, 뭐 그런 DB나 잡다한 역할을 하려고 만들어진 서버임


>프로토버퍼
프로토버퍼를 이용해서 C#서버와 C++ 클라 둘다 패킷직렬화를 자동화시키는 프로젝트도 포함함

언리얼 클라 C++, 서버 C#이기 때문에 C# 서버 Sln에서 NuGetPackage로 ProtoBuffer를 다운로드해도 C++ 클라이언트에는 소스파일,lib,dll이 다 없어서 쓸수없음

그래서 해야하는게

C# 서버
1. NuGetPackage로 ProtoBuffer를 다운


C++ 언리얼 클라
1. vcpkg를 이용해 설치 (이건 ProtoBuf 깃 들어가면 다 나와있음)

2. vcpkg 내 Installed 폴더에서 C++에서 필요한 소스파일,lib,dll을 가져오기

3. 언리얼 클라에서 사용하는 플러그인의 build.cs에 dll,lib 연결, 소스파일은 경로설정

4. 나 같은 경우는 STNet을 플러그인으로 따로 빼서 Uplugin의 loadingPhase를 PreDefault로 바꿈

5. 정의되지 않은 전처리기 2개 port_def.inc에 따로정의함 (build.cs에 정의하면 작동안됨)
#define PROTOBUF_BUILTIN_ATOMIC 0
#ifdef _DEBUG
#define	PROTOBUF_ENABLE_DEBUG_LOGGING_MAY_LEAK_PII 1
#else
#define	PROTOBUF_ENABLE_DEBUG_LOGGING_MAY_LEAK_PII 0
#endif
