공부하면서 만드는 포트폴리오
Dedicated Server 기반 AI와 Player 전투 시스템 위주로 만들예정


[현재 구현 항목]


데이터 매니저 - JSON 데이터를 Unreal Struct형태의 테이블이나 UDataTable을 들고있어서 전역적으로 접근해서 데이터 ID에 따른 구조체 추출하는기능

GAS - 서버 환경아래 작동할 수 있도록 초기화, AttributeSet 데이터매니저를 통해 캐릭터 ID별로 초기화 하도록 수정 

인풋 - 데이터 매니저를 통해 캐릭터 ID 별 IA,GA,로직 데이터바인딩, C++에 함수구현 후 Enum으로 노출되어있다는 가정하에 블루프린트로만으로도 인풋로직바인딩이 가능

콤보 - ComboManagingComponent를 만들어서 캐릭터의 상태에 따라서 ComboContext를 선택하고, 현재 상황에 가장 알맞는 콤보를 중개하여 실행시키는 시스템을 제작

아이템 - 인벤토리, 컨테이너형 인벤토리(벨트, 주머니..) 같은 아이템시스템 FastArray형태로 관리하여 제작, 데이터의 경우 아이템의 동적데이터만 리플리케이션해서 네트워크 코스트 최적화

레벨 - 타이틀화면 꾸미기 및 데디케이트 서버 접속기능(아직은 IP로), 랜드스케이프 브러쉬나 폴리지 절차생성으로 간단한 게임맵 생성

[TODO LIST]

서버 - 로비와 데디를 중개해주는 C#서버와 클라통신시스템 제작 (가능하면 DB도 사용예정)

GA - GA 추가 및 디테일 추가 필요

애니메이션 -  캐릭터의 상태 별 기본 동작 구현 및 세부동작(워핑,모션매칭,IK)구현,전투이동(대쉬)같은 로직 모션워핑을 이용해 구현

피,타격 -  현재 애니메이션에 맞는 적 탐지 시스템 (메쉬부착형태, 박스형태), 탐지 후 공격처리를 하는 파이프라인 제작(해당 공격에 대한 정보를 담는 HitInfo Data형태로 제작 필요)

기본 이동 -  캐릭터무브먼트 확장 

네트워크 동기화 -  GAS에서는 Client Prediction 실패 시, 커스텀롤백을 하는 기능 제작, CharacterMovement의 경우 서버의 동기화처리 ServerMove_PerformMovement 커스터마이징, 클라의 MovePacket 전송데이터 FCharacterNetworkMoveData 커스터마이징, 서버의 경우 캐릭터의 위치 같은 시간간격으로 캐싱해 두어 복원하는 시스템 제작

UI - 기본적인 HP나 스태미너같은 요소 보여주는 용도로 제작

인터랙션 - 인터랙션컴포넌트를 제작하여 지니기만해도 상호작용이 가능한 만능형으로 만들예정

기타 - 신박한기능이나 액션 추가
