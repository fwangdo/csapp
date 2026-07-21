## chapter 3. 

## x86-64의 기본. 
- 명령어들은 1-15 바이트로 표현될 수 있음. 
- .으로 시작하는 정보들은 일반적으로 어셈블러 / 링커를 위한 정보임. 

## Notation
- Immediate: $ 기호와 함께 그값을 직접 작성(주소가 아님!), 32비트까지만 표현가능. 
- R: 레지스터 안에서, 해당 주소에 있는 값을 가져옴 
- M: 메모리 안에서 해당 주소 안에 있는 값을 가져옴. 

## 이동 명령어 
- movb, movw, movl, movq 각각 1 / 2 / 4 / 8 바이트를 이동. 
- mov S D 꼴로 작성되고, S에 저장된 값을 D에 복사하는 방식임. 
  - S는 immediate / register / memory 모두 사용 가능. 
  - D는 register / memory 주소 중 하나.
  - 여기서 x86-64는 S / D 둘다 메모리 주소인 것은 mov에서 불가능함. 
- movabsq는 절대값을 복사함. 
- mov는 정해진 바이트만을 변경하지만, movl은 예외임. 
  - 이유? register에서 4byte를 변경하는 경우는 그 위의 higher order byte들에 0을 padding하기 때문. 
- S / D를 일종의 연산으로 생각해야. S는 읽기. D는 쓰기. 
  - 예제. %rax가 S에서는 rax에 저장된 "값"이지만, D에서는 "저장될 주소 자체"를 의미함


## 이동 명령어(작은 S에서, 큰 D로!)
- movz, movs. 여기서 z는 zero fill을 s는 sign fill을 수행. 
- 명령어는 각각 z는 5개, s는 6개(e.g., movzbw, movzbl, movzbq, movzwl, movzwq)
- movzlq는 왜 없는가? A. 컨벤션에 의해 4바이트 복사는 zero-fill을 자동으로 수행하므로(=movl과 동일한 동작)
- cltq -> SingExtend(%eax) -> %rax. 

## Push and Pop
- pushq S: R\[%rsp\] <- R\[%rsp\] - 8; M\[R\[%rsp\]\] <- S
  - 복잡해보이지만, 1) stack이 더 작을수록 top에 가깝다는 것, 2) 동작이 stack pointer(rsp)를 움직인 다음 그 자리에 S를 저장한다는 것을 유념하면 쉬움.
- popq D: D <- M\[R\[%rsp\]\]; R\[%rsp\] <- R\[%rsp\] + 8 
  - 마찬가지. 꺼내고, 포인터 이동시키고. 

## 3.5 logical & arithmetic operation
- leaq: mov의 specialized case. 메모리에서 레지스터로 이동시키는데 특화된 명령어. leaq S D가 D <- &S의 의미를 가짐. 
  - &S의 의미? "메모리 주소" 자체임. 예를 들자면, 다음과 같은 차이가 있다. 
  - movq 7(%rdx, %rdx, 4) %rax의 의미는? %rax = M\[5x + 7\] where %rdx = x. 
  - leaq 7(%rdx, %rdx, 4) %rax? %rax = 5x + 7 where %rdx = x. 다시 말하자면, 메모리에 저장된 값이 아닌, 메모리 주소 자체가 저장됨. 

- shift 연산
  - shift는 immediate를 이용하거나 cl 레지스터를 이용한 방식으로 사용할 수 있음.
    - 여기서 cl은 8바이트 레지스터지만 실제로는 quad(64 - 1)까지만 사용할 수 있고, 그 위 비트는 무시됨. 
    - 예: salq $cl, %rax가 있고, $cl에 65가 저장되어 있을 경우, 65 % 1 연산이 수행되어 rax를 1bit shift를 수행. 
  - 왼쪽 shift인 sal, shl은 동일한 연산을 수행한다. 이유는 부호를 고려할 필요 없기 때문. 
  - 반대로 오른쪽 shift인 sar, shr은 그 의미가 다르다. arithmetic shift이냐, logical shift냐의 차이. 

- Oct(128bit 연산)
  - 64 비트 레지스터에서는 계산하기 어려우므로, 두 레지스터에 저장하는 형태로 계산함.
  - imulq, mulq, cqto, idivq, divq로 구성되어있으며 하나의 인자(unary)만 받는 함수들임.  
    - i는 two's complement, i가 없으면 unsigned로 이해하면 됨.  
  - S 값을 받고 rax / rdx 레지스터를 사용하는 형태임. 
    - 곱셈의 경우: R\[%rdx\]:R\[%rax\] <- S * R\[%rax\]
    - 나눗셈의 경우: R\[%rdx\] <- R\[%rdx\]:R\[%rax\] mod S; R\[%rax\] <- R\[%rdx\]:R\[%rax\] / S; 

## 3.6 Control. 

### Condition codes 
- 조건 분기(e.g., c의 if, switch 등)를 레지스터에서 표현할 방법이 필요하다. 아래는 하나의 비트로 조건을 인코딩하는 방법들. 
  - carry out: 계산의 결과가 제한된 비트의 범위 바깥으로 나가는 경우(예: w비트 연산에서 계산의 결과가 2^w 이상인 경우)
- CF: carry flag, 바로 이전 연산에서 carry out이 발생했으면 1, 그렇지 않으면 0. 주로 unsigned 연산의 overflow를 판단하기 위해 사용.  
- ZF: zero flag. 가장 최근의 연산이 0을 만들었는가? 
- SF: Sign flag : 가장 최근 연산이 음수를 만들었는가?
- OF: overflow flag: 가장 최근 연산이 signed 관점에서 overflow를 만들었는가? 예컨대 덧셈을 가정하면, 부호가 동일한 두 개의 수를 계산했을 때 부호가 바뀌었는가? 

### Comparison and test 명령어들 
- mov, add 등 레지스터 변경을 해야만 condition code를 변경할 수 있는 명령어들과 다르게 cmp, test는 레지스터 변경 없이 condition code만 변경할 수 있음. 
- cmp: subtraction과 똑같음. b, w, l, q로 구성됨. 주의 사항은 뒤의 값으로부터 앞의 값을 뺀다는 것. cmpb S_1 S_2는 S_2 - S_1을 바이트 크기만큼 수행함. 
- test: and와 똑같음. 마찬가지로 b, w, l, q로 구성. 
- cmp / test 모두 destination의 레지스터 값을 변경시키지 않음. 


