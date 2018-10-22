             _    ____  ____            _____                      _ 
            / \  |  _ \|  _ \          |_   _|__ _ __ ___   __ _  / |
           / _ \ | |_) | | | |  _____    | |/ _ \ '_ ` _ \ / _` | | |
          / ___ \|  __/| |_| | |_____|   | |  __/ | | | | | (_| | | |
         /_/   \_\_|   |____/            |_|\___|_| |_| |_|\__,_| |_|


  ___          _     _     ___ ___   _      _   
 | _ \__ _ _ _| |_  / |   / __/ __| /_\    /_\  
 |  _/ _` | '_|  _| | |_  \__ \__ \/ _ \  / _ \ 
 |_| \__,_|_|  \__| |_(_) |___/___/_/ \_\/_/ \_\


Structuri folosite:

 - image: conține datele din fișierul PPM/PGM
   * type: tipul imaginii (5 pentru P5/PGM, 6 pentru P6/PPM)
   * width: lățimea imaginii
   * height: înălțimea imaginii
   * maxval: valoarea maximă a unui pixel (<= 255)
   * image: matrice care conține valorile pixelilor ce desciu imaginea
 - thread_func_args: conține argumentele de care are nevoie threadFunc
   * in: imaginea inițială
   * out: imaginea obținută după redimensionare\
   * thread_id: id-ul thread-ului


Funcții:

 - allocImage:  alocă matricea de pixeli dintr-o imagine
 - freeImage:   eliberează matricea de pixeli dintr-o imagine
 - resizeBw:    redimensionează imaginea inițială (greyscale) în funcție de
                resize_factor
 - resizeColor: redimensionează imaginea inițială (color) în funcție de
                resize_factor
 - readInput:   citește datele dintr-un fișier și le memorează într-o structură
                de tip <image>
 - writeData:   scrie datele dintr-o structură de tip <image> într-un fișier
 - resize:      folosește funcțiile resizeBw și resizeColor pentru a redimen-
                siona o imagine (mai exact, a o micșora de resize_factor ori)
                folosind mai multe fire de execuție (num_threads).


Memorie:

 Deoarece nu putem modifica funcția main, memoria este alocată și eliberată în
funcțiile din homework.c astfel:

-> alocare in: readInput
-> alocare out: resize
-> eliberare in: resize
-> eliberare out: writeData


Algoritm redimensionare:

 Pentru fiecare pixel din noua imagine, se calculează valoarea (pe un singur
canal de culoare în cazul resizeBw, sau pe trei canale în cazul resizeColor)
în funcție de cei resize_factor * resize_factor pixeli corespunzători din ima-
ginea originală. În cazul în care resize_factor este diferit de 3, se calculea-
ză media aritmetică a pixelilor din imaginea originală. În cazul în care
resize_factor este 3, se înmulțește fiecare dintre cei 9 pixeli corespunzători
din imaginea originală cu elementul de pe poziția sa din kernel-ul gaussian, iar
rezultatul se împarte la 16 (suma elementelor din kernel-ul gaussian). 


Paralelizare:

 Imaginea este împărțită pe verticală în num_threads, iar fiecare thread calcu-
lează valorile din partea corespunzătoare a imaginii. Această abordare scalează
bine în cazul imaginilor a căror înălțime este mai mare decât lățimea.

 Alternativ, dacă imaginile pe care lucrăm au predominant lățimea mai mare decât
înălțimea (un caz mai puțin întâlnit în practică, în general), imaginea ar putea
fi împărțită pe orizontală, cazul în care primele 2 bucle din funcție ar arăta
așa:

     for (i1 = 0; i1 < out->height; i1++)
        for (j1 = start; j1 < end; j1++)

unde:

     start = thread_id * (out->width) / num_threads
     end = (thread_id + 1) * (out->width) / num_threads

  ___          _     ___     ___             _                 
 | _ \__ _ _ _| |_  |_  )   | _ \___ _ _  __| |___ _ _ ___ _ _ 
 |  _/ _` | '_|  _|  / / _  |   / -_) ' \/ _` / -_) '_/ -_) '_|
 |_| \__,_|_|  \__| /___(_) |_|_\___|_||_\__,_\___|_| \___|_|  

Structuri folosite:

 - image: conține datele din fișierul PPM/PGM
   * width: lățimea imaginii
   * height: înălțimea imaginii
   * maxval: valoarea maximă a unui pixel (<= 255)
   * image: matrice care conține valorile pixelilor ce desciu imaginea
 - thread_func_args: conține argumentele de care are nevoie threadFunc
   * img: imaginea inițială
   * thread_id: id-ul thread-ului


Funcții:

 - initialise:  inițializează și alocă o structură de tip image
 - pixelsToCm:  convertește din pixeli în centimetri în funcție de dimensiunea
                imaginii (în cm) și rezoluție
 - getDistance: calculează distanța dintre un punct și dreapta Ax+By+C
 - threadFunc:  randează imaginea
 - render:      randează imaginea pe mai multe fire de execuție folosind
                threadFunc
 - writeData:   scrie datele dintr-o structură de tip <image> într-un fișier


Memorie:

 Deoarece nu putem modifica funcția main, memoria este alocată în funcția
initialise și eliberată în funcția writeData.


Algoritm randare:

 Pentru fiecare pixel din imagine, se calculează distanța de la centrul pixel-
ului la linia Ax+By+C (în centimetri). Dacă ese mai mică decât 3, pixelul va fi
negru, iar în caz contrar, pixelul va fi alb.

     Fie resolution = 4. Avem imaginea următoare, unde valorile de tip "x,y"
    sunt coordonatele care indică poziția pixelului respectiv:

    <-------- 100cm -------->
    +-----+-----+-----+-----+
    | 0,3 | 1,3 | 2,3 | 3,3 |
    +-----+-----+-----+-----+
    | 0,2 | 1,2 | 2,2 | 3,2 |
    +-----+-----+-----+-----+
    | 0,1 | 1,1 | 2,1 | 3,1 |
    +-----+-----+-----+-----+
    | 0,0 | 1,0 | 2,0 | 3,0 |
    +-----+-----+-----+-----+

     Fie pixelul (1,2). Coordonatele colțului din stânga jos ale acestui pixel,
    în centimetri, sunt următoarele:

      x1 = 1 * 100 / 4 = 25
      y1 = 2 * 100 / 4 = 50

      Obținem coordonatele centrului pixelului (în centimetri) adăugând jumătate
    din latura pixelului la fiecare din coordonate:

      x2 = x1 + 100 / 4 / 2 = 25 + 2.5 = 27.5
      y2 = y1 + 100 / 4 / 2 = 50 + 2.5 = 52.5

      Calculăm distanța de la centrul pixelului (punctul de coordonate x2, y2)
    folosind formula:

      d = | a * x + b * y + c | / sqrt(a ^ 2 + b ^ 2) unde a = -1, b = 2, c = 0

      Obținem:

      d = (-1 * 27.5 + 2 * 52.5) / √5 = 34.65 > 3

      Deoarece d > 3, pixelul va fi alb.


Paralelizare:

 Imaginea este împărțită pe verticală în num_threads, iar fiecare thread calcu-
lează valorile din partea corespunzătoare a imaginii. Această abordare scalează
bine în cazul imaginilor a căror înălțime este mai mare decât lățimea.

 Alternativ, dacă imaginile pe care lucrăm au predominant lățimea mai mare decât
înălțimea (un caz mai puțin întâlnit în practică, în general), imaginea ar putea
fi împărțită pe orizontală, cazul în care primele 2 bucle din funcție ar arăta
așa:

     for (i1 = 0; i1 < out->height; i1++)
        for (j1 = start; j1 < end; j1++)

unde:

     start = thread_id * (out->width) / num_threads
     end = (thread_id + 1) * (out->width) / num_threads

=========================== ALEXANDRU Ioana | 324CB ===========================