# bsk1
Computer Systems Security course on MIM UW, task 1 - Containerised app for use by officers and clients of a bank

Duże zadanie 1: mała odizolowana aplikacja (termin oddania 10.12.2021, godzina 19:00)
Wprowadzenie i polecenie
Zadanie będzie polegało na zaimplementowaniu i testowym wdrożeniu pewnej prostej aplikacji do obsługi kredytów i lokat w banku Green Forest Bank. Aplikacja będzie się składała z dwóch modułów dostępowych: (1) przez WWW dla klientów banku i (2) za pomocą interfejsu tekstowego dla pracowników banku. Obie współdziałające ze sobą moduły mają być umieszczone w kontenerze Dockera. Przewidujemy następujące szczegółowe elementy zadania (w nawiasie przewidywana maksymalna liczba punktów za spełnienie wymagania):

Stworzenie początkowej struktury użytkowników, katalogów i praw do nich (1 pkt.)
Stworzenie aplikacji webowej dla klientów
Zapewnienie identyfikacji serwera za pomocą certyfikatu (1 pkt.)
Oprogramowanie logowania (1 pkt.)
Oprogramowanie wyświetlania lokat i kredytów (1 pkt.)
Stworzenie aplikacji dla pracowników banku
Oprogramowanie logowania z użyciem funkcji PAM (1 pkt.)
Oprogramowanie wyświetlania lokat i kredytów (1 pkt.)
Oprogramowanie dodawania lokat i kredytów (1 pkt.)
Oprogramowanie modyfikacji zawartości lokat i kredytów (1 pkt.)
Stworzenie reguł ściany ogniowej zabezpieczających interakcję z aplikacją (1 pkt.)
Użycie dostępnych narzędzi do badania bezbłędności oprogramowania ze szczególnym uwzględnieniem problemów wynikających z przepełnienia bufora oraz zapewnienie klarownej struktury oprogramowania - znaczące identyfikatory, komentarze dokumentujące odległe zależności w kodzie (1 pkt.)
Reprezentacja lokat i kredytów w systemie plików
Informacje o lokatach i kredytach są umieszczone odpowiednio w dwóch katalogach deposits i credits. Informacje o każdej lokacie lub kredycie są umieszczone w osobnym pliku w odpowiednim katalogu. Nazwy plików mają być unikalne. Właścicielem pliku jest klient, który zakłada lokatę lub bierze kredyt. Plik opisujący lokatę lub kredyt ma format:

Name: <imię i nazwisko klienta>
Number: <unikalny numer lokaty lub kredytu>
Sum: <suma>
Date: <data początkowa okresu rozliczeniowego>
Procent: <procent w okresie rozliczeniowym>
Date: <data końcowa starego okresu rozliczeniowego i jednocześnie nowego>
Procent: <procent w okresie rozliczeniowym>
 .
 .
 .
Date: <data końcowa starego okresu rozliczeniowego>
Sum: <nowa suma>
Date: <data początkowa okresu rozliczeniowego>
Procent: <procent w okresie rozliczeniowym>
Date: <data końcowa starego okresu rozliczeniowego i jednocześnie nowego>
Procent: <procent w okresie rozliczeniowym>
 .
 .
 .
Date: <data końcowa starego okresu rozliczeniowego>
Sum: <nowa suma>
 .
 .
 .
Plik na początku zawiera informacje identyfikujące dotyczące klienta i numeru lokaty lub kredytu. Następnie występuje pewna liczba sekcji zaczynających się od wiersza postaci

Sum: <jakaś suma>
Każda taka sekcja obejmuje ciąg okresów rozliczeniowych, w których klient nie podejmował żadnych działań związanych z lokatą lub kredytem (nie wypłacał/wpłacał pieniędzy), ale za to w okresach tych bank mógł zmieniać oprocentowanie. Przyjmujemy, że wskazana data końcowa okresu rozliczeniowego jest pierwszą datą, która do niego nie należy. Ostatni okres rozliczeniowy w pliku może nie mieć daty końcowej. Oprocentowanie zawsze jest podawane w skali rocznej. Zapis w wierszu z etykietą Sum: oznacza, że nastąpiła wpłata lub wypłata na lokatę lub kredyt (w przypadku kredytów zapewne wpłaty będą się zdarzały w regularnych okresach, np. co miesiąc).

Przykład
Wyobraźmy sobie, że Zwonimir Babacki wziął kredyt na 100000 zł, który jest spłacany w miesięcznych ratach, dla którego procent doliczany jest po każdym kwartale zgodnie z obowiązującymi w kwartale oprocentowaniami, ale w stosunku rocznym jego początkowa wartość to 10%, ale w połowie drugiego miesiąca płacenia zmieniła się stawka procentowa na 9%. Wtedy plik opisujący ten kredyt po 4 miesiącach jego funkcjonowania przy miesięcznych ratach 900 zł wyglądałby tak:

Name: Zwonimir Babacki
Number: 123342
Sum: 100000
Date: 01.07.2021
Procent: 10
Date: 01.08.2021
Sum: 99100
Date: 01.08.2021
Procent: 10
Date: 16.08.2021
Procent: 9
Date: 01.09.2021
Sum: 98200
Date: 01.09.2021
Procent: 9
Date: 01.10.2021
Sum: 99673,96
Date: 01.10.2021
Procent: 9
Date: 01.11.2021
Sum: 98773,96
Date: 01.11.2021
Procent: 9
Proszę zwrócić uwagę, że po regularnych zmniejszeniach kwoty kredytu, 1.10 następuje zwiększenie o kwotę wynikającą z oprocentowania kredytu, która uwzględnia zmianę, jaka nastąpiła w połowie sierpnia. Wartość ta jest obliczona jako 99673,96=100000-3*900+849,31+407,27+390,97+726,41.

Szczegółowy opis aplikacji klienta
Aplikacja klienta ma udostępniać klientowi za pomocą interfejsu WWW możliwość logowania się z użyciem hasła. Dla uproszczenia należy przyjąć, że hasła są predefiniowane (tzn. klient ani pracownik nie ma możliwości zmiany hasła, które zostało ustanowione przy uruchamianiu aplikacji). Hasła i loginy klientów są umieszczone w plikach systemowych kontenera (/etc/shadow, /etc/passwd). Po zalogowaniu się klient ma możliwość obejrzenia stanu i historii wszystkich swoich lokat i kredytów. Funkcjonalność ta może zostać zrealizowana w sposób uproszczony, np. przez wyświetlenie bezpośrednio zawartości odpowiednich plików.

Serwer WWW udostępniający aplikację klienta ma być identyfikowany za pomocą odpowiedniego certyfikatu X.509 z nazwą serwera postaci: <login-na-students>.zadnaie1.bsk wystawionego przez własne CA o nazwie BSK-CA-2021, gdzie <login-na-students> oznacza login rozwiązującego zadanie na wydziałowej maszynie students.

Szczegółowy opis aplikacji pracownika banku
Aplikacja pracownika banku ma być napisana w języku C. Pracownik banku powinien móc połączyć się z kontenerem za pomocą SSH i tam automatycznie zamiast domyślnego programu, w którym prowadzona jest sesja interakcyjna, uruchomić program udostępniający wskazane funkcjonalności za pomocą minimalistycznego interfejsu tekstowego (numerowane opcje wypisywane na ekranie, pracownik banku wpisuje numer wybranej opcji).

Przed udostępnieniem właściwej funkcjonalności program powinien przeprowadzić procedury logowania się z użyciem własnej funkcji konwersacji PAM. W trakcie procedury logowania po podaniu loginu i hasła użytkownik powinien zostać poproszony o podanie czasu systemowego (liczone w sekundach od daty początku epoki - tzn. 00:00:00 dnia 1.01.1970 UTC). Jeśli podany czas różni się od czasu sprawdzonego przez program o mniej niż 15 sekund, to należy pracownika dopuścić do korzystania z głównej funkcjonalności.

Główne menu modułu tekstowego powinno zawierać cztery opcje:

Określenie klienta, dla którego będą wykonywane operacje
Oprogramowanie wyświetlania lokat i kredytów
Oprogramowanie dodawania lokat i kredytów
Oprogramowanie modyfikacji zawartości lokat i kredytów
Wybranie pierwszej opcji powoduje ustawienie klienta, dla którego będą wykonywane następne opcje.

Wybranie drugiej opcji powoduje wczytanie wszystkich lokat i kredytów wskazanego klienta i wypisanie ich zawartości na ekranie przy czym dane z plików należy wczytać do odpowiednio zaprojektowanych wewnętrznych struktur danych i listę okresów należy uporządkować względem ich początków i wypisać od zaczynającego się najbliżej obecnej daty do rozpoczynającego się najdawniej.

Wybranie trzeciej opcji pozwala pracownikowi na wskazanie czy dodawana będzie lokata czy kredyt. Następnie pracownik może wprowadzić identyfikator klienta, sumę i datę rozpoczęcia pierwszego okresu rozliczeniowego wraz z procentem. Dane te pozwolą na wypełnienie początkowych pięciu wierszy pliku z opisem lokaty lub kredytu. Nie są podawane bezpośrednio imię i nazwisko – to należy pobrać z systemowej informacji o użytkownikach (np. za pomocą wywołania getpwnam) oraz numer lokaty lub kredytu – ten musi zostać ustalony na podstawie istniejących już numerów lokat i kredytów.

Wybranie czwartej opcji pozwala pracownikowi na wskazanie lokaty lub kredytu, dla którego będzie wykonywana operacja, a następnie

dodanie do wskazanej lokaty lub kredytu nowej pozycji Sum: z datą jej początku i procentem lub
dodanie nowej daty zakończenia okresu rozliczeniowego wraz datą rozpoczęcia nowego okresu oraz nowym procentem, lub
dodanie nowej daty zakończenia okresu rozliczeniowego (taki wpis kończy wybraną lokatę lub kredyt).
Program musi sprawdzać, że data zakończenia okresu rozliczeniowego jest po znajdującej się w pliku lokaty lub kredytu dacie jego rozpoczęcia.

Inicjalizacja aplikacji
Aplikacja obejmująca dwa powyższe moduły musi być umieszczona w kontenerze Dockera. Kompilacja programów wchodzących w skład aplikacji może odbywać się poza kontenerem. W ramach konfiguracji kontenera powinno odbyć się

utworzenie grup użytkowników officers oraz clients,
utworzenie kont pracowników i klientów na podstawie zadanego pliku o formacie podanym niżej oraz włożenie ich do odpowiednich grup powyżej,
utworzenie katalogów deposits i credits,
ustawienie praw dostępu zgodnie z opisem
każdy klient ma prawo do odczytu zawartości katalogów deposits i credits,
żaden klient nie ma prawa do odczytu i zapisu nowo tworzonych plików w katalogach deposits i credits oprócz plików, które są jego własnością,
każdy pracownik ma prawo do odczytu i zmiany zawartości katalogów deposits i credits,
każdy pracownik ma prawa do odczytu i zapisu nowo tworzonych plików w katalogach deposits i credits.
Plik opisujący początkowy zestaw pracowników i klientów składa się z wierszy. Każdy wiersz opisuje klienta lub pracownika:

<identyfikator> <rola> <imię> <nazwisko>
gdzie <identyfikator> to identyfikator użytkownika używany w kontenerze, <rola> to jedna z ról client lub officer, <imię> i <nazwisko> to odpowiednio imię i nazwisko klienta lub pracownika. Należy przyjąć, że identyfikatory są unikalne. Przykładowe dane znajdują się w załączonym pliku.

Ściana ogniowa
Na maszynie, na której funkcjonować będzie aplikacja należy ustawić reguły ściany ogniowej tak, aby * możliwa była komunikacja SSH z maszyną, * możliwa była komunikacja za pomocą opisanej powyżej strony WWW, * możliwa była komunikacja z aplikacją pracownika banku, * żadna inna komunikacja TCP i UDP nie była możliwa, * dostępna była możliwość diagnostyki stanu serwera za pomocą protokołu ICMP.

Na pytania do zadania odpowiada Aleksy Schubert na forum. Pytania zadane w ostatnim tygodniu przed terminem oddania zadania pozostaną bez odpowiedzi. Pytania zadane przed początkiem ostatniego tygodnia mogą spowodować uszczegółowienia treści zadania pomagające w jego interpretacji.
