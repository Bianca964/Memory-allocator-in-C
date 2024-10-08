
## Segregated free list
    Ideea principala a acestui program este crearea unui alocator de memorie
virtual (implicand lucrul cu structuri, liste generice dublu inlantuite si
vectori de liste) conform unor comenzi introduse in terminal.Pentru a
interpreta aceste comenzi, citesc numele functiei si compar cu optiunile pe
care le am la dispozitie, daca nu e niciuna din optiuni, trece mai departe
fara a face nimic. Acest lucru se intampla intr-un while pana cand varibila
destroy_done devine -1 (adica functia destroy a fost deja apelata in functiile
READ sau WRITE), caz in care memoria a fost deja eliberata, elieberez si ceea
ce am alocat in main, dupa care se incheie programul.
	Am inceput acest program prin a crea o structura generica pentru liste
dublu inalntuite si, implicit, noduri, dupa care mi-am creat structura de baza,
structura heap_t in care retin lucrurile esentiale, cat si vectorul de liste
propriu-zis. Mai am o structura separata pentru informatiile pe care le
contine fiecare nod in parte.

# Functii
	Primele functii sunt cele clasice pentru prelucrarea listelor dublu
inlantuite: crearea, adaugarea pe pozitia n a unui nod, stergerea de pe
pozitia n, returnarea unui nod de pe o anumita pozitie, cat si eliberarea
unei astfel de liste. Toate lucreaza cu nodurile si modifica conexiunile
corespunzator.

# INIT_HEAP
	Prima functie este cea de initializare, si anume "INIT_HEAP". Aceasta
initializeaza structura heap, aloca memorie vectorului de liste si fiecarei
liste in parte si pune in fiecare nod datele corespunzatoare, tinand cont de
regulile pentru continuitatea adreselor si dimensiunea pe care o pun la
dispozitie (size-ul): initial, porneste de la size-ul 8 si de la adresa citita
din terminal, multiplicandu-se cu 2 cand trece la urmatoarea lista. Campul
'date' este deocamadata NULL. Ma folosesc de functiile create in prealabil
pentru gestionarea listei si nodurilor.

# MALLOC
	Cea de-a doua functie este cea de alocare a memoriei. In urma functiei
init, tuturor nodurilor le corespunde o adresa. Functia 'MALLOC' primeste ca
parametru numarul de bytes pe care trebuie sa-i aloc. Astfel, caut in tot
vectorul prima lista care are cel mai mic numar de bytes mai mare decat cel
citit (pentru a avea loc). Fac acest lucru afland diferenta dintre numarul
citit si cati bytes am la dispozitie in lista respectiva(compar cu size-ul
din head-ul fiecarei liste deaorece intreeaga lista dispune de aceeasi memorie
disponibila). In functie de cat imi da diferenta(diff) disting 3 cazuri:
	1. diff este mai mic decat 0, caz in care nu face nimc si trece la
urmatoarea lista (nu incape in lista curenta)
	2. diff este egal cu 0, caz in care muta tot nodul inn lista alocata
	3. diff este strict mai mare decat 0, (creste numarul fragementarilor)
caz in care trebuie sa aloc doar numarul de bytes ceruti, adaug nodul cu
size-ul cerut in lista alocata, iar pentru restul (diff) creez o noua lista
in vector ce va avea size-ul diff, modificand si adresa astfel incat sa
respecte regulile. De asemenea, verific de fiecare data cand diff >= 0 daca,
dupa ce-am eliminat un nod dintr-o lista free, aceasta mai are alte noduri.
Daca nu mai are, trebuie stearsa din vectorul de liste. Am creat functiile
"add_new_list" si "remove_list" pentru a facilita acest lucru. Mesajul
'Out of memory' se afiseaza cand nu a gasit niciun nod destul de incapator.

# FREE
	Functia 'my_free" elibereaza memoria alocata de la o adresa data, cu alte
cuvinte muta nodul respectiv din lista alocata in vectorul de liste free
pentru a fi disponibil ulterior, nu inainte de a verifica daca adresa este
una valida, alocata. Inainte de a o adauga in free_list, gasesc indexul listei
corespunzatoare size-ului, dar si pozitia in lista, astfel incat sa fie sortat
dupa adrese. Mesajul de eroare 'Invalid free' se afiseaza daca nu gaseste
adresa in lista alocata.

# WRITE
	Functia 'my_write' realizeaza scrierea unui anumit numar de bytes in
adresele alocate, incepand de la o adresa citita de la tastatura. Mai intai
verific daca adresa respectiva se afla in allocated_list. Daca se afla, ma
plasez pe nodul cu adresa respectiva si am 2 cazuri: daca incape sa scriu
toti bytes ce mi se cer in adresa respectiva, ii scriu direct acolo, altfel
scriu in mai multe noduri pana ajung la numarul de bytes ceruti, nu inainte
de a verifica daca am toate adresele valide cu ajutorul unei functii separate
(check_seg_fault_in_write). De fiecare data cand adaug ceva intr-un nod,
modific sirul meu stergand elemenele scrise anterior. Scrierea in noduri se
realizeaza treptat, la final ramanand un numar de bytes mai mic sau egal cu
cat incape in ultimul nod, asa ca am facut o functie separata pentru adaugarea
partiala.

# READ
	Functia "my_read" realizeaza citirea unui anumit numar de bytes din
adresele deja alocate din allocated_list, incepand cu o anumita adresa citita
de la tastatura. Mai intai verific daca adresa respectiva se afla in
allocated_list. Daca se afla, ma plasez pe nodul cu adresa respectiva si am 2
cazuri: daca toti bytes pe care vreau sa-i afisez se gasesc doar in acea adresa,
ii afisez direct, altfel trebuie sa mi creez sirul de caractere mergand din
nod pentru a adauga in string-ul meu ce gasesc scris pana ating numarul de
bytes care trebuie afisat, nu inainte de a verifica daca am toate adresele
valide (cu ajutorul unei functii separate).

# DUMP_MEMORY
	Functia "dump_memory" realizeaza afisarea tuturor datelor cerute,
parcurgand atat vectorul de liste in intregime, cat si lista dublu inlantuita
pentru afisare. Afisez dupa sablonul indicat.

# DESTROY_HEAP
	Functia "destroy_heap" realizeaza eliberarea tuturor resurselor : parcurg
fiecare lista din vectorul de free_list pentru a aplica functia de eliberare a
memoriei pentru o intreaga lista. La final, eliberez memoria alocata
vectorului de liste si structurii in sine, dar si listei dublu inlantuite :
allocated_list. Se realizeaza incheierea programului. De asemenea, functia
"destroy_heap" se poate apela si in cazul in care se detecteaza o inconsistenta
a adreselor pentru functiile 'read' si 'write', caz in care trebuie sa marchez
acest lucru in main pentru a nu apela de 2 ori functia destroy_heap. Fac acest
lucru cu o varibila care devine -1 cand deja s-a apelat o data functia
destroy_heap. Totul se termina cu bine. :)

# MAIN
	Principala functie citeste numele functiilor introduse de la tastatura si
apeleaza functia corespunzatoare, citind si celelalate date trimise ca
parametru functiilor. La final, eliberez si memoria alocata pentru numele
functiilor.
