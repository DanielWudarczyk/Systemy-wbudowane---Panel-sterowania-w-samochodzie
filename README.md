#Projekt z systemów wbodowanych
##PANEL STEROWANIA W SAMOCHODZIE

Opis projektu:
Projekt realizuje symulację panelu sterowania w samochodzie, który umożliwia sterowanie radiem, klimatyzacją i kontrolą trakcji oraz wyświetlanie ustawionych parametrów.
Projekt został stworzony w środowisku Keil, jest oparty na mikrokontrolerze AT89C52 oraz schemacie zaprojektowanym w programie Proteus.

###Opis funkcjonalności jakie wykonuje układ:
1. Funkcje mikrokontrolera nr 1 podłączonego do klawiatury i diody LED:
-Dioda pokazuje czy radio jest włączone
-Przełączanie między stacjami radia (efekt na ekranie BCD)
-Sterowanie głośnością radia oraz wyciszanie go i odciszanie (efekt na ekranie BCD)
-Włączanie / wyłączanie klimatyzacji (efekt na silniczku i ekranie LCD)
-Sterowanie poziomem nadmuchu i trybem działania klimatyzacji (efekt na silniczku, żarówce i ekranie LCD)
-Przełączanie między sterowaniem radiem a kontrolą trakcji (podwójne mignięcie diody)
-Wybieranie trybu kontroli trakcji w samochodzie (efekt na ekranie LCD)
2. Funkcje mikrokontrolera nr 2 podłączonego do przycisku i wyświetlacza BCD:
-Włączanie / wyłączanie radia (przerwanie INT0)
-Wyświetlanie na wyświetlaczu aktualnie słuchanej stacji radiowej
-W chwili zmiany głośności radia pokazanie ustawionej głośności
-Wyświetlanie symbolu wyciszenia radia
3. Funkcje mikrokontrolera nr 3 podłączonego do silnika i żarówki:
-Silniczek imituje pracę klimatyzacji – kręci się z prędkością adekwatną do poziomu nadmuchu (użycie PWM do ustalenia prędkości silnika i przerwania INT0 do zmiany kierunku kręcenia)
-Żarówki wskazują tryb pracy klimatyzacji – górna świecąca żółtopomarańczowym kolorem oznacza ogrzewanie, a dolna świecąca białym kolorem chłodzenie
4. Funkcje mikrokontrolera nr 4 podłączonego do przycisku i ekranu LCD:
-Wyświetlanie na ekranie LCD trybu działania i poziomu nadmuchu klimatyzacji
-Wyświetlanie na ekranie LCD parametrów kontroli trakcji
-Przełączanie widoku za pomocą przycisku

##Instrukcja obsługi urządzenia:
###Radio:
- włączenie – wciśnięcie przycisku przy wyświetlaczu BCD – zapali się dioda LED i zacznie wyświetlać się aktualna stacja radiowa,
- wyłączenie – przytrzymanie przycisku przy wyświetlaczu BCD przez 2 sekundy – zgasi się dioda LED i wyłączy się wyświetlacz,
- sterowanie stacjami radia:
	- poprzednia stacja – klawisz 1,
	- następna stacja – klawisz 2,
- wyciszenie / odciszenie radia – klawisz 3 – podczas wyciszenia na ekranie miga symbol wyciszenia,
- regulacja głośności (skala 1 – 30, na chwilę pojawi się na ekranie BCD aktualna głośność):
	- zmniejszenie głośności – klawisz 4,
	- zwiększenie głośności – klawisz 5.
###Ustawienia kontroli trakcji:
- parametry kontroli trakcji są wyświetlane na wyświetlaczu LCD,
- włączenie / wyłączanie możliwości zmiany ustawień kontroli trakcji – naciśnięcie po kolei klawiszy *, 0, # (podwójne mignięcie diody) – podczas włączenia sterowania kontrolą trakcji wyłączone jest sterowanie radiem za pomocą klawiatury,
- wybranie trybu działania kontroli trakcji:
	- normalny (tryb domyślny) – naciśnięcie klawisza 1,
	- sportowy – naciśnięcie klawisza 2,
	- wyłączona – naciśnięcie klawisza 3.
###Klimatyzacja:
- włączenie / wyłączenie – wciśnięcie klawisza 6 (podczas działania klimatyzacji na ekranie LCD jest pokazywany aktualny poziom nadmuchu i tryb działania, kierunek kręcenia się silnika zmienia się po 20 obrotach),
- zmiana trybu – ogrzewanie lub chłodzenie – wciśnięcie klawisza 7 (zapalona górna żarówka na żółtopomarańczowy kolor oznacza grzanie, a dolna na biały kolor - chłodzenie),
- zmiana poziomu nadmuchu (1-6):
	- zmniejszenie – wciśnięcie klawisza 8,
	- zwiększenie – wciśnięcie klawisza 9. 
###Ekran LCD:
- ma 2 tryby wyświetlania – tryb wyświetlania parametrów klimatyzacji i tryb wyświetlania trybu kontroli trakcji,
- zmiana trybu wyświetlania – przytrzymanie przez sekundę przycisku przy ekranie.

