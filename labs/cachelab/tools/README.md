# Mac용 Cache Lab 테스트

Python 3와 Clang만 사용합니다. 별도 패키지를 설치하지 않아도 됩니다.
`csim.c`, `trans.c`, 제공된 Linux 실행 파일과 공식 채점기는 변경하지 않습니다.

## 먼저 도구 자체 검증

```sh
make check-native-tools
```

공식 문서에 공개된 Part A의 8개 예상 결과, 손으로 계산한 짧은 trace,
독립적인 접근 이력 검사, Part B 접근 기록 순서 및 실패 감지를 확인합니다.
제출 코드가 아직 비어 있어도 이 검증은 통과해야 합니다.

## Part A

```sh
make test-native-a
```

`csim`을 빌드하고 공개된 8개 사례, 작은 경계 사례 6개, 재현 가능한 무작위
사례 24개에서 출력과 `.csim_results`를 검사합니다. 실행마다 작업 폴더를
임시로 만들므로 프로젝트의 `.csim_results`를 덮어쓰지 않습니다.
실패하면 예상 결과와 실제 결과를 출력하고 종료 코드 1을 반환합니다.

첫 실패에서 멈추거나 무작위 사례를 늘릴 수 있습니다.

```sh
python3 tools/test_native.py csim --fail-fast
python3 tools/test_native.py csim --seed 100 --random-cases 20
```

특정 trace의 정답이나 접근별 결과만 확인하려면 다음처럼 실행합니다.

```sh
python3 tools/cache_oracle.py -s 4 -E 1 -b 4 -t traces/yi.trace
python3 tools/cache_oracle.py -v -s 4 -E 1 -b 4 -t traces/yi.trace
```

**학습 참고:** oracle의 소스에는 Part A의 핵심 풀이가 담겨 있습니다.
직접 구현하는 동안에는 위 명령으로 출력만 확인하는 사용법을 권합니다.
oracle은 `csim.c`를 호출하거나 그 구현을 재사용하지 않습니다.
`csim-ref` 자체를 실행해 비교한 도구는 아닙니다.

이 oracle은 Cache Lab 명세를 따릅니다. 명령어 접근 `I`는 무시하고,
메모리 접근은 블록 경계를 넘지 않는다고 가정합니다. 일반적인 다중 블록
접근용 시뮬레이터는 아닙니다. 잘못된 CLI 인자의 처리까지 공식 프로그램과
동일함을 검사하는 도구도 아닙니다.

## Part B

```sh
make test-native-b
```

기본값은 등록된 함수 0번, `M x N = 32x32, 64x64, 61x67`, 각 크기당
서로 다른 입력 3개입니다. 제공된 기본 전치 함수는 현재 1번입니다.

```sh
python3 tools/test_native.py trans --function 1
python3 tools/test_native.py trans --sizes 1x1 7x9 31x33 --trials 5
```

Clang이 `trans.c`의 메모리 읽기/쓰기 앞에 기록 함수를 자동으로 삽입합니다.
제출 파일에 추적 코드를 넣을 필요가 없습니다. 실제 A/B 배열 범위의 접근만
기록하고 스택과 다른 전역 변수의 접근은 제외합니다. 테스트 실행 파일은
임시 폴더에서 빌드하며, 기존 `trans.o`를 덮어쓰지 않습니다.

검증은 전치 결과 전체, 입력 A 보존 여부, AddressSanitizer가 감지하는
경계 밖 접근 등을 확인합니다. 과제의 지역 변수 개수나 코딩 규칙을 검사하는
기능은 없고, B를 쓰기 전에 읽는 등 모든 잘못된 동작을 잡는 것은 아닙니다.
테스트 통과가 모든 입력에 대한 정당성 증명을 뜻하지는 않습니다.

추적 결과를 남겨서 자세히 확인할 수도 있습니다. 같은 이름의 trace가 있으면
새 실행 결과로 덮어씁니다.

```sh
python3 tools/test_native.py trans --trace-dir /tmp/cachelab-traces
python3 tools/cache_oracle.py -v -s 5 -E 1 -b 5 \
  -t /tmp/cachelab-traces/f0-32x32-seed1.trace
```

**성능 수치는 자체 모델의 결과이며 공식 점수가 아닙니다.**

- 캐시는 `s=5, E=1, b=5`입니다. 실행 시간이나 Mac 하드웨어 카운터를 재지 않습니다.
- 실제 프로세스 주소를 고정된 모형 주소 `A=0`, `B=0x40000`으로 변환합니다.
  B의 기본 위치는 원본 `tracegen.c`에 있는 두 256x256 int 배열의 간격을
  모델링합니다. 두 배열은 같은 캐시 set에서 시작합니다.
- `--b-offset 262176` 등으로 B의 위치를 바꿔 충돌 민감도를 탐색할 수 있습니다.
- Clang `-O0`의 배열 접근만 기록하므로 공식 Linux/GCC/Valgrind 결과와
  명령어 선택, 주소 정렬, marker 접근 등이 다릅니다. miss 수가 몇 개 정도만
  다르리라고 보장할 수 없고, 공식 점수로 환산하지 않습니다.
- 최종 제출 전에는 원래 Linux x86-64 환경의 공식 도구로 확인해야 합니다.

## 근거

- [공식 Cache Lab 명세와 공개 테스트 결과](https://csapp.cs.cmu.edu/2e/cachelab.pdf), 특히 4.2절과 6.1절.
- [Clang의 메모리 접근 기록 기능](https://clang.llvm.org/docs/SanitizerCoverage.html#tracing-data-flow).
