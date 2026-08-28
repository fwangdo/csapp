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

- DRAM은 마더보드에 memory module이라는 형태로 존재함.
  - 예: 64MB의 경우 하나당 8M인 칩이 8개 존재하는 형태로.
  - 이때 메인 메모리는 각 DRAM의 supercell로 구성된다. 구체적으로, 메인메모리의 word(8bytes)를 생각해보자.
    - 낮은 순서의 DRAM chip부터 낮은 order의 바이트를 채워간다. 예를 들어 0번째 dram이 word의 0 -> 7을 채우는 식.
    - 다시 말하자면 모든 dram의 i, j 인덱스가 가리키는 원소가 이어붙어져서 메인메모리를 구성하게 되는 것.

### Nonvolatile Memory

- 전원공급이 끊기면 상태를 보존할 수 없는 SRAM / DRAM과 달리 ROM(read-only memory)은 파워공급 없이도 상태 보존이 가능하다.
  - 프로그래밍이 가능한 여러형태의 ROM도 존재(PROM, EPROM 등!)
- Flash memory: 상태보존(nonvolatile)이고 solid state disk(SSD)로 많이 알려져있음.

### 메인 메모리 접근

- cpu와 DRAM은 도관(conduits)을 통해 정보를 교환하고, 이걸 bus라고 부름. 따라서 cpu <-> dram은 bus transaction을 통해서 정보 교환
  - read transaction은 메모리에서 칩으로. write transaction은 칩에서 메모리로.
- cpu와 메인 메모리(DRAM) 사이에는 io bridge가 존재함.
  - 이때 cpu -> memory를 system bus, memory -> cpu를 memory bus라고 칭하며, 두 종류 버스 모두 io bridge를 통해서 전달됨.
- RAM과 구분되는 매우 큰 저장소인 disk도 있고, os에서는 disk를 B개의 가상 섹터 시퀀스로 표현함(0부터 B-1까지의 인덱스)

### IO bus

- system / memory bus와 구분되며, i/o 기기들을 위한 bus인 io bus도 존재함.
- usb가 대표적이며 다른 io 기기들에게 정보를 전달하기 위해 사용됨.
- I/O에게 정보를 전달하는 방법? memory-mapped i/o.
  - 기본 접근: 각 io 기기마다 기기에 대응하는 메모리 주소를 할당하고, 이를 io port라고 함.
- DMA: direct memory access의 줄임말로 cpu의 개입 없이 메모리간 정보를 이동(e.g., disk -> main memory, vice versa)을 수행하는 것을 의미.

### SSD

- flash translation layer라는 특정한 영역에 펌웨어가 설치되어 logical block -> physical layer 주소를 대응시켜줌.
- read가 write보다 빠르다. 이유? write의 경우 실행시 page 단위를 다 지우고, 해당 위치에 write를 수행하기 때문
  - write는 모든 비트를 1로 변환하는 작업
  - 이때 소실되는 일부 정보들은 별도의 복사를 수행해야하므로, 복사 비용도 별도로 존재.

## 6.2 Locality

### Basic idea

- locality는 두 축으로 나누어짐. temporal / spatial.
- temporal: 시간축. "이전에 접근했던 데이터를 얼마나 가까운 시간 안에 또다시 사용할 것인가?"
- spatial: 공간축. "이전에 접근했던 데이터와 인접한 데이터를 가까운 시간 안에 사용할 것인가?"

### stride-k

- stride-k pattern: 연속된 메모리에서 k칸씩 건너뛰며 참조(reference)하는 패턴을 의미함.
  - k 값이 커질수록 spatial locality는 떨어짐. 참조되는 메모리마다 인접성이 떨어지니까.
- row-major order: 두 개 이상의 중첩된 loop이 있고 2차원 배열을 순회한다고 가정했을 때, 행(row)을 우선적으로 돌면(=바깥쪽 loop이 행기준) row-major order.
  - 일반적인 배열의 메모리 저장 방식과 궁합이 좋음. 행을 기준으로 연속저장하기 때문에 공간 locality가 좋다.

### cache에 대한 신기한 사실들

- 하나 신기한 점은 isa-level에서도 locality가 존재할 수 있다는 것.
  - 예컨대 loop의 경우 동일한 작업을 반복하고(e.g., mov), 이러한 이유로 동일한 명령어 바이트를 메모리에서 불러올 필요가 없음.
  - 여기서, "결국 실행이라는 게 메모리를 보아야 알 수 있는 건데, 다음 실행이 캐시에 있는지 아닌지를 어떻게 판단할 수 있는가"를 질문할 수 있음.
  - A. cpu는 다음에 실행할 명령어의 주소(pc)를 알고 있고, 해당 주소의 데이터가 캐시에 있는지 없는지 여부만 판단하면 됨.

## 6.3 메모리 계층
