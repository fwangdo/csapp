## chapter 3. 

### x86-64의 기본. 
- 명령어들은 1-15 바이트로 표현될 수 있음. 
- .으로 시작하는 정보들은 일반적으로 어셈블러 / 링커를 위한 정보임. 

### Notation
- Immediate: $ 기호와 함께 그값을 직접 작성(주소가 아님!), 32비트까지만 표현가능. 
- R: 레지스터 안에서, 해당 주소에 있는 값을 가져옴 
- M: 메모리 안에서 해당 주소 안에 있는 값을 가져옴. 

### 이동 명령어 
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


### 이동 명령어(작은 S에서, 큰 D로!)
- movz, movs. 여기서 z는 zero fill을 s는 sign fill을 수행. 
- 명령어는 각각 z는 5개, s는 6개(e.g., movzbw, movzbl, movzbq, movzwl, movzwq)
- movzlq는 왜 없는가? A. 컨벤션에 의해 4바이트 복사는 zero-fill을 자동으로 수행하므로(=movl과 동일한 동작)
- cltq -> SingExtend(%eax) -> %rax. 

### Push and Pop
- pushq S: R[%rsp] <- R[%rsp] - 8; M[R[%rsp]] <- S
  - 복잡해보이지만, 1) stack이 더 작을수록 top에 가깝다는 것, 2) 동작이 stack pointer(rsp)를 움직인 다음 그 자리에 S를 저장한다는 것을 유념하면 쉬움.
- popq D: D <- M[R[%rsp]]; R[%rsp] <- R[%rsp] + 8 
  - 마찬가지. 꺼내고, 포인터 이동시키고. 

### 3.5 logical & arithmetic operation
- leaq: mov의 specialized case. 메모리에서 레지스터로 이동시키는데 특화된 명령어. leaq S D가 D <- &S의 의미를 가짐. 
  - &S의 의미? "메모리 주소" 자체임. 예를 들자면, 다음과 같은 차이가 있다. 
  - movq 7(%rdx, %rdx, 4) %rax의 의미는? %rax = M[5x + 7] where %rdx = x. 
  - leaq 7(%rdx, %rdx, 4) %rax? %rax = 5x + 7 where %rdx = x. 다시 말하자면, 메모리에 저장된 값이 아닌, 메모리 주소 자체가 저장됨. 


