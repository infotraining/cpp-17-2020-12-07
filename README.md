# Szkolenie - Programowanie w C++17 #

## Dokumentacja + slajdy

* https://infotraining.bitbucket.io/cpp-17
* https://gitpitch.com/infotraining-dev/cpp-17/develop?grs=bitbucket&p=slides_pl#/

## Zdalna maszyna wirtualna (SSH)

* link do rejestracji: https://labs.azure.com/register/rrteny9y
  - uczestnik powinien mieć konto Github lub Microsoft
  - ustawić swoje hasło przy rejestracji

## Konfiguracja lokalna - toolchain

Jeśli uczestnik chce skonfigurować lokalnie swoje środowisko, to należy zainstalować:

### Kompilator + CMake

* Dowolny kompilator C++ (gcc, clang, Visual C++) wspierający C++17
  * polecany kompilator pod Windows (https://nuwen.net/mingw.html) - instalacja polega na rozpakowaniu archiwum do katalogu C:\MinGW. Szczegółowy opis instalacji znajduje się na stronie.

* [CMake > 3.15](https://cmake.org/)
  * proszę sprawdzić wersję w lini poleceń

  ```
  cmake --version
  ```

## Visual Studio Code

* [Visual Studio Code](https://code.visualstudio.com/)

### Zainstalować wtyczki C++

* C/C++
* CMakeTools

### Zainstalować wtyczki do pracy zdalnej

* Remote - SSH
  - opis pracy z wtyczką: https://code.visualstudio.com/docs/remote/ssh
  - UWAGA! Windows 10 - [wymagana jest instalacja klienta OpenSSH lub Git for Windows (wcześniejsze wersje Windows)](https://code.visualstudio.com/docs/remote/troubleshooting#_installing-a-supported-ssh-client)
  - sprawdzić, czy klient SSH jest dostępny w terminalu (dodany do zmiennej PATH)
  - przy połączeniu z maszyną zdalną należy kliknąć na link **details** w pop-upie w prawym dolnym rogu. Umożliwi to wprowadzenie ustawionego wcześniej hasła.

* Live Share

* W przypadku pracy ze zdalną maszyna przez SSH należy za(re)instalować wtyczki C++ po połączeniu z maszyną.