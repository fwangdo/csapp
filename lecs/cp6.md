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

### cache in memory

- 메모리 구조를 고려할 때, 특정 층 k는 그 아래층 k + 1의 일부 정보를 "캐싱"한다.
  - 둘 다 동일한 블록 크기를 가정하고 k는 그 아래층의 일부 블록을 소유하고 있는 식.
  - 인접해있는 두 계층은 동일한 블럭크기를 공유하지만, 다른 층들은 그렇지 않을 수 있다. 레지스터와 하드의 블록크기는 다름.
- 찾고자 하는 데이터가 캐시에 있으면 cache hit 그렇지 않으면 miss.
  - miss가 일어나는 경우 데이터를 가져와야 하므로 덮어쓰기(overwriting)가 필연적이고, 덮어써지는 block을 victim block이라고 부름
  - "어떤 block을 덮어쓸 것인가"는 중요한 문제이고, LRU(가장 적게 사용된 블럭 사용)가 자주 쓰임.

### 캐시 미스의 종류

- cold cache: 캐시 자체가 비워져있을 때 발생하는 것. 필연적인 것이고, 웜업 이후엔 등장하지 않음.
- miss시 어떤 block에 아랫 단계의 정보를 쓸 것인가는 중요한 문제. 단, 빠르게 일어나야 하다보니 복잡한 알고리즘은 사용 불가능  
- 캐시를 저장할 때 마구잡이로 저장하면, 정보를 찾을 때 불편해진다. 적절한 인덱싱이 필요 예를 들면,
  - k + 1 계층의 block index에 modulo 4를 한 값을 사용할 수도. 예: 5 -> 1.
- conflict miss: 캐시 공간이 충분함에도 위와 같이 인덱싱함으로 동일한 캐시 블록을 매핑함으로서 발생하는 문제.
  - 근본적으로 해싱에서 발생하는 conflict과 동일.
- capacity miss: 캐시 크기 자체가 필요한 양보다 작은 경우를 의미
  - 예: nested loop에서 동일한 원소를 계속 접근(working set)할 때, 해당 원소들의 크기보다 캐시크기가 작아 miss가 발생하는 경우.

### 캐시 관리

- 캐시를 블록 단위로 나누어 관리하는 것을 cpu에서는 cache line, os에서는 page라고 부름

### 캐시와 메모리주소, 접근과정

- 캐시는 Set S, S 안에 Line E, E 안에 블록 B가 존재하는 형태다.
  - S는 말 그대로 일종의 집합이고, E는 S의 원소이다. B는 하나의 E가 저장하는 데이터 블록의 바이트수를 의미.
  - E는 하나일 수도 그렇지 않을 수도.
- cache line 안에 데이터 블록이 있고, valid / tag / b-byte data-block 이 순서대로 있는 구조.
  - valid: 0이면 의미 없는 데이터가 없음을, 1이면 유효한 데이터가 있음을 의미.
  - tag: 여러 블록이 같은 set으로 매핑될 수 있으므로 추가적인 식별자
  - block: 실제 데이터.
- 캐시의 위치를 식별할 수 있는 "메모리 주소"가 필요하고, 메모리 주소는 당연히 캐시 구조를 고려해서 설계되어야 함.
  - 구체적으로 메모리 주소는 tag / set index / block offset으로 이루어져있음.
  - 먼저 set index를 통해 set에 접근하고,
  - 캐시라인에서 태그를 찾고, 그게 valid가 1이라면 데이터를 가져옴.
  - 못 찾으면 miss.

### 계층과 사용과정 재정리

- cpu에서 특정 주소를 보고 "이게 캐시에 있나 없나 확인하는 주소는",
  - tag / set index / block offset 형태임. 정확히는 처음부터 이렇게 생겼다기보단 캐시가 이렇게 해석을 해줌.
- 캐시 내부는 valid / stored tag(찾아지는 검색인덱스) / b byte data block 형태고.
- 캐시로부터 가져와짐을 당하는 메모리는 연속된 메모리주소 배열이지만
  - tag / set index / bit offset / block number와 같이 해석할 수도 있음.
  - block number가 추가된 이유? 캐시 크기가 메모리 크기보다 작으므로 반드시 블락을 확인하는 작업이 필요하기 때문.

### set associative cache

- conflict cache가 잦은 이유: 하나의 set에 하나의 line 밖에 없기 때문. line(이하 E)을 늘리면 S 내에서도 구분을 할 수 있어 conflict을 줄일 수 있음.
  - E를 늘린 형태를 E-way set associative cache라고 칭함. 1 < E < B / C 형태.
- set associative cache는 태그와 valid bits를 concat하면 해당 block의 컨텐츠를 제공하는 일종의 associative memory(map) 형태임.
  - valid bits가 1이고 tag가 일치하는 것을 찾자마자 바로 hit함. 중복되는 태그는 존재하지 않기 때문.

### full associative cache

- E가 C / B가 되는 경우. 다시 말해 set을 하나로 두고 line을 극한으로 두는 방식.

### Write에서 발생하는 상황

- 캐시된 지역에 write를 해야하는 경우, 이를 write-hit이라고 한다.
  - 예를 들어, cpu가 주소 x를 20으로 변경하라는 지시를 내렸을 때, 캐시도 x를 포함하고 있으면 캐시 / 메모리 둘 다 변경해서 동기화해야함.
  - 이때 크게 두 가지 방법이 존재함.
- write-through: 캐시를 변경해야할 때마다, lower level 메모리에도 그대로 반영하기. 단순하고 직관적이나 비용이 큼(매번 반영해야 하니까)
- write-back: 캐시만 변경해두고, 해당 데이터가 캐시에서 쫓겨날 경우에만 메모리에 업데이트하기. 큰 효율 상승이 있으나 복잡한 요소가 들어가야.
  - 예? dirty-bit. 캐시와 메모리 간 불일치가 있을 경우 dirty-bit을 1로 변경해두어야

- 반대로 write-miss 상황도 존재할 수 있다. 이때도 방법은 크게 두가지.
  - write-allocate: 변경해야하는 걸 캐시에 올려두고 쓴다. write의 spatial locality를 기대하는 방법.
  - no-write-allocate: lower memory를 직접 변경하고, 캐시는 변경하지 않는다.
- 보통은 write-through는 no-writhe-allocate와, write-back은 write-allocate와 어울려 사용한다.
- 현대 시스템에서는 write-back & write-allocate를 가정하는 게 좋음. 특히 아래 계층의 메모리에서 write-back은 필수적임.

### i-cache / d-cache

- 캐시는 최근 i-cache / d-cache / unfied cache로도 분화.
- i-cache: instruction만 저장. d-cache: data만 저장. unifired cache 둘 다 저장.
- i와 d를 분리하면 좋은 이유? 둘을 "동시에" 처리하는 것이 가능하기 때문.
- instruction cache의 경우 따로 두면 conflict miss가 없어진다. 단, capacity miss가 발생할 가능성이 높아짐.

### 캐시 퍼포먼스에 대한 특징들

- 캐시 사이즈: 캐시가 크면 클수록 hit time(set selection, line identification, word selection / l1 캐시에서 cpu에 데이터를 전달하는 시간)이 커지기 마련.
- 블록 사이즈: 장단점이 있음.
  - 장점: 공간 locality를 고려한 캐시 힛 가능성이 높아진다. 블록사이즈가 크기 때문에 직관적으로 성립함.
  - 단점: 시간 locality에서는 좋지 않을 수도. 예를 들어 유사 시간에 사용되는 변수들이 메모리 여기저기 흩어져있는 경우. 또 한 번 miss 할 때마다 가져와야하는 데이터의 크기가 크기 때문에 miss penalty(lower memory에서 끌고오는 시간)이 길다.
- 라인 사이즈(associativity):
  - 장점: conflict miss 수를 줄여준다. 직관적으로 당연. 라인에 겹치는 게 conflict miss인데 라인수가 많아지니까.
  - 단점: 본질적으로 빠르게 만들기 어렵다! 여러 개의 라인에 대해서 LRU를 결정해야하는 등 머리 아픈 지점이 많음.
    - 하나 알아두어야 할 것은, "여러개를 조회해야하는 것" 자체는 문제가 아닐 수 있다는 것. 태그를 한 개씩 순차조회 하는 게 아니고 여러 비교기로 병렬 비교가 가능하므로.
    - 그러나 병렬비교가 가능하다고 하더라도 회로 자체가 커져야 하고(mux 생각하면 됨), 이것 자체가 느리게 만드는 요소임을 감안해야.
    - 정리하자면, 병렬비교가 가능하지만 결국 큰 회로가 필요하다.

## How to write cache-friendly code

### 원칙

- Make the common case go fast: 많은 프로그램들은 시간을 많이 소요하는 "주된 영역(e.g., inner loop"이 있음. 이 지점을 핵심으로 보아야.
- 룹에서 캐시미스 줄이기
