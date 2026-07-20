## chapter 3. 

### x86-6의 기본. 
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
  - D는 immediate / register / memory 주소 중 하나.
  - 여기서 x86-64는 S / D 둘다 메모리 주소인 것은 mov에서 불가능함. 
- movabsq는 8바이트 절대값을 복사함. 
- mov는 정해진 바이트만을 변경하지만, movl은 예외임. 
  - 이유? register에서 4byte를 변경하는 경우는 그 위의 higher order byte들에 0을 padding하기 때문. 
- S / D를 일종의 연산으로 생각해야. S는 읽기. D는 쓰기. 
  - 예제. %rax가 S에서는 rax에 저장된 "값"이지만, D에서는 "저장될 주소 자체"를 의미함
