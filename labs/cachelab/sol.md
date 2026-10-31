# Part A

## summary of section 6.4

- 본 요약의 목적? 캐시의 구조를 정확히 파악하고 s, E, b에 따른 캐시 구조 및 동작원리를 정확히 파악하기.
- s -> set index의 비트수, E -> set 당 라인수, b -> block offset의 비트수.

### 6.4.1. 일반적인 캐시의 구조

- 먼저 m비트가 있다고 치고, 이 비트로 메모리 주소가 구성된다고 치자. 다시 말하자면 2^m임.  
- 캐시에서 이 메모리 공간은 S개의 cache set으로 구성되어 있다. 그래서 set index는 S = 2^s를 만족하는 s임.
- set은 line인 E로 구성되어있다. 이때 line은 하나일 수도, 여러개일 수도.
- line은 block인 B로 구성되어있다. B = 2^b를 만족하는 b를 기억하고.
- 마지막으로 1비트짜리 valid bit이 있어서 정말 의미있는 정보가 들어있는지 아닌지를 말한다 정리하자면,
- 하나의 line은 살펴보자. 공간 안에 여러개의 set, set 안에 여러개의 line이 있을 '수'도 있음을 기억하면서.
- | valid bit(1bit) | tag index (t) | B (0, ..., B-1 bytes로 구성) | <- line.

### 캐시라인을 찾기 위한 메모리 주소

- 지금까지 우리는 cache가 어떻게 생겼는지를 살펴봤다. set 안에 line, line은 valid bit, tag, blocks로 구성되어있다.
- 우리는 메모리 주소를 통해서 캐시의 위치를 표현하고 싶다. 그럼 m비트로 어떻게 표현할 수 있을까?
- 전체 m개의 비트 중에서 s와 b는 빼야한다. s와 b는 표현해주어야 하니까. 그럼 남은 건? t로 쓴다.
- 다시 말하자면 m비트의 메모리주소를 통해, tag(t) / set(s) / offset(b) 형태로 표현할 수 있게 된다. m비트로 캐시주소를 표현할 수 있게 되는 것.
  - 이게 일종의 캐시주소다. 해당 주소를 통해서 "캐시에 내가 원하는 값이 있는가?"를 본다.
- 자연스럽게 tag index는 t와 동일한 값이 되고.

### 동작과정

- cpu에서 메모리에 있는 거 읽으라는 요청 오면 캐시에 메모리 존재 여부를 확인함. 그럼 어떻게 존재여부를 파악하는가?
- set 보고, tag 매칭 여부 보고, valid bit 1인 거 보고 모두 well-matched면 byte offset 계산해서 가져옴.
- 조금 더 구체화시키자. cpu가 어떤 지점의 메모리를 참조하려고 하고, 캐시를 확인할 때 어떤 일이 벌어지는가?
  - cpu가 메모리 주소 A(예: 0x1234)의 데이터를 요청한다고 가정한다.
  - 주소 A를 tag / set index / block offset으로 바꾼다.
    - 단순한 수학 계산으로 가능. 이 메모리가 캐시에 있다면 xx에 있다라는 게 명확히 보이기 때문.
    - t는 line의 id가 아니다. 오히려 블록의 id에 가깝다. 메모리 주소는 "이게 캐시에 있으면 어느 set / offset에 있다"는 결정해주지만 line 위치까지 결정하지 않는다. 대신 line에 저장됐을 때 그걸 확인할 수 있는 index는 제공한다. 그게 tag index임.
  - cache에서 확인한다.

### 6.4.2 Direct-Mapped caches

- set에 line이 몇 개 있느냐에 따라 캐시의 종류가 구분됨. set당 line이 하나다? directed-mapped cache라고 하고 가장 단순한 형태임.
- 이건 3단계로 구분된다고 함. 1) set selection, 2) line matching, 3) word extraction
  - set selection을 보자. 메모리주소는 t, s, b로 구성되어 있으므로 가운데 s값을 추출한다.
  - line matching은? 필요가 없다. 이유는 set에 라인이 하나니까. valid bit이 1이면 hit, 아니면 miss.
- 그럼 eviction을 생각해보자.
  - 먼저 언제 miss가 발생하는가? set을 찾았다. valid bit가 0이거나, tag가 다르거나. 이러면 miss.
  - 그럼 어떤 애를 지우고(eviction), 다시 써야하나? 답은 명확. 바로 그자리(set index가 일치하는!)

### 6.4.3 set-associative caches

- 라인이 하나면 쉽게 miss할 수밖에 없음. 이런 문제로 라인이 여러 개인 set associative cache가 나온 거임.
  - 라인의 수가 몇개 있을 수 있나. 1 < E < C / B가 가능. 이런 걸 E-way set associative cache라고 함.
  - 여기서 C는 캐시 전체의 크기, B는 라인 하나에서의 블록 크기를 의미함.
- set selection은 direct-mapped cache랑 같다. 상식적으로 라인에서만 다를 것.

### Line Matching and Word selction in set associative caches

- 이제는 라인이 여러개니까 조금더 정교하게 매칭해야함. 어떻게 매칭할 것인가?
- 전통적인 메모리 모델은 "주소"를 주면 그 주소에 저장된 "값"을 준다고 한다. 그러나 associative memory는 다르고, set-associative cache는 associatvie memory로 볼 수 있다.
- 그럼 associative memory가 뭔데? key, value가 있고 key를 입력으로 받으면, 일치되는 key에 대한 value를 주는 것. 파이썬의 딕셔너리 같은.
- 그래서 key를 tag & valid bit로 본다고 한다. value는 contents of blocks고. 별로 어렵지 않은 이야기를 형식화한 듯.

### miss 발생시 라인 대체하기

- 이제는 라인이 여러개니까 대체할 때 뭘 선택할지 고민해야한다.
- 깊게는 안 다루고, LFU / LRU 같은 게 있다는 언급.

### 6.4.4. fully associative caches

- 이제 direct mapped caches에 대척점에 있는 캐시임. set g하나.

# Part B

## summary of section 6.5 / 6.6
