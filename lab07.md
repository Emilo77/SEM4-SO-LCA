
## 1. Rzut oka na architekturę systemu

MINIX jest systemem o architekturze mikrojądra. Oznacza to, że jądro zapewnia najbardziej podstawową funkcjonalność: obsługuje przerwania, umożliwia zarządzania procesami, implementuje obsługę komunikacji międzyprocesowej oraz odpowiada za przełączanie procesów. Wszystkie pozostałe elementy, takie jak obsługa systemów plików, sieci, sterowniki urządzeń itp., są uruchomione jako serwery i działają poza przestrzenią jądra. Większość wywołań systemowych obsługiwanych jest przez serwery. Samo jądro obsługuje niewielką grupę wywołań systemowych, zwanych zadaniami. Więcej o architekturze systemu i o serwerach dowiemy się na kolejnych zajęciach. Dzisiaj skupimy się na tym, jak dodać nowe wywołanie systemowe.

## 2. Nowe wywołanie systemowe

Podamy teraz przepis na dodanie nowego wywołania systemowego do MINIX-a. Do obsługi naszego wywołania wybieramy serwer `ipc`. Zgodnie z zasadami sztuki dodanie nowego wywołania systemowego składa się z dwóch etapów:

- napisanie kodu funkcji obsługującej wywołanie systemowe,
- napisanie funkcji bibliotecznej, która będzie wywoływać tę funkcję.

Funkcja biblioteczna ma zadanie ukryć szczegóły techniczne przed użytkownikiem i ułatwić korzystanie z danego wywołania systemowego.

### 2.1. Argumenty funkcji – struktura `message`

Struktura `message` zawiera informację o źródle wywołującym funkcję systemową, a także argumenty przekazane do funkcji, o ile takie są. Argumenty muszą zostać umieszczone w postaci jednego z dostępnych formatów. Cała struktura i wszystkie dostępne formaty są zdefiniowane w pliku `/usr/include/minix/ipc.h`

Funkcja systemowa może przekazać wynik do źródła również poprzez odpowiednią modyfikację zawartości struktury message.

---
#### Ćwiczenie 1

Obejrzyj dostępne formaty struktury message w pliku /usr/include/minix/ipc.h.

---

<details>
  <summary>Wskazówka</summary>
  
Struktura message to unia. Jeśli wybierzemy mess_1, to pierwsze trzy pola (poza źródłem) m1_i1, m1_i2, m1_i3 są typu int i to nam w zupełności wystarczy. Korzystamy ze struktury m na przykład tak:
  
```c
message *m;
m->m1_i1 = x;
```
  
Pozwalają na to definicje na końcu pliku ipc.h.
  
---
  
</details>

### 2.2. Funkcja biblioteczna

Pakowaniem parametrów do jednego z formatów struktury `message`, przekazywaniem ich do funkcji obsługującej wywołanie systemowe i odczytywaniem wyników zajmuje się funkcja biblioteczna. Pełni ona rolę opakowania (ang. *wrapper*) właściwego wywołania systemowego. Możemy założyć, że każde wywołanie funkcji systemowej nastąpi za pośrednictwem biblioteki.

### 2.3. Pliki źródłowe systemu MINIX

Pliki źródłowe znajdują się w katalogu `/usr/src/minix`. Natomiast pliki nagłówkowe w katalogach `/usr/src/include` oraz `/usr/src/minix/include/minix`. Należy zwrócić uwagę, że te same pliki nagłówkowe są też odpowiednio w katalogach `/usr/include` oraz `/usr/include/minix`. Kompilator `clang` szuka plików nagłówkowych w `/usr/include`, ponieważ tak ma ustawioną ścieżkę. Wywołanie make includes w katalogu `/usr/src` kopiuje pliki nagłówkowe z `/usr/src/…` do `/usr/include`. Kilka innych wywołań `make`, m.in. `make install`, wywołuje `make includes`. Budując fragmenty systemu, trzeba zadbać o to, żeby zmienione pliki nagłówkowe były identyczne w obydwu miejscach. Można to robić „ręcznie”, albo uruchamiać `make includes` przed rozpoczęciem właściwej kompilacji.

## 3. Tworzenie funkcji obsługującej wywołanie systemowe

Tradycyjnie, nasze pierwsze wywołanie systemowe wypisze na ekran „Hello world!”

## 3.1. Numer wywołania systemowego

Każde wywołanie systemowe ma unikalny numer. W pierwszej kolejności musimy wybrać numer dla naszego nowego wywołania systemowego. Ponieważ będzie ono umieszczone w serwerze `ipc`, warto zadbać o to, aby number wywołania należał do grupy związanej z `ipc`.

Numery poszczególnych wywołań systemowych związanych z `ipc` znajdują się w plikach:

- `/usr/src/minix/include/minix/com.h`
- `/usr/include/minix/com.h`

W sekcji dotyczącej `ipc` (wyszukaj `IPC_BASE`) należy dopisać wiersz

```c
#define IPC_PRINTMESSAGE (IPC_BASE+8)
```

Magiczna liczba 8 jest po prostu kolejnym wolnym numerem w tablicy wywołań systemowych – poprzednie są zajęte na wywołania związane z semaforami i segmentami pamięci dzielonej.

### 3.2. Dodanie wywołania do odpowiedniego serwera

Następnie trzeba dodać kod funkcji obsługującej to wywołanie systemowe oraz zarejestrować nowe wywołanie systemowe.

W kodzie serwera `ipc`, którego pliki źródłowe znajdują się w katalogu `/usr/src/minix/servers/ipc`, należy zmodyfikować lub stworzyć następujące pliki:

- `inc.h`
- `do_printmessage.c`
- `main.c`
- `Makefile`

#### 3.2.1. Nagłówek funkcji obsługującej wywołanie systemowe

W pliku `inc.h` dodajemy na końcu nagłówek naszej funkcji obsługującej wywołanie systemowe.

```c
int do_printmessage(message *);
```

#### 3.2.2. Właściwa funkcja

W pliku `do_printmessage.c` umieszczamy kod naszej funkcji obsługującej wywołanie systemowe:

```c
#include "inc.h"
#include <stdio.h>

int do_printmessage(message *m)
{
        printf("Hello world!\n");
        return OK;
}
```

#### 3.2.3. Rejestracja funkcji

W pliku `main.c` rejestrujemy naszą funkcję pod numerem `IPC_PRINTMESSAGE`.

```c
// (...)
} ipc_calls[] = {
        // (...)
        {IPC_SEMOP,        do_semop,        1},
        {IPC_PRINTMESSAGE, do_printmessage, 0},
};
```

Trzeci element tablicy informuje o tym, czy funkcja ma być blokująca. Wartość 0 oznacza, że nie, a 1, że tak. Przyjrzymy się temu bliżej na kolejnych zajęciach.

#### 3.2.4. Plik Makefile

W pliku `Makefile` dodajemy informację o nowym pliku do skompilowania.

```console
SRCS=	main.c utility.c shm.c sem.c do_printmessage.c
```

### 3.3. Kompilowanie

Po wykonaniu tych kroków należy skompilować i zainstalować serwer `ipc`. W katalogu `/usr/src/minix/servers/ipc` wykonujemy

```minix
# make
# make install
# reboot
```

## 4. Bezpośrednie wywołanie funkcji systemowej

Teraz możemy już korzystać z naszego nowego wywołania systemowego. Sprawdźmy to przed napisaniem funkcji bibliotecznej.

Do wywołania funkcji systemowej służy funkcja `_syscall()` zdefiniowana w pliku `/usr/include/lib.h`:

```c
int _syscall(endpoint_t who, int syscallnr, message *msgptr);
```

Parametr `who` oznacza adresata funkcji systemowej (w naszym wypadku będzie to serwer `ipc`), syscallnr jest numerem wywołania funkcji systemowej (czyli `IPC_PRINTMESSAGE`), a `msgptr` jest wskaźnikiem na odpowiednią strukturę `message`, która zostanie przekazana funkcji obsługującej wywołanie systemowe. Aby otrzymać identyfikator adresata, można skorzystać z funkcji `minix_rs_lookup()`, zdefiniowanej w pliku `/usr/include/minix/rs.h`:

```c
int minix_rs_lookup(const char *name, endpoint_t *value)
```

Funkcja przekazuje 0 w przypadku sukcesu, a −1 w przypadku porażki. Parametr `name` oznacza nazwę szukanego adresata (`"ipc"`), a w zmiennej, na którą wskazuje `value`, zostanie zapisany identyfikator odbiorcy.

Następujący program (`test-direct-printmessage.c`) wywoła nową funkcję systemową.

```c
#include <lib.h>
#include <minix/rs.h>

int main()
{
        message m;
        endpoint_t ipc_ep;
        minix_rs_lookup("ipc", &ipc_ep);
        _syscall(ipc_ep, IPC_PRINTMESSAGE, &m);
}
```

Program możesz skompilować poleceniem `make` i nie musisz pisać wcześniej pliku `Makefile` (zalety `NetBSD`).

```minix
# make test-direct-printmessage
```
---

#### Ćwiczenie 2

Wykonaj wszystkie wymienione kroki i sprawdź, czy po wywołaniu `test-direct-printmessage` zobaczysz napis „Hello world!” na konsoli, a na końcu pliku `/var/log/messages` wpis

```minix
<aktualna data i czas>  kernel: Hello world!
```

## 5. Tworzenie funkcji bibliotecznej

Funkcja biblioteczna zajmuje się stworzeniem i wypełnieniem struktury `message`, a następnie przekazaniem jej do wywołania systemowego. Naszą przykładową funkcję biblioteczną dodamy do biblioteki `libc`. Funkcja musi być widoczna w programach użytkowych, więc należy ją zadeklarować w jednym z plików nagłówkowych biblioteki. Wybieramy do tego `unistd.h`.

### 5.1. Nagłówek funkcji bibliotecznej

Nagłówek naszej funkcji dodamy w plikach `/usr/include/unistd.h` i `/usr/src/include/unistd.h`.

```c
int printmessage(void);
```

### 5.2. Funkcja biblioteczna
#### 5.2.1. Treść funkcji

W katalogu `/usr/src/lib/libc/misc` umieszczamy plik `printmessage.c` z implementacją wywołania `printmessage()`:

```c
#include <lib.h>
#include <minix/rs.h>

int get_ipc_endpt(endpoint_t *pt)
{
        return minix_rs_lookup("ipc", pt);
}

int printmessage(void)
{
        endpoint_t ipc_pt;
        message m;
        if (get_ipc_endpt(&ipc_pt) != 0)
        {
                errno = ENOSYS;
                return -1;
        }
        return (_syscall(ipc_pt, IPC_PRINTMESSAGE, &m));
}
```

### 5.2.2. Plik Makefile

Dodajemy także informację o naszej nowej funkcji do pliku `/usr/src/lib/libc/misc/Makefile.inc`:

```minix
SRCS+=  stack_protector.c printmessage.c
```

### 5.3. Kompilowanie

Aby zbudować nową wersję biblioteki, należy po wejściu do katalogu `/usr/src/lib/libc` wykonać polecenia

```minix
# make
# make install
```

Po zainstalowaniu nowej biblioteki powinien się skompilować przykładowy program `test-printmessage.c`:

```c
#include <unistd.h>

int main()
{
        printmessage();
        return 0;
}
```
a następnie po jego uruchomieniu na konsoli i w pliku `/var/log/messages` powinniśmy zobaczyć „Hello world! ”.

---

#### Ćwiczenie 3
Sprawdź działanie funkcji bibliotecznej `printmessage()`.

---

#### Ćwiczenie 4

Zaimplementuj jednoelementowy schowek, dostępny przez wywołanie systemowe obsługiwane przez serwer `ipc`. Nagłówek funkcji bibliotecznej powinien być następujący:

```c
int storage(int number);
```

Początkowo w schowku znajduje się liczba 0. Po wywołaniu funkcji w schowku ma znaleźć się liczba przekazana jako parametr `number`. Wywołanie systemowe przekazuje jako wynik poprzednią wartość przechowywaną w schowku.

Wywołanie systemowe ma być dostępne po dołączeniu nagłówka `unistd.h`.

Przykłady użycia:

```c
int main()
{
        int x = storage(1);
        // x == 0;
        int y = storage(2);
        // y == 1;
        return 0;
}
```
Kolejne wywołanie:
```c
int main()
{
        int x = storage(3);
        // x == 2;
        int y = storage(0);
        // y == 3;
        return 0;
}
```

W pliku `test-storage.c` znajduje się program testujący poprawność implementacji funkcji `storage()`.

<details>
  <summary>Wskazówka</summary>
  
  Wartość przechowywaną w schowku możesz przechowywać w zmiennej `static`.
  
</details>

<details>
  <summary>Odpowiedź</summary>
  
```c
/* /usr/src/minix/servers/ipc/do_storage.c */
#include "inc.h"

int do_storage(message *m)
{
        static int n = 0;
        int old;
        old = n;
        n = m->m1_i1;
        printf("Syscall storage called: old = %d, new = %d\n", old, n);
        m->m1_i1 = old;
        return OK;
}
```
  
```c
/* /usr/src/lib/libc/misc/storage.c */
#include <lib.h>
#include <minix/rs.h>

int storage(int x)
{
        message m;
        endpoint_t ipc_ep;
        minix_rs_lookup("ipc", &ipc_ep);
        m.m1_i1 = x;
        _syscall(ipc_ep, IPC_STORAGE, &m);
        printf("syscall return: %d\n", m.m1_i1);
        return m.m1_i1;
}
```
</details>

## 6. Dodawanie wywołań systemowych obsługiwanych przez inne serwery

Schemat dodawania nowych wywołań do innych serwerów jest taki sam, ale niektóre szczegóły różnią się. Na przykład jeśli będziemy chcieli dodać funkcję do serwera `pm`, to konieczna będzie zmiana następujących plików (ścieżki względne dotyczą katalogu `/usr/src/minix`):

- `include/minix/callnr.h` oraz `/usr/include/minix/callnr.h` – dodanie numeru nowej funkcji (patrz komentarz na początku pliku `include/minix/com.h`)
- `servers/pm/proto.h` – dodanie nagłówka nowej funkcji
- `servers/pm/<nowa-funkcja.c>` – implementacja nowej funkcji
- `servers/pm/table.c` – rejestracja nowej funkcji
- `servers/pm/Makefile`

W przypadku niektórych serwerów nie wystarczy wykonać `make && make install`. Na przykład zmiana w serwerze `pm` wymaga także przeinstalowania jądra systemu, ponieważ część funkcjonalności tego serwera wchodzi w skład jądra. W katalogu `/usr/src/releasetools` należy wykonać `make do-hdboot`, a po restarcie systemu wybrać najnowszą wersję jądra.
  
---

#### Ćwiczenie 5
  
Dodaj wywołanie systemowe wypisujące „Hello world from server PM!” obsługiwane przez serwer `pm`. Napisz także odpowiednią funkcję biblioteczną.
