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

- 실제 머신에서 실행되는 동작은 코드의 순서와 다소 다름. 명령어 레벨 병렬처리라는 것이 가능함.
- 성능 측정을 위한 두 가지 기준. latency bound / throughput bound이 있고, 성능의 "하한"을 제시함.
- latency bound: 의존성(dependency)으로 인해 병렬계산이 불가능한 경우. 계산시간(latency)이 L이고 걔산해야할 게 n개 있으면 Ln으로 계산되고, CPE는 L(element n으로 나누니까)
- throughput bound: 한 사이클당 계산되는 element의 비율. 예컨대 계산할 수 있는 functional unit이 2개 있고, 한 명령어를 실행하는 주기가 4라면, 2 / 4로 계산. 0.5의 비율

### 동작 과정

- 인텔 프로세서는 수퍼스칼라를 가정하고, out of order 기반 병렬 처리를 수행할 수 있음. 크게 두 가지 요소가 디자인에 존재함.
- ICU: 메모리로부터 명령어를 순서대로 읽거나 생성함. 일종의 실행 준비.
- EU: 읽어둔 걸 실제로 실행.
- ICU는 EU가 실행을 마무리할 때까지 기다리지 않고 미리미리 읽어두는 작업을 수행. 여기서 문제는 branch임.
- 이때 branch prediction을 수행하고 실행될 것 같은 branch를 선택한다. 예측에 실패하면 다시 되돌리는(reset) 작업을 수행.
- fetch control은 다음에 어떤 instruction을 가져올지 결정하는 로직으로 branch prediction도 이중 일부임.

### instruction decoding

- 하나의 어셈블리 명령어도 더 원초적인 형태로 쪼갤 수 있음. 이런 과정을 instruction decoding이라고 하고.
- 더 쪼개면 좋은 점? 쉽게 병렬화 시킬 수 있음.
- EU는 이런 명령어들을 매 사이클마다 받고, 각 명령어에 특화된 functional unit으로 보냄.

### retire & flush

- instruction decoding이 수행되고 FIFO로 실행된다고 했을 때, 실행한 것들이 옳은 것으로 밝혀지면,
  - retire했다고 표현하고, 해당 실행 결과를 최종 결과로 확정한다.
- 반대로 실패했다는 게 밝혀지면?
  - flush했다고 표현하고, 해당 실행 결과를 버린다.

### register renaming

- out-of-order 실행을 하다보면 인자간 값을 주고받아야 하는 상황이 존재함. 이걸 편리하게 할 수 있는 방법론이 필요하고. 그래서 register renaming 등장.
- 예를 들어 레지스터 r에서 계산할 게 있으면 태그를 붙여준다. 테이블엔 (r, t) 꼴로 기록하고,
- EU에게 보내서 계산을 끝낸 값이 v가 나오면 (v, t)로 재기록.
- 해당 값이 필요한 경우에 태그 t를 검색해서 v를 가져가도록 만든다.

### 용어 정의

- latency: end-to-end 실행시간. 실행하는데 얼마나 걸리나?
- issue time: 동일 종류의 계산일 때, 두 연산의 시작 시점 사이에 최소 몇 사이클을 띄워야 하는가?
- capacity: functional unit을 얼마나 사용할 수 있는가?
- 계산이 빨라도 load가 오래걸리면 결국 load에 시간이 종속됨. 총시간은 파이프라인의 "가장 오래 걸리는 작업"에 종속됨.
- 동일한 계산도 병렬 처리 할 수 있는 환경인지, 직렬로 실행해야하는지에 따라 다름. 예컨대,
  - issue time이 5, 처리기계가 2개 하나당 처리 시간이 1cycle일 경우,
  - 모두 병렬처리가 가능하면 CPE는 0.5지만,
  - 병렬처리가 불가능하고, 많은 작업이 뒤이어 존재하면 5라고 보아야함.

### data-flow

- 코드 조각들의 실제 컴파일 결과들을 보면 실행시 각 레지스터간의 의존관계를 볼 수 있다. 해당 관점에서 레지스터들은 크게 네 가지 범주로 구분할 수 있다.
- read-only: 오직 읽기만. 해당 레지스터에 무언가 쓰이는 일은 존재하지 않음.
- write-only: 데이터 이동(mov)의 도착지점으로만 사용되는 경우.
- local: 룹 속에서 업데이트 되고, 사용되기도 하지만 한 iteration과 다른 iteration 사이에는 의존 관계가 없는 경우(e.g., cmp의 결과)
- loop: 이전 룹에서 사용되던 게 다음룹에서도 사용되는 경우. 누적되는 합이 대표적.
- 아래와 같은 asm을 생각해보자

```asm
Inner loop of combine4. data_t = double, OP = *
acc in %xmm0, data+i in %rdx, data+length in %rax
1 .L25: loop:
2 vmulsd (%rdx), %xmm0, %xmm0 Multiply acc by data[i]
3 addq $8, %rdx Increment data+i
4 cmpq %rax, %rdx Compare to data+length
5 jne .L25 If !=, goto loop
```

- xmm0 / rdx는loop이고, rax는 read-only.
- 의존성 그래프를 그려보면 load, mul, add는 명백히 다음 iteration 실행에 영향을 줌(어느 주소에 있는 값을 가져올 건가? 얼마만큼 저장되어있나?)
- cmp, jne도 rdx 의존성이 존재함. 단 branch prediction을 가정하면,
  - cmp는 결국 "비교결과를 레지스터에 표현할 뿐"이고, 다음 iteration에 영향을 끼치진 않음.
  - jne는 branch prediction에서 taken되었다고 가정하면 결국 다음 iteration 자체엔 영향이 없음(=할지 말지를 결정할 뿐, 하면 무얼할지를 결정하지는 않음)
- 이러한 이유로 load -> mul & add를 critical path로 볼 수 있음
- 여기서 더 정확히는load -> mul / add는 dest가 달라 병렬 계산이 가능하고, 이러한 이유로 add보다 더 긴 수행시간을 갖는 mul의 cpe가 최종 cpe가 됨.

## 5.8 Loop unrolling

- loop unrolling이란 loop의 iteration 수를 줄이고, 한 loop 안에서 처리되는 element의 수를 늘리는 방법이다. 크게 두 가지 장점이 있는데,
- 1. 프로그램의 결과에 개입하지 않는 요소인 "loop indexing"을 계산하는 횟수를 줄일 수 있고,
- 1. critical path에 들어가는 element의 수를 줄일 수 있다.

### 2 by 1 unrolling

```c
1 /* 2 x 1 loop unrolling */
2 void combine5(vec_ptr v, data_t *dest)
3 {
4 long i;
5 long length = vec_length(v);
6 long limit = length-1;
7 data_t *data = get_vec_start(v);
8 data_t acc = IDENT;
9
10 /* Combine 2 elements at a time */
11 for (i = 0; i < limit; i+=2) {
12 acc = (acc OP data[i]) OP data[i+1];
13 }
14
15 /* Finish any remaining elements */
16 for (; i < length; i++) {
17 acc = acc OP data[i];
18 }
19 *dest = acc;
20 }
```

- 왜 2 by 1인가? 한 iteration에서 처리하는 element가 2개이고, accumulator는 1개이므로.
- 이 방법은 누적값(accumulator)이 하나라 loop의 값을 매번 비교하는 오버헤드는 줄일 수 있지만, 본질적으로 critical path를 병렬화 시킬 수는 없음.

## 5.9 병렬성 강화하기

- combine operation(우리 예제에서는 덧셈과 곱셈)이 결합법칙과 교환법칙을 만족한다면 우리는 아래와 같은 방법을 고려해볼 수 있음.
- 짝수번째 원소들과 홀수번째 원소들을 별도의 누적으로 계산하는 방법. 코드로 보면 아래와 같음.

```c
1 /* 2 x 2 loop unrolling */
2 void combine6(vec_ptr v, data_t *dest)
3 {
4 long i;
5 long length = vec_length(v);
6 long limit = length-1;
7 data_t *data = get_vec_start(v);
8 data_t acc0 = IDENT;
9 data_t acc1 = IDENT;
10
11 /* Combine 2 elements at a time */
12 for (i = 0; i < limit; i+=2) {
13 acc0 = acc0 OP data[i];
14 acc1 = acc1 OP data[i+1];
15 }
16
17 /* Finish any remaining elements */
18 for (; i < length; i++) {
19 acc0 = acc0 OP data[i];
20 }
21 *dest = acc0 OP acc1;
22 }
```
