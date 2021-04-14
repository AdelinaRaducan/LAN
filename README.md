# Introducere

Pentru a funcționa, o rețea locală de calculatoare are nevoie de dispozitive de interconectare.
Un astlfel de dispozitiv este switch-ul.
Vom caracteriza simplist un switch prin:
- Id - un număr natural
- Denumire - denumirea switch-ului(șir de caractere alocat dinamic)
- Adresa IPv4 - un număr pe 32 biți (4 octeți, separați de punct). Ex: 192.168.13.10
- Mod de funcționare - 2 moduri: mod stivă, mod individual\

Pentru a putea fi administrate mai usor, switch-urile se pot grupa in stive. Fiecare stivă
are un singur switch principal. Administrarea switch-urilor se poate face individual sau prin
intermediul switch-ului principal al stivei. Astfel configurarea unui switch se poate realiza princonectarea administratorului de rețea la switch-ul principal din stivă sau prin conectarea directă
la switch-ul ce va fi configurat.
Switch-ul principal dintr-o stivă va fi mereu localizat la baza acesteia, iar restul switch-urilor vor
fi aranjate după id. Astfel switch-ul cu id-ul cel mai mare se va afla mai aproape de baza stivei.
Switch-urile care nu lucrează in modulul stivă vor fi ținute intr-o coadă sortate crescator după
IPv4.

# Cerinta
Va trebui să executați un set de operații care se vor citi dintr-un fișier.
Operatiile pot fi:
- add - adaugarea unui switch in rețea
- del - scoaterea unui switch din rețea
- set - setează mod de lucru pentru switch.
- ipmin - afișarea celei mai mici adrese IPv4 a switch-urilor care funcționează in
- mod individual
- show - afișarea cozii și a stivelor

# Descrierea operațiilor și a datelor de intrare

Pe prima linie a fișierului de intrare se va afla numărul de stive din rețea.
Pe următoarele linii se găsesc operațiile care pot fi aplicate asupra switch-urilor.
Se va citi codul comenzii, iar în funcție de acesta se citește descrierea comenzii ca mai jos.
- add [id] [denumire] [IP] [funcționare] [id_stiva] [principal]\
Modul de funcționare poate avea valorile:\
-SINGLE -> functionează individual\
-STACK -> functionează in stivă\
Un switch poate fi principal sau secundar, în funcție de valorile argumentului principal:\
-NON_BASE -> switch secudar\
-BASE -> switch principal\

Observații:
1. Dacă switch-ul care va fi adaugat funcționeaza in mod stiva și este switch principal, atunci
switch-ul care este principal inaintea inserării acestuia nu va mai fi principal.
2. Dacă un switch este inserat într-o stivă goală, atunci el va deveni switch principal.

- del id\
Switch-ul va fi scos din rețea doar dacă acesta există\

- set [id] [functionare] [id_stiva] [principal]\
1. Dacă $functionare = modul curent de funcționare al switchului atunci nu se
efectueaza nicio schimbare.
2. Dacă switch-ul principal este scos din stivă, locul lui este luat de switch-ul cu id-ul cel mai mare
din acea stivă.
3. Dacă un switch este inserat in stiva ca switch principal atunci switch-ul care a fost principal pan ă
la acel moment iși pierde acest rol, iar stiva trebuie refacut ă (reordonată).

- ipmin\
Dacă toate switchurile lucreaz ă in mod stivă atunci se va afișa 0.
Ex: In loc de adresa 192.168.1.13 va fi afișat numărul 3232235789

- show\
Se vor afișa pe o linie id-urile switch-urilor din coadă. Stivele se vor afișa in ordinea id-urilor.
Switch-urile se vor afișa de la varful stivei la bază, fiecare pe câte o linie astfel: id IPv4 denumire.
