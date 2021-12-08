Aplikacja nie zawiera implementacji aplikacji webowej dla klientów

Opis plików:


*Docker*

1. ./start_docker.sh, ./nocache_start_docker.sh, ./root.sh, ./kill_docker.sh
Proste skrypty do obsługi dockera.

1. ./Dockerfile
Dockerfile zadany w treści.

1. ./loop.c
Proteza serwera aplikacji web dla klientów.
Normalnie ostatnim poleceniem w Dockerfile'u byłoby "CMD komenda-do-uruchomienia-serwera".
Serwera w tej aplikacji nie ma, a aplikacja "loop" podtrzymuje żywotność kontenera,
nie zabierając dużo mocy procesora.


*Aplikacja pracownika*

1. ./officerssh.c, ./officerfiles.c, ./officerfiles.h, ./officeractions.c, ./officeractions.h
Pliki źródłowe aplikacji pracownika banku.

1. ./Makefile
Makefile do 

1. ./pam_currenttime.c, ./officerapp
Źródło modułu PAM dla aplikacji pracownika oraz plik reguł, z którego korzysta ./officerssh

1. ./clock.py
Wspierająca aplikacja ułatwiająca logowanie się z PAM. Wypisuje aktualny czas EPOCH.

1. ./sshd_config
Konfiguracja logowania ssh dla pracownika.

1. ./sudoers
Plik /etc/sudoers, aby pracownik mógł wykonać "chown" po stworzeniu pliku dla klienta,
tak aby stworzony plik należał do klienta.

1. ./add_credit.lck, ./add_deposit.lck
W officerapp, przy dodawaniu nowego pliku trzeba przejrzeć wszystkie inne pliki,
znaleźć w nich taki o największym numerze i stworzyć nowy plik z numerem o jeden większym od znalezionego.
W celu zapewnienia współbieżności, przed rozpoczęciem przeglądania plików w poszukiwaniu numeru,
zakładany jest flock na odpowiednim pliku (osobny dla kredytów i lokat).

1. ./code_check.sh
Skrypt do sprawdzenia jakości kodu, używający narzędzi do tego przeznaczonych.


*Konfiguracja*

1. ./uzytkownicy.txt, ./configure_users.sh
Lista użytkowników i skrypt konfigurujący strukturę plików i użytkowników względem tej listy.

1. ./firewall.nft
Zasady firewalla.


