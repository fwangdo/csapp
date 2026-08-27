# 6. Memeory hierarchy

## 6.0

- locality: 프로그램이 동일한 메모리 혹은 인접한 메모리에 계속 접근하는 것.
  - 이 행동이 많으면 많을수록 상위 레벨(upper level) 메모리에 접근할 가능성이 높아짐.

## 6.1. storage technologies

- 큰 구분 SRAM vs DRAM
- SRAM: 비싸고 빠르다. cpu의 안과 밖에서 주로 캐시로 사용되는 형태.
  - 두 개의 stable 상태가 존재(왼쪽 / 오른쪽).
  - stable 상태는 파워가 공급되는 한 변하지 않음. 따라서 노이즈에 상대적으로 강함.
- DRAM: 상대적으로 싸고 느리다. 메인메모리로 사용되는 경우가 많음.
  - charge를 통해서 정보를 표현하는 방식. 문제는 시간이 지남에 따라 에너지가 소실되는 경우 존재.
  - 해당 문제를 해결하기위해 주기적으로 정보를 복원시키는 방법을 사용

### Conventional DRAM

- d개의 supercell과 w개의 dram cell로 구성
  - d는 2차원 배열을 생각하면 되고, w는 배열의 각 원소의 크기라고 볼 수 있음.
  - 연결자인 pin을 통해 외부 메모리와 소통함. pin은 1 비트씩 전달함.
    - 예를 들어 addr정보가 필요한 경우 8개의 pin으로 전달.
    - supercell 좌표가 필요한 경우? 2개의 pin으로 전달(i, j in d).

### RAS & CAS

- DRAM은 메모리 컨트롤러와 연결되어 시간당 w비트를 전송함.
- 이때 전송되는 비트는 supercell 좌표 i, j에 대응되는 원소임.
- 이때 row 좌표 i를 RAS, column 좌표 j를 CAS라고 칭함.
- 방식은 조금 독특함. row buffer라는 것이 dram의 내부에 있고,
  - i를 봤을 때 i row의 모든 정보를 row buffer에 넣은 다음,
  - j에 대응되는 값을 가지고 오는 형태로 동작.

### Supercell의 특징

-
