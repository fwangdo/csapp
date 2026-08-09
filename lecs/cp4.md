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
- set cc: condition code 조정. rax & rax 와 같은 연산을 수행하면 동일 값이 나오므로 cc를 보고 해당 값의 성질을 파악할 수 있음.

### 애매한 지점들

- pushq / popq: 스택 포인터를 저장할 때 rsp 주소도 저장하므로, "rsp를 변경하고 저장할 건가", "저장하고 변경할 건가"를 결정해야.
- 변경 전 값을 저장해둔다.

## 회로와 하드웨어 제어 언어(HCL)

### 구성

- 비트가 있을 때 고전압(high voltage)는 1에 저전압은 0에 매핑되도록 설정.
- 디지털 시스템을 구성하는 3요소: combinational logic, memeory elements, clock signal.

### Logic Gates

- and/ or / not으로 구성. 항상 실행(active) 중인 상태이기에, 입력이 변화하면 조금의 시간 후에 출력도 변화
- combinational circuit의 특징
  - 반드시 하나의 1)input 2)output 3)memory element의 output connection 중 하나와 결합되어 있어야.
  - output은 gate의 ouptput, memory element의 output connection은 회로 계산 중 저장된 값.
- 여기서 "하나"란 input pin을 의미함. 따라서 입력이 여러개라도 핀당 하나씩 들어온다면 여러개의 입력도 처리 가능
- wire(=edge)는 반드시 하나의 값만 전달해야.

### multiplexor

- c로 정의하자면, bool out = (s && a) || (!s && b) 에 대응함.
- s, a, b 모두 비트레벨로서 0 / 1 만 존재하는 도메인임. 쉽게 확장 가능.

### TODO p 406
