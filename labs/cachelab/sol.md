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

-

# Part B

## summary of section 6.5 / 6.6
