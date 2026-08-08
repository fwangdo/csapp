# Chapter 4

## Y86-64 architecture

### 간단한 구성들

- 15개의 레지스터들(이름이 있는 레지스터 + r8 ~ r14)
- condition codes(flags). stats, program counter, memory.

### 명령어

- 8bytes(=64bits)를 word로 정의.
- 모든 명령어 구성은 1 ~ 10 바이트 크기를 가질 수 있고,
- 이때 가장 앞의 1바이트는 명령어의 종류를 설명하는데,
  - 앞의 4비트를 code라고 부르고, 뒤의 4비트를 function이라고 부름.
  - 동일 멍령어군에서 function 4비트로 구분되는 경우들이 존재함. 예를 들어 jmp의 다양한 종류들.

### 용어설명

- program register: 프로그래머가 보는 레지스터(e.g., \%rax)
- register file: 프로그램의 레지스터 값을 cpu 안에서 실제로 저장하는 작은 장치.
- hardware register: 하드웨어 회로에서 한 word 등을 저장하는 개별 소자. 물리적인 장치로, 프로그래머가 생각하는 레지스터(e.g., rax)와 관련 없을 수도.
- random access memory: 임의의 주소에 해당하는 word를 직접 골라 보고 쓸 수 있는 장치.

### isa 사용시 구성

- rA, rB: 레지스터 파일. operand로서 source와 dest를 표현. 필요 없는 경우엔 0xf로 표현(레지스터가 15개니까, 의미없는 숫자인 15가 사용)

### TODO -> 4.1.4
