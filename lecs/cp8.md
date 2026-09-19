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

### ECF 전 함수 call과 stack에 대한 복습

- 함수 P가 함수 Q를 호출하는 상황을 가정한다면,
- P에서 Q를 호출할 때, call Q 뒤 다음 명령어가 call Q가 종료되고 수행해야할 명령어이고, 이 명령어가 있는 "주소"를 Q의 스택 프레임의 맨위에 작성해둔다.
  - 이때 (rsp)는 return address이다. 다시 말하자면, rsp는 return address를 저장해둔 Q 스택프레임의 맨 위 주소를 가진다.
  - 그리고 그 이후 rsp는 Q의 stack frame에 필요한 메모리만큼 아래로 내려간다. 그래서 rsp가 stack top(가장 아래)라고 불린다.
- Q가 끝나고 P로 돌아갈 때는 아래로 내려간만큼 위로 올라가고 그렇게 return address를 확보한다.
  - Q. 어떻게 "내려간 정도"를 아나요? A. 이건 레지스터간 계산으로 이루어지는 게 아니고 컴파일러가 계산하니까 제약 없음.

### 8.1.2

- exception table이라는 jump table이 존재함. 하드웨어 / 운영체제 개발자들이 존재하는 예외(e.g., div by zero)에 이름을 붙여둔 것.
- 컴퓨터가 켜질 때 초기화되고, 하나의 레지스터에 이 테이블의 시작주소가 들어가있음. 예외가 발생했을 때 이 주소를 통해 테이블 값을 가져옴.
  - 하나 주의해야할 것은, 이 레지스터는 "예외용" 레지스터이고, rax 같은 16개의 범용 레지스터와는 다름.
- exception(예외)는 procedure call과 유사하지만 다른 것도 있다. 예를 들자면
  - 일반적인 call은 호출 후 돌아갈 위치가 정해지지만, ecf는 그렇지 않음(그자리일수도, 그 다음자리일수도, 아니면 안 돌아올 수도.)
  - ECF는 핸들러가 돌아왔을 때(예외로부터 복귀했을 때), 동작을 하기위해 함수 호출에는 들어가지 않는 요소(e.g., EFLAGS)를 스택에 푸쉬할 수도.
  - 컨트롤이 유저에서 커널로 가면 아이템이 커널로 다 간다(정확히 무슨 의미인지는 모르겠는데, 향후 규명될 듯.)
  - ECF는 커널 모드로 사용되어 시스템 리소스를 사용할 수 있다.

### 예외의 네 가지 종류

- 크게 interrupt / trap / fault / abort가 있다.
- interrupt: 유일하게 비동기로 발생하는 예외임. I/O 디바이스에 발생한 예외를 감지하는 경우(e.g., 네트워크 패킷)
  - i/o 디바이스라 핀을 통해 칩에 신호를 전달하면 수신했을 때, 수행하던 명령어는 수행하고 핸들러에게 control을 넘김. 적절한 예외를 발생시키고, 아무일 없던 것처럼 다음 진행부터 수행.
- traps / system calls: 프로그램을 실행하기 위한 의도적인 예외. system call이 대표적. 예를 들어 읽는 system call(read)을 생각해보면, 유저모드에서 수행되던 control flow를 잠깐 벗어나, kernel mode에서 작업해야함.
  - 일반적인 의미의 exception보다도, control flow가 "현재 상태로부터 나와야하는가"가 ECF의 필요조건인 듯.
- faults: handler가 해결할 수도 있는 에러 컨디션. 해결에 성공하면 다음 실행을 하게 해주고, 실패하면 abort를 함. 대표적인게 fault page
  - 핵심 기준: 해당 instruction이 실패해서 발생한 에러인가?
- abort: 실행 / 회복 불가능. DRAM / SRAM이 이미 오염된(corrupted) 경우에 발생하는 게 대표적.

### x86 - linux 환경에서의 syscall

- c에서도 syscall을 함수형태로 사용할 수 있도록 지원함.
  - 사용되는 레지스터는 일반 함수와 다르다. syscall 넘버가 rax, 6개의 인자들은 rdi, rsi, rdx, r10, r8, r9에 들어가서 실행.

### Key abstraction

- 프로세스는 프로그램 실행의 인스턴스라고 볼 수 있으며, 실행을 위해 대응되는 자원(stack, register)이 필요햐다.
- 우리가 어떤 프로그램을 실행시키면 정말 "그 프로그램만 동작하는 것처럼" 보인다. 사실은 그런 게 아닌데도. 향후 책에서는,
- 1. 어떻게 현재 실행되는 프로세스가 독점적으로 실행되는 것처럼 보이는지,
- 1. 어떻게 현재 실행되는 프로세스가 독점적으로 메모리 시스템을 가지는 것처럼 보이는지,
- 를 중점적으로 살펴볼 예정. 핵심은 "어떻게 그렇게 보이도록 만들었는가"이다.

### Logical control flow

- 하나의 프로그램을 실행할 때 나타나는 프로그램 카운터(pc)의 시퀀스를 logical control flow라고 칭한다. 먼저 이것부터 정의를 해보자.
- 하나의 "물리적 logical flow(하나의 프로세스에서 나타나는 pc 시퀀스)"도 여러개의 "논리적 logical control flow"로 나누어 생각할 수 있다.
  - 예를 들어 공유 라이브러리 파일이 있는 경우, 실행하다가 메모리를 크게 점프해서 실행해야하는 경우가 있는데, 이런 경우를 논리적 logical flow가 나눠지는 구간으로 볼 수 있음.
- 프로세서가 각각의 logical flow가 어떤 순서로 실행될 건지를 결정.

### concurrent flows

- 여러개의 프로세스가 교차로 실행될 때 그 프로세스들을 concurrent flows라고 한다.
  - 조금더 정밀하게 정의하자면, X가 시작되고 종료되는 사이에 Y가 실행되는 경우(혹은 그 반대)라고 정의할 수 있음.
  - 정말로 "동시에 실행"될 필요는 없고, 각 logical flow 간의 시작 / 끝지점에서 오버랩되는 구간이 있으면 됨.
- 단순한 오버랩을 떠나서 정말로 다른 코어에서 "동시에" 실행되는 경우도 있을 수 있고, 이건 parallel flow라는 이름으로 지칭.
- 하나의 코어에서 여러개의 프로세스가 실행되는 것을 멀티태스킹이라고 함(멀티프로세싱은 꼭 하나의 코어일 필요 없고, 여러개의 프로세스 실행되면 ok.)

### Private Address space

- 하나의 프로세스에 대응되는 메모리 공간을 만들고, 해당 프로세스만 접근할 수 있도록 격리.

### 커널 모드 / 유저 모드

- 운영체제는 프로세서가 허용되지 않은 메모리 주소에 접근하는 것을 막기 위해 적절한 메모리 요약 형태를 제공할 수 있어야함.
- 커널 모드 / 유저 모드를 표현하는 건 mode bit이고, 이게 켜지면 커널모드가 되어 임의의 메모리 주소에 접근하는 것이 가능함.
- 빈먄 유저모드는 많은 것들이 제약됨. 커널 모드를 위해 할당된 메모리 주소 접근 불가능하고, io 초기화 / 프로세스 중지 / 모드비트 변환등이 불가능함.
- Q. 어떻게 유저 -> 커널 전환하나? A. ECF를 통해 커널로 변환됨.
  - 예외 발생하면 실행권이 예외 핸들러한테 가고, 이게 모드 비트를 변환하는 방식으로.

### Context Switches

- 맥락 변환(Context Switches)은 ECF를 통해서 멀티태스킹을 수행하는 것으로 정의할 수 있음.
- 그럼 context란 뭔가? 선점을 당한(=중단된) 프로세스가 작업을 재개하기 위해 필요한 상태(state)로 정의할 수 있음. 예를 들어,
  - 레지스터, 유저 스택, pc 등이 있고, page / process / file이라는 중요한 세 개의 테이블도 포함됨.
    - page table: address table. 주소들이 페이지 단위로 관리되는 듯(확인 필요)
    - process table: 현재 프로세스에 대한 정보를 가지고 있음(다소 모호)
    - file table: 현재 프로세스가 열어둔 파일들에 대한 목록.
- 여러개의 프로그램이 존재할 때, 어떤 프로그램을 선점(preempt)하여 실행한 건지를 결정하는 모듈을 스케줄러라고 하고, 선점여부를 결정하여 특정 프로그램을 실행하면 "스케줄"했다고 표현함. 이때 과정은 크게 세 단계로 구성.
  - 선점당하는 현재 프로세스를 저장하고 -> 선점"당했던"(그리고 이제 실행할) 프로세스를 복구(restore)하고 -> 복구된 프로세스에 실행(control)권을 전달.

### 프로세스의 생성과 종료

- 프로그래머 관점에서는 프로세스는 크게 세 개의 상태를 가지고 있음. 1. 실행되고 있거나, 2. 잠깐 중단되었거나 3. 종료되었거나.
  - 1. 실행중(running): 실제로 실행 중이거나, 잠깐 대기중이고 커널에 의해 다시 실행될 프로세스를 의미.
  - 1. 중단됨(stopped): 특별한 시그널을 받아 중단중인 상태, SIGCONT를 통한 재개신호를 받지 않는 한, 커널에 의해 다시 실행되지 않음
  - 1. 종료됨(terminated): 더이상 실행되지 않을 상태. exit을 받았거나 main routine을 받았거나, default action이 종료인 프로시져를 받았거나.
- 생성의 관점에서 보자. 대표적으로 fork. 부모 프로세스를 동일하게 복제하여 자녀 프로세스를 만드는 fork는 다음과 같은 특징을 지닌다.
  - 한 번 실행, 두 번 리턴: fork를 한 번 실행하면 응답이 부모와 자녀 둘 다로부터 온다.
  - 동시 실행: 자녀 프로세스와 부모 프로세스는 동시에 실행된다.
  - 동일한 내용을 다른 주소에 저장: 스택 프레임 등 프로그램 실행에 필요한 메모리는 모두 동일하지만 별개의 주소에 저장된다.
  - 공유되는 파일들: 파일은 복사하지 않고 공유한다(당연하다)

### 자녀 프로세스 수확(reaping)하기

- 자녀 프로세스가 끝나면 커널이 해당 프로세스로부터 자원을 회수하지 않음. 부모 프로세스가 reaping하면, 그제서야 자원을 회수함.
  - 그래서 실질적인 실행이 끝났음에도 아직 부모 프로세스로부터 reaping되지 않은 프로세스들을 좀비라고 함.
- Q. 그럼 부모 프로세스가 자녀 프로세스보다 먼저 죽는 경우는 어떡하나요?
  - A. init 프로세스(pid가 1, 프로그램 실행할 때 바로 시작되는 프로세스)가 입양하고, 실행이 종료되면 reap한다.
  - 알아두어야하는 건, reap은 "종료된 상태에서 수행될 수 있는 것"으로 정의된다는 것임. 따라서 강제 종료란 없음.

### waitpid

- reaping하려면 알아야할 게 뭘까. 자녀프로세스가 다 죽었는지 안 죽었는지다. waitpid는 그런 역할을 한다.
- waitpid: 내가 지정한 자식 프로세스의 범위에서, 내가 관심있는 상태변화가 일어났을 때까지 기다렸다가, 어떤 자식에게 어떤 상태변화가 일어났는지 알려달라.
- 기본적으로 `pid_t waitpid(pid_t pid, int *statusp, int options);`와 같은 형태임. 각각의 인자를 살펴봐야함.
  - pid > 0 이면 자식 중 하나를 가리키고, pid == -1 이면 임의의 자식 중 하나(전체 대상 중 하나라도)를 의미함.
  - options는 기본 0이고, 자식이 종료될 때까지 기다리는 동작을 줌.
  - statusp는 리턴되는 자녀 프로세스의 상태(STATUS)를 커널이 기록해주기 위한 주소임.
- 그럼 리턴값은 무엇인가? 상태변화가 일어난 자녀 프로세스의 pid. status는 앞서 언급한 statusp에 커널이 작성해줌.

### waitpid 옵션의 종류

- WNOHANG: no hang. 말 그대로 "기다리지마". 기본 옵션 0이 끝날 때까지 기다리지 않는다면, no hang은 아직 아무것도 일어나지 않았더라도 return 0하고 끝낸다. 물론 변화 있으면 적당한 값을 리턴한다.
  - 서버가 계속 기다리지 않고, 어떤 작업이라고 하고 싶을 때 유용.
- WUNTRACED: 죽거나(terminated)나, 멈출(stopped)될 때까지 기다려. 중단 정보도 알고 싶은 경우. 이 옵션을 사용해서 결과를 받았다면 status가 중단인지 종료인지 확인하게 되는 작업이 필요.
- WCONTINUED: 중단되었던 프로세스가 재개되거나 종료되면 알려줘.

### waitpid 상태의 종류

- WIF -> 무슨 일이 있었는가? / W...STATUS, W...SIG -> 상태가 어떤가?
- WIFEXITED: 정상적으로 종료되는 경우. exit을 호출하거나 리턴하는 경우.
- WEXITSTATUS: 정상적으로 종료됐을 때 exit code가 몇 번인지?
- WIFSIGNALED: 정상 exit이 아니라, signal 때문에 죽었는지?
- WTERMSIG: 어떤 시그널 때문에 죽었는지?
- WIFSTOPPED: waitpid로 돌아온 이유가 중지(stop) 때문인지?
- WSTOPSIG: 어떤 stop signal 때문인지?
- WIFCONTINUED: sigcont 때문에 재시작했는지?

### loading and running program

- execve를 기준으로 설명해보자. 현재 프로세스에서 프로그램을 실행해주는 함수이다.
  - 형태는 `int execve(const char *filename, const char *argv[], const char *envp[]);`과 같이 생김.
- 통상적인 프로그램의 main과 굉장히 유사. filename은 말 그대로 파일 이름이고, argv는 인자로 들어오는 string의 주소, envp는 환경변수들의 포인터.
- execve는 성공하면 리턴이 없고, 실패하는 경우에만 리턴이 존재한다. 실패하는 경우는 대응하는 파일이 없는 경우 등.

### signals

- signal이란 프로세스가 시스템에 일어난 일을 파악하게 해주는 일종의 짧은 메시지이다.
- 각각의 signal은 그에 대응하는 이벤트가 있고, 이걸 프로세스가 볼 수 있도록 해줌.
- signal을 전달하는 과정은 크게 두 가지로 나누어진다.
  - 시그널 보내기: 목적지 프로세스의 context에 있는 일부 상태(state)를 업데이트하는 방식으로 수행. div by zero나 child process가 죽은 걸 감지했을 때 실행되고, kill function을 실행한다. 이유는 커널이 목적지 프로세스에 대해 무언가를 하도록 만드려고.
  - 시그널 받기: 커널에 의해 목적지 프로세스는 대응을 하기를 요구당한다. 이때 시그널 핸들러를 통해서 무시하거나, 종료하거나, 캐치하거나. 셋 중 하나를 할 수 있다.

### pending

- 시그널을 받았지만 아직 처리되지 않은 경우를 pending이라고 한다.
- 현재 pending 상태인 시그널이 있다면 다른 시그널은 받을 수 없고 모두 버려지(discard)게 된다.

### pid / pgid

- pid는 프로세스 아이디, pgid는 프로세스 그룹아이디이며, 자녀 프로세스는 부모 프로세스와 동일한 pgid를 갖는다.
- getpgid, setpgid로 얻고 설정할 수 있으며 인자가 0일 경우 자신의 id를 의미하게 된다.

### 시그널 수신(receiving)

- 프로세스가 실행 중에 커널 모드에서 유저 모드로 들어오는 순간(e.g., syscall 종료)이 있다고 가정하자. 이때 p는 "pending 신호"가 있는지 확인한다.
  - 이때 pending signal이 없으면 그냥 프로세스(p)를 그대로 실행하면 된다.
  - signal이 있다면 pending signal set 집합에서 smallest k를 선택한다. 아마도 숫자가 작을수록 중요도가 큰것으로 추정.
    - Q. pending은 최대 하나인데 어떻게 set이 있나? A. 시그널 타입마다 최대 하나인 거고, 여러개의 시그널 타입은 동시에 존재할 수 있다.
- 각각의 시그널 타입은 "만났을 때 하도록 정의된 행동"이 있고, 이걸 default action이라고 함. 크게 네가지임.
  - 프로세스 종료 / 종료 & 덤프 코어 / sigcont 오기 전까지 중단 / 시그널 무시.
- signal 함수로 default action을 변경할 수 있음. 이걸 installing handler라고 부른다.

### 안전한 시그널 핸들링

- G0. 가능한한 핸들러를 단순하게 만들기
- G1. 핸들러 안에서는 async-signal-safe 함수만 사용하기. safe한 함수들이 사전에 정의되어 있음.
- G2. errno를 기록하기: safe 함수들은 에러를 만날 경우 errno를 기록해두는데, 다른 함수도 errno를 쓸 경우, 덮어쓰게 된다. 해당 문제를 방지하기위해 핸들러 안에서 errno를 로컬변수로 저장하는 게 방법. 이건 핸들러가 리턴으로 종류되는 경우에만 유효하고.
- G3. 공유되는 전역 데이터 구조(shared global data structure)에 대해 blocking으로 보호하기: 핸들러가 메인 프로그램과 함께 공유되는 데이터(d)에 대한 권한을 가지고 있다고 하자. 메인프로그램이 d를 만질 때 핸들러가 데이터에 접근할 경우, 데이터를 온전히 신뢰하기 어렵다. 획득한 정보가 이미 바뀌었을 수도.  
- G4. volatile 키워드 사용하기: 핸들러와 프로세스가 동시에 사용하는 글로벌 변수 g가 있다고 할 때, main이 g를 캐싱해두고 쓰면 실제로 g가 변했을 때도 문제가 된다. 캐싱하지 않고, 항상 글로벌 변수를 읽도록 하려면 volatile 키워드를 사용해야.
- G5. sig_atomic_t를 쓰자: 프로세스와 핸들러는 공유변수를 같이 사용할 수 있다. 이걸 flag를 통해서 소통하게 된다. 여기서 중요한 점은, 프로세스는 공유변수를 읽을 때 원자단위의 read / write만 써야한다는 것이다.
  - Q. 원자단위의 read / write가 뭔가? A. 정말 읽거나, 대입만 하는 것. x = 10과 같은 것. x++는 "읽고 현재값을 기준으로 10을 더하는" 행동이기 때문에 원자단위 write가 아님.
  - Q. 왜 그래야 하는가? 프로세스가 이전 값에 의존해서 프로그래밍을 했는데, 핸들러가 공유변수를 바꾸면 논리가 깨질 수 있음. "읽어둔 공유변수값이 현재는 다를 수 있다는 전제에서", "안전하게 프로그래밍할 수 있는" 방법론이 필요.

### Portable signal handling

- unix의 안 좋은 점 중 하나는 signal hanldling semantics가 제각기라는 것.
- 이런 문제를 해결하기위해 handler install을 비교적 쉽게 하게 해주는 sigaction 함수가 있음.

### 플로우 동기화: 동시성 버그 피하기

- 어느 시점에 concurrent flow를 실행시킬 것이냐는 굉장이 중요한 문제. 옳은 실행을 가능하게 해주는 지점도, 그렇지 않은 지점도 있다. 문제는 후보가 너무 많음.
- 다음 예제를 고려해보자.

```c
1 /* WARNING: This code is buggy! */
2 void handler(int sig)
3 {
4 int olderrno = errno;
5 sigset_t mask_all, prev_all;
6 pid_t pid;
7
8 Sigfillset(&mask_all);
9 while ((pid = waitpid(-1, NULL, 0)) > 0) { /* Reap a zombie child */
10 Sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
11 deletejob(pid); /* Delete the child from the job list */
12 Sigprocmask(SIG_SETMASK, &prev_all, NULL);
13 }
14 if (errno != ECHILD)
15 Sio_error("waitpid error");
16 errno = olderrno;
17 }
18
19 int main(int argc, char **argv)
20 {
21 int pid;
22 sigset_t mask_all, prev_all;
23
24 Sigfillset(&mask_all);
25 Signal(SIGCHLD, handler);
26 initjobs(); /* Initialize the job list */
27
28 while (1) {
29 if ((pid = Fork()) == 0) { /* Child process */
30 Execve("/bin/date", argv, NULL);
31 }
32 Sigprocmask(SIG_BLOCK, &mask_all, &prev_all); /* Parent process */
33 addjob(pid); /* Add the child to the job list */
34 Sigprocmask(SIG_SETMASK, &prev_all, NULL);
35 }
```

- 별로 문제가 없어보이지만 `main에서 만든 addjob 전에 자녀 프로세스가 죽는 경우`를 생각해보자.
- 자녀 프로세스가 SIGCHLD 시그널 전송 -> 핸들러 동작 -> deletejob 수행의 과정을 거치면 addjob 전에 deletejob을 수행하는 문제가 발생한다.
- 아래와 같이 sigchld를 막음으로서, 핸들러가 가장 빠르게 동작한다고 가정해도 순서를 지킬 수밖에 없도록 하는 게 해결책.

```c
29 while (1) {
30 Sigprocmask(SIG_BLOCK, &mask_one, &prev_one); /* Block SIGCHLD */
31 if ((pid = Fork()) == 0) { /* Child process */
32 Sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */
33 Execve("/bin/date", argv, NULL);
34 }
35 Sigprocmask(SIG_BLOCK, &mask_all, NULL); /* Parent process */
36 addjob(pid); /* Add the child to the job list */
37 Sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */
38 }
39 exit(0);
```

### syscall 정리

- 더 논리를 진행하기 전에, unix에서 제공하는 syscall과 그 인자를 파악하는 작업이 필요.
- 먼저 sigprocmask에 대해 알아야한다. 아래와 같이 생김.

```c
int sigprocmask(
    int how,
    const sigset_t *set,
    sigset_t *oldset
);
```

- 여기서 how는 SIG_BLOCK, SIG_UNBLOCK, SIG_SETMASK 세 가지 값만이 가능하고,
  - blcok은 현재 blocked set에 set을 추가하고, unblock은 빼고, setmask는 set으로 아예 대체한다.
  - setmask는 조금 독특한데, set을 변경한뒤, oldset 포인터에 기존 blocked set의 주소를 저장해둔다.
- sigset_t mask_all, mask_one, prev_one 같은 변수들이 자주쓰이는데, 시그널 집합을 표현하는 타입이라고 생각하면 되고,
- 이 타입들은 Sigemptyset, Sigaddset, Sigfillset과 같이 쓰인다. emptyset은 해당 주소에 빈집합을 만들어주고, addset은 원소를 더하며 fillset은 해당 집합을 모든 시그널로 채운다.

### Explicitly Waiting for Signals

- 실행을 위해 시그널 핸들러를 기다려야 하는 형태도 존재함. 예를 들면 쉘. 입력을 받기 위해 자녀 프로세스가 수확되고 프로세스가 종료되는 작업이 필요.
- 아래 코드를 생곡해보자.

```c
while (!pid) {
  1. pause(); 
  2. sleep(1); 
}
```

- 1은 correcntess가 훼손될 수 있음. 이유? pid를 확인하고 pause로 넘어가는 순간 pid가 핸들러에 의해 변할 수도.
- 2는 correctness 이슈는 없지만, 너무 느리다. 다른 방법이 필요함.

### nonlocal jumps

- C에서는 한 함수가 리턴되지 않고도 실행권을 다른 함수에 넘길 수 있으며, 유저가 사용할 수 있는 ECF를 제공한다. setjmp, longjmp가 그것.
- 먼저 setjmp는 아래와 같이 정의된다.

```c
int setjmp(jmp_buf env);
int sigsetjmp(sigjmp_buf env, int savesigs);
```

- env에는 calling environment(e.g., 스택 포인터, pc 등)이 저장된다. 리턴값은 0인데, 해당 리턴 값을 저장해두는 것은 잘못된 행동임.
- 다음은 longjmp.

```c
void longjmp(jmp_buf env, int retval);
void siglongjmp(sigjmp_buf env, int retval);
```

- longjmp는 받은 env를 복원해서 함수를 실행한다. 실행한 함수는 가장 최근에 setjmp가 실행된 지점에 리턴을 한다. 또한 retval은 non-zero 값이다.
- setjmp는 longjmp로 리턴받기 때문에, 한 번 호출하고, 여러 번 리턴 받는 것이 가능하다.
- 왜 이런 형태의 함수가 필요한가? 함수 실행시 에러를 받아낼 경우 stack unwinding의 비용을 줄이고 싶기 때문이다.
  - 예를 들어 매우 nested된 call이 에러를 발견했다고 가정해보자. 돌아가야하는 게 너무 많을 것. 이때 setjmp <-> longjmp로 바로 이동하도록 만들어주면 부담이 거의 없이 사용 가능하다.
