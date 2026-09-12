# Exception control flow

## 개요

- 프로그램은 pc를 증가하면서 실행이 되고, 한 실행(k)에서 다음 실행(k+1)으로 가는 걸 control flow라고 한다.
- 이때 control flow가 인접한 메모리 경계(다음 주소)에서 이루어지는 경우도 있고, abrupt change로 인해 동떨어진 메모리로 넘어가야하는 경우도 있다.
  - call, return, jmp 등이 대표적.
- 이런 변경은 프로그램 내부에서 뿐만 아니라 외부(e.g., 네트워크 패킷 도착)에서 이뤄져야 할 수도 있음.
- 이걸 Exceptional control flow(ECF)로 해결.
- ECF는 프로그램 내 / 외부에서 발생하는 사건을 대응하기위해 정상적인 cf 순차실행을 중단하고 다른 cf를 실행하는 것을 의미.
  - 그러므로 call, jmp 등은 ECF는 아니다. 이유? 먼 메모리에 접근하긴 하지만 cf 순차실행은 맞기 때문.

## 8.1 Exceptions

- exception은 ecf의 형태로 부분적으로는 하드웨어에 부분적으로는 os에서 정의되어있음.
  - 이유? 하드웨어마다 exception의 형태가 조금씩 다르기 때문.
- exception이 발생할 때 크게 세가지 상태가 존재한다. 예외를 야기한 명령어를 I_curr, 다음 명령어를 I_next라고 할 때,
  - 예외 처리기(handler)가 실행권(control)을 I_curr에게 주는 경우
  - I_next에게 주는 경우
  - 그냥 프로그램을 중단시키고 버리는(aborted) 경우

### 8.1.2

- 정리해야.
