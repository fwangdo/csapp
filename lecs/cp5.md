# 5. Optimizing Program Performance

## 5.1 컴파일러 최적화의 능력과 한계

- gcc의 경우 최적화 옵션(e.g., O1, O2..)에 따라 큰 성능 차이를 보임.
- 항상 "안전한" 최적화를 하는 것이 중요함.

### alias problem

- 예를 들어 아래와 같은 코드를 생각해보자.

```c
1 void twiddle1(long *xp, long *yp)
2 {
3 *xp += *yp;
4 *xp += *yp;
5 }
6
7 void twiddle2(long *xp, long *yp)
8 {
9 *xp += 2* *yp;
10 }
```

- xp와 yp가 다른 주소를 가리킨다면 두 함수는 동일하지만, xp와 yp가 "같다면" 두 함수는 다르다.
- 예를 들어, xp에 2가 들어 있을 경우 첫 번째 함수는 8을, 두 번째 함수는 6을 리턴한다.
  - "저장하는 곳을 다시 사용하느냐 아니냐"에 따라, "더하는 값이 변하냐 아니냐"가 달라지기 때문.
- 이렇듯 alias(다른 변수가 동일한 주소를 지칭하는 것)는 프로그램 분석의 중요한 요소임.

## 5.2 프로그램 퍼포먼스

- CPE: cycles per element.
  - loop iteration 등 반복적인 작업의 퍼포먼스를 측정하기에 유용한 지표임.
  - 프로그램이 반족적으로 처리하는 데이터의 논리적 단위당 몇 사이클이 드는가?
    - 예: 룹에서 벡터 원소 `하나`를 계산하는데 걸리는 사이클 수. 한 번의 룹에서 두 개의 원소를 계산하면 두 개로 나누어야.

## 5.3 프로그램 예제

```c
1 /* Move call to vec_length out of loop */
2 void combine2(vec_ptr v, data_t *dest)
3 {
4 long i;
5 long length = vec_length(v);
6
7 *dest = IDENT;
8 for (i = 0; i < length; i++) { // 여기서 매번 vec_length를 호출할 수도.
9 data_t val;
10 get_vec_element(v, i, &val);
11 *dest = *dest OP val;
12 }
13 }
```

- 위 코드에서 수행하는 최적화는 "매 반복마다 동일한 값을 가지지만 함수 호출로 평가하던 값"을 한 번 계산하도록(라인 5) 만들기였음
- 불필요한 함수 호출을 줄이는 것만으로도 성능 이득을 볼 수 있음.
- 이런 걸 `code motion`이라고 함. 여러 번 실행되지만 결과가 변하지 않는 연산들을 찾아서 더 적게 실행되는 위치로 옮기는 것.

## 5.6 불필요한 메모리 참조 줄이기

- 아래 코드를 생각해보자.

```c
1 /* Direct access to vector data */
2 void combine3(vec_ptr v, data_t *dest)
3 {
4 long i;
5 long length = vec_length(v);
6 data_t *data = get_vec_start(v);
7
8 *dest = IDENT;
9 for (i = 0; i < length; i++) {
10 *dest = *dest OP data[i];
11 }
12 }
```

- 이 코드를 컴파일하면,

```asm
1 .L17: loop:
2 vmovsd (%rbx), %xmm0 Read product from dest
3 vmulsd (%rdx), %xmm0, %xmm0 Multiply product by data[i]
4 vmovsd %xmm0, (%rbx) Store product at dest
5 addq $8, %rdx Increment data+i
6 cmpq %rax, %rdx Compare to data+length
7 jne .L17 If !=, goto loop
```

- 와 같은 결과가 나온다. 여기서 핵심은 2, 3, 4번 라인이고,
  - 값을 읽고, 계산하고, 다시 더하는 작업을 수행한다.
  - 여기서 생각해두어야 할 점은 "다시 읽는 작업"을 할 필요가 없다는 것. 어차피 위치를 알고, 동일 지점에 더하기만 더해주면 되는데..
  - 포인터이기 때문에 "읽고 가져와서 계산하고 다시 쓰는" 작업이 있는 것으로 추정. 아래와 같은 코드를 생각해보자.

```c
1 /* Accumulate result in local variable */
2 void combine4(vec_ptr v, data_t *dest)
3 {
4 long i;
5 long length = vec_length(v);
6 data_t *data = get_vec_start(v);
7 data_t acc = IDENT;
8
9 for (i = 0; i < length; i++) {
10 acc = acc OP data[i];
11 }
12 *dest = acc;
13 }
```

- 이렇게 작성하면 성능이 훨씬 개선된다. 포인터 변수가 아닌 일반 값 변수라 "메모리를 다시 읽을" 필요가 없어지기 때문.
- "불필요한 메모리 참조"로 인해 "다른 곳에서 계산하고 다시 쓰는 작업"이 있지는 않을지를 고민해보아야 함.
- 비효율의 핵심은 포인터라기보단, "이 변수값이 이 룹 안에서 매번 같은 메모리를 참조하고 있는가?"이다.

## 5.7 현대 프로세서에 대한 이해
