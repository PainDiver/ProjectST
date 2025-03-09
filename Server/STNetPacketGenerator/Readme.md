# C# 패킷제네레이터
<br>

Protobuf로 패킷의 직렬화를 자동생성한 후, C# 서버, 언리얼 클라이언트에게 배치하는 용도임
<br>

# C# 서버 자동생성 출력물
<br>
1. 프로토버퍼로 만들어진 Generated파일

# 언리얼엔진 자동생성 출력물
<br>

1. 프로토버퍼로 만들어진 Generated 파일
<br>

2. Generated파일로 만들어진 프로토버퍼에 대응되는 블루프린트 노출가능한 USTRUCT()와 Delegate를 명시해주는 파일
<br>

3. 프로토버퍼 메시지를 대응되는 UStruct로 자동변환해주는 함수를 담은 Converter파일 (ToProtobuf,ToUnrealStruct라는 함수셋으로 존재하며, 이미 정의한 프로토버퍼가 메시지 내부에 있을경우, 재귀적으로 구현)
<br>

4. Client to Server의 경우 델리게이트를 선언, 송신,수신(델리게이트 호출) 로직을 UStruct <-> Protobuf 자동변환을 이용하여 수행하는 Handler 파일
