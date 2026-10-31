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
- 자연스럽게 tag index는 t와 동일한 값이 되고.

### 동작과정

- cpu에서 메모리에 있는 거 읽으라는 요청 오면 캐시에 메모리 존재 여부를 확인함. 그럼 어떻게 존재여부를 파악하는가?
- set 보고, tag 매칭 여부 보고, valid bit 1인 거 보고 모두 well-matched면 byte offset 계산해서 가져옴.

### 6.4.2 Direct-Mapped caches

- set에 line이 몇 개 있느냐에 따라 캐시의 종류가 구분됨. set당 line이 하나다? directed-mapped cache라고 하고 가장 단순한 형태임.
- 이건 3단계로 구분된다고 함. 1) set selection, 2) line matching, 3) word extraction
  - set selection을 보자. 메모리주소는 t, s, b로 구성되어 있으므로 가운데 s값을 추출한다.
  - line matching은? 필요가 없다. 이유는 set에 라인이 하나니까. valid bit이 1이면 hit, 아니면 miss.
- 그럼 eviction을 생각해보자.
  - 먼저 언제 miss가 발생하는가? set을 찾았다. valid bit가 0이거나, tag가 다르거나. 이러면 miss.
  - 그럼 어떤 애를 지우고(eviction), 다시 써야하나? 답은 명확. 바로 그자리(set index가 일치하는!)

### 6.4.3 set-associative caches

-

# Part B

## summary of section 6.5 / 6.6
